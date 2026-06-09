#include "FieldBossSystem.h"
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/components/Transform.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>

void FieldBossSystem::LoadFieldBossData(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f) {
        spdlog::warn("FieldBossSystem: no field boss data found at {}", json_path);

        // Add default field bosses
        FieldBossData default_boss;
        default_boss.id = 1;
        default_boss.monster_id = 5001;
        default_boss.map_id = 1;
        default_boss.spawn_position = glm::vec3(100.0f, 0.0f, 100.0f);
        default_boss.respawn_hours = 4.0f;
        default_boss.min_party_size = 3;
        default_boss.loot_table = {201, 202, 301};
        default_boss.phases = {
            {1, 0.75f, 101, 30.0f, "Phase 1"},
            {2, 0.50f, 102, 30.0f, "Phase 2"},
            {3, 0.25f, 103, 30.0f, "Phase 3"},
        };
        field_bosses_.push_back(default_boss);
        return;
    }

    try {
        nlohmann::json j;
        f >> j;
        for (auto& entry : j) {
            FieldBossData boss;
            boss.id = entry["id"].get<uint32_t>();
            boss.monster_id = entry["monster_id"].get<uint32_t>();
            boss.map_id = entry["map_id"].get<int>();
            boss.spawn_position = glm::vec3(
                entry["spawn_x"].get<float>(),
                entry["spawn_y"].get<float>(),
                entry["spawn_z"].get<float>()
            );
            boss.respawn_hours = entry.value("respawn_hours", 4.0f);
            boss.min_party_size = entry.value("min_party_size", 1);
            for (auto& lt : entry["loot_table"])
                boss.loot_table.push_back(lt.get<uint32_t>());
            for (auto& ph : entry["phases"]) {
                BossPhase phase;
                phase.phase_id = ph["phase_id"].get<uint32_t>();
                phase.hp_threshold = ph["hp_threshold"].get<float>();
                phase.skill_id = ph.value("skill_id", 0);
                phase.duration = ph.value("duration", 30.0f);
                phase.name = ph.value("name", "Phase");
                boss.phases.push_back(phase);
            }
            field_bosses_.push_back(boss);
        }
        spdlog::info("FieldBossSystem: loaded {} field bosses", field_bosses_.size());
    } catch (const std::exception& e) {
        spdlog::warn("FieldBossSystem: failed to parse field boss data: {}", e.what());
    }
}

void FieldBossSystem::Update(entt::registry& registry, float dt) {
    for (auto it = respawn_timers_.begin(); it != respawn_timers_.end(); ) {
        it->second -= dt / 3600.0f;
        if (it->second <= 0.0f) {
            // Find boss data
            for (auto& boss : field_bosses_) {
                if (boss.id == it->first) {
                    SpawnFieldBoss(boss.id, boss.map_id, registry);
                    break;
                }
            }
            it = respawn_timers_.erase(it);
        } else {
            ++it;
        }
    }

    // Check active field boss entities for phase transitions
    auto view = registry.view<CharacterStats, Transform>();
    for (auto entity : view) {
        auto& stats = view.get<CharacterStats>(entity);
        if (!registry.all_of<TagMonster>(entity)) continue;

        // Check if this entity is a field boss (has field_boss tag or similar)
        for (auto& boss : field_bosses_) {
            if (static_cast<uint32_t>(entity) == boss.monster_id) {
                float hp_pct = static_cast<float>(stats.hp) / static_cast<float>(stats.max_hp);
                for (auto& phase : boss.phases) {
                    if (hp_pct <= phase.hp_threshold) {
                        spdlog::debug("FieldBoss {} entering {}", boss.id, phase.name);
                        break;
                    }
                }
            }
        }
    }
}

void FieldBossSystem::SpawnFieldBoss(uint32_t boss_id, int map_id, entt::registry& registry) {
    for (auto& boss : field_bosses_) {
        if (boss.id != boss_id) continue;

        auto entity = registry.create();
        CharacterStats stats;
        stats.level = 80;
        stats.max_hp = 500000;
        stats.hp = stats.max_hp;
        stats.physic_attack = 500.0f;
        stats.physic_defense = 300.0f;
        registry.emplace<CharacterStats>(entity, stats);
        registry.emplace<Transform>(entity, boss.spawn_position);
        registry.emplace<TagMonster>(entity);

        BroadcastFieldBossSpawn(boss_id, map_id, boss.spawn_position);
        spdlog::info("FieldBossSystem: spawned boss {} on map {}", boss_id, map_id);
        return;
    }
    spdlog::warn("FieldBossSystem: boss id {} not found in data", boss_id);
}

void FieldBossSystem::OnFieldBossDeath(entt::entity boss, entt::entity killer, entt::registry& registry) {
    uint32_t boss_id = 0;
    uint32_t monster_id = static_cast<uint32_t>(boss);

    for (auto& fb : field_bosses_) {
        if (fb.monster_id == monster_id) {
            boss_id = fb.id;
            respawn_timers_[fb.id] = fb.respawn_hours;

            std::string killer_name = "Unknown";
            if (auto* stats = registry.try_get<CharacterStats>(killer)) {
                killer_name = "Entity " + std::to_string(static_cast<uint32_t>(killer));
            }
            BroadcastFieldBossDeath(boss_id, killer_name);
            spdlog::info("FieldBossSystem: boss {} killed by {}, respawn in {:.1f}h",
                          fb.id, killer_name, fb.respawn_hours);
            break;
        }
    }
}

void FieldBossSystem::BroadcastFieldBossSpawn(uint32_t boss_id, int map_id, const glm::vec3& pos) {
    spdlog::info("FieldBossSystem: [BROADCAST] Field boss {} spawned on map {} at ({:.1f},{:.1f},{:.1f})",
                  boss_id, map_id, pos.x, pos.y, pos.z);
}

void FieldBossSystem::BroadcastFieldBossDeath(uint32_t boss_id, const std::string& killer_name) {
    spdlog::info("FieldBossSystem: [BROADCAST] Field boss {} has been defeated by {}!",
                  boss_id, killer_name);
}
