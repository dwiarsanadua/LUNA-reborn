#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <network/TcpServer.hpp>
#include <ecs/systems/GameDataDB.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Movement.hpp>
#include <ecs/components/AIComponent.hpp>
#include <ecs/components/CombatState.hpp>
#include <ecs/components/SpawnInfo.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/components/VisualInfo.hpp>
#include <ecs/systems/MovementSystem.hpp>
#include <ecs/systems/AISystem.hpp>
#include <ecs/systems/SpawnSystem.hpp>
#include <ecs/systems/CombatSystem.hpp>
#include <game/network/PacketDispatcher.hpp>
#include <PacketType_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <Entity_generated.h>
#include <Combat_generated.h>
#include <Types_generated.h>
#include <cstdlib>
#include <csignal>
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <unordered_map>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <entt/entt.hpp>
#include <game/Log.h>
#include <sys/stat.h>

static std::unique_ptr<TcpServer> g_server;
static bool g_running = true;
static entt::registry* g_registry = nullptr;
static std::unordered_map<uint32_t, std::shared_ptr<TcpConnection>> g_clients;
static std::unordered_map<uint32_t, entt::entity> g_player_entities;
static std::mutex g_client_mutex;
static Luna::PacketDispatcher g_dispatcher;

void signal_handler(int) { g_running = false; }

static void SendEntitySpawn(std::shared_ptr<TcpConnection> conn, entt::entity e) {
    auto& xform = g_registry->get<Transform>(e);
    auto& stats = g_registry->get<CharacterStats>(e);
    
    std::string name = "Entity";
    std::string model = "monster_placeholder.glb";
    float scale = 1.0f;
    luna::protocol::EntityType type = luna::protocol::EntityType_Monster;

    if (auto* visual = g_registry->try_get<VisualInfo>(e)) {
        name = visual->name;
        model = visual->model_file;
        scale = visual->scale;
    }
    
    if (g_registry->all_of<TagPlayer>(e)) {
        type = luna::protocol::EntityType_Player;
        model = "d_man.glb";
    }

    uint16_t level = stats.level;
    float hp_pct = stats.max_hp > 0 ? (float)stats.hp / stats.max_hp : 1.0f;
    uint32_t eid = static_cast<uint32_t>(e);

    flatbuffers::FlatBufferBuilder fbb;
    auto pos = luna::protocol::Vec3(xform.position.x, xform.position.y, xform.position.z);
    auto spawn = luna::protocol::CreateEntitySpawnDirect(fbb, eid, type, model.c_str(), &pos, 0, scale, name.c_str(), level, hp_pct);
    fbb.Finish(spawn);

    PacketHeader hdr;
    hdr.magic = 0x4C4E50; hdr.length = static_cast<uint16_t>(fbb.GetSize());
    hdr.type = 0x1001; hdr.sequence = 0; hdr.checksum = 0;
    
    std::vector<uint8_t> packet;
    packet.insert(packet.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));
    packet.insert(packet.end(), fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize());
    g_server->Send(conn, packet.data(), packet.size());
}

static void BroadcastPacket(uint16_t type, const uint8_t* data, size_t len) {
    PacketHeader hdr;
    hdr.magic = 0x4C4E50; hdr.length = static_cast<uint16_t>(len);
    hdr.type = type; hdr.sequence = 0; hdr.checksum = 0;
    
    std::vector<uint8_t> packet;
    packet.insert(packet.end(), (uint8_t*)&hdr, (uint8_t*)&hdr + sizeof(hdr));
    packet.insert(packet.end(), data, data + len);
    
    std::lock_guard<std::mutex> lock(g_client_mutex);
    for (auto& [id, conn] : g_clients) {
        g_server->Send(conn, packet.data(), packet.size());
    }
}

int main(int argc, char* argv[]) {
    mkdir("logs", 0777);
    Luna::InitLog("map", "map.log");
    spdlog::info("LUNA Plus Reborn — MapServer v{}", "1.0.0");

    int map_id = 51;
    int port = 8300;
    int distribute_port = 8200;
    if (argc > 1) map_id = std::atoi(argv[1]);
    if (argc > 2) port = std::atoi(argv[2]);

    signal(SIGINT, signal_handler); signal(SIGTERM, signal_handler);

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) {
        spdlog::error("Failed to open game database");
        return 1;
    }
    auto monsters = db.GetAllMonsters();
    spdlog::info("Loaded {} monster definitions", monsters.size());

    entt::registry registry;
    g_registry = &registry;
    MovementSystem movement_sys;
    AISystem ai_sys;
    SpawnSystem spawn_sys;
    CombatSystem combat_sys;

    // --- Register Protocol Handlers ---
    g_dispatcher.Register(luna::protocol::PacketType_MP_MOVE_WALK, [](uint32_t conn_id, uint16_t type, const std::vector<uint8_t>& payload) {
        std::lock_guard<std::mutex> lock(g_client_mutex);
        if (g_player_entities.count(conn_id)) {
            auto e = g_player_entities[conn_id];
            auto& move = g_registry->get<Movement>(e);
            move.is_moving = true;
            move.current_speed = 5.0f;
            spdlog::debug("Player {} walk command received", conn_id);
        }
    });

    g_dispatcher.Register(luna::protocol::PacketType_MP_MOVE_STOP, [](uint32_t conn_id, uint16_t type, const std::vector<uint8_t>& payload) {
        std::lock_guard<std::mutex> lock(g_client_mutex);
        if (g_player_entities.count(conn_id)) {
            auto e = g_player_entities[conn_id];
            auto& move = g_registry->get<Movement>(e);
            move.is_moving = false;
            spdlog::debug("Player {} stop command received", conn_id);
        }
    });

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    int spawned = 0;
    for (auto& m : monsters) {
        if (m.monster_id == 0) continue;
        auto entity = registry.create();
        float x = (static_cast<float>(rng() % 200) - 100);
        float z = (static_cast<float>(rng() % 200) - 100);
        registry.emplace<Transform>(entity, glm::vec3{x, 0, z});
        auto& stats = registry.emplace<CharacterStats>(entity);
        stats.level = m.level;
        stats.hp = m.hp; stats.max_hp = m.hp;
        stats.physic_attack = m.attack; stats.physic_defense = m.defense;
        stats.accuracy = 80.0f; stats.evasion = 10.0f;
        registry.emplace<Movement>(entity);
        
        auto& visual = registry.emplace<VisualInfo>(entity);
        visual.name = m.name;
        visual.model_file = m.model_file;
        visual.scale = 1.0f + (m.level * 0.005f);

        auto& ai = registry.emplace<AIComponent>(entity);
        ai.spawn_position = glm::vec3{x, 0, z};
        ai.aggro_range = 25.0f; ai.attack_range = 3.5f; ai.chase_range = 50.0f;
        registry.emplace<TagMonster>(entity);
        registry.emplace<CombatState>(entity);
        spawned++;
    }
    spdlog::info("Spawned {} monsters on map {}", spawned, map_id);

    // --- NEW: Spawn Dummy Player for AI Testing ---
    auto test_player = registry.create();
    registry.emplace<Transform>(test_player, glm::vec3{10.0f, 0, 10.0f});
    auto& pstats = registry.emplace<CharacterStats>(test_player);
    pstats.hp = 10000; pstats.max_hp = 10000; pstats.level = 50;
    pstats.evasion = 20;
    registry.emplace<TagPlayer>(test_player);
    spdlog::info("Spawned DUMMY PLAYER at (10, 0, 10) for AI testing");

    g_server = std::make_unique<TcpServer>();
    g_server->SetNewConnectionCallback([](std::shared_ptr<TcpConnection> conn) {
        spdlog::info("Client connected: id={}", conn->id);
        entt::entity player;
        {
            std::lock_guard<std::mutex> lock(g_client_mutex);
            g_clients[conn->id] = conn;
            player = g_registry->create();
            g_player_entities[conn->id] = player;
        }

        g_registry->emplace<Transform>(player, glm::vec3{0, 0, 0});
        auto& stats = g_registry->emplace<CharacterStats>(player);
        stats.level = 10; stats.hp = 1000; stats.max_hp = 1000;
        stats.physic_attack = 100; stats.physic_defense = 50;
        stats.accuracy = 100; stats.evasion = 50;
        g_registry->emplace<Movement>(player);
        g_registry->emplace<TagPlayer>(player);
        g_registry->emplace<CombatState>(player);

        // Notify new client about existing entities
        auto view = g_registry->view<Transform, CharacterStats>();
        for (auto entity : view) {
            SendEntitySpawn(conn, entity);
        }
    });

    g_server->SetDataCallback([](std::shared_ptr<TcpConnection> conn, uint8_t* data, size_t len, uint16_t type) {
        g_dispatcher.Dispatch(conn->id, type, std::vector<uint8_t>(data, data + len));
    });

    g_server->SetCloseCallback([](std::shared_ptr<TcpConnection> conn) {
        std::lock_guard<std::mutex> lock(g_client_mutex);
        if (g_player_entities.count(conn->id)) {
            g_registry->destroy(g_player_entities[conn->id]);
            g_player_entities.erase(conn->id);
        }
        g_clients.erase(conn->id);
        spdlog::info("Client disconnected: id={}", conn->id);
    });

    if (!g_server->Start(static_cast<uint16_t>(port))) {
        spdlog::error("Failed to start server on port {}", port);
        return 1;
    }

    spdlog::info("MapServer (map {}) running on port {} with {} monsters", map_id, port, spawned);

    const float tick_rate = 1.0f / 20.0f;
    while (g_running && g_server->IsRunning()) {
        g_server->Poll();
        auto tick_start = std::chrono::steady_clock::now();
        
        movement_sys.Update(*g_registry, tick_rate);
        ai_sys.Update(*g_registry, tick_rate);
        combat_sys.Update(*g_registry, tick_rate);

        // Sync transforms to clients
        static float sync_timer = 0;
        sync_timer += tick_rate;
        if (sync_timer >= 0.1f) {
            auto view = g_registry->view<Transform, Movement>();
            for (auto entity : view) {
                auto& mv = view.get<Movement>(entity);
                if (mv.is_moving || sync_timer >= 1.0f) {
                    auto& xform = view.get<Transform>(entity);
                    flatbuffers::FlatBufferBuilder fbb;
                    auto pos = luna::protocol::Vec3(xform.position.x, xform.position.y, xform.position.z);
                    auto trans = luna::protocol::CreateEntityTransformDirect(fbb, static_cast<uint32_t>(entity), &pos, 0, "walk", 1.0f, 0);
                    fbb.Finish(trans);
                    BroadcastPacket(0x1004, fbb.GetBufferPointer(), fbb.GetSize());
                }
            }
            sync_timer = 0;
        }

        auto elapsed = std::chrono::steady_clock::now() - tick_start;
        auto sleep_time = std::chrono::duration<float>(tick_rate) - elapsed;
        if (sleep_time.count() > 0)
            std::this_thread::sleep_for(sleep_time);
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    db.Close();
    spdlog::info("MapServer shutdown");
    return 0;
}
