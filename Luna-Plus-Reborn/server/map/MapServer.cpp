#include "MapServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include "systems/CombatSystem.h"
#include "systems/AISystem.h"
#include "systems/MovementSystem.h"
#include "systems/ItemSystem.h"
#include "systems/QuestSystem.h"
#include "systems/SpawnSystem.hpp"
#include "systems/GridSystem.hpp"
#include "systems/TriggerSystem.hpp"
#include "systems/MapScriptRuntime.hpp"
#include "systems/SecondarySystem.hpp"
#include <ecs/systems/SkillSystem.hpp>
#include <ecs/components/SkillBook.hpp>
#include <ecs/components/QuestLog.hpp>
#include <sqlite3.h>
#include <ecs/components/Inventory.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/SpawnInfo.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/Tag.hpp>
#include <entt/entt.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Character_generated.h>
#include <Entity_generated.h>
#include <Movement_generated.h>
#include <Combat_generated.h>
#include <Chat_generated.h>
#include <Inventory_generated.h>
#include <CharLife_generated.h>
#include <MapChange_generated.h>
#include <Party_generated.h>
#include <Storage_generated.h>
#include <Friend_generated.h>
#include <Guild_generated.h>
#include <Trade_generated.h>
#include <Consignment_generated.h>
#include <StreetStall_generated.h>
#include <Quest_generated.h>
#include <Trigger_generated.h>
#include <Dungeon_generated.h>
#include <Family_generated.h>
#include <Pet_generated.h>
#include <Fishing_generated.h>
#include <Secondary_generated.h>
#include <ecs/components/AIComponent.hpp>
#include <PacketType_generated.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <chrono>
#include <thread>
#include <random>
#include <sstream>
#include <filesystem>

static std::mt19937 s_dungeon_rng(std::random_device{}());

MapServer::MapServer()
    : network_(std::make_unique<NetworkLayer>())
    , db_(std::make_unique<Database>())
    , registry_(std::make_unique<entt::registry>())
    , combat_(std::make_unique<CombatSystem>())
    , ai_(std::make_unique<AISystem>())
    , movement_(std::make_unique<MovementSystem>())
    , item_(std::make_unique<ItemSystem>())
    , quest_(std::make_unique<QuestSystem>())
    , spawn_sys_(std::make_unique<SpawnSystem>())
{}

MapServer::~MapServer() { Shutdown(); }

glm::vec3 MapServer::GetMapSpawnPosition(uint16_t map_id) {
    switch (map_id) {
        case 13: return {12.0f, 0.0f, 8.0f};
        case 20: return {6.0f, 0.0f, 4.0f};
        case 51: return {0.0f, 0.0f, 0.0f};
        default: return {0.0f, 0.0f, 0.0f};
    }
}

void MapServer::RegisterWithDistribute() {
    std::thread([map_id = map_id_, port = port_]() {
        NetworkLayer dist;
        if (!dist.Connect("127.0.0.1", 8300)) {
            spdlog::warn("MapServer: could not register with DistributeServer (port 8300)");
            return;
        }
        flatbuffers::FlatBufferBuilder fbb;
        std::string name = (map_id == 51) ? "Alker Harbor"
            : (map_id == 13) ? "Red Orc Outpost"
            : (map_id == 20) ? "Alker Harbor Town"
            : ("Map_" + std::to_string(map_id));
        auto reg = luna::protocol::CreateMapServerRegister(
            fbb, static_cast<uint16_t>(map_id), port, fbb.CreateString(name));
        fbb.Finish(reg);
        dist.SendPacket(luna::protocol::PacketType_MP_MAPSERVER_REGISTER_SYN,
            fbb.GetBufferPointer(), fbb.GetSize());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        dist.Disconnect();
        spdlog::info("MapServer: registered map {} with DistributeServer", map_id);
    }).detach();
}

void MapServer::HandleChangeMap(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;

    auto req = flatbuffers::GetRoot<ChangeMapRequest>(payload);
    uint16_t target = req->target_map_id();
    glm::vec3 spawn = GetMapSpawnPosition(target);
    uint16_t target_port = static_cast<uint16_t>(8200 + target);

    flatbuffers::FlatBufferBuilder fbb;
    Vec3 pos{spawn.x, spawn.y, spawn.z};

    if (target == static_cast<uint16_t>(map_id_)) {
        pos = Vec3(connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z);
        auto resp = CreateChangeMapResponse(fbb, 2, target, port_, &pos);
        fbb.Finish(resp);
        network_->SendPacket(PacketType_MP_USERCONN_CHANGEMAP_NACK,
            fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    SavePlayerPosition(connected_player_.id,
        connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z);
    SendEntityDespawn(static_cast<uint32_t>(connected_player_.id),
        static_cast<int8_t>(DespawnReason_Logout));
    DespawnPlayer(connected_player_.id);
    player_joined_ = false;
    player_entity_ = entt::null;
    monster_net_.clear();

    auto resp = CreateChangeMapResponse(fbb, 0, target, target_port, &pos);
    fbb.Finish(resp);
    network_->SendPacket(PacketType_MP_USERCONN_CHANGEMAP_ACK,
        fbb.GetBufferPointer(), fbb.GetSize());
    spdlog::info("MapServer: player {} changemap {} -> {} (port {})",
        connected_player_.id, map_id_, target, target_port);
}

bool MapServer::Initialize(int map_id, uint16_t port) {
    map_id_ = map_id;
    port_ = port;

    if (!db_->Initialize("assets/data/luna_map.db")) {
        spdlog::error("MapServer: database init failed");
        return false;
    }

    db_->Execute(
        "CREATE TABLE IF NOT EXISTS player_saves ("
        "  entity_id INTEGER PRIMARY KEY,"
        "  account_id INTEGER,"
        "  name TEXT,"
        "  level INTEGER DEFAULT 1,"
        "  class_id INTEGER DEFAULT 0,"
        "  map_id INTEGER,"
        "  pos_x REAL DEFAULT 0,"
        "  pos_y REAL DEFAULT 0,"
        "  pos_z REAL DEFAULT 0,"
        "  hp INTEGER DEFAULT 500,"
        "  max_hp INTEGER DEFAULT 500,"
        "  exp INTEGER DEFAULT 0,"
        "  gold INTEGER DEFAULT 0"
        ");"
    );

    db_->Execute(
        "CREATE TABLE IF NOT EXISTS dungeon_instances ("
        "  instance_id INTEGER PRIMARY KEY,"
        "  template_id INTEGER,"
        "  state INTEGER DEFAULT 0,"
        "  created_at INTEGER,"
        "  completed_at INTEGER"
        ");"
    );

    if (!network_->Initialize(port)) {
        spdlog::error("MapServer: network init failed on port {}", port);
        return false;
    }

    network_->SetReceiveCallback([this](uint16_t type, const uint8_t* data, size_t size) {
        HandlePacket(type, data, size);
    });

    // Load monster spawns and NPCs via SpawnSystem + direct DB
    {
        spawn_sys_->LoadSpawnData("assets/data/monsters.json");
        spawn_sys_->SpawnMonstersForMap(*registry_, map_id);

        sqlite3* gamedb = nullptr;
        if (sqlite3_open("assets/data/game_data.db", &gamedb) == SQLITE_OK) {
            // Query NPC positions for this map
            const char* npc_sql = "SELECT col_0000, col_0001, col_0002, col_0003, col_0004, col_0005 FROM game_npc WHERE col_0002=?";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(gamedb, npc_sql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, map_id);
                int npc_count = 0;
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    int npc_id = sqlite3_column_int(stmt, 0);
                    const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                    float x = static_cast<float>(sqlite3_column_double(stmt, 3));
                    float y = static_cast<float>(sqlite3_column_double(stmt, 4));
                    float z = static_cast<float>(sqlite3_column_double(stmt, 5));
                    spdlog::info("MapServer: NPC '{}' (id={}) at ({},{},{}) for map {}",
                                 name ? name : "?", npc_id, x, y, z, map_id);
                    npc_count++;
                }
                sqlite3_finalize(stmt);
                spdlog::info("MapServer: loaded {} NPCs for map {}", npc_count, map_id);
            }
            sqlite3_close(gamedb);
        } else {
            spdlog::warn("MapServer: no game_data.db, continuing without spawns");
        }
    }

    script_runtime_.Initialize();
    triggers_.LoadForMap(*db_, map_id_);
    quest_->LoadQuestTemplates("assets/data/game_data.db");
    quest_->LoadQuestTemplatesFromDatabase(*db_);
    secondary_.Init(db_.get(), map_id_);

    running_ = true;
    SeedMarketData();
    RegisterWithDistribute();
    spdlog::info("MapServer: map {} initialized on port {}", map_id, port);
    return true;
}

void MapServer::Shutdown() {
    running_ = false;
    script_runtime_.Shutdown();
    grid_.Clear();
    for (auto& [id, dungeon] : dungeons_) {
        CleanupDungeon(id);
    }
    dungeons_.clear();
    network_->Shutdown();
    db_->Shutdown();
}

void MapServer::Update(float dt) {
    network_->Update();

    spawn_sys_->Update(*registry_, dt);
    BroadcastNewMonsterSpawns();
    movement_->Update(*registry_, dt);
    ai_->Update(*registry_, dt);
    combat_->Update(*registry_, dt);
    skill_sys_.UpdateCooldowns(*registry_, dt);
    quest_->Update(*registry_, dt);
    BroadcastMonsterMovement(dt);

    if (player_joined_ && registry_->valid(player_entity_)) {
        triggers_.Update(this, player_entity_, connected_player_.pos_x, connected_player_.pos_z);
        int hp = registry_->get<CharacterStats>(player_entity_).hp;
        if (hp != last_sent_hp_) {
            last_sent_hp_ = hp;
            connected_player_.hp = hp;
            SendCharLifeUpdate();
        }
    }

    // Check encounter triggers for all moving players
    {
        auto view = registry_->view<PlayerData>();
        for (auto entity : view) {
            auto& pd = view.get<PlayerData>(entity);
            if (CheckEncounterTrigger(pd.pos_x, pd.pos_z)) {
                spdlog::debug("MapServer: encounter check for player {} ({}, {})",
                              pd.id, pd.pos_x, pd.pos_z);
            }
        }
    }

    // Update dungeon instances
    std::vector<uint32_t> to_cleanup;
    for (auto& [id, dungeon] : dungeons_) {
        if (dungeon.state == DungeonState::WAITING) {
            dungeon.state = DungeonState::ACTIVE;
        }

        if (dungeon.state == DungeonState::ACTIVE ||
            dungeon.state == DungeonState::BOSS_ACTIVE) {
            dungeon.elapsed_time += dt;

            if (dungeon.elapsed_time >= dungeon.time_limit) {
                dungeon.state = DungeonState::TIMEOUT;
                spdlog::info("MapServer: dungeon {} timed out", id);
                to_cleanup.push_back(id);
            }
        }

        if (dungeon.state == DungeonState::BOSS_ACTIVE && dungeon.boss_defeated) {
            dungeon.state = DungeonState::COMPLETED;
            DistributeDungeonRewards(id);
            to_cleanup.push_back(id);
        }
    }

    for (auto id : to_cleanup) {
        CleanupDungeon(id);
    }

    // Respawn timer loop every 30 seconds
    respawn_timer_ += dt;
    if (respawn_timer_ >= 30.0f) {
        respawn_timer_ = 0.0f;
        spawn_sys_->SpawnMonstersForMap(*registry_, map_id_);
        spdlog::debug("MapServer: respawn check for map {}", map_id_);
    }

    // Auto-save player state every 60 seconds
    auto_save_timer_ += dt;
    if (auto_save_timer_ >= 60.0f) {
        auto_save_timer_ = 0.0f;
        auto view = registry_->view<PlayerData>();
        for (auto entity : view) {
            auto& pd = view.get<PlayerData>(entity);
            SavePlayerQuests(pd.id);
            std::string sql = "INSERT OR REPLACE INTO player_saves "
                "(entity_id, account_id, name, level, class_id, map_id, pos_x, pos_y, pos_z, hp, max_hp) VALUES ("
                + std::to_string(pd.id) + ","
                + std::to_string(pd.id) + ",'"
                + pd.name + "',"
                + std::to_string(pd.level) + ","
                + std::to_string(pd.class_id) + ","
                + std::to_string(map_id_) + ","
                + std::to_string(pd.pos_x) + ","
                + std::to_string(pd.pos_y) + ","
                + std::to_string(pd.pos_z) + ","
                + std::to_string(pd.hp) + ","
                + std::to_string(pd.max_hp) + ")";
            db_->Execute(sql);
        }
        spdlog::debug("MapServer: auto-saved player states");
    }
}

void MapServer::SpawnPlayer(int entity_id, const PlayerData& data) {
    if (registry_->valid(player_entity_)) {
        registry_->destroy(player_entity_);
    }
    player_entity_ = registry_->create();
    registry_->emplace<PlayerData>(player_entity_, data);
    auto& xform = registry_->emplace<Transform>(player_entity_);
    xform.position = glm::vec3(data.pos_x, data.pos_y, data.pos_z);
    auto& stats = registry_->emplace<CharacterStats>(player_entity_);
    stats.level = static_cast<uint16_t>(std::max(1, data.level));
    stats.max_hp = data.max_hp > 0 ? data.max_hp : 500;
    stats.hp = data.hp > 0 ? data.hp : stats.max_hp;
    stats.mp = 100;
    stats.max_mp = 100;
    stats.physic_attack = 20.0f + data.level * 3.0f;
    stats.physic_defense = 5.0f + data.level;
    registry_->emplace<TagPlayer>(player_entity_);
    auto& book = registry_->emplace<SkillBook>(player_entity_);
    book.LearnSkill(1);
    book.LearnSkill(10);
    book.LearnSkill(11);
    book.hotbar[0] = 1;
    book.hotbar[1] = 10;
    book.hotbar[2] = 11;
    registry_->emplace<QuestLog>(player_entity_);
    grid_.Insert(static_cast<uint32_t>(entity_id), data.pos_x, data.pos_z);
    SendNPCList(entity_id);
    spdlog::info("MapServer: player {} spawned, NPC list sent", data.name);
}

void MapServer::DespawnPlayer(int entity_id) {
    auto view = registry_->view<PlayerData>();
    for (auto entity : view) {
        auto& pd = view.get<PlayerData>(entity);
        if (pd.id == entity_id) {
            SavePlayerQuests(entity_id);
            SavePlayerPosition(entity_id, pd.pos_x, pd.pos_y, pd.pos_z);
            registry_->destroy(entity);
            spdlog::info("MapServer: player {} despawned, position saved", entity_id);
            return;
        }
    }
}

void MapServer::SpawnMonster(int template_id, const glm::vec3& pos) {
    auto entity = registry_->create();
    spdlog::info("MapServer: monster {} spawned at ({},{},{})", template_id, pos.x, pos.y, pos.z);
}

void MapServer::DespawnMonster(int entity_id) {
    auto view = registry_->view<entt::entity>();
    for (auto entity : view) {
        if (static_cast<int>(entity) == entity_id) {
            registry_->destroy(entity);
            return;
        }
    }
}

void MapServer::SendNPCList(int player_entity_id) {
    sqlite3* gamedb = nullptr;
    if (sqlite3_open("assets/data/game_data.db", &gamedb) != SQLITE_OK) return;
    const char* npc_sql = "SELECT col_0000, col_0001, col_0003, col_0004, col_0005, col_0006 FROM game_npc WHERE col_0002=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(gamedb, npc_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, map_id_);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int npc_id = sqlite3_column_int(stmt, 0);
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            float x = static_cast<float>(sqlite3_column_double(stmt, 3));
            float y = static_cast<float>(sqlite3_column_double(stmt, 4));
            float z = static_cast<float>(sqlite3_column_double(stmt, 5));
            spdlog::info("MapServer: sending NPC '{}' (id={}) to player {} at ({},{},{})",
                         name ? name : "?", npc_id, player_entity_id, x, y, z);
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(gamedb);
}

bool MapServer::CheckEncounterTrigger(float player_x, float player_z) {
    sqlite3* gamedb = nullptr;
    if (sqlite3_open("assets/data/game_data.db", &gamedb) != SQLITE_OK) return false;
    const char* spawn_sql = "SELECT col_0000, col_0001, col_0002, col_0003, col_0004, col_0005 FROM game_monsterlist WHERE col_0002=?";
    sqlite3_stmt* stmt = nullptr;
    bool triggered = false;
    if (sqlite3_prepare_v2(gamedb, spawn_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, map_id_);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            float radius = static_cast<float>(sqlite3_column_double(stmt, 5));
            float dist = std::sqrt(player_x * player_x + player_z * player_z);
            if (dist < radius) {
                int monster_id = sqlite3_column_int(stmt, 1);
                spdlog::info("MapServer: encounter triggered (monster {} within radius {})",
                             monster_id, radius);
                triggered = true;
                break;
            }
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(gamedb);
    return triggered;
}

void MapServer::SavePlayerPosition(int entity_id, float x, float y, float z) {
    auto view = registry_->view<PlayerData>();
    for (auto entity : view) {
        auto& pd = view.get<PlayerData>(entity);
        if (pd.id == entity_id) {
            pd.pos_x = x;
            pd.pos_y = y;
            pd.pos_z = z;
            std::string sql = "INSERT OR REPLACE INTO player_saves "
                "(entity_id, account_id, name, level, class_id, map_id, pos_x, pos_y, pos_z, hp, max_hp) VALUES ("
                + std::to_string(pd.id) + ","
                + std::to_string(pd.id) + ",'"
                + pd.name + "',"
                + std::to_string(pd.level) + ","
                + std::to_string(pd.class_id) + ","
                + std::to_string(map_id_) + ","
                + std::to_string(x) + ","
                + std::to_string(y) + ","
                + std::to_string(z) + ","
                + std::to_string(pd.hp) + ","
                + std::to_string(pd.max_hp) + ")";
            db_->Execute(sql);
            spdlog::info("MapServer: saved position for player {} at ({},{},{})",
                         entity_id, x, y, z);
            return;
        }
    }
}

// ─── Dungeon Instance Management ───────────────────────────────────────────────

uint32_t MapServer::CreateDungeonInstance(uint32_t template_id,
                                          const std::vector<uint32_t>& party_ids) {
    DungeonInstance inst;
    inst.instance_id = next_dungeon_id_++;
    inst.dungeon_template_id = template_id;
    inst.state = DungeonState::WAITING;
    inst.party_member_ids = party_ids;
    inst.boss_defeated = false;
    inst.elapsed_time = 0.0f;
    inst.time_limit = 1800.0f;

    DungeonRewardEntry sample_reward;
    sample_reward.item_id = 10001;
    sample_reward.count = 1;
    sample_reward.drop_chance = 0.5f;
    inst.reward_table.push_back(sample_reward);

    sample_reward.item_id = 10002;
    sample_reward.count = 2;
    sample_reward.drop_chance = 0.3f;
    inst.reward_table.push_back(sample_reward);

    uint32_t id = inst.instance_id;
    dungeons_[id] = inst;

    spdlog::info("MapServer: dungeon instance {} created (template={})",
                 id, template_id);
    return id;
}

bool MapServer::EnterDungeon(uint32_t instance_id, entt::entity player) {
    auto it = dungeons_.find(instance_id);
    if (it == dungeons_.end()) return false;
    if (it->second.state != DungeonState::WAITING &&
        it->second.state != DungeonState::ACTIVE) return false;

    spdlog::info("MapServer: player {} enters dungeon {}",
                 static_cast<uint32_t>(player), instance_id);
    return true;
}

bool MapServer::TriggerBossEncounter(uint32_t instance_id, uint32_t boss_template_id) {
    auto it = dungeons_.find(instance_id);
    if (it == dungeons_.end()) return false;

    auto& dungeon = it->second;
    dungeon.state = DungeonState::BOSS_ACTIVE;

    glm::vec3 boss_pos(50.0f, 0.0f, 50.0f);
    auto boss_entity = registry_->create();
    registry_->emplace<TagMonster>(boss_entity);
    dungeon.boss_entity_id = static_cast<uint32_t>(boss_entity);
    dungeon.boss_defeated = false;

    spdlog::info("MapServer: boss encounter triggered in dungeon {} (boss template {})",
                 instance_id, boss_template_id);
    return true;
}

void MapServer::CompleteDungeon(uint32_t instance_id) {
    auto it = dungeons_.find(instance_id);
    if (it == dungeons_.end()) return;

    auto& dungeon = it->second;
    if (!dungeon.boss_defeated) {
        auto boss = static_cast<entt::entity>(dungeon.boss_entity_id);
        if (registry_->valid(boss)) {
            registry_->destroy(boss);
        }
        dungeon.boss_defeated = true;
    }
    dungeon.state = DungeonState::COMPLETED;
    DistributeDungeonRewards(instance_id);

    spdlog::info("MapServer: dungeon {} completed", instance_id);
}

void MapServer::CleanupDungeon(uint32_t instance_id) {
    auto it = dungeons_.find(instance_id);
    if (it == dungeons_.end()) return;

    auto& dungeon = it->second;
    if (dungeon.boss_entity_id != 0) {
        auto boss = static_cast<entt::entity>(dungeon.boss_entity_id);
        if (registry_->valid(boss)) {
            registry_->destroy(boss);
        }
    }

    if (dungeon.dungeon_root != entt::null && registry_->valid(dungeon.dungeon_root)) {
        registry_->destroy(dungeon.dungeon_root);
    }

    db_->Execute(
        "INSERT INTO dungeon_instances (instance_id, template_id, state, created_at, completed_at) VALUES ("
        + std::to_string(dungeon.instance_id) + ","
        + std::to_string(dungeon.dungeon_template_id) + ","
        + std::to_string(static_cast<int>(dungeon.state)) + ","
        + std::to_string(static_cast<int>(dungeon.elapsed_time)) + ","
        + std::to_string(static_cast<int>(std::time(nullptr))) + ")"
    );

    dungeons_.erase(it);
    spdlog::info("MapServer: dungeon instance {} cleaned up", instance_id);
}

DungeonInstance* MapServer::GetDungeonInstance(uint32_t instance_id) {
    auto it = dungeons_.find(instance_id);
    return it != dungeons_.end() ? &it->second : nullptr;
}

void MapServer::DistributeDungeonRewards(uint32_t instance_id) {
    auto it = dungeons_.find(instance_id);
    if (it == dungeons_.end()) return;

    auto& dungeon = it->second;
    for (auto member_id : dungeon.party_member_ids) {
        auto player = static_cast<entt::entity>(member_id);
        if (!registry_->valid(player) || !registry_->all_of<Inventory>(player)) continue;

        auto& inv = registry_->get<Inventory>(player);
        for (auto& reward : dungeon.reward_table) {
            float roll = static_cast<float>(s_dungeon_rng() % 1000) / 1000.0f;
            if (roll <= reward.drop_chance) {
                inv.AddItem(reward.item_id, reward.count);
                spdlog::info("MapServer: dungeon reward {}x{} -> player {}",
                             reward.item_id, reward.count, member_id);
            }
        }

        if (registry_->all_of<CharacterStats>(player)) {
            auto& stats = registry_->get<CharacterStats>(player);
            uint32_t bonus_exp = static_cast<uint32_t>(100 + dungeon.elapsed_time * 0.5f);
            stats.exp += bonus_exp;
        }
    }
}

CombatSystem& MapServer::GetCombatSystem() { return *combat_; }
AISystem& MapServer::GetAISystem() { return *ai_; }
MovementSystem& MapServer::GetMovementSystem() { return *movement_; }
ItemSystem& MapServer::GetItemSystem() { return *item_; }
QuestSystem& MapServer::GetQuestSystem() { return *quest_; }
Database& MapServer::GetDatabase() { return *db_; }
NetworkLayer& MapServer::GetNetworkLayer() { return *network_; }

void MapServer::SendEntitySpawn(uint32_t entity_id, int8_t entity_type,
                                const std::string& model_id, const std::string& name,
                                uint16_t level, float x, float y, float z, float hp_pct) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    Vec3 pos{x, y, z};
    auto spawn = CreateEntitySpawnDirect(fbb, entity_id, static_cast<EntityType>(entity_type),
        model_id.c_str(), &pos, 0.0f, 1.0f, name.c_str(), level, hp_pct);
    fbb.Finish(spawn);
    network_->SendPacket(PacketType_MP_ENTITY_SPAWN, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::SendEntityTransform(uint32_t entity_id, float x, float y, float z, const char* anim) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    Vec3 pos{x, y, z};
    auto trans = CreateEntityTransformDirect(fbb, entity_id, &pos, 0.0f, anim, 1.0f, 0);
    fbb.Finish(trans);
    network_->SendPacket(PacketType_MP_ENTITY_TRANSFORM, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::BroadcastMonsterMovement(float dt) {
    if (!player_joined_) return;

    auto view = registry_->view<TagMonster, Transform>();
    for (auto entity : view) {
        uint32_t eid = static_cast<uint32_t>(entt::to_entity(entity));
        if (!visible_monsters_.count(eid)) continue;
        auto& xform = view.get<Transform>(entity);
        grid_.Update(eid, xform.position.x, xform.position.z);
        auto& net = monster_net_[eid];

        if (!net.initialized) {
            net.last_sent = xform.position;
            net.initialized = true;
            SendEntityTransform(eid, xform.position.x, xform.position.y, xform.position.z, "idle");
            continue;
        }

        net.broadcast_timer += dt;
        float moved = glm::distance(xform.position, net.last_sent);
        if (net.broadcast_timer < monster_broadcast_interval_ && moved < 0.08f)
            continue;

        net.broadcast_timer = 0.0f;
        net.last_sent = xform.position;
        const char* anim = moved > 0.08f ? "walk" : "idle";
        SendEntityTransform(eid, xform.position.x, xform.position.y, xform.position.z, anim);
    }
    UpdatePlayerVisibility();
}

void MapServer::GrantLootToPlayer(uint32_t item_id, uint16_t count) {
    using namespace luna::protocol;
    bool merged = false;
    for (auto& slot : player_inventory_) {
        if (slot.item_id == item_id) {
            slot.count = static_cast<uint16_t>(slot.count + count);
            merged = true;
            break;
        }
    }
    if (!merged) {
        player_inventory_.push_back({next_loot_slot_++, item_id, count});
    }
    flatbuffers::FlatBufferBuilder fbb;
    auto upd = CreateInventoryUpdate(fbb, next_loot_slot_ - 1, item_id, count);
    fbb.Finish(upd);
    network_->SendPacket(PacketType_MP_INVENTORY_UPDATE, fbb.GetBufferPointer(), fbb.GetSize());
    ApplyQuestProgress(QuestObjective::CollectItem, item_id, count);
    spdlog::info("MapServer: loot item {} x{} -> player {}", item_id, count, connected_player_.id);
}

void MapServer::SendCharLifeUpdate() {
    if (!registry_->valid(player_entity_)) return;
    using namespace luna::protocol;
    auto& stats = registry_->get<CharacterStats>(player_entity_);
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateCharLifeUpdate(fbb,
        static_cast<uint32_t>(connected_player_.id),
        stats.hp, stats.max_hp, stats.mp, stats.max_mp,
        player_gold_, player_exp_);
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_CHAR_LIFE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::SendInventorySync() {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<InventorySlot>> slots;
    for (const auto& s : player_inventory_) {
        InventorySlotBuilder sb(fbb);
        sb.add_slot_index(s.slot);
        sb.add_item_id(s.item_id);
        sb.add_count(s.count);
        sb.add_enchant(0);
        slots.push_back(sb.Finish());
    }
    auto inv = CreateInventoryData(fbb, player_gold_, fbb.CreateVector(slots));
    fbb.Finish(inv);
    network_->SendPacket(PacketType_MP_ITEM_STORAGEITEM_INFO,
        fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::BroadcastNewMonsterSpawns() {
    if (!player_joined_) return;
    auto view = registry_->view<TagMonster, Transform>();
    for (auto entity : view) {
        uint32_t eid = static_cast<uint32_t>(entt::to_entity(entity));
        auto& xform = view.get<Transform>(entity);
        grid_.Insert(eid, xform.position.x, xform.position.z);
    }
    UpdatePlayerVisibility();
}

void MapServer::SendWorldSnapshot() {
    if (!player_joined_) return;
    auto monsters = registry_->view<TagMonster, Transform>();
    for (auto entity : monsters) {
        uint32_t eid = static_cast<uint32_t>(entt::to_entity(entity));
        auto& xform = monsters.get<Transform>(entity);
        grid_.Insert(eid, xform.position.x, xform.position.z);
    }
    visible_monsters_.clear();
    monster_net_.clear();
    UpdatePlayerVisibility();
    spdlog::info("MapServer: sent AOI world snapshot to player {}", connected_player_.name);
}

void MapServer::HandlePacket(uint16_t type, const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;

    if (type == PacketType_MP_USERCONN_GAMEIN_SYN) {
        auto req = flatbuffers::GetRoot<EnterWorldRequest>(payload);
        connected_player_.id = static_cast<int>(req->character_id());
        connected_player_.name = "Player";
        connected_player_.level = 1;
        connected_player_.class_id = 0;
        connected_player_.pos_x = 0;
        connected_player_.pos_y = 0;
        connected_player_.pos_z = 0;
        connected_player_.hp = 500;
        connected_player_.max_hp = 500;
        player_joined_ = true;
        player_inventory_ = {{0, 1001, 1}, {1, 21000001, 5}};
        player_storage_.clear();
        storage_gold_ = 0;
        player_gold_ = 100;
        player_exp_ = 0;
        next_loot_slot_ = 10;
        last_sent_hp_ = -1;
        monster_net_.clear();
        visible_monsters_.clear();
        has_party_ = false;
        party_.members.clear();
        party_.party_id = 0;
        party_.leader_id = 0;
        friends_.clear();
        has_guild_ = false;
        guild_.members.clear();
        guild_.guild_id = 0;
        guild_.name.clear();
        guild_.master_id = 0;
        trade_ = {};
        SpawnPlayer(connected_player_.id, connected_player_);
        LoadPlayerQuests(connected_player_.id);
        SendWorldSnapshot();
        SendInventorySync();
        SendCharLifeUpdate();
        Vec3 pos{connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z};
        flatbuffers::FlatBufferBuilder fbb;
        auto ack = CreateEnterWorldResponse(fbb, 0,
            static_cast<uint16_t>(map_id_), &pos,
            static_cast<uint32_t>(connected_player_.id));
        fbb.Finish(ack);
        network_->SendPacket(PacketType_MP_USERCONN_GAMEIN_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        SendQuestList(0);
        return;
    }

    if (type == PacketType_MP_USERCONN_CHANGEMAP_SYN) {
        HandleChangeMap(payload, len);
        return;
    }

    if (!player_joined_) return;

    if (type == PacketType_MP_MOVE_WALK || type == PacketType_MP_MOVE_RUN) {
        auto req = flatbuffers::GetRoot<MoveRequest>(payload);
        if (req->target_position()) {
            connected_player_.pos_x = req->target_position()->x();
            connected_player_.pos_y = req->target_position()->y();
            connected_player_.pos_z = req->target_position()->z();
            if (registry_->valid(player_entity_)) {
                auto& px = registry_->get<Transform>(player_entity_);
                px.position = glm::vec3(connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z);
                grid_.Update(static_cast<uint32_t>(connected_player_.id), connected_player_.pos_x, connected_player_.pos_z);
                UpdatePlayerVisibility();
            }
            SavePlayerPosition(connected_player_.id,
                connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z);
            SendEntityTransform(static_cast<uint32_t>(connected_player_.id),
                connected_player_.pos_x, connected_player_.pos_y, connected_player_.pos_z);
        }
        return;
    }

    if (type == PacketType_MP_COMBAT_ATTACK_SYN || type == PacketType_MP_SKILL_CAST_SYN) {
        HandleCombatAttack(
            type == PacketType_MP_SKILL_CAST_SYN ? PacketType_MP_SKILL_CAST_ACK : PacketType_MP_COMBAT_ATTACK_ACK,
            payload, len);
        return;
    }

    if (type == PacketType_MP_CHAT_ALL_SYN) {
        HandleChat(payload, len);
        return;
    }

    if (type == PacketType_MP_CHAT_PARTY_SYN) {
        HandlePartyChat(payload, len);
        return;
    }

    if (type == PacketType_MP_CHAT_WHISPER_SYN) {
        HandleWhisper(payload, len);
        return;
    }

    if (type == PacketType_MP_PARTY_CREATE_SYN) {
        HandlePartyCreate(payload, len);
        return;
    }

    if (type == PacketType_MP_PARTY_INVITE_SYN) {
        HandlePartyInvite(payload, len);
        return;
    }

    if (type == PacketType_MP_PARTY_LEAVE_SYN) {
        HandlePartyLeave(payload, len);
        return;
    }

    if (type == PacketType_MP_STORAGE_LIST_SYN) {
        HandleStorageList(payload, len);
        return;
    }

    if (type == PacketType_MP_STORAGE_DEPOSIT_SYN) {
        HandleStorageDeposit(payload, len);
        return;
    }

    if (type == PacketType_MP_STORAGE_WITHDRAW_SYN) {
        HandleStorageWithdraw(payload, len);
        return;
    }

    if (type == PacketType_MP_FRIEND_LIST_SYN) {
        HandleFriendList(payload, len);
        return;
    }

    if (type == PacketType_MP_FRIEND_ADD_SYN) {
        HandleFriendAdd(payload, len);
        return;
    }

    if (type == PacketType_MP_FRIEND_DEL_SYN) {
        HandleFriendDelete(payload, len);
        return;
    }

    if (type == PacketType_MP_GUILD_CREATE_SYN) {
        HandleGuildCreate(payload, len);
        return;
    }

    if (type == PacketType_MP_GUILD_INFO_SYN) {
        HandleGuildInfo(payload, len);
        return;
    }

    if (type == PacketType_MP_GUILD_ADDMEMBER_SYN) {
        HandleGuildInvite(payload, len);
        return;
    }

    if (type == PacketType_MP_GUILD_SECEDE_SYN) {
        HandleGuildLeave(payload, len);
        return;
    }

    if (type == PacketType_MP_CHAT_GUILD_SYN) {
        HandleGuildChat(payload, len);
        return;
    }

    if (type == PacketType_MP_EXCHANGE_APPLY_SYN) {
        HandleTradeApply(payload, len);
        return;
    }

    if (type == PacketType_MP_EXCHANGE_CANCEL_SYN) {
        HandleTradeCancel(payload, len);
        return;
    }

    if (type == PacketType_MP_EXCHANGE_ADDITEM_SYN) {
        HandleTradeAddItem(payload, len);
        return;
    }

    if (type == PacketType_MP_EXCHANGE_SETGOLD_SYN) {
        HandleTradeSetGold(payload, len);
        return;
    }

    if (type == PacketType_MP_EXCHANGE_CONFIRM_SYN) {
        HandleTradeConfirm(payload, len);
        return;
    }

    if (type == PacketType_MP_CONSIGNMENT_GETLIST_SYN ||
        type == PacketType_MP_CONSIGNMENT_SEARCH_SYN) {
        HandleConsignmentList(payload, len,
            type == PacketType_MP_CONSIGNMENT_SEARCH_SYN
                ? PacketType_MP_CONSIGNMENT_SEARCH_ACK
                : PacketType_MP_CONSIGNMENT_GETLIST_ACK);
        return;
    }

    if (type == PacketType_MP_CONSIGNMENT_REGIST_SYN) {
        HandleConsignmentRegister(payload, len);
        return;
    }

    if (type == PacketType_MP_CONSIGNMENT_UPDATE_SYN) {
        HandleConsignmentTrade(payload, len);
        return;
    }

    if (type == PacketType_MP_CONSIGNMENT_CANCEL_SYN) {
        HandleConsignmentCancel(payload, len);
        return;
    }

    if (type == PacketType_MP_STREETSTALL_OPEN_SYN) {
        HandleStreetStallOpen(payload, len);
        return;
    }

    if (type == PacketType_MP_STREETSTALL_ADDITEM_SYN) {
        HandleStreetStallAddItem(payload, len);
        return;
    }

    if (type == PacketType_MP_STREETSTALL_BUY_SYN) {
        HandleStreetStallBuy(payload, len);
        return;
    }

    if (type == PacketType_MP_STREETSTALL_CLOSE_SYN) {
        HandleStreetStallClose(payload, len);
        return;
    }

    if (type == PacketType_MP_STREETSTALL_LIST_SYN) {
        HandleStreetStallList(payload, len);
        return;
    }

    if (type == PacketType_MP_QUEST_START_SYN) {
        HandleQuestStart(payload, len);
        return;
    }
    if (type == PacketType_MP_QUEST_END_SYN) {
        HandleQuestEnd(payload, len);
        return;
    }
    if (type == PacketType_MP_QUEST_LIST_SYN) {
        HandleQuestList(payload, len);
        return;
    }
    if (type == PacketType_MP_DUNGEON_ENTRANCE_SYN) {
        HandleDungeonEntrance(payload, len);
        return;
    }
    if (type == PacketType_MP_DUNGEON_INFO_SYN) {
        HandleDungeonInfo(payload, len);
        return;
    }

    if (type >= PacketType_MP_FAMILY_INFO_SYN && type <= PacketType_MP_FAMILY_ACCEPT_ACK) {
        MapPlayerContext ctx;
        ctx.character_id = static_cast<uint32_t>(connected_player_.id);
        ctx.name = connected_player_.name;
        ctx.guild_id = has_guild_ ? guild_.guild_id : 0;
        ctx.guild_name = guild_.name;
        ctx.gold = &player_gold_;
        ctx.grant_loot = [this](uint32_t item_id, uint16_t count) {
            GrantLootToPlayer(item_id, count);
        };
        secondary_.HandlePacket(network_.get(), ctx, type, payload, len);
        return;
    }
    if (type >= PacketType_MP_PET_INFO_SYN && type <= PacketType_MP_PET_ACTION_NACK) {
        MapPlayerContext ctx;
        ctx.character_id = static_cast<uint32_t>(connected_player_.id);
        ctx.name = connected_player_.name;
        ctx.gold = &player_gold_;
        ctx.grant_loot = [this](uint32_t item_id, uint16_t count) {
            GrantLootToPlayer(item_id, count);
        };
        secondary_.HandlePacket(network_.get(), ctx, type, payload, len);
        return;
    }
    if (type >= PacketType_MP_FISHING_CAST_SYN && type <= PacketType_MP_FISHING_CAST_NACK) {
        MapPlayerContext ctx;
        ctx.character_id = static_cast<uint32_t>(connected_player_.id);
        ctx.grant_loot = [this](uint32_t item_id, uint16_t count) {
            GrantLootToPlayer(item_id, count);
        };
        secondary_.HandlePacket(network_.get(), ctx, type, payload, len);
        return;
    }
    if (type >= PacketType_MP_SIEGE_INFO_SYN && type <= PacketType_MP_CASHSHOP_BUY_ACK) {
        MapPlayerContext ctx;
        ctx.character_id = static_cast<uint32_t>(connected_player_.id);
        ctx.name = connected_player_.name;
        ctx.guild_id = has_guild_ ? guild_.guild_id : 0;
        ctx.guild_name = guild_.name;
        ctx.gold = &player_gold_;
        ctx.grant_loot = [this](uint32_t item_id, uint16_t count) {
            GrantLootToPlayer(item_id, count);
        };
        secondary_.HandlePacket(network_.get(), ctx, type, payload, len);
        return;
    }
}

void MapServer::SendEntityDespawn(uint32_t entity_id, int8_t reason) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateEntityDespawn(fbb, entity_id, static_cast<DespawnReason>(reason));
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_ENTITY_DESPAWN, fbb.GetBufferPointer(), fbb.GetSize());
}

entt::entity MapServer::FindMonsterEntity(uint32_t entity_id) const {
    entt::entity e = static_cast<entt::entity>(entity_id);
    if (registry_->valid(e) && registry_->all_of<TagMonster, CharacterStats>(e))
        return e;
    auto view = registry_->view<TagMonster, CharacterStats>();
    for (auto entity : view) {
        if (static_cast<uint32_t>(entt::to_entity(entity)) == entity_id)
            return entity;
    }
    return entt::null;
}

void MapServer::HandleCombatAttack(uint16_t ack_type, const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!registry_->valid(player_entity_)) return;

    auto req = flatbuffers::GetRoot<AttackRequest>(payload);
    uint32_t target_id = req->target_id();
    uint16_t skill_id = req->skill_id();

    entt::entity target = FindMonsterEntity(target_id);
    if (target == entt::null) {
        auto view = registry_->view<TagMonster, Transform, CharacterStats>();
        float best = 9999.0f;
        for (auto entity : view) {
            auto& xform = view.get<Transform>(entity);
            float dx = xform.position.x - connected_player_.pos_x;
            float dz = xform.position.z - connected_player_.pos_z;
            float d = dx * dx + dz * dz;
            if (d < best) { best = d; target = entity; target_id = static_cast<uint32_t>(entt::to_entity(entity)); }
        }
        if (best > 144.0f) return;
    }

    auto& def = registry_->get<CharacterStats>(target);
    if (skill_id > 0) {
        if (!skill_sys_.CanUseSkill(*registry_, player_entity_, skill_id)) {
            flatbuffers::FlatBufferBuilder fbb;
            auto result = CreateAttackResult(fbb,
                static_cast<uint32_t>(connected_player_.id), target_id, 0,
                DamageType_Skill, false, true, def.hp, 0);
            fbb.Finish(result);
            network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
            return;
        }
    }

    if (auto* ai = registry_->try_get<AIComponent>(target)) {
        uint32_t pid = static_cast<uint32_t>(entt::to_entity(player_entity_));
        ai->aggro_target = pid;
        ai->AddThreat(pid, 50);
        ai->state = AIComponent::Chase;
        ai->state_timer = 0.0f;
    }

    int32_t hp_before = def.hp;
    if (skill_id > 0) {
        skill_sys_.UseSkill(*registry_, player_entity_, target, skill_id);
    } else {
        combat_->HandleAttack(*registry_, player_entity_, target, skill_id);
    }
    int32_t damage_dealt = std::max(0, hp_before - def.hp);

    auto& xform = registry_->get<Transform>(target);
    flatbuffers::FlatBufferBuilder fbb;
    auto result = CreateAttackResult(fbb,
        static_cast<uint32_t>(connected_player_.id),
        target_id,
        damage_dealt > 0 ? damage_dealt : 1,
        skill_id > 0 ? DamageType_Skill : DamageType_Normal,
        false, damage_dealt == 0,
        def.hp,
        0);
    fbb.Finish(result);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());

    if (def.hp <= 0) {
        monster_net_.erase(target_id);
        uint32_t gold_gain = 10 + def.level * 5;
        uint64_t exp_gain = 50 + def.level * 10;
        player_gold_ += gold_gain;
        player_exp_ += exp_gain;
        GrantLootToPlayer(21000001, static_cast<uint16_t>(1 + (target_id % 3)));
        if ((target_id % 4) == 0)
            GrantLootToPlayer(1001, 1);
        SendCharLifeUpdate();
        SendEntityDespawn(target_id, static_cast<int8_t>(DespawnReason_Death));
        if (registry_->all_of<SpawnInfo>(target)) {
            auto& spawn = registry_->get<SpawnInfo>(target);
            OnMonsterKilled(spawn.monster_id);
            spawn_sys_->RespawnMonster(*registry_, spawn.spawn_rule_id, spawn.respawn_time);
        } else {
            OnMonsterKilled(target_id);
        }
        grid_.Remove(target_id);
        registry_->destroy(target);
        spdlog::info("MapServer: monster {} defeated by {}", target_id, connected_player_.id);
    } else {
        SendEntityTransform(target_id, xform.position.x, xform.position.y, xform.position.z, "hit");
    }
}

void MapServer::SendPartyInfo(uint8_t result, uint16_t ack_type) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<PartyMemberInfo>> members;
    if (has_party_) {
        for (const auto& m : party_.members) {
            members.push_back(CreatePartyMemberInfoDirect(fbb, m.character_id,
                m.name.c_str(), m.level, m.hp, m.max_hp, m.map_id, m.is_leader));
        }
    }
    auto party = CreatePartyInfo(fbb, has_party_ ? party_.party_id : 0,
        has_party_ ? party_.leader_id : 0, 0, fbb.CreateVector(members));
    auto resp = CreatePartyResponse(fbb, result, party);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandlePartyCreate(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<PartyCreateRequest>(payload);
    if (req->character_id() != static_cast<uint32_t>(connected_player_.id)) return;
    if (has_party_) {
        SendPartyInfo(1, PacketType_MP_PARTY_CREATE_NACK);
        return;
    }
    has_party_ = true;
    party_.party_id = next_party_id_++;
    party_.leader_id = static_cast<uint32_t>(connected_player_.id);
    party_.members.clear();
    PartyMemberState leader;
    leader.character_id = party_.leader_id;
    leader.name = connected_player_.name;
    leader.level = static_cast<uint16_t>(std::max(1, connected_player_.level));
    leader.hp = connected_player_.hp;
    leader.max_hp = connected_player_.max_hp;
    leader.map_id = static_cast<uint16_t>(map_id_);
    leader.is_leader = true;
    party_.members.push_back(leader);
    SendPartyInfo(0, PacketType_MP_PARTY_CREATE_ACK);
    spdlog::info("MapServer: party {} created by {}", party_.party_id, connected_player_.id);
}

void MapServer::HandlePartyInvite(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_party_) {
        SendPartyInfo(1, PacketType_MP_PARTY_INVITE_NACK);
        return;
    }
    if (party_.leader_id != static_cast<uint32_t>(connected_player_.id)) {
        SendPartyInfo(2, PacketType_MP_PARTY_INVITE_NACK);
        return;
    }
    auto req = flatbuffers::GetRoot<PartyInviteRequest>(payload);
    std::string target = req->target_name() ? req->target_name()->str() : "";
    if (target.empty() || party_.members.size() >= 6) {
        SendPartyInfo(3, PacketType_MP_PARTY_INVITE_NACK);
        return;
    }
    for (const auto& m : party_.members) {
        if (m.name == target) {
            SendPartyInfo(4, PacketType_MP_PARTY_INVITE_NACK);
            return;
        }
    }
    PartyMemberState member;
    member.character_id = 9000 + static_cast<uint32_t>(party_.members.size());
    member.name = target;
    member.level = 10;
    member.hp = 400;
    member.max_hp = 400;
    member.map_id = static_cast<uint16_t>(map_id_);
    member.is_leader = false;
    party_.members.push_back(member);
    SendPartyInfo(0, PacketType_MP_PARTY_INVITE_ACK);
    SendPartyInfo(0, PacketType_MP_PARTY_INFO_UPDATE);
    spdlog::info("MapServer: {} invited to party {}", target, party_.party_id);
}

void MapServer::HandlePartyLeave(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_party_) return;
    auto req = flatbuffers::GetRoot<PartyLeaveRequest>(payload);
    if (req->party_id() != party_.party_id) return;

    if (party_.leader_id == static_cast<uint32_t>(connected_player_.id)) {
        has_party_ = false;
        party_.members.clear();
        party_.party_id = 0;
        party_.leader_id = 0;
    } else {
        party_.members.erase(
            std::remove_if(party_.members.begin(), party_.members.end(),
                [&](const PartyMemberState& m) {
                    return m.character_id == static_cast<uint32_t>(connected_player_.id);
                }),
            party_.members.end());
        if (party_.members.size() <= 1) {
            has_party_ = false;
            party_.members.clear();
            party_.party_id = 0;
            party_.leader_id = 0;
        }
    }
    SendPartyInfo(0, PacketType_MP_PARTY_LEAVE_ACK);
    spdlog::info("MapServer: player {} left party", connected_player_.id);
}

void MapServer::SendStorageSync(uint8_t result, uint16_t ack_type) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<StorageSlot>> slots;
    for (const auto& s : player_storage_) {
        slots.push_back(CreateStorageSlot(fbb, s.slot, s.item_id, s.count));
    }
    auto storage = CreateStorageData(fbb, storage_gold_, fbb.CreateVector(slots));
    auto resp = CreateStorageResponse(fbb, result, storage);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleStorageList(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StorageListRequest>(payload);
    if (req->character_id() != static_cast<uint32_t>(connected_player_.id)) return;
    SendStorageSync(0, PacketType_MP_STORAGE_LIST_ACK);
}

void MapServer::HandleStorageDeposit(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StorageDepositRequest>(payload);
    uint8_t inv_slot = req->inv_slot();
    uint16_t count = req->count() ? req->count() : 1;
    PlayerInvSlot* from = nullptr;
    for (auto& s : player_inventory_) {
        if (s.slot == inv_slot) { from = &s; break; }
    }
    if (!from || from->count < count) {
        SendStorageSync(1, PacketType_MP_STORAGE_DEPOSIT_ACK);
        return;
    }
    bool merged = false;
    for (auto& s : player_storage_) {
        if (s.item_id == from->item_id) {
            s.count = static_cast<uint16_t>(s.count + count);
            merged = true;
            break;
        }
    }
    if (!merged) {
        uint8_t slot = req->storage_slot();
        if (slot == 0) {
            for (uint8_t i = 0; i < 40; ++i) {
                bool used = false;
                for (const auto& s : player_storage_) {
                    if (s.slot == i) { used = true; break; }
                }
                if (!used) { slot = i; break; }
            }
        }
        player_storage_.push_back({slot, from->item_id, count});
    }
    from->count = static_cast<uint16_t>(from->count - count);
    if (from->count == 0) from->item_id = 0;
    SendInventorySync();
    SendStorageSync(0, PacketType_MP_STORAGE_DEPOSIT_ACK);
}

void MapServer::HandleStorageWithdraw(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StorageWithdrawRequest>(payload);
    uint8_t storage_slot = req->storage_slot();
    uint16_t count = req->count() ? req->count() : 1;
    PlayerInvSlot* from = nullptr;
    for (auto& s : player_storage_) {
        if (s.slot == storage_slot) { from = &s; break; }
    }
    if (!from || from->count < count) {
        SendStorageSync(1, PacketType_MP_STORAGE_WITHDRAW_ACK);
        return;
    }
    bool merged = false;
    for (auto& s : player_inventory_) {
        if (s.item_id == from->item_id) {
            s.count = static_cast<uint16_t>(s.count + count);
            merged = true;
            break;
        }
    }
    if (!merged) {
        uint8_t inv_slot = req->inv_slot();
        if (inv_slot == 0) {
            for (uint8_t i = 0; i < 40; ++i) {
                bool used = false;
                for (const auto& s : player_inventory_) {
                    if (s.slot == i) { used = true; break; }
                }
                if (!used) { inv_slot = i; break; }
            }
        }
        player_inventory_.push_back({inv_slot, from->item_id, count});
    }
    from->count = static_cast<uint16_t>(from->count - count);
    if (from->count == 0) {
        player_storage_.erase(
            std::remove_if(player_storage_.begin(), player_storage_.end(),
                [&](const PlayerInvSlot& s) { return s.slot == storage_slot; }),
            player_storage_.end());
    }
    SendInventorySync();
    SendStorageSync(0, PacketType_MP_STORAGE_WITHDRAW_ACK);
}

void MapServer::HandleWhisper(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<ChatMessage>(payload);
    std::string target = req->sender_name() ? req->sender_name()->str() : "";
    std::string text = req->message() ? req->message()->str() : "";
    std::string reply = "To [" + target + "]: " + text;
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateChatMessageDirect(fbb,
        static_cast<uint32_t>(connected_player_.id),
        connected_player_.name.c_str(),
        reply.c_str(),
        ChatChannel_Whisper,
        0);
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_CHAT_WHISPER_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandlePartyChat(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_party_) return;
    auto req = flatbuffers::GetRoot<ChatMessage>(payload);
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateChatMessageDirect(fbb,
        static_cast<uint32_t>(connected_player_.id),
        connected_player_.name.c_str(),
        req->message() ? req->message()->c_str() : "",
        ChatChannel_Party,
        0);
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_CHAT_PARTY_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::SendFriendList(uint8_t result, uint16_t ack_type) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<FriendEntry>> entries;
    for (const auto& f : friends_) {
        entries.push_back(CreateFriendEntryDirect(fbb, f.character_id, f.name.c_str(),
            f.level, f.online, f.map_id));
    }
    auto list = CreateFriendList(fbb, fbb.CreateVector(entries));
    auto resp = CreateFriendResponse(fbb, result, list);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleFriendList(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<FriendListRequest>(payload);
    if (req->character_id() != static_cast<uint32_t>(connected_player_.id)) return;
    SendFriendList(0, PacketType_MP_FRIEND_LIST_ACK);
}

void MapServer::HandleFriendAdd(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<FriendAddRequest>(payload);
    std::string target = req->target_name() ? req->target_name()->str() : "";
    if (target.empty() || friends_.size() >= 50) {
        SendFriendList(1, PacketType_MP_FRIEND_ADD_NACK);
        return;
    }
    for (const auto& f : friends_) {
        if (f.name == target) {
            SendFriendList(2, PacketType_MP_FRIEND_ADD_NACK);
            return;
        }
    }
    FriendState entry;
    entry.character_id = next_friend_id_++;
    entry.name = target;
    entry.level = 10;
    entry.online = true;
    entry.map_id = static_cast<uint16_t>(map_id_);
    friends_.push_back(entry);
    SendFriendList(0, PacketType_MP_FRIEND_ADD_ACK);
    spdlog::info("MapServer: {} added friend {}", connected_player_.name, target);
}

void MapServer::HandleFriendDelete(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<FriendDeleteRequest>(payload);
    uint32_t fid = req->friend_id();
    friends_.erase(
        std::remove_if(friends_.begin(), friends_.end(),
            [fid](const FriendState& f) { return f.character_id == fid; }),
        friends_.end());
    SendFriendList(0, PacketType_MP_FRIEND_DEL_ACK);
}

void MapServer::SendGuildInfo(uint8_t result, uint16_t ack_type) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<GuildMemberInfo>> members;
    if (has_guild_) {
        for (const auto& m : guild_.members) {
            members.push_back(CreateGuildMemberInfoDirect(fbb, m.character_id,
                m.name.c_str(), m.level, m.rank, m.online));
        }
    }
    auto guild = CreateGuildInfo(fbb,
        has_guild_ ? guild_.guild_id : 0,
        has_guild_ ? fbb.CreateString(guild_.name) : 0,
        has_guild_ ? guild_.level : 0,
        has_guild_ ? guild_.gp : 0,
        has_guild_ ? guild_.master_id : 0,
        fbb.CreateVector(members));
    auto resp = CreateGuildResponse(fbb, result, guild);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleGuildCreate(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<GuildCreateRequest>(payload);
    if (req->character_id() != static_cast<uint32_t>(connected_player_.id)) return;
    std::string gname = req->guild_name() ? req->guild_name()->str() : "";
    if (gname.empty() || has_guild_) {
        SendGuildInfo(1, PacketType_MP_GUILD_CREATE_NACK);
        return;
    }
    has_guild_ = true;
    guild_.guild_id = next_guild_id_++;
    guild_.name = gname;
    guild_.level = 1;
    guild_.gp = 0;
    guild_.master_id = static_cast<uint32_t>(connected_player_.id);
    guild_.members.clear();
    GuildMemberState master;
    master.character_id = guild_.master_id;
    master.name = connected_player_.name;
    master.level = static_cast<uint16_t>(std::max(1, connected_player_.level));
    master.rank = 2;
    master.online = true;
    guild_.members.push_back(master);
    SendGuildInfo(0, PacketType_MP_GUILD_CREATE_ACK);
    SendGuildInfo(0, PacketType_MP_GUILD_INFO);
    spdlog::info("MapServer: guild '{}' created by {}", gname, connected_player_.name);
}

void MapServer::HandleGuildInfo(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<GuildInfoRequest>(payload);
    if (req->character_id() != static_cast<uint32_t>(connected_player_.id)) return;
    SendGuildInfo(0, PacketType_MP_GUILD_INFO);
}

void MapServer::HandleGuildInvite(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_guild_) {
        SendGuildInfo(1, PacketType_MP_GUILD_ADDMEMBER_NACK);
        return;
    }
    if (guild_.master_id != static_cast<uint32_t>(connected_player_.id)) {
        SendGuildInfo(2, PacketType_MP_GUILD_ADDMEMBER_NACK);
        return;
    }
    auto req = flatbuffers::GetRoot<GuildInviteRequest>(payload);
    std::string target = req->target_name() ? req->target_name()->str() : "";
    if (target.empty() || guild_.members.size() >= 30) {
        SendGuildInfo(3, PacketType_MP_GUILD_ADDMEMBER_NACK);
        return;
    }
    for (const auto& m : guild_.members) {
        if (m.name == target) {
            SendGuildInfo(4, PacketType_MP_GUILD_ADDMEMBER_NACK);
            return;
        }
    }
    GuildMemberState member;
    member.character_id = 9500 + static_cast<uint32_t>(guild_.members.size());
    member.name = target;
    member.level = 10;
    member.rank = 0;
    member.online = true;
    guild_.members.push_back(member);
    SendGuildInfo(0, PacketType_MP_GUILD_ADDMEMBER_ACK);
    SendGuildInfo(0, PacketType_MP_GUILD_INFO);
    spdlog::info("MapServer: {} invited to guild {}", target, guild_.name);
}

void MapServer::HandleGuildLeave(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_guild_) return;
    auto req = flatbuffers::GetRoot<GuildLeaveRequest>(payload);
    if (req->guild_id() != guild_.guild_id) return;

    if (guild_.master_id == static_cast<uint32_t>(connected_player_.id)) {
        has_guild_ = false;
        guild_.members.clear();
        guild_.guild_id = 0;
        guild_.name.clear();
        guild_.master_id = 0;
    } else {
        guild_.members.erase(
            std::remove_if(guild_.members.begin(), guild_.members.end(),
                [&](const GuildMemberState& m) {
                    return m.character_id == static_cast<uint32_t>(connected_player_.id);
                }),
            guild_.members.end());
    }
    SendGuildInfo(0, PacketType_MP_GUILD_SECEDE_ACK);
    spdlog::info("MapServer: player {} left guild", connected_player_.id);
}

MapServer::PlayerInvSlot* MapServer::FindInvSlot(uint8_t slot) {
    for (auto& s : player_inventory_) {
        if (s.slot == slot) return &s;
    }
    return nullptr;
}

const MapServer::PlayerInvSlot* MapServer::FindInvSlot(uint8_t slot) const {
    for (const auto& s : player_inventory_) {
        if (s.slot == slot) return &s;
    }
    return nullptr;
}

void MapServer::SendTradeState(uint8_t result, uint16_t ack_type, bool completed) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    if (!trade_.active) {
        auto resp = CreateTradeResponse(fbb, result, 0);
        fbb.Finish(resp);
        network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    std::vector<flatbuffers::Offset<TradeItemOffer>> my_items;
    std::vector<flatbuffers::Offset<TradeItemOffer>> their_items;
    if (trade_.active) {
        for (const auto& it : trade_.player_items) {
            my_items.push_back(CreateTradeItemOffer(fbb, it.trade_slot, it.item_id, it.count, it.inv_slot));
        }
        for (const auto& it : trade_.partner_items) {
            their_items.push_back(CreateTradeItemOffer(fbb, it.trade_slot, it.item_id, it.count, 255));
        }
    }
    auto self = CreateTradeSide(fbb,
        static_cast<uint32_t>(connected_player_.id),
        fbb.CreateString(connected_player_.name),
        trade_.player_gold,
        fbb.CreateVector(my_items),
        trade_.player_confirmed);
    auto partner = CreateTradeSide(fbb,
        trade_.partner_id,
        fbb.CreateString(trade_.partner_name),
        trade_.partner_gold,
        fbb.CreateVector(their_items),
        trade_.partner_confirmed);
    auto session = CreateTradeSessionInfo(fbb,
        trade_.session_id, self, partner, completed);
    auto resp = CreateTradeResponse(fbb, result, session);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleTradeApply(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    if (trade_.active) {
        SendTradeState(2, PacketType_MP_EXCHANGE_APPLY_NACK);
        return;
    }
    auto req = flatbuffers::GetRoot<TradeApplyRequest>(payload);
    std::string target = req->target_name() ? req->target_name()->str() : "NPC_Trader";
    trade_ = {};
    trade_.active = true;
    trade_.session_id = next_trade_id_++;
    trade_.partner_id = 9100;
    trade_.partner_name = target.empty() ? "NPC_Trader" : target;
    trade_.partner_items = {
        {0, 255, 201, 5},
        {1, 255, 202, 3},
    };
    trade_.partner_gold = 500;
    SendTradeState(0, PacketType_MP_EXCHANGE_APPLY_ACK);
    SendTradeState(0, PacketType_MP_EXCHANGE_START);
    spdlog::info("MapServer: trade {} started with {}", trade_.session_id, trade_.partner_name);
}

void MapServer::HandleTradeCancel(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<TradeSessionRequest>(payload);
    if (!trade_.active || req->session_id() != trade_.session_id) return;
    trade_ = {};
    SendTradeState(0, PacketType_MP_EXCHANGE_CANCEL_ACK);
    spdlog::info("MapServer: trade cancelled");
}

void MapServer::HandleTradeAddItem(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !trade_.active) {
        SendTradeState(1, PacketType_MP_EXCHANGE_ADDITEM_ACK);
        return;
    }
    auto req = flatbuffers::GetRoot<TradeAddItemRequest>(payload);
    if (req->session_id() != trade_.session_id) return;
    uint8_t inv_slot = req->inv_slot();
    uint16_t count = req->count() ? req->count() : 1;
    const PlayerInvSlot* inv = FindInvSlot(inv_slot);
    if (!inv || inv->item_id == 0 || inv->count < count || trade_.player_items.size() >= 12) {
        SendTradeState(2, PacketType_MP_EXCHANGE_ADDITEM_ACK);
        return;
    }
    trade_.player_confirmed = false;
    trade_.partner_confirmed = false;
    for (auto& offer : trade_.player_items) {
        if (offer.inv_slot == inv_slot) {
            offer.count = static_cast<uint16_t>(offer.count + count);
            SendTradeState(0, PacketType_MP_EXCHANGE_STATE_UPDATE);
            return;
        }
    }
    uint8_t trade_slot = req->trade_slot();
    if (trade_slot == 0) trade_slot = static_cast<uint8_t>(trade_.player_items.size());
    trade_.player_items.push_back({trade_slot, inv_slot, inv->item_id, count});
    SendTradeState(0, PacketType_MP_EXCHANGE_ADDITEM_ACK);
    SendTradeState(0, PacketType_MP_EXCHANGE_STATE_UPDATE);
}

void MapServer::HandleTradeSetGold(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !trade_.active) {
        SendTradeState(1, PacketType_MP_EXCHANGE_SETGOLD_ACK);
        return;
    }
    auto req = flatbuffers::GetRoot<TradeSetGoldRequest>(payload);
    if (req->session_id() != trade_.session_id) return;
    uint32_t gold = req->gold();
    if (gold > player_gold_) {
        SendTradeState(2, PacketType_MP_EXCHANGE_SETGOLD_ACK);
        return;
    }
    trade_.player_gold = gold;
    trade_.player_confirmed = false;
    trade_.partner_confirmed = false;
    SendTradeState(0, PacketType_MP_EXCHANGE_SETGOLD_ACK);
    SendTradeState(0, PacketType_MP_EXCHANGE_STATE_UPDATE);
}

void MapServer::ExecuteTrade() {
    using namespace luna::protocol;
    for (const auto& offer : trade_.player_items) {
        PlayerInvSlot* inv = FindInvSlot(offer.inv_slot);
        if (!inv || inv->item_id != offer.item_id || inv->count < offer.count) {
            trade_ = {};
            SendTradeState(3, PacketType_MP_EXCHANGE_APPLY_NACK);
            return;
        }
    }
    if (trade_.player_gold > player_gold_) {
        trade_ = {};
        SendTradeState(4, PacketType_MP_EXCHANGE_APPLY_NACK);
        return;
    }

    for (const auto& offer : trade_.player_items) {
        PlayerInvSlot* inv = FindInvSlot(offer.inv_slot);
        if (!inv) continue;
        inv->count = static_cast<uint16_t>(inv->count - offer.count);
        if (inv->count == 0) inv->item_id = 0;
    }
    player_gold_ -= trade_.player_gold;

    for (const auto& offer : trade_.partner_items) {
        bool merged = false;
        for (auto& inv : player_inventory_) {
            if (inv.item_id == offer.item_id) {
                inv.count = static_cast<uint16_t>(inv.count + offer.count);
                merged = true;
                break;
            }
        }
        if (!merged) {
            uint8_t slot = 0;
            for (uint8_t i = 0; i < 40; ++i) {
                bool used = false;
                for (const auto& s : player_inventory_) {
                    if (s.slot == i) { used = true; break; }
                }
                if (!used) { slot = i; break; }
            }
            player_inventory_.push_back({slot, offer.item_id, offer.count});
        }
    }
    player_gold_ += trade_.partner_gold;

    SendInventorySync();
    SendCharLifeUpdate();
    SendTradeState(0, PacketType_MP_EXCHANGE_COMPLETE, true);
    spdlog::info("MapServer: trade {} completed", trade_.session_id);
    trade_ = {};
}

void MapServer::HandleTradeConfirm(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !trade_.active) {
        SendTradeState(1, PacketType_MP_EXCHANGE_CONFIRM_ACK);
        return;
    }
    auto req = flatbuffers::GetRoot<TradeSessionRequest>(payload);
    if (req->session_id() != trade_.session_id) return;
    trade_.player_confirmed = true;
    trade_.partner_confirmed = true;
    SendTradeState(0, PacketType_MP_EXCHANGE_CONFIRM_ACK);
    SendTradeState(0, PacketType_MP_EXCHANGE_STATE_UPDATE);
    ExecuteTrade();
}

void MapServer::SeedMarketData() {
    if (!consignment_listings_.empty()) return;
    auto add = [&](uint32_t seller, uint32_t item, uint16_t cnt, uint32_t bid, uint32_t buyout) {
        ConsignmentListingState l;
        l.id = next_consignment_id_++;
        l.seller_id = seller;
        l.seller_name = "NPC_" + std::to_string(seller);
        l.item_id = item;
        l.count = cnt;
        l.bid_price = bid;
        l.buyout_price = buyout;
        consignment_listings_.push_back(l);
    };
    add(2, 101, 1, 200, 500);
    add(2, 102, 5, 50, 120);
    add(3, 201, 1, 1000, 2500);
    add(4, 301, 1, 5000, 12000);

    StreetStallState npc;
    npc.owner_id = 9200;
    npc.owner_name = "NPC_Merchant";
    npc.title = "Discount Goods";
    npc.open = true;
    npc.items = {{0, 21000001, 10, 25}, {1, 1001, 1, 150}};
    street_stalls_[npc.owner_id] = npc;
}

void MapServer::SendConsignmentList(uint8_t result, uint16_t ack_type,
    const std::string& query, bool mine_only, bool bids_only) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<ConsignmentListing>> entries;
    uint32_t player_id = static_cast<uint32_t>(connected_player_.id);
    for (const auto& l : consignment_listings_) {
        if (mine_only && l.seller_id != player_id) continue;
        if (bids_only && l.bidder_id != player_id) continue;
        if (!mine_only && !bids_only && (!l.active || l.sold)) continue;
        std::string iname = "Item_" + std::to_string(l.item_id);
        if (!query.empty() && iname.find(query) == std::string::npos &&
            l.seller_name.find(query) == std::string::npos)
            continue;
        entries.push_back(CreateConsignmentListingDirect(fbb, l.id, l.seller_id,
            l.seller_name.c_str(), l.item_id, iname.c_str(), l.count, l.enchant,
            l.bid_price, l.buyout_price, l.current_bid, l.bidder_id,
            l.bidder_name.c_str(), l.time_remaining, l.sold, l.active));
    }
    auto list = CreateConsignmentList(fbb, fbb.CreateVector(entries));
    auto resp = CreateConsignmentResponse(fbb, result, list);
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleConsignmentList(const uint8_t* payload, size_t len, uint16_t ack_type) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<ConsignmentListRequest>(payload);
    std::string query = req->query() ? req->query()->str() : "";
    SendConsignmentList(0, ack_type, query, req->my_listings_only(), req->my_bids_only());
}

void MapServer::HandleConsignmentRegister(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<ConsignmentRegisterRequest>(payload);
    PlayerInvSlot* inv = FindInvSlot(req->inv_slot());
    uint16_t count = req->item_count() ? req->item_count() : 1;
    uint32_t fee = req->bid_price() * 5 / 100;
    if (!inv || inv->item_id == 0 || inv->count < count || player_gold_ < fee) {
        SendConsignmentList(1, PacketType_MP_CONSIGNMENT_REGIST_NACK, "", false, false);
        return;
    }
    ConsignmentListingState l;
    l.id = next_consignment_id_++;
    l.seller_id = static_cast<uint32_t>(connected_player_.id);
    l.seller_name = connected_player_.name;
    l.item_id = inv->item_id;
    l.count = count;
    l.bid_price = req->bid_price();
    l.buyout_price = req->buyout_price() ? req->buyout_price() : req->bid_price() * 2;
    consignment_listings_.push_back(l);
    inv->count = static_cast<uint16_t>(inv->count - count);
    if (inv->count == 0) inv->item_id = 0;
    player_gold_ -= fee;
    SendInventorySync();
    SendCharLifeUpdate();
    SendConsignmentList(0, PacketType_MP_CONSIGNMENT_REGIST_ACK, "", false, false);
}

void MapServer::HandleConsignmentTrade(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<ConsignmentTradeRequest>(payload);
    for (auto& l : consignment_listings_) {
        if (l.id != req->listing_id() || !l.active || l.sold) continue;
        uint32_t buyer = static_cast<uint32_t>(connected_player_.id);
        if (req->buyout()) {
            if (l.buyout_price == 0 || player_gold_ < l.buyout_price) {
                SendConsignmentList(2, PacketType_MP_CONSIGNMENT_UPDATE_NACK, "", false, false);
                return;
            }
            player_gold_ -= l.buyout_price;
            l.current_bid = l.buyout_price;
            l.bidder_id = buyer;
            l.bidder_name = connected_player_.name;
            l.sold = true;
            l.active = false;
        } else {
            uint32_t min_bid = l.current_bid > 0 ? l.current_bid + 1 : l.bid_price;
            uint32_t amount = req->amount() ? req->amount() : min_bid;
            if (amount < min_bid || player_gold_ < amount) {
                SendConsignmentList(3, PacketType_MP_CONSIGNMENT_UPDATE_NACK, "", false, false);
                return;
            }
            player_gold_ -= amount;
            l.current_bid = amount;
            l.bidder_id = buyer;
            l.bidder_name = connected_player_.name;
        }
        bool merged = false;
        for (auto& slot : player_inventory_) {
            if (slot.item_id == l.item_id) {
                slot.count = static_cast<uint16_t>(slot.count + l.count);
                merged = true;
                break;
            }
        }
        if (!merged) {
            uint8_t slot_idx = 0;
            for (uint8_t i = 0; i < 40; ++i) {
                bool used = false;
                for (const auto& s : player_inventory_) {
                    if (s.slot == i) { used = true; break; }
                }
                if (!used) { slot_idx = i; break; }
            }
            player_inventory_.push_back({slot_idx, l.item_id, l.count});
        }
        if (l.seller_id == static_cast<uint32_t>(connected_player_.id))
            player_gold_ += l.current_bid;
        SendInventorySync();
        SendCharLifeUpdate();
        SendConsignmentList(0, PacketType_MP_CONSIGNMENT_UPDATE_ACK, "", false, false);
        return;
    }
    SendConsignmentList(4, PacketType_MP_CONSIGNMENT_UPDATE_NACK, "", false, false);
}

void MapServer::HandleConsignmentCancel(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<ConsignmentCancelRequest>(payload);
    for (auto& l : consignment_listings_) {
        if (l.id != req->listing_id()) continue;
        if (l.seller_id != static_cast<uint32_t>(connected_player_.id)) {
            SendConsignmentList(1, PacketType_MP_CONSIGNMENT_CANCEL_ACK, "", true, false);
            return;
        }
        if (!l.sold && l.active) {
            player_inventory_.push_back({0, l.item_id, l.count});
            l.active = false;
            SendInventorySync();
        }
        SendConsignmentList(0, PacketType_MP_CONSIGNMENT_CANCEL_ACK, "", true, false);
        return;
    }
    SendConsignmentList(2, PacketType_MP_CONSIGNMENT_CANCEL_ACK, "", true, false);
}

void MapServer::ReturnStallItems(uint32_t owner_id) {
    auto it = street_stalls_.find(owner_id);
    if (it == street_stalls_.end()) return;
    if (owner_id != static_cast<uint32_t>(connected_player_.id)) return;
    for (const auto& item : it->second.items) {
        bool merged = false;
        for (auto& inv : player_inventory_) {
            if (inv.item_id == item.item_id) {
                inv.count = static_cast<uint16_t>(inv.count + item.count);
                merged = true;
                break;
            }
        }
        if (!merged)
            player_inventory_.push_back({item.slot, item.item_id, item.count});
    }
    it->second.items.clear();
    it->second.open = false;
}

void MapServer::SendStreetStallState(uint8_t result, uint16_t ack_type) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    uint32_t pid = static_cast<uint32_t>(connected_player_.id);
    auto build_stall = [&](const StreetStallState& s) {
        std::vector<flatbuffers::Offset<StreetStallItem>> items;
        for (const auto& it : s.items) {
            std::string name = "Item_" + std::to_string(it.item_id);
            items.push_back(CreateStreetStallItemDirect(fbb, it.slot, it.item_id,
                name.c_str(), it.count, it.price));
        }
        return CreateStreetStallInfo(fbb, s.owner_id, fbb.CreateString(s.owner_name),
            fbb.CreateString(s.title), s.open, fbb.CreateVector(items));
    };
    StreetStallState empty;
    auto my = street_stalls_.count(pid) ? build_stall(street_stalls_[pid]) : build_stall(empty);
    std::vector<flatbuffers::Offset<StreetStallInfo>> nearby;
    for (const auto& [id, stall] : street_stalls_) {
        (void)id;
        if (!stall.open) continue;
        if (stall.owner_id == pid) continue;
        nearby.push_back(build_stall(stall));
    }
    auto resp = CreateStreetStallResponse(fbb, result, my, fbb.CreateVector(nearby));
    fbb.Finish(resp);
    network_->SendPacket(ack_type, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleStreetStallOpen(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StreetStallOpenRequest>(payload);
    uint32_t pid = static_cast<uint32_t>(connected_player_.id);
    ReturnStallItems(pid);
    auto& stall = street_stalls_[pid];
    stall.owner_id = pid;
    stall.owner_name = connected_player_.name;
    stall.title = req->title() ? req->title()->str() : "My Stall";
    stall.open = true;
    stall.items.clear();
    SendStreetStallState(0, PacketType_MP_STREETSTALL_OPEN_ACK);
}

void MapServer::HandleStreetStallAddItem(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StreetStallAddItemRequest>(payload);
    uint32_t pid = static_cast<uint32_t>(connected_player_.id);
    auto& stall = street_stalls_[pid];
    if (!stall.open) {
        SendStreetStallState(1, PacketType_MP_STREETSTALL_ADDITEM_ACK);
        return;
    }
    PlayerInvSlot* inv = FindInvSlot(req->inv_slot());
    uint16_t count = req->count() ? req->count() : 1;
    if (!inv || inv->item_id == 0 || inv->count < count) {
        SendStreetStallState(2, PacketType_MP_STREETSTALL_ADDITEM_ACK);
        return;
    }
    uint8_t slot = req->stall_slot();
    if (slot == 0) slot = static_cast<uint8_t>(stall.items.size());
    stall.items.push_back({slot, inv->item_id, count, req->price()});
    inv->count = static_cast<uint16_t>(inv->count - count);
    if (inv->count == 0) inv->item_id = 0;
    SendInventorySync();
    SendStreetStallState(0, PacketType_MP_STREETSTALL_ADDITEM_ACK);
}

void MapServer::HandleStreetStallBuy(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<StreetStallBuyRequest>(payload);
    auto it = street_stalls_.find(req->owner_id());
    if (it == street_stalls_.end() || !it->second.open) {
        SendStreetStallState(1, PacketType_MP_STREETSTALL_BUY_ACK);
        return;
    }
    for (auto slot_it = it->second.items.begin(); slot_it != it->second.items.end(); ++slot_it) {
        if (slot_it->slot != req->stall_slot()) continue;
        uint16_t count = req->count() ? req->count() : slot_it->count;
        uint32_t cost = slot_it->price * count;
        if (player_gold_ < cost || slot_it->count < count) {
            SendStreetStallState(2, PacketType_MP_STREETSTALL_BUY_ACK);
            return;
        }
        player_gold_ -= cost;
        player_inventory_.push_back({0, slot_it->item_id, count});
        slot_it->count = static_cast<uint16_t>(slot_it->count - count);
        if (slot_it->count == 0)
            it->second.items.erase(slot_it);
        SendInventorySync();
        SendCharLifeUpdate();
        SendStreetStallState(0, PacketType_MP_STREETSTALL_BUY_ACK);
        return;
    }
    SendStreetStallState(3, PacketType_MP_STREETSTALL_BUY_ACK);
}

void MapServer::HandleStreetStallClose(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    (void)flatbuffers::GetRoot<StreetStallCloseRequest>(payload);
    ReturnStallItems(static_cast<uint32_t>(connected_player_.id));
    SendInventorySync();
    SendStreetStallState(0, PacketType_MP_STREETSTALL_CLOSE_ACK);
}

void MapServer::HandleStreetStallList(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    (void)flatbuffers::GetRoot<StreetStallListRequest>(payload);
    if (!player_joined_) return;
    SendStreetStallState(0, PacketType_MP_STREETSTALL_LIST_ACK);
}

void MapServer::HandleGuildChat(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !has_guild_) return;
    auto req = flatbuffers::GetRoot<ChatMessage>(payload);
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateChatMessageDirect(fbb,
        static_cast<uint32_t>(connected_player_.id),
        connected_player_.name.c_str(),
        req->message() ? req->message()->c_str() : "",
        ChatChannel_Guild,
        0);
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_CHAT_GUILD_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleChat(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    auto req = flatbuffers::GetRoot<ChatMessage>(payload);
    flatbuffers::FlatBufferBuilder fbb;
    auto msg = CreateChatMessageDirect(fbb,
        static_cast<uint32_t>(connected_player_.id),
        connected_player_.name.c_str(),
        req->message() ? req->message()->c_str() : "",
        ChatChannel_All,
        0);
    fbb.Finish(msg);
    network_->SendPacket(PacketType_MP_CHAT_ALL_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::TeleportPlayer(uint32_t map_id, float x, float y, float z) {
    if (!player_joined_) return;
    if (map_id && map_id != static_cast<uint32_t>(map_id_)) {
        spdlog::info("MapServer: cross-map teleport requested to map {} (not implemented inline)", map_id);
        return;
    }
    connected_player_.pos_x = x;
    connected_player_.pos_y = y;
    connected_player_.pos_z = z;
    if (registry_->valid(player_entity_)) {
        registry_->get<Transform>(player_entity_).position = glm::vec3(x, y, z);
    }
    grid_.Update(static_cast<uint32_t>(connected_player_.id), x, z);
    SendEntityTransform(static_cast<uint32_t>(connected_player_.id), x, y, z, "teleport");
}

void MapServer::RunMapScript(const std::string& path, entt::entity player) {
    if (player == entt::null) return;
    script_runtime_.RunScript(path, static_cast<uint32_t>(entt::to_entity(player)));
}

void MapServer::OnMonsterKilled(uint32_t monster_template_id) {
    ApplyQuestProgress(QuestObjective::KillMonster, monster_template_id, 1);
}

void MapServer::ApplyQuestProgress(QuestObjective::Type type, uint32_t target_id, uint16_t amount) {
    if (!registry_->valid(player_entity_)) return;
    auto* log = registry_->try_get<QuestLog>(player_entity_);
    if (!log) return;

    const char* fsm_event = (type == QuestObjective::CollectItem) ? "collect"
        : (type == QuestObjective::TalkToNPC) ? "npc_talk" : "kill";

    for (auto& entry : log->active_quests) {
        if (entry.is_completed) continue;
        for (size_t oi = 0; oi < entry.objectives.size(); ++oi) {
            auto& obj = entry.objectives[oi];
            if (obj.type != type || obj.target_id != target_id) continue;
            if (obj.current_count >= obj.required_count) continue;
            uint16_t before = obj.current_count;
            obj.current_count = std::min<uint16_t>(
                static_cast<uint16_t>(before + amount), obj.required_count);
            if (obj.current_count != before) {
                SendQuestUpdate(entry.quest_id, static_cast<uint8_t>(oi),
                    obj.current_count, obj.required_count);
                script_runtime_.RunQuestFsm(entry.quest_id,
                    static_cast<uint32_t>(connected_player_.id), fsm_event,
                    type == QuestObjective::CollectItem ? 0 : target_id,
                    type == QuestObjective::CollectItem ? target_id : 0);
            }
        }
    }
    quest_->Update(*registry_, 0.0f);
    SavePlayerQuests(connected_player_.id);
}

void MapServer::SendQuestList(uint8_t result) {
    using namespace luna::protocol;
    if (!registry_->valid(player_entity_)) return;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<QuestEntryState>> active;
    std::vector<uint32_t> completed;

    if (auto* log = registry_->try_get<QuestLog>(player_entity_)) {
        for (const auto& entry : log->active_quests) {
            auto it = quest_->GetTemplates().find(entry.quest_id);
            std::string name = it != quest_->GetTemplates().end() ? it->second.name : "Quest";
            std::vector<flatbuffers::Offset<QuestObjectiveState>> objs;
            for (const auto& obj : entry.objectives) {
                objs.push_back(CreateQuestObjectiveState(fbb,
                    static_cast<uint8_t>(obj.type), obj.target_id,
                    obj.current_count, obj.required_count));
            }
            active.push_back(CreateQuestEntryState(fbb, entry.quest_id,
                fbb.CreateString(name), entry.is_completed, entry.is_reward_taken,
                fbb.CreateVector(objs)));
        }
        for (uint32_t cid : log->completed_quest_ids)
            completed.push_back(cid);
    }

    auto resp = CreateQuestListResponse(fbb, result,
        fbb.CreateVector(active), fbb.CreateVector(completed));
    fbb.Finish(resp);
    network_->SendPacket(PacketType_MP_QUEST_LIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::SendQuestUpdate(uint32_t quest_id, uint8_t obj_index, uint16_t current, uint16_t required) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto n = CreateQuestUpdateNotify(fbb, quest_id, obj_index, current, required);
    fbb.Finish(n);
    network_->SendPacket(PacketType_MP_QUEST_UPDATE_NOTIFY, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleQuestStart(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !registry_->valid(player_entity_)) return;
    auto req = flatbuffers::GetRoot<QuestStartRequest>(payload);
    uint32_t qid = req->quest_id();
    bool ok = quest_->StartQuest(*registry_, player_entity_, qid);
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = CreateQuestStartResponse(fbb, ok ? 0 : 1, qid,
        fbb.CreateString(ok ? "Quest started" : "Cannot start quest"));
    fbb.Finish(resp);
    network_->SendPacket(ok ? PacketType_MP_QUEST_START_ACK : PacketType_MP_QUEST_START_NACK,
        fbb.GetBufferPointer(), fbb.GetSize());
    if (ok) {
        char script_path[128];
        snprintf(script_path, sizeof(script_path), "assets/scripts/quests/quest_%04u.lua", qid);
        RunMapScript(script_path, player_entity_);
        script_runtime_.RunQuestFsm(qid, static_cast<uint32_t>(connected_player_.id), "accept");
        if (req->npc_id())
            script_runtime_.RunQuestFsm(qid, static_cast<uint32_t>(connected_player_.id),
                "npc_talk", req->npc_id(), 0);
        SendQuestList(0);
        SavePlayerQuests(connected_player_.id);
    }
}

void MapServer::HandleQuestEnd(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_ || !registry_->valid(player_entity_)) return;
    auto req = flatbuffers::GetRoot<QuestCompleteRequest>(payload);
    uint32_t qid = req->quest_id();
    bool completed = quest_->CompleteQuest(*registry_, player_entity_, qid);
    bool rewarded = completed && quest_->ClaimReward(*registry_, player_entity_, qid);
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = CreateQuestCompleteResponse(fbb, rewarded ? 0 : 1, qid);
    fbb.Finish(resp);
    network_->SendPacket(rewarded ? PacketType_MP_QUEST_END_ACK : PacketType_MP_QUEST_END_NACK,
        fbb.GetBufferPointer(), fbb.GetSize());
    if (rewarded) {
        SendInventorySync();
        SendCharLifeUpdate();
        SendQuestList(0);
        SavePlayerQuests(connected_player_.id);
    }
}

void MapServer::HandleQuestList(const uint8_t* payload, size_t len) {
    (void)payload;
    (void)len;
    SendQuestList(player_joined_ ? 0 : 1);
}

void MapServer::HandleDungeonEntrance(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<DungeonEntranceRequest>(payload);
    uint32_t template_id = req->dungeon_template_id();
    std::vector<uint32_t> party = {static_cast<uint32_t>(connected_player_.id)};
    uint32_t instance_id = CreateDungeonInstance(template_id, party);
    EnterDungeon(instance_id, player_entity_);
    TriggerBossEncounter(instance_id, template_id + 1000);

    flatbuffers::FlatBufferBuilder fbb;
    auto resp = CreateDungeonEntranceResponse(fbb, 0, instance_id, template_id,
        static_cast<uint16_t>(map_id_), 1800,
        fbb.CreateString("Dungeon instance created"));
    fbb.Finish(resp);
    network_->SendPacket(PacketType_MP_DUNGEON_ENTRANCE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::HandleDungeonInfo(const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;
    if (!player_joined_) return;
    auto req = flatbuffers::GetRoot<DungeonInfoRequest>(payload);
    uint32_t iid = req->instance_id();
    auto* inst = GetDungeonInstance(iid);
    flatbuffers::FlatBufferBuilder fbb;
    if (!inst) {
        auto resp = CreateDungeonInfoResponse(fbb, 1, iid, 0, 0, false);
        fbb.Finish(resp);
        network_->SendPacket(PacketType_MP_DUNGEON_INFO_NACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    auto resp = CreateDungeonInfoResponse(fbb, 0, iid,
        static_cast<uint8_t>(inst->state), static_cast<uint32_t>(inst->elapsed_time),
        inst->state == DungeonState::BOSS_ACTIVE);
    fbb.Finish(resp);
    network_->SendPacket(PacketType_MP_DUNGEON_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

namespace {

std::string SerializeQuestProgress(const QuestEntry& entry) {
    std::string s;
    for (size_t i = 0; i < entry.objectives.size(); ++i) {
        if (i) s += ';';
        s += std::to_string(i) + ":" + std::to_string(entry.objectives[i].current_count);
    }
    return s;
}

void ParseStoredQuestProgress(QuestEntry& entry, const std::string& progress) {
    size_t p = 0;
    while (p < progress.size()) {
        size_t semi = progress.find(';', p);
        std::string tok = progress.substr(p, semi == std::string::npos ? std::string::npos : semi - p);
        size_t colon = tok.find(':');
        if (colon != std::string::npos) {
            size_t idx = static_cast<size_t>(std::stoul(tok.substr(0, colon)));
            uint16_t cnt = static_cast<uint16_t>(std::stoul(tok.substr(colon + 1)));
            if (idx < entry.objectives.size())
                entry.objectives[idx].current_count = std::min(cnt, entry.objectives[idx].required_count);
        }
        if (semi == std::string::npos) break;
        p = semi + 1;
    }
}

} // namespace

void MapServer::SendTriggerNotify(uint32_t trigger_id, uint8_t trigger_type,
                                  uint32_t param0, uint32_t param1, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto n = CreateTriggerNotify(fbb, trigger_id, trigger_type, param0, param1,
        fbb.CreateString(message));
    fbb.Finish(n);
    network_->SendPacket(PacketType_MP_TRIGGER_NOTIFY, fbb.GetBufferPointer(), fbb.GetSize());
}

void MapServer::LoadPlayerQuests(int character_id) {
    if (!registry_->valid(player_entity_)) return;
    auto& log = registry_->get<QuestLog>(player_entity_);
    log.active_quests.clear();
    log.completed_quest_ids.clear();

    auto rows = db_->Query(
        "SELECT quest_id,state,progress FROM player_quests WHERE character_id=" +
        std::to_string(character_id));
    const auto& templates = quest_->GetTemplates();
    for (const auto& row : rows) {
        if (row.size() < 3) continue;
        uint32_t qid = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        int state = std::atoi(row[1].c_str());
        if (state >= 2) {
            log.completed_quest_ids.push_back(qid);
            continue;
        }
        auto it = templates.find(qid);
        if (it == templates.end()) continue;
        QuestEntry entry;
        entry.quest_id = qid;
        entry.giver_npc_id = it->second.giver_npc_id;
        entry.completer_npc_id = it->second.completer_npc_id;
        for (const auto& obj : it->second.objectives)
            entry.objectives.push_back(obj);
        ParseStoredQuestProgress(entry, row[2]);
        if (state == 1) entry.is_completed = true;
        log.active_quests.push_back(std::move(entry));
    }
    spdlog::info("MapServer: loaded {} active quests for character {}", log.active_quests.size(), character_id);
}

void MapServer::SavePlayerQuests(int character_id) {
    if (!registry_->valid(player_entity_)) return;
    auto* log = registry_->try_get<QuestLog>(player_entity_);
    if (!log) return;

    db_->Execute("DELETE FROM player_quests WHERE character_id=" + std::to_string(character_id));
    for (const auto& entry : log->active_quests) {
        int state = entry.is_completed ? 1 : 0;
        std::string progress = SerializeQuestProgress(entry);
        db_->Execute(
            "INSERT INTO player_quests (character_id,quest_id,state,progress) VALUES (" +
            std::to_string(character_id) + "," + std::to_string(entry.quest_id) + "," +
            std::to_string(state) + ",'" + progress + "')");
    }
    for (uint32_t cid : log->completed_quest_ids) {
        db_->Execute(
            "INSERT OR REPLACE INTO player_quests (character_id,quest_id,state,progress) VALUES (" +
            std::to_string(character_id) + "," + std::to_string(cid) + ",2,'')");
    }
}

void MapServer::UpdatePlayerVisibility() {
    if (!player_joined_) return;
    using namespace luna::protocol;

    float px = connected_player_.pos_x;
    float pz = connected_player_.pos_z;
    float r2 = aoi_radius_ * aoi_radius_;

    std::unordered_set<uint32_t> now_visible;
    for (uint32_t eid : grid_.QueryRadius(px, pz, aoi_radius_)) {
        auto entity = static_cast<entt::entity>(eid);
        if (!registry_->valid(entity) || !registry_->all_of<TagMonster, Transform>(entity))
            continue;
        auto& xform = registry_->get<Transform>(entity);
        float dx = xform.position.x - px;
        float dz = xform.position.z - pz;
        if (dx * dx + dz * dz <= r2)
            now_visible.insert(eid);
    }

    for (uint32_t old_id : visible_monsters_) {
        if (now_visible.count(old_id)) continue;
        SendEntityDespawn(old_id, static_cast<int8_t>(DespawnReason_Despawn));
        monster_net_.erase(old_id);
    }

    auto view = registry_->view<TagMonster, Transform, SpawnInfo, CharacterStats>();
    for (auto entity : view) {
        uint32_t eid = static_cast<uint32_t>(entt::to_entity(entity));
        if (!now_visible.count(eid) || visible_monsters_.count(eid)) continue;
        auto& xform = view.get<Transform>(entity);
        auto& spawn = view.get<SpawnInfo>(entity);
        auto& stats = view.get<CharacterStats>(entity);
        std::string model = "m224.chx";
        std::string name = "Monster_" + std::to_string(spawn.monster_id);
        SendEntitySpawn(eid, static_cast<int8_t>(EntityType_Monster),
            model, name, stats.level,
            xform.position.x, xform.position.y, xform.position.z,
            stats.max_hp > 0 ? static_cast<float>(stats.hp) / stats.max_hp : 1.0f);
        monster_net_[eid] = {};
        monster_net_[eid].last_sent = xform.position;
        monster_net_[eid].initialized = true;
    }

    visible_monsters_ = std::move(now_visible);
}
