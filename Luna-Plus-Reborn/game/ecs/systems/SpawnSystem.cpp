#include "SpawnSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/SpawnInfo.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <fstream>

void SpawnSystem::Update(entt::registry& registry, float dt) {
    // Process pending respawns
    for (auto it = pending_respawns_.begin(); it != pending_respawns_.end(); ) {
        it->elapsed += dt;
        if (it->elapsed >= it->delay) {
            auto sp_it = std::find_if(spawn_points_.begin(), spawn_points_.end(),
                [&](const SpawnPoint& sp) { return sp.id == it->spawn_point_id; });
            if (sp_it != spawn_points_.end() && sp_it->current_count < sp_it->max_count) {
                SpawnSingle(registry, *sp_it);
                sp_it->current_count++;
            }
            it = pending_respawns_.erase(it);
        } else {
            ++it;
        }
    }

    // Original respawn logic for entities with SpawnInfo
    auto view = registry.view<SpawnInfo, Transform>();
    for (auto entity : view) {
        auto& spawn = view.get<SpawnInfo>(entity);
        if (!spawn.is_alive) {
            spawn.respawn_timer += dt;
            if (spawn.respawn_timer >= spawn.respawn_time) {
                spawn.is_alive = true;
                spawn.respawn_timer = 0.0f;
            }
        }
    }
}

void SpawnSystem::SpawnMonster(entt::registry& registry, uint32_t monster_id,
                                const glm::vec3& pos, uint16_t level) {
    auto entity = registry.create();
    auto& xform = registry.emplace<Transform>(entity);
    xform.position = pos;
    auto& stats = registry.emplace<CharacterStats>(entity);
    stats.level = level;
    registry.emplace<TagMonster>(entity);
}

void SpawnSystem::DespawnEntity(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) registry.destroy(entity);
}

void SpawnSystem::LoadSpawnData(const std::string& json_path) {
    spawn_points_.clear();
    map_spawn_indices_.clear();
    pending_respawns_.clear();

    std::ifstream f(json_path);
    if (!f.is_open()) {
        spdlog::warn("SpawnSystem: cannot open {}", json_path);
        return;
    }

    json root;
    try {
        f >> root;
    } catch (...) {
        spdlog::error("SpawnSystem: JSON parse error in {}", json_path);
        return;
    }

    auto add_spawn = [&](const json& item, uint32_t monster_id_default) {
        SpawnPoint sp;
        sp.id            = item.value("id", static_cast<uint32_t>(spawn_points_.size() + 1));
        sp.map_id        = item.value("map_id", item.value("map", 0));
        sp.monster_id    = item.value("monster_id", monster_id_default);
        sp.x             = item.value("x", item.value("pos_x", 0.0f));
        sp.y             = item.value("y", item.value("pos_y", 0.0f));
        sp.z             = item.value("z", item.value("pos_z", 0.0f));
        sp.respawn_time  = item.value("respawn_time", item.value("respawn", 10.0f));
        sp.max_count     = item.value("max_count", item.value("count", 1));
        sp.current_count = 0;
        sp.aggro_range   = item.value("aggro_range", 10.0f);
        sp.patrol_radius = item.value("patrol_radius", 0u);
        if (sp.map_id == 0 || sp.monster_id == 0) return;

        spawn_points_.push_back(sp);
        map_spawn_indices_[sp.map_id].push_back(
            static_cast<uint32_t>(spawn_points_.size() - 1));
    };

    if (root.is_array()) {
        for (const auto& item : root) {
            if (item.contains("spawns") && item["spawns"].is_array()) {
                uint32_t monster_id = item.value("id", item.value("monster_id", 0u));
                for (const auto& sp : item["spawns"]) add_spawn(sp, monster_id);
            } else if (item.contains("map_id") || item.contains("map")) {
                add_spawn(item, item.value("monster_id", 0u));
            }
        }
    } else if (root.is_object()) {
        if (root.contains("spawns") && root["spawns"].is_array()) {
            for (const auto& sp : root["spawns"]) add_spawn(sp, 0u);
        }
    } else {
        spdlog::error("SpawnSystem: unsupported JSON root in {}", json_path);
        return;
    }

    spdlog::info("SpawnSystem: loaded {} spawn points ({} maps) from {}",
                 spawn_points_.size(), map_spawn_indices_.size(), json_path);
}

void SpawnSystem::SpawnMonstersForMap(entt::registry& registry, int map_id) {
    auto it = map_spawn_indices_.find(map_id);
    if (it == map_spawn_indices_.end()) {
        spdlog::info("SpawnSystem: no spawns for map {}", map_id);
        return;
    }

    int count = 0;
    for (uint32_t idx : it->second) {
        auto& sp = spawn_points_[idx];
        for (int i = 0; i < sp.max_count; ++i) {
            SpawnSingle(registry, sp);
            sp.current_count++;
            count++;
        }
    }
    spdlog::info("SpawnSystem: spawned {} monsters for map {}", count, map_id);
}

void SpawnSystem::RespawnMonster(entt::registry& registry, uint32_t spawn_id, float delay) {
    pending_respawns_.push_back({spawn_id, delay, 0.0f});
}

void SpawnSystem::DespawnAll(entt::registry& registry) {
    auto view = registry.view<TagMonster>();
    for (auto entity : view) {
        registry.destroy(entity);
    }
    pending_respawns_.clear();
    for (auto& sp : spawn_points_) {
        sp.current_count = 0;
    }
    spdlog::info("SpawnSystem: despawned all monsters");
}

entt::entity SpawnSystem::SpawnSingle(entt::registry& registry, const SpawnPoint& sp) {
    auto entity = registry.create();
    auto& xform = registry.emplace<Transform>(entity);
    xform.position = glm::vec3(sp.x, sp.y, sp.z);
    auto& stats = registry.emplace<CharacterStats>(entity);
    stats.level = 1;
    registry.emplace<TagMonster>(entity);
    auto& sinfo = registry.emplace<SpawnInfo>(entity);
    sinfo.spawn_rule_id = sp.id;
    sinfo.monster_id = sp.monster_id;
    sinfo.level = 1;
    sinfo.quantity = 1;
    sinfo.respawn_time = sp.respawn_time;
    sinfo.respawn_timer = 0.0f;
    sinfo.is_alive = true;
    sinfo.is_boss = false;
    return entity;
}
