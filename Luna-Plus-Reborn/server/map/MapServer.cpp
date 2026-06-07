#include "MapServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include "systems/CombatSystem.h"
#include "systems/AISystem.h"
#include "systems/MovementSystem.h"
#include "systems/ItemSystem.h"
#include "systems/QuestSystem.h"
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <chrono>
#include <thread>

MapServer::MapServer()
    : network_(std::make_unique<NetworkLayer>())
    , db_(std::make_unique<Database>())
    , registry_(std::make_unique<entt::registry>())
    , combat_(std::make_unique<CombatSystem>())
    , ai_(std::make_unique<AISystem>())
    , movement_(std::make_unique<MovementSystem>())
    , item_(std::make_unique<ItemSystem>())
    , quest_(std::make_unique<QuestSystem>())
{}

MapServer::~MapServer() { Shutdown(); }

bool MapServer::Initialize(int map_id, uint16_t port) {
    map_id_ = map_id;

    if (!db_->Initialize("data/luna_map.db")) {
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

    if (!network_->Initialize(port)) {
        spdlog::error("MapServer: network init failed on port {}", port);
        return false;
    }

    network_->SetReceiveCallback([this](const uint8_t* data, size_t size) {
        // Packet dispatch would parse type and route to correct system
        spdlog::debug("MapServer: packet received size={}", size);
    });

    running_ = true;
    spdlog::info("MapServer: map {} initialized on port {}", map_id, port);
    return true;
}

void MapServer::Shutdown() {
    running_ = false;
    network_->Shutdown();
    db_->Shutdown();
}

void MapServer::Update(float dt) {
    network_->Update();

    // ECS systems run in order: Movement -> AI -> Combat -> Cleanup
    movement_->Update(*registry_, dt);
    ai_->Update(*registry_, dt);
    combat_->Update(*registry_, dt);

    // Auto-save player state every 60 seconds
    auto_save_timer_ += dt;
    if (auto_save_timer_ >= 60.0f) {
        auto_save_timer_ = 0.0f;
        // Persist player entities
        auto view = registry_->view<PlayerData>();
        for (auto entity : view) {
            auto& pd = view.get<PlayerData>(entity);
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
    auto entity = registry_->create();
    registry_->emplace<PlayerData>(entity, data);
    spdlog::info("MapServer: player {} spawned", data.name);
}

void MapServer::DespawnPlayer(int entity_id) {
    auto view = registry_->view<PlayerData>();
    for (auto entity : view) {
        auto& pd = view.get<PlayerData>(entity);
        if (pd.id == entity_id) {
            registry_->destroy(entity);
            spdlog::info("MapServer: player {} despawned", entity_id);
            return;
        }
    }
}

void MapServer::SpawnMonster(int template_id, const glm::vec3& pos) {
    auto entity = registry_->create();
    // Would load from Monster DB template
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

CombatSystem& MapServer::GetCombatSystem() { return *combat_; }
AISystem& MapServer::GetAISystem() { return *ai_; }
MovementSystem& MapServer::GetMovementSystem() { return *movement_; }
ItemSystem& MapServer::GetItemSystem() { return *item_; }
QuestSystem& MapServer::GetQuestSystem() { return *quest_; }
Database& MapServer::GetDatabase() { return *db_; }
NetworkLayer& MapServer::GetNetworkLayer() { return *network_; }
