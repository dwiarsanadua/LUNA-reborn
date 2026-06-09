#include "SpawnSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/SpawnInfo.hpp"
#include "../components/AIComponent.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <random>

static std::mt19937& GetRng() {
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

static std::string MonsterTypeToString(SpawnMonsterType t) {
    switch (t) {
        case SpawnMonsterType::Normal: return "Normal";
        case SpawnMonsterType::Elite: return "Elite";
        case SpawnMonsterType::Boss: return "Boss";
        case SpawnMonsterType::FieldBoss: return "FieldBoss";
        case SpawnMonsterType::SubMonster: return "SubMonster";
        default: return "Unknown";
    }
}

// ── Update: respawn queue ───────────────────────────────────────────────────

void SpawnSystem::Update(entt::registry& registry, float dt) {
    for (auto it = pending_respawns_.begin(); it != pending_respawns_.end(); ) {
        it->elapsed += dt;
        if (it->elapsed >= it->delay) {
            auto sp_it = std::find_if(spawn_points_.begin(), spawn_points_.end(),
                [&](const SpawnPoint& sp) { return sp.id == it->spawn_point_id; });
            if (sp_it != spawn_points_.end() && sp_it->current_count < sp_it->max_count) {
                SpawnSingle(registry, *sp_it);
                sp_it->current_count++;
                // Group spawn: spawn all members of the group
                if (sp_it->group_id > 0) {
                    auto git = group_spawn_indices_.find(sp_it->group_id);
                    if (git != group_spawn_indices_.end()) {
                        for (uint32_t gidx : git->second) {
                            auto& gsp = spawn_points_[gidx];
                            if (gsp.id != sp_it->id && gsp.current_count < gsp.max_count) {
                                SpawnSingle(registry, gsp);
                                gsp.current_count++;
                            }
                        }
                    }
                }
            }
            it = pending_respawns_.erase(it);
        } else {
            ++it;
        }
    }

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

// ── Spawn individual monster ────────────────────────────────────────────────

void SpawnSystem::SpawnMonster(entt::registry& registry, uint32_t monster_id,
                                const glm::vec3& pos, uint16_t level) {
    auto entity = registry.create();
    auto& xform = registry.emplace<Transform>(entity);
    xform.position = pos;
    auto& stats = registry.emplace<CharacterStats>(entity);
    stats.level = level;

    auto* tmpl = GetMonsterTemplate(monster_id);
    if (tmpl) {
        stats.max_hp = tmpl->hp;
        stats.hp = tmpl->hp;
        stats.physic_attack = tmpl->attack;
        stats.physic_defense = tmpl->defense;
        stats.move_speed = tmpl->move_speed;
    } else {
        stats.max_hp = 50 + level * 10;
        stats.hp = stats.max_hp;
        stats.physic_attack = 8 + level * 2;
        stats.physic_defense = 3 + level;
    }
    registry.emplace<TagMonster>(entity);
}

void SpawnSystem::DespawnEntity(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) registry.destroy(entity);
}

// ── Load spawn data from JSON ───────────────────────────────────────────────

void SpawnSystem::LoadSpawnData(const std::string& json_path) {
    spawn_points_.clear();
    map_spawn_indices_.clear();
    group_spawn_indices_.clear();
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

    if (root.is_array()) {
        ParseMonsterArray(root);
    } else if (root.is_object()) {
        if (root.contains("spawns") && root["spawns"].is_array()) {
            for (const auto& sp : root["spawns"])
                ParseSpawnItem(sp, 0u);
        } else {
            spdlog::error("SpawnSystem: unsupported JSON structure in {}", json_path);
            return;
        }
    } else {
        spdlog::error("SpawnSystem: unsupported JSON root in {}", json_path);
        return;
    }

    spdlog::info("SpawnSystem: loaded {} spawn points ({} maps) from {}",
                 spawn_points_.size(), map_spawn_indices_.size(), json_path);
}

void SpawnSystem::ParseMonsterArray(const json& arr) {
    for (const auto& item : arr) {
        if (item.contains("spawns") && item["spawns"].is_array()) {
            uint32_t monster_id = item.value("id", item.value("monster_id", 0u));
            for (const auto& sp : item["spawns"])
                ParseSpawnItem(sp, monster_id);
        } else if (item.contains("map_id") || item.contains("map")) {
            ParseSpawnItem(item, item.value("monster_id", 0u));
        }
    }
}

void SpawnSystem::ParseSpawnItem(const json& item, uint32_t monster_id_default) {
    SpawnPoint sp;
    sp.id            = item.value("id", static_cast<uint32_t>(spawn_points_.size() + 1));
    sp.map_id        = item.value("map_id", item.value("map", 0));
    sp.monster_id    = item.value("monster_id", monster_id_default);
    sp.x             = item.value("x", item.value("pos_x", 0.0f));
    sp.y             = item.value("y", item.value("pos_y", 0.0f));
    sp.z             = item.value("z", item.value("pos_z", 0.0f));
    sp.respawn_time  = item.value("respawn_time", item.value("respawn", 10.0f));
    sp.respawn_variance = item.value("respawn_variance", 0.0f);
    sp.max_count     = item.value("max_count", item.value("count", 1));
    sp.current_count = 0;
    sp.aggro_range   = item.value("aggro_range", 10.0f);
    sp.patrol_radius = item.value("patrol_radius", 0u);
    sp.spawn_radius  = item.value("spawn_radius", item.value("spawn_range", 0.0f));
    sp.level         = static_cast<uint16_t>(item.value("level", 1));
    sp.group_id      = item.value("group_id", 0u);

    // Determine monster type
    std::string type_str = item.value("type", item.value("monster_type", "normal"));
    if (type_str == "boss" || type_str == "Boss") {
        sp.monster_type = SpawnMonsterType::Boss;
        sp.is_boss = true;
    } else if (type_str == "fieldboss" || type_str == "FieldBoss") {
        sp.monster_type = SpawnMonsterType::FieldBoss;
        sp.is_boss = true;
    } else if (type_str == "elite" || type_str == "Elite") {
        sp.monster_type = SpawnMonsterType::Elite;
    } else if (type_str == "sub" || type_str == "SubMonster") {
        sp.monster_type = SpawnMonsterType::SubMonster;
    } else {
        sp.monster_type = SpawnMonsterType::Normal;
    }

    if (sp.map_id == 0 || sp.monster_id == 0) return;
    AddSpawnPoint(sp);
}

void SpawnSystem::AddSpawnPoint(const SpawnPoint& sp) {
    spawn_points_.push_back(sp);
    uint32_t idx = static_cast<uint32_t>(spawn_points_.size() - 1);
    map_spawn_indices_[sp.map_id].push_back(idx);
    if (sp.group_id > 0)
        group_spawn_indices_[sp.group_id].push_back(idx);
}

// ── Load monster templates ──────────────────────────────────────────────────

void SpawnSystem::LoadMonsterTemplates(const std::string& json_path) {
    monster_templates_.clear();
    std::ifstream f(json_path);
    if (!f.is_open()) {
        spdlog::warn("SpawnSystem: cannot open monster templates {}", json_path);
        return;
    }
    json root;
    try {
        f >> root;
    } catch (...) {
        spdlog::error("SpawnSystem: JSON parse error in {}", json_path);
        return;
    }

    int count = 0;
    auto parse_one = [&](const json& item) {
        SpawnMonsterTemplate mt;
        mt.id = item.value("id", item.value("monster_id", 0u));
        if (mt.id == 0) return;
        mt.name = item.value("name", "");
        mt.level = static_cast<uint16_t>(item.value("level", 1));
        mt.hp = item.value("hp", 100);
        mt.attack = item.value("attack", item.value("physic_attack", 10));
        mt.defense = item.value("defense", item.value("physic_defense", 5));
        mt.move_speed = item.value("move_speed", 3.5f);
        mt.aggro_range = item.value("aggro_range", 10.0f);
        mt.exp_reward = item.value("exp_reward", item.value("exp", 0));
        mt.gold_reward = item.value("gold_reward", item.value("gold", 0));

        std::string type_str = item.value("type", item.value("monster_type", "normal"));
        if (type_str == "boss" || type_str == "Boss" || item.value("is_boss", false)) {
            mt.type = SpawnMonsterType::Boss;
            mt.is_boss = true;
        } else if (type_str == "fieldboss" || type_str == "FieldBoss") {
            mt.type = SpawnMonsterType::FieldBoss;
            mt.is_boss = true;
        } else if (type_str == "elite" || type_str == "Elite") {
            mt.type = SpawnMonsterType::Elite;
        } else {
            mt.type = SpawnMonsterType::Normal;
        }

        monster_templates_[mt.id] = mt;
        count++;
    };

    if (root.is_array()) {
        for (const auto& item : root) parse_one(item);
    } else if (root.is_object()) {
        parse_one(root);
    }

    spdlog::info("SpawnSystem: loaded {} monster templates from {}", count, json_path);
}

const SpawnMonsterTemplate* SpawnSystem::GetMonsterTemplate(uint32_t monster_id) const {
    auto it = monster_templates_.find(monster_id);
    return it != monster_templates_.end() ? &it->second : nullptr;
}

// ── Spawn monsters for a map ────────────────────────────────────────────────

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

// ── Spawn group ─────────────────────────────────────────────────────────────

void SpawnSystem::SpawnGroup(entt::registry& registry, uint32_t group_id) {
    auto git = group_spawn_indices_.find(group_id);
    if (git == group_spawn_indices_.end()) {
        spdlog::warn("SpawnSystem: no spawn points for group {}", group_id);
        return;
    }
    int count = 0;
    for (uint32_t idx : git->second) {
        auto& sp = spawn_points_[idx];
        for (int i = 0; i < sp.max_count; ++i) {
            SpawnSingle(registry, sp);
            sp.current_count++;
            count++;
        }
    }
    spdlog::info("SpawnSystem: spawned {} monsters for group {}", count, group_id);
}

void SpawnSystem::RespawnMonster(entt::registry& registry, uint32_t spawn_id, float delay) {
    // Add random variance to respawn time
    auto sp_it = std::find_if(spawn_points_.begin(), spawn_points_.end(),
        [&](const SpawnPoint& sp) { return sp.id == spawn_id; });
    float actual_delay = delay;
    if (sp_it != spawn_points_.end() && sp_it->respawn_variance > 0.0f) {
        std::uniform_real_distribution<float> dist(
            -sp_it->respawn_variance, sp_it->respawn_variance);
        actual_delay = std::max(0.5f, delay + dist(GetRng()));
    }
    pending_respawns_.push_back({spawn_id, actual_delay, 0.0f});
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

// ── Generate random position within spawn radius ────────────────────────────

glm::vec3 SpawnSystem::RandomPosition(const SpawnPoint& sp) const {
    if (sp.spawn_radius <= 0.0f)
        return glm::vec3(sp.x, sp.y, sp.z);

    std::uniform_real_distribution<float> angle_dist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> radius_dist(0.0f, sp.spawn_radius);
    float angle = angle_dist(GetRng());
    float dist = radius_dist(GetRng());
    return glm::vec3(
        sp.x + std::cos(angle) * dist,
        sp.y,
        sp.z + std::sin(angle) * dist
    );
}

// ── Create single monster entity ────────────────────────────────────────────

entt::entity SpawnSystem::SpawnSingle(entt::registry& registry, const SpawnPoint& sp) {
    auto entity = registry.create();
    glm::vec3 pos = RandomPosition(sp);
    auto& xform = registry.emplace<Transform>(entity);
    xform.position = pos;

    auto& stats = registry.emplace<CharacterStats>(entity);
    auto* tmpl = GetMonsterTemplate(sp.monster_id);

    if (tmpl) {
        stats.level = tmpl->level;
        stats.max_hp = tmpl->hp;
        stats.hp = tmpl->hp;
        stats.physic_attack = tmpl->attack;
        stats.physic_defense = tmpl->defense;
        stats.move_speed = tmpl->move_speed;
    } else {
        uint16_t lvl = sp.level > 0 ? sp.level : 1;
        stats.level = lvl;
        stats.max_hp = 50 + lvl * 10;
        stats.hp = stats.max_hp;
        stats.physic_attack = 8 + lvl * 2;
        stats.physic_defense = 3 + lvl;
        stats.move_speed = 3.5f;
    }

    registry.emplace<TagMonster>(entity);

    auto& ai = registry.emplace<AIComponent>(entity);
    ai.spawn_position = pos;
    ai.aggro_range = sp.aggro_range > 0.0f ? sp.aggro_range :
                     (tmpl ? tmpl->aggro_range : 12.0f);
    ai.attack_range = 2.5f;
    ai.chase_range = 30.0f;
    ai.is_boss = sp.is_boss;

    auto& sinfo = registry.emplace<SpawnInfo>(entity);
    sinfo.spawn_rule_id = sp.id;
    sinfo.monster_id = sp.monster_id;
    sinfo.level = stats.level;
    sinfo.quantity = 1;
    sinfo.respawn_time = sp.respawn_time;
    sinfo.respawn_timer = 0.0f;
    sinfo.is_alive = true;
    sinfo.is_boss = sp.is_boss;

    return entity;
}
