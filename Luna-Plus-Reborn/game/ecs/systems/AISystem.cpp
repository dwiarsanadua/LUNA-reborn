#include "AISystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/AIComponent.hpp"
#include "../components/CombatState.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"
#include "BossData.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <fstream>

// 3.2B: NavMesh placeholder for pathfinding integration
static glm::vec3 QueryNavMeshPath(const glm::vec3& start, const glm::vec3& end) {
    return end; 
}

void AISystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<AIComponent, Transform, CharacterStats>();
    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        auto& xform = view.get<Transform>(entity);

        switch (ai.state) {
            case AIComponent::Idle:  UpdateIdle(registry, entity, ai, dt); break;
            case AIComponent::Patrol: {
                auto& mv = registry.get<Movement>(entity);
                UpdatePatrol(registry, entity, ai, xform, mv, dt);
                break;
            }
            case AIComponent::Chase: {
                auto& mv = registry.get<Movement>(entity);
                UpdateChase(registry, entity, ai, xform, mv, dt);
                break;
            }
            case AIComponent::Attack: UpdateAttack(registry, entity, ai, xform, dt); break;
            case AIComponent::Return: {
                auto& mv = registry.get<Movement>(entity);
                UpdateReturn(registry, entity, ai, xform, mv, dt);
                break;
            }
            default: break;
        }
        ai.state_timer += dt;

        if (ai.is_boss) {
            auto& stats = registry.get<CharacterStats>(entity);
            HandleBossAI(registry, entity, ai, stats, dt);
        }
    }
}

void AISystem::UpdateIdle(entt::registry& reg, entt::entity e, AIComponent& ai, float dt) {
    ScanForTargets(reg, e, ai, reg.get<Transform>(e));
    if (!ai.patrol_points.empty() && ai.state_timer > 3.0f) {
        TransitionState(ai, AIComponent::Patrol);
    }
}

void AISystem::UpdatePatrol(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    ScanForTargets(reg, e, ai, xform);
    if (ai.patrol_points.empty()) { TransitionState(ai, AIComponent::Idle); return; }
    size_t idx = ai.current_patrol_index % ai.patrol_points.size();
    glm::vec3 target = ai.patrol_points[idx];
    float dist = glm::distance(xform.position, target);
    if (dist < 1.0f) {
        ai.current_patrol_index++;
        ai.patrol_wait_timer = 0.0f;
        TransitionState(ai, AIComponent::Idle);
    } else {
        mv.destination = target;
        mv.is_moving = true;
    }
}

void AISystem::UpdateChase(entt::registry& reg, entt::entity e,
                            AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    if (!reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    auto& target_xform = reg.get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    float dist = glm::distance(xform.position, target_xform.position);
    if (dist > ai.chase_range) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    if (dist <= ai.attack_range) {
        mv.is_moving = false;
        TransitionState(ai, AIComponent::Attack);
        return;
    }
    
    // 3.2B: Use NavMesh for pathfinding
    mv.destination = QueryNavMeshPath(xform.position, target_xform.position);
    mv.current_speed = mv.speed > 0 ? mv.speed : 5.0f;
    mv.is_moving = true;
}

void AISystem::UpdateAttack(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, float dt) {
    if (!reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    
    auto& target_xform = reg.get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    float dist = glm::distance(xform.position, target_xform.position);
    
    if (dist > ai.attack_range * 1.2f) {
        TransitionState(ai, AIComponent::Chase);
        return;
    }

    // Trigger CombatSystem if not already acting
    auto* combat = reg.try_get<CombatState>(e);
    if (!combat) {
        combat = &reg.emplace<CombatState>(e);
    }

    if (!combat->is_casting && !combat->is_animation_locked) {
        // Basic attack (id 0) with 0.5s cast time
        combat->StartCast(0, 0.5f, static_cast<entt::entity>(ai.aggro_target));
        spdlog::debug("Monster {} initiated attack on player {}", static_cast<uint32_t>(e), ai.aggro_target);
    }
}

void AISystem::UpdateReturn(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    float dist = glm::distance(xform.position, ai.spawn_position);
    if (dist < 1.0f) {
        xform.position = ai.spawn_position;
        mv.is_moving = false;
        ai.ClearAggro();
        TransitionState(ai, AIComponent::Patrol);
    } else {
        mv.destination = ai.spawn_position;
        mv.is_moving = true;
    }
}

void AISystem::ScanForTargets(entt::registry& reg, entt::entity e,
                               AIComponent& ai, const Transform& xform) {
    auto view = reg.view<Transform, CharacterStats, TagPlayer>();
    for (auto target : view) {
        auto& t_xform = view.get<Transform>(target);
        float dist = glm::distance(xform.position, t_xform.position);
        if (dist < ai.aggro_range) {
            ai.aggro_target = static_cast<uint32_t>(target);
            ai.AddThreat(ai.aggro_target, 100);
            spdlog::info("Monster {} aggroed on player {} (dist: {:.1f})", static_cast<uint32_t>(e), ai.aggro_target, dist);
            TransitionState(ai, AIComponent::Chase);
            return;
        }
    }
}

void AISystem::TransitionState(AIComponent& ai, AIComponent::State new_state) {
    if (ai.state != new_state) {
        spdlog::debug("Monster AI state transition: {} -> {}", static_cast<int>(ai.state), static_cast<int>(new_state));
        ai.state = new_state;
        ai.state_timer = 0.0f;
    }
}

void AISystem::LoadBossDefinitions(const std::string& json_path) {
    boss_definitions_ = CreateDefaultBossDefinitions();
    std::ifstream file(json_path);
    if (!file.is_open()) {
        spdlog::warn("Could not open boss JSON: {}, using defaults", json_path);
        return;
    }
    try {
        nlohmann::json j;
        file >> j;
        for (auto& item : j) {
            uint32_t id = item["id"].get<uint32_t>();
            BossDefinition def;
            def.monster_id = id;
            def.name = item.value("name", "Unknown Boss");
            def.map_id = item.value("map_id", 32);
            def.spawn_x = item.value("spawn_x", 0.0f);
            def.spawn_y = item.value("spawn_y", 0.0f);
            def.spawn_z = item.value("spawn_z", 0.0f);
            def.respawn_time = item.value("respawn_time", 300.0f);
            def.has_enrage = item.value("has_enrage", false);
            def.enrage_time = item.value("enrage_time", 180.0f);
            def.min_party_size = item.value("min_party_size", 1.0f);
            def.loot_table_id = item.value("loot_table_id", "default_boss_loot");
            if (item.contains("phase_hp")) {
                for (auto& v : item["phase_hp"])
                    def.phase_hp.push_back(v.get<float>());
            }
            if (item.contains("special_abilities")) {
                for (auto& v : item["special_abilities"])
                    def.special_abilities.push_back(v.get<std::string>());
            }
            boss_definitions_[id] = def;
        }
        spdlog::info("Loaded {} boss definitions from {}", boss_definitions_.size(), json_path);
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse boss JSON: {}", e.what());
    }
}

void AISystem::HandleBossAI(entt::registry& reg, entt::entity e,
                             AIComponent& ai, CharacterStats& stats, float dt) {
    if (!ai.is_boss) return;

    float hp_pct = stats.max_hp > 0
        ? static_cast<float>(stats.hp) / static_cast<float>(stats.max_hp) * 100.0f
        : 0.0f;

    // --- Phase transitions based on HP thresholds ---
    int new_phase = ai.boss_phase;
    if (hp_pct <= 10.0f && ai.boss_phase < 5) new_phase = 5;
    else if (hp_pct <= 25.0f && ai.boss_phase < 4) new_phase = 4;
    else if (hp_pct <= 50.0f && ai.boss_phase < 3) new_phase = 3;
    else if (hp_pct <= 75.0f && ai.boss_phase < 2) new_phase = 2;
    else if (ai.boss_phase == 0) new_phase = 1;

    if (new_phase != ai.boss_phase) {
        ai.boss_phase = new_phase;
        auto boss_state = static_cast<AIComponent::State>(
            AIComponent::BossPhase1 + (new_phase - 1));
        TransitionState(ai, boss_state);
        spdlog::info("Boss {} entered phase {} (HP: {:.1f}%)",
                     static_cast<uint32_t>(e), new_phase, hp_pct);
    }

    // --- Enrage countdown ---
    if (ai.enrage_threshold > 0.0f) {
        ai.enrage_timer += dt;
        bool is_enraged = (ai.enrage_timer >= ai.enrage_threshold);
        if (is_enraged) {
            // +50% ATK, -50% DEF while enraged
            stats.physic_attack *= 1.005f;
            stats.magic_attack *= 1.005f;
            stats.magic_defense *= 0.995f;
            stats.physic_defense *= 0.995f;
            ai.enrage_timer = 0.0f;
            spdlog::warn("Boss {} is ENRAGED!", static_cast<uint32_t>(e));
        }
    }

    // --- Special attack rotation ---
    ai.special_attack_timer += dt;
    float cd = ai.special_attack_cooldown;
    if (ai.boss_phase >= 5) cd *= 0.5f;
    else if (ai.boss_phase >= 4) cd *= 0.75f;

    if (ai.special_attack_timer >= cd) {
        ai.special_attack_timer = 0.0f;
        if (reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
            auto& target_xform = reg.get<Transform>(
                static_cast<entt::entity>(ai.aggro_target));
            auto& boss_xform = reg.get<Transform>(e);
            float dist = glm::distance(boss_xform.position, target_xform.position);
            if (dist < ai.attack_range * 3.0f) {
                auto* combat = reg.try_get<CombatState>(e);
                if (!combat) {
                    combat = &reg.emplace<CombatState>(e);
                }
                if (!combat->is_casting) {
                    // Map phase to skill_id: 1000 + phase * 100
                    uint16_t skill_id = static_cast<uint16_t>(1000 + ai.boss_phase * 100);
                    float cast_time = 1.0f;
                    if (ai.boss_phase >= 5) cast_time = 0.3f;
                    else if (ai.boss_phase >= 4) cast_time = 0.5f;
                    combat->StartCast(skill_id, cast_time,
                                      static_cast<entt::entity>(ai.aggro_target));
                    spdlog::info("Boss {} uses special attack skill {} (phase {})",
                                 static_cast<uint32_t>(e), skill_id, ai.boss_phase);
                }
            }
        }
    }

    // --- Phase-specific behavior ---
    switch (ai.boss_phase) {
        case 1:
            break;
        case 2: {
            ai.special_attack_cooldown = 7.0f;
            break;
        }
        case 3: {
            ai.special_attack_cooldown = 6.0f;
            ai.attack_range = ai.attack_range * 1.3f;
            break;
        }
        case 4: {
            ai.special_attack_cooldown = 5.0f;
            ai.aggro_range *= 1.2f;
            break;
        }
        case 5: {
            ai.special_attack_cooldown = 3.0f;
            ai.aggro_range *= 1.5f;
            break;
        }
        default:
            break;
    }
}
