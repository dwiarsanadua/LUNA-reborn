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
        stats.hp = std::min(stats.hp + static_cast<int32_t>(stats.max_hp * 0.05f), stats.max_hp);
        spdlog::info("Boss {} entered phase {} (HP: {:.1f}%)",
                     static_cast<uint32_t>(e), new_phase, hp_pct);
    }

    // --- Enrage countdown ---
    bool enraged = false;
    if (ai.enrage_threshold > 0.0f) {
        ai.enrage_timer += dt;
        if (ai.enrage_timer >= ai.enrage_threshold) {
            enraged = true;
        }
    }

    // --- Phase-specific stat modifiers & behaviors ---
    float atk_mult = 1.0f;
    float def_mult = 1.0f;
    float speed_mult = 1.0f;
    float cd_mult = 1.0f;

    switch (ai.boss_phase) {
        case 1:
            cd_mult = 1.0f;
            break;
        case 2:
            cd_mult = 0.9f;
            break;
        case 3: {
            cd_mult = 0.8f;
            atk_mult = 1.15f;
            if (!enraged && static_cast<int>(ai.special_attack_timer * 10) % 8 == 0) {
                // AOE attack every 8s in phase 3
                auto boss_view = reg.view<Transform, CharacterStats, TagPlayer>();
                for (auto player : boss_view) {
                    auto& p_xform = boss_view.get<Transform>(player);
                    auto& boss_xform = reg.get<Transform>(e);
                    float d = glm::distance(boss_xform.position, p_xform.position);
                    if (d < ai.attack_range * 2.0f) {
                        int32_t aoe_dmg = static_cast<int32_t>(stats.physic_attack * 1.5f);
                        reg.get<CharacterStats>(player).hp = std::max(0, reg.get<CharacterStats>(player).hp - aoe_dmg);
                        spdlog::debug("Boss AOE hits player {} for {}", static_cast<uint32_t>(player), aoe_dmg);
                    }
                }
            }
            break;
        }
        case 4:
            cd_mult = 0.7f;
            atk_mult = 1.50f;
            def_mult = 0.50f;
            break;
        case 5:
            cd_mult = 0.5f;
            atk_mult = 2.0f;
            def_mult = 0.25f;
            speed_mult = 2.0f;
            break;
    }

    if (enraged) {
        atk_mult *= 2.0f;
        def_mult *= 0.25f;
        speed_mult *= 2.0f;
    }

    // Apply stat multipliers
    stats.physic_attack = stats.physic_attack * (1.0f + (atk_mult - 1.0f) * dt * 0.1f);
    stats.physic_defense = stats.physic_defense * (1.0f - (1.0f - def_mult) * dt * 0.1f);
    stats.magic_attack = stats.magic_attack * (1.0f + (atk_mult - 1.0f) * dt * 0.1f);
    stats.magic_defense = stats.magic_defense * (1.0f - (1.0f - def_mult) * dt * 0.1f);

    // --- Determine boss type from monster_id via boss_definitions_ ---
    std::string boss_type;
    auto it = boss_definitions_.find(static_cast<uint32_t>(e));
    if (it != boss_definitions_.end()) {
        for (auto& ab : it->second.special_abilities) {
            if (boss_type.empty()) boss_type = ab;
        }
    }
    // Detect type from abilities or monster_id
    uint32_t mid = static_cast<uint32_t>(e);
    bool is_arach = (mid == 9701);
    bool is_dragonian = (mid == 9702);
    bool is_leostein = (mid == 9703);
    bool is_tarintus = (mid == 9704);
    bool is_kierra = (mid == 9705);

    // --- Special attack rotation ---
    ai.special_attack_timer += dt;
    float cd = ai.special_attack_cooldown * cd_mult;

    if (ai.special_attack_timer >= cd) {
        ai.special_attack_timer = 0.0f;

        if (!reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
            ScanForTargets(reg, e, ai, reg.get<Transform>(e));
        }

        if (reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
            auto* combat = reg.try_get<CombatState>(e);
            if (!combat) {
                combat = &reg.emplace<CombatState>(e);
            }
            if (!combat->is_casting && !combat->is_animation_locked) {
                uint16_t skill_id = static_cast<uint16_t>(1000 + ai.boss_phase * 100 + mid % 100);
                float cast_time = std::max(0.3f, 1.5f - ai.boss_phase * 0.2f);

                // Boss-type specific special attacks
                if (is_arach) {
                    if (ai.boss_phase >= 2) {
                        // Poison spit — AOE poison damage
                        auto player_view = reg.view<Transform, CharacterStats, TagPlayer>();
                        for (auto p : player_view) {
                            auto& px = player_view.get<Transform>(p);
                            auto& boss_xform = reg.get<Transform>(e);
                            if (glm::distance(px.position, boss_xform.position) < ai.attack_range * 2.5f) {
                                int32_t poison_dmg = static_cast<int32_t>(stats.physic_attack * 0.8f);
                                player_view.get<CharacterStats>(p).hp = std::max(0, player_view.get<CharacterStats>(p).hp - poison_dmg);
                                spdlog::debug("Arach poison spit hits player {} for {}", static_cast<uint32_t>(p), poison_dmg);
                            }
                        }
                    }
                    if (ai.boss_phase >= 3) {
                        // Web trap — root player (simulated as reducing speed to 0)
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            target_stats->move_speed *= 0.1f;
                            spdlog::debug("Arach web trap roots player {}", ai.aggro_target);
                        }
                    }
                } else if (is_dragonian) {
                    if (ai.boss_phase >= 2) {
                        // Fire breath — cone AOE
                        auto player_view = reg.view<Transform, CharacterStats, TagPlayer>();
                        for (auto p : player_view) {
                            auto& px = player_view.get<Transform>(p);
                            auto& boss_xform = reg.get<Transform>(e);
                            if (glm::distance(px.position, boss_xform.position) < ai.attack_range * 2.0f) {
                                int32_t fire_dmg = static_cast<int32_t>(stats.physic_attack * 1.2f);
                                player_view.get<CharacterStats>(p).hp = std::max(0, player_view.get<CharacterStats>(p).hp - fire_dmg);
                                spdlog::debug("Dragonian fire breath hits player {} for {}", static_cast<uint32_t>(p), fire_dmg);
                            }
                        }
                    }
                    if (ai.boss_phase >= 3) {
                        // Tail sweep — knockback (simulated as stun)
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            target_stats->move_speed = 0.0f;
                        }
                    }
                } else if (is_leostein) {
                    if (ai.boss_phase >= 2) {
                        // Lightning strike — single target high damage
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            int32_t lightning_dmg = static_cast<int32_t>(stats.magic_attack * 2.5f);
                            target_stats->hp = std::max(0, target_stats->hp - lightning_dmg);
                            spdlog::debug("Leostein lightning strike hits player {} for {}", ai.aggro_target, lightning_dmg);
                        }
                    }
                    if (ai.boss_phase >= 3) {
                        // Roar — fear (reduces accuracy)
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            target_stats->accuracy *= 0.5f;
                            spdlog::debug("Leostein roar fears player {}", ai.aggro_target);
                        }
                    }
                } else if (is_tarintus) {
                    if (ai.boss_phase >= 2) {
                        // Ice shard — slow
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            target_stats->move_speed *= 0.4f;
                            spdlog::debug("Tarintus ice shard slows player {}", ai.aggro_target);
                        }
                    }
                    if (ai.boss_phase >= 3) {
                        // Blizzard — AOE
                        auto player_view = reg.view<Transform, CharacterStats, TagPlayer>();
                        for (auto p : player_view) {
                            auto& px = player_view.get<Transform>(p);
                            auto& boss_xform = reg.get<Transform>(e);
                            if (glm::distance(px.position, boss_xform.position) < ai.attack_range * 2.5f) {
                                int32_t ice_dmg = static_cast<int32_t>(stats.magic_attack * 1.8f);
                                player_view.get<CharacterStats>(p).hp = std::max(0, player_view.get<CharacterStats>(p).hp - ice_dmg);
                                spdlog::debug("Tarintus blizzard hits player {} for {}", static_cast<uint32_t>(p), ice_dmg);
                            }
                        }
                    }
                } else if (is_kierra) {
                    if (ai.boss_phase >= 2) {
                        // Dark bolt — magic damage
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            int32_t dark_dmg = static_cast<int32_t>(stats.magic_attack * 2.0f);
                            target_stats->hp = std::max(0, target_stats->hp - dark_dmg);
                            spdlog::debug("Kierra dark bolt hits player {} for {}", ai.aggro_target, dark_dmg);
                        }
                    }
                    if (ai.boss_phase >= 3) {
                        // Life drain — heals self
                        auto target_stats = reg.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
                        if (target_stats) {
                            int32_t drain_dmg = static_cast<int32_t>(stats.magic_attack * 1.5f);
                            target_stats->hp = std::max(0, target_stats->hp - drain_dmg);
                            int32_t heal = static_cast<int32_t>(drain_dmg * 0.6f);
                            stats.hp = std::min(stats.max_hp, stats.hp + heal);
                            spdlog::debug("Kierra life drain steals {} HP from player {}, heals for {}", drain_dmg, ai.aggro_target, heal);
                        }
                    }
                }

                // General special attack per phase
                combat->StartCast(skill_id, cast_time,
                                  static_cast<entt::entity>(ai.aggro_target));
                spdlog::info("Boss {} uses special attack skill {} (phase {}, cd_mult: {:.2f})",
                             static_cast<uint32_t>(e), skill_id, ai.boss_phase, cd_mult);
            }
        }
    }

    // --- Enraged berserk pulse ---
    if (enraged) {
        spdlog::warn("Boss {} is ENRAGED! +100% ATK, +100% speed, -75% DEF, berserk!",
                     static_cast<uint32_t>(e));
        // Every 3s while enraged, deal burst damage to all nearby players
        if (static_cast<int>(ai.special_attack_timer * 10) % 30 == 0) {
            auto player_view = reg.view<Transform, CharacterStats, TagPlayer>();
            for (auto p : player_view) {
                auto& px = player_view.get<Transform>(p);
                auto& boss_xform = reg.get<Transform>(e);
                if (glm::distance(px.position, boss_xform.position) < ai.attack_range * 3.0f) {
                    int32_t burst = static_cast<int32_t>(stats.physic_attack * 0.5f);
                    player_view.get<CharacterStats>(p).hp = std::max(0, player_view.get<CharacterStats>(p).hp - burst);
                    spdlog::debug("Enrage burst hits player {} for {}", static_cast<uint32_t>(p), burst);
                }
            }
        }
    }
}
