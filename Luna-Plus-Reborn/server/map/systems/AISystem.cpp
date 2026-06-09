#include "AISystem.h"
#include <spdlog/spdlog.h>
#include <random>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

static std::mt19937 ai_rng(std::random_device{}());

AISystem::AISystem()
    : group_manager_(std::make_unique<AIGroupManager>())
    , rng_(std::random_device{}())
{
}

AISystem::~AISystem() = default;

// ─── Monster ID Generator ──────────────────────────────────

uint32_t AISystem::GenerateMonsterId() {
    return next_monster_id_++;
}

void AISystem::ReleaseMonsterId(uint32_t id) {
    (void)id;
}

// ─── AIGroupManager ────────────────────────────────────────

AIGroup& AIGroupManager::AddGroup(uint32_t group_index, uint32_t grid_index) {
    GroupKey key{group_index, grid_index};
    auto it = groups_.find(key);
    if (it != groups_.end())
        return *it->second;

    auto group = std::make_unique<AIGroup>();
    group->SetGroupIndex(group_index);
    group->SetGridIndex(grid_index);
    AIGroup& ref = *group;
    groups_[key] = std::move(group);
    return ref;
}

AIGroup* AIGroupManager::GetGroup(uint32_t group_index, uint32_t grid_index) {
    GroupKey key{group_index, grid_index};
    auto it = groups_.find(key);
    return (it != groups_.end()) ? it->second.get() : nullptr;
}

void AIGroupManager::RegenProcess(entt::registry& registry, float dt) {
    for (auto& [key, group] : groups_) {
        group->UpdateRegen(dt);
        if (group->ReadyToRegen()) {
            spdlog::info("AIGroup {} (grid {}) regenerating", key.group_index, key.grid_index);
            group->ResetRegenTimer();
            for (const auto& monster : group->GetMonsters()) {
                (void)monster;
            }
        }
    }
}

// ─── Script Loading ─────────────────────────────────────────

void AISystem::LoadAiScript(const std::string& filepath) {
    script_spawns_.clear();
    unique_script_spawns_.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::warn("AISystem: cannot open script file: {}", filepath);
        return;
    }

    const uint32_t oneMinuteMs = 60000;
    AIGroupSpawn* current_spawn = nullptr;
    bool is_unique = false;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "group") {
            uint32_t idx;
            iss >> idx;
            is_unique = false;
            script_spawns_.emplace_back();
            current_spawn = &script_spawns_.back();
            current_spawn->group_index = idx;
            current_spawn->is_unique = false;
        }
        else if (token == "unique") {
            uint32_t idx;
            iss >> idx;
            is_unique = true;
            unique_script_spawns_.emplace_back();
            current_spawn = &unique_script_spawns_.back();
            current_spawn->group_index = idx;
            current_spawn->is_unique = true;
        }
        else if (token == "addCondition" && current_spawn) {
            AIGroupCondition cond;
            iss >> cond.target_group_index >> cond.ratio >> cond.delay_ms;
            int regen_flag;
            iss >> regen_flag;
            cond.regen = (regen_flag != 0);
            iss >> cond.range;
            if (cond.range < 1) cond.range = 1;
            current_spawn->conditions.push_back(cond);
        }
        else if (token == "add" && current_spawn) {
            AIGroupMonster mon;
            uint32_t obj_kind, mon_kind;
            float x, z;
            std::string machine;
            iss >> obj_kind >> mon_kind >> x >> z >> machine;
            mon.object_kind = obj_kind;
            mon.monster_kind = mon_kind;
            mon.position = glm::vec3(x, 0.0f, z);
            mon.machine_name = machine;
            current_spawn->monsters.push_back(mon);
        }
        else if (token == "fieldBossRegenPosition" && current_spawn) {
            float x, z;
            iss >> x >> z;
            current_spawn->field_boss_positions.emplace_back(x, 0.0f, z);
        }
        else if (token == "randomRegenDelay" && current_spawn) {
            uint32_t min_sec, max_sec;
            iss >> min_sec >> max_sec;
            current_spawn->regen_min_ms = min_sec * oneMinuteMs;
            current_spawn->regen_max_ms = max_sec * oneMinuteMs;
        }
        else if (token == "uniqueRegenDelay" && current_spawn) {
            uint32_t sec;
            iss >> sec;
            current_spawn->regen_min_ms = sec * oneMinuteMs;
            current_spawn->regen_max_ms = sec * oneMinuteMs;
        }
        else if (token == "uniqueRandomRegenDelay" && current_spawn) {
            uint32_t max_sec;
            iss >> max_sec;
            current_spawn->regen_min_ms = oneMinuteMs;
            current_spawn->regen_max_ms = max_sec * oneMinuteMs;
        }
        else if (token == "uniqueRandomRegenDelay2" && current_spawn) {
            uint32_t min_sec, max_sec;
            iss >> min_sec >> max_sec;
            current_spawn->regen_min_ms = min_sec * oneMinuteMs;
            current_spawn->regen_max_ms = max_sec * oneMinuteMs;
        }
    }

    spdlog::info("AISystem: loaded {} groups, {} unique groups from {}",
                 script_spawns_.size(), unique_script_spawns_.size(), filepath);
}

void AISystem::LoadAiScriptFromData(entt::registry& registry,
                                    const std::vector<AIGroupSpawn>& spawns,
                                    uint32_t grid_index) {
    for (const auto& spawn : spawns) {
        AIGroup& group = group_manager_->AddGroup(spawn.group_index, grid_index);
        for (const auto& cond : spawn.conditions) {
            group.AddCondition(cond.target_group_index, cond.ratio,
                              cond.delay_ms, cond.regen, cond.range);
        }
        for (const auto& mon : spawn.monsters) {
            group.AddMonster(mon.object_kind, mon.monster_kind, mon.position, mon.machine_name);
        }
        for (const auto& pos : spawn.field_boss_positions) {
            group.AddFieldBossPosition(pos);
        }
        group.SetRegenDelay(spawn.regen_min_ms, spawn.regen_max_ms);
        group.SetIsUnique(spawn.is_unique);
    }
}

// ─── Summon System ──────────────────────────────────────────

void AISystem::Summon(entt::registry& registry, uint32_t grid_index) {
    LoadAiScriptFromData(registry, script_spawns_, grid_index);
}

void AISystem::SummonOnAllChannels(entt::registry& registry, uint32_t channel_count) {
    for (uint32_t channel = 0; channel < channel_count; ++channel) {
        LoadAiScriptFromData(registry, script_spawns_, channel);
    }

    if (channel_count > 0 && !unique_script_spawns_.empty()) {
        uint32_t random_channel = std::uniform_int_distribution<uint32_t>(0, channel_count - 1)(rng_);
        LoadAiScriptFromData(registry, unique_script_spawns_, random_channel);
    }
}

void AISystem::SummonGroup(entt::registry& registry, const AIGroupSpawn& spawn, uint32_t grid_index) {
    AIGroup& group = group_manager_->AddGroup(spawn.group_index, grid_index);

    for (const auto& mon : spawn.monsters) {
        entt::entity entity = registry.create();
        auto& ai = registry.emplace<AIComponent>(entity);
        auto& stats = registry.emplace<CharacterStats>(entity);
        auto& xform = registry.emplace<Transform>(entity);

        uint32_t entity_id = GenerateMonsterId();
        ai.spawn_position = mon.position;
        xform.position = mon.position;
        ai.aggro_range = 10.0f;
        ai.attack_range = 3.0f;
        ai.chase_range = 30.0f;
        stats.max_hp = 100.0f;
        stats.hp = stats.max_hp;
        stats.move_speed = 3.0f;

        group.AddAliveEntity(entity_id);
        spdlog::debug("AISystem: spawned monster kind={} at ({}, {})",
                      mon.monster_kind, mon.position.x, mon.position.z);
    }
}

// ─── AI Update ──────────────────────────────────────────────

void AISystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<AIComponent, CharacterStats>();
    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        auto& stats = view.get<CharacterStats>(entity);

        if (stats.hp <= 0) {
            ai.state_timer += dt;
            if (ai.state_timer >= 30.0f) {
                stats.hp = stats.max_hp;
                ai.state = AIComponent::Idle;
                ai.state_timer = 0.0f;
                ai.ClearAggro();
                ai.enrage_timer = 0;
                if (auto* xform = registry.try_get<Transform>(entity))
                    xform->position = ai.spawn_position;
                spdlog::debug("AI: entity {} respawned", static_cast<uint32_t>(entity));
            }
            continue;
        }

        UpdateState(registry, entity, ai, stats, dt);
    }

    // Process group regen every 3 seconds
    regen_check_timer_ += dt * 1000.0f;
    if (regen_check_timer_ >= 3000.0f) {
        regen_check_timer_ = 0.0f;
        group_manager_->RegenProcess(registry, dt);
    }
}

void AISystem::UpdateState(entt::registry& registry, entt::entity entity,
                            AIComponent& ai, CharacterStats& stats, float dt) {
    ai.state_timer += dt;
    ai.skill_timer += dt;

    // Enrage timer for boss AI
    if (ai.aggro_target != 0) {
        ai.enrage_timer += dt;
    }

    switch (static_cast<int>(ai.state)) {
    case AIComponent::Idle:
        if (ai.state_timer > 3.0f + static_cast<float>(ai_rng() % 2000) / 1000.0f) {
            ai.state = AIComponent::Patrol;
            ai.state_timer = 0.0f;
        }
        FindNearestTarget(registry, entity, ai);
        break;

    case AIComponent::Patrol:
        Patrol(registry, entity, ai, stats, dt);
        FindNearestTarget(registry, entity, ai);
        break;

    case AIComponent::Chase:
        Chase(registry, entity, ai, stats, dt);
        break;

    case AIComponent::Attack:
        Attack(registry, entity, ai, stats, dt);
        break;

    case AIComponent::Flee:
        Flee(registry, entity, ai, stats, dt);
        break;

    case AIComponent::Return:
        ReturnToSpawn(registry, entity, ai, stats, dt);
        break;

    case AIComponent::Stun:
    case AIComponent::Sleep:
        // Wait for stun/sleep to expire (handled by CombatSystem status effects)
        if (ai.state_timer > 3.0f) {
            ai.state = AIComponent::Idle;
            ai.state_timer = 0.0f;
        }
        break;
    }
}

void AISystem::FindNearestTarget(entt::registry& registry, entt::entity entity, AIComponent& ai) {
    auto* xform = registry.try_get<Transform>(entity);
    if (!xform) return;

    // First check threat table for highest threat target
    uint32_t threat_target = ai.GetHighestThreat();
    if (threat_target != 0 && registry.valid(static_cast<entt::entity>(threat_target))) {
        auto* target_xform = registry.try_get<Transform>(static_cast<entt::entity>(threat_target));
        if (target_xform) {
            float dist = glm::distance(xform->position, target_xform->position);
            if (dist <= ai.aggro_range) {
                ai.aggro_target = threat_target;
                ai.state = AIComponent::Chase;
                ai.state_timer = 0.0f;
                return;
            }
        }
    }

    // Fallback: nearest player in aggro range
    float nearest_dist = ai.aggro_range;
    entt::entity nearest = entt::null;

    auto players = registry.view<Transform, CharacterStats, TagPlayer>();
    for (auto player : players) {
        auto& px = players.get<Transform>(player);
        float dist = glm::distance(xform->position, px.position);
        if (dist < nearest_dist) {
            nearest_dist = dist;
            nearest = player;
        }
    }

    if (nearest != entt::null) {
        uint32_t pid = static_cast<uint32_t>(entt::to_entity(nearest));
        ai.aggro_target = pid;
        ai.AddThreat(pid, 1);
        ai.state = AIComponent::Chase;
        ai.state_timer = 0.0f;
        spdlog::debug("AI: entity {} aggro on player {}", static_cast<uint32_t>(entity),
                      static_cast<uint32_t>(nearest));
    }
}

void AISystem::Patrol(entt::registry& registry, entt::entity entity,
                       AIComponent& ai, CharacterStats& stats, float dt) {
    ai.state_timer += dt;
    auto* xform = registry.try_get<Transform>(entity);
    if (!xform) return;

    if (ai.patrol_points.empty() && ai.state_timer > 1.0f) {
        glm::vec3 waypoint(
            ai.spawn_position.x + static_cast<float>(ai_rng() % 20 - 10),
            ai.spawn_position.y,
            ai.spawn_position.z + static_cast<float>(ai_rng() % 20 - 10));
        ai.patrol_points.push_back(waypoint);
        ai.state_timer = 0.0f;
    }

    if (!ai.patrol_points.empty()) {
        glm::vec3 target = ai.patrol_points.front();
        float dist = glm::distance(xform->position, target);
        if (dist < 1.0f) {
            ai.patrol_points.clear();
            ai.state = AIComponent::Idle;
            ai.state_timer = 0.0f;
        } else {
            glm::vec3 dir = glm::normalize(target - xform->position);
            xform->position += dir * stats.move_speed * 0.6f * dt;
        }
    } else if (ai.state_timer > 5.0f) {
        ai.state = AIComponent::Idle;
        ai.state_timer = 0.0f;
    }
}

void AISystem::Chase(entt::registry& registry, entt::entity entity,
                      AIComponent& ai, CharacterStats& stats, float dt) {
    if (ai.aggro_target == 0 || !registry.valid(static_cast<entt::entity>(ai.aggro_target))) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
        return;
    }

    auto* my_xform = registry.try_get<Transform>(entity);
    auto* target_xform = registry.try_get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    if (!my_xform || !target_xform) {
        ai.state = AIComponent::Return;
        return;
    }

    float dist = glm::distance(my_xform->position, target_xform->position);

    // Move toward target (simple linear movement)
    if (dist > ai.attack_range) {
        glm::vec3 dir = glm::normalize(target_xform->position - my_xform->position);
        my_xform->position += dir * stats.move_speed * dt;
    }

    if (dist <= ai.attack_range) {
        ai.state = AIComponent::Attack;
        ai.state_timer = 0.0f;
    } else if (dist > ai.chase_range) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
    }
}

void AISystem::Attack(entt::registry& registry, entt::entity entity,
                       AIComponent& ai, CharacterStats& stats, float dt) {
    if (ai.aggro_target == 0 || !registry.valid(static_cast<entt::entity>(ai.aggro_target))) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
        return;
    }

    auto* target_stats = registry.try_get<CharacterStats>(static_cast<entt::entity>(ai.aggro_target));
    if (!target_stats) {
        ai.state = AIComponent::Return;
        return;
    }

    // Attack every 2 seconds
    ai.skill_timer += dt;
    if (ai.skill_timer >= 2.0f) {
        ai.skill_timer = 0.0f;
        target_stats->hp = std::max(0.0f, target_stats->hp - stats.physic_attack);
        ai.AddThreat(ai.aggro_target, static_cast<int32_t>(stats.physic_attack));
        spdlog::debug("AI: entity {} attacks target {} for {} damage",
                      static_cast<uint32_t>(entity), ai.aggro_target, static_cast<int>(stats.physic_attack));

        if (target_stats->hp <= 0) {
            ai.state = AIComponent::Idle;
            ai.ClearAggro();
            ai.enrage_timer = 0;
        }
    }

    // Check flee threshold
    float hp_pct = static_cast<float>(stats.hp) / stats.max_hp;
    if (hp_pct < 0.2f) {
        ai.state = AIComponent::Flee;
        ai.ClearAggro();
    }

    // Boss phase check
    HandleBossPhase(registry, entity, ai, stats);

    // Enrage check
    HandleEnrage(registry, entity, ai, stats);
}

void AISystem::Flee(entt::registry& registry, entt::entity entity,
                     AIComponent& ai, CharacterStats& stats, float dt) {
    auto* my_xform = registry.try_get<Transform>(entity);
    if (!my_xform) { ai.state = AIComponent::Return; return; }

    // Move away from threat (toward spawn)
    glm::vec3 flee_dir = glm::normalize(ai.spawn_position - my_xform->position);
    my_xform->position += flee_dir * stats.move_speed * 1.5f * dt;

    ai.state_timer += dt;
    if (ai.state_timer > 5.0f || glm::distance(my_xform->position, ai.spawn_position) < 2.0f) {
        ai.state = AIComponent::Return;
        ai.state_timer = 0.0f;
    }
}

void AISystem::ReturnToSpawn(entt::registry& registry, entt::entity entity,
                              AIComponent& ai, CharacterStats& stats, float dt) {
    auto* xform = registry.try_get<Transform>(entity);
    if (!xform) return;

    glm::vec3 dir = glm::normalize(ai.spawn_position - xform->position);
    if (glm::length(dir) > 0.1f)
        xform->position += dir * stats.move_speed * dt;

    float dist = glm::distance(xform->position, ai.spawn_position);
    if (dist < 1.0f) {
        xform->position = ai.spawn_position;
        ai.state = AIComponent::Idle;
        ai.state_timer = 0.0f;
        ai.enrage_timer = 0;
    }
}

void AISystem::HandleBossPhase(entt::registry& registry, entt::entity entity,
                                AIComponent& ai, CharacterStats& stats) {
    // Only for boss monsters (detected by high HP threshold)
    if (stats.max_hp < 10000) return;

    float hp_pct = static_cast<float>(stats.hp) / stats.max_hp;
    int new_phase = 0;

    if (hp_pct <= 0.75f && hp_pct > 0.50f) new_phase = 1;
    else if (hp_pct <= 0.50f && hp_pct > 0.25f) new_phase = 2;
    else if (hp_pct <= 0.25f) new_phase = 3;

    if (new_phase > ai.current_patrol_index) {
        ai.current_patrol_index = new_phase;
        switch (new_phase) {
        case 1:
            stats.physic_attack = static_cast<int32_t>(stats.physic_attack * 1.3f);
            spdlog::info("BOSS PHASE 1: attack +30% (entity {})", static_cast<uint32_t>(entity));
            break;
        case 2:
            stats.move_speed *= 1.5f;
            ai.attack_range *= 1.2f;
            spdlog::info("BOSS PHASE 2: speed +50%, range +20% (entity {})", static_cast<uint32_t>(entity));
            break;
        case 3:
            stats.physic_attack = static_cast<int32_t>(stats.physic_attack * 1.5f);
            stats.physic_defense = static_cast<int32_t>(stats.physic_defense * 0.5f);
            spdlog::info("BOSS PHASE 3: attack +50%, defense -50% (entity {})", static_cast<uint32_t>(entity));
            break;
        }
    }
}

void AISystem::HandleEnrage(entt::registry& registry, entt::entity entity,
                             AIComponent& ai, CharacterStats& stats) {
    // Enrage after 180 seconds of combat
    if (ai.enrage_timer >= 180.0f) {
        stats.physic_attack = static_cast<int32_t>(stats.physic_attack * 2.0f);
        stats.physic_defense = 0;
        spdlog::warn("BOSS ENRAGED: entity {} (attack x2, defense = 0)", static_cast<uint32_t>(entity));
        ai.enrage_timer = 0; // reset to avoid spamming
    }
}
