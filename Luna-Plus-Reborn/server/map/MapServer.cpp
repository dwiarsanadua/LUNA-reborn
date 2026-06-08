#include "MapServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include "systems/CombatSystem.h"
#include "systems/AISystem.h"
#include "systems/MovementSystem.h"
#include "systems/ItemSystem.h"
#include "systems/QuestSystem.h"
#include "systems/SpawnSystem.hpp"
#include <sqlite3.h>
#include <ecs/components/Inventory.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Tag.hpp>
#include <entt/entt.hpp>
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

    network_->SetReceiveCallback([this](const uint8_t* data, size_t size) {
        spdlog::debug("MapServer: packet received size={}", size);
    });

    // Load monster spawns and NPCs via SpawnSystem + direct DB
    {
        spawn_sys_->LoadSpawnData("assets/data/monsters.json");
        spawn_sys_->SpawnMonstersForMap(*registry_, map_id);

        sqlite3* gamedb = nullptr;
        if (sqlite3_open("data/game_data.db", &gamedb) == SQLITE_OK) {
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

    running_ = true;
    spdlog::info("MapServer: map {} initialized on port {}", map_id, port);
    return true;
}

void MapServer::Shutdown() {
    running_ = false;
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
    movement_->Update(*registry_, dt);
    ai_->Update(*registry_, dt);
    combat_->Update(*registry_, dt);

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
    SendNPCList(entity_id);
    spdlog::info("MapServer: player {} spawned, NPC list sent", data.name);
}

void MapServer::DespawnPlayer(int entity_id) {
    auto view = registry_->view<PlayerData>();
    for (auto entity : view) {
        auto& pd = view.get<PlayerData>(entity);
        if (pd.id == entity_id) {
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
    if (sqlite3_open("data/game_data.db", &gamedb) != SQLITE_OK) return;
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
    if (sqlite3_open("data/game_data.db", &gamedb) != SQLITE_OK) return false;
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
