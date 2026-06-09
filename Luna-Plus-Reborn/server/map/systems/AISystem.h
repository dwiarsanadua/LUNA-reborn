// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components/AIComponent.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/Tag.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

struct AIGroupCondition {
    uint32_t target_group_index = 0;
    float ratio = 1.0f;
    uint32_t delay_ms = 0;
    bool regen = true;
    uint32_t range = 10;
};

struct AIGroupMonster {
    uint32_t object_kind = 0;
    uint32_t monster_kind = 0;
    glm::vec3 position{0.0f};
    std::string machine_name;
};

struct AIGroupSpawn {
    uint32_t group_index = 0;
    std::vector<AIGroupCondition> conditions;
    std::vector<AIGroupMonster> monsters;
    std::vector<glm::vec3> field_boss_positions;
    uint32_t regen_min_ms = 60000;
    uint32_t regen_max_ms = 300000;
    bool is_unique = false;
};

class AIGroup {
public:
    AIGroup() = default;

    void SetGroupIndex(uint32_t idx) { group_index_ = idx; }
    uint32_t GetGroupIndex() const { return group_index_; }

    void SetGridIndex(uint32_t idx) { grid_index_ = idx; }
    uint32_t GetGridIndex() const { return grid_index_; }

    void SetRegenDelay(uint32_t min_ms, uint32_t max_ms) {
        regen_min_ms_ = min_ms;
        regen_max_ms_ = max_ms;
        regen_timer_ = 0.0f;
    }

    void AddCondition(uint32_t target_group, float ratio, uint32_t delay_ms,
                      bool regen, uint32_t range) {
        AIGroupCondition c;
        c.target_group_index = target_group;
        c.ratio = ratio;
        c.delay_ms = delay_ms;
        c.regen = regen;
        c.range = range;
        conditions_.push_back(c);
    }

    void AddMonster(uint32_t object_kind, uint32_t monster_kind,
                    const glm::vec3& pos, const std::string& machine) {
        AIGroupMonster m;
        m.object_kind = object_kind;
        m.monster_kind = monster_kind;
        m.position = pos;
        m.machine_name = machine;
        monsters_.push_back(m);
    }

    void AddFieldBossPosition(const glm::vec3& pos) {
        field_boss_positions_.push_back(pos);
    }

    void Die(uint32_t entity_id) {
        alive_entities_.erase(
            std::remove(alive_entities_.begin(), alive_entities_.end(), entity_id),
            alive_entities_.end());
    }

    void AddAliveEntity(uint32_t entity_id) {
        alive_entities_.push_back(entity_id);
    }

    bool AllDead() const { return alive_entities_.empty(); }
    size_t AliveCount() const { return alive_entities_.size(); }

    void UpdateRegen(float dt) {
        if (!AllDead()) return;
        regen_timer_ += dt * 1000.0f;
    }

    bool ReadyToRegen() const {
        return AllDead() && regen_timer_ >= static_cast<float>(regen_max_ms_);
    }

    void ResetRegenTimer() { regen_timer_ = 0.0f; }

    const std::vector<AIGroupMonster>& GetMonsters() const { return monsters_; }
    const std::vector<AIGroupCondition>& GetConditions() const { return conditions_; }

    void SetIsUnique(bool v) { is_unique_ = v; }
    bool IsUnique() const { return is_unique_; }

private:
    uint32_t group_index_ = 0;
    uint32_t grid_index_ = 0;
    std::vector<AIGroupCondition> conditions_;
    std::vector<AIGroupMonster> monsters_;
    std::vector<glm::vec3> field_boss_positions_;
    std::vector<uint32_t> alive_entities_;
    uint32_t regen_min_ms_ = 60000;
    uint32_t regen_max_ms_ = 300000;
    float regen_timer_ = 0.0f;
    bool is_unique_ = false;
};

class AIGroupManager {
public:
    AIGroup& AddGroup(uint32_t group_index, uint32_t grid_index);
    AIGroup* GetGroup(uint32_t group_index, uint32_t grid_index);
    void RegenProcess(entt::registry& registry, float dt);

private:
    struct GroupKey {
        uint32_t group_index;
        uint32_t grid_index;
        bool operator==(const GroupKey& o) const {
            return group_index == o.group_index && grid_index == o.grid_index;
        }
    };
    struct GroupKeyHash {
        size_t operator()(const GroupKey& k) const {
            return std::hash<uint32_t>()(k.group_index) ^ (std::hash<uint32_t>()(k.grid_index) << 1);
        }
    };
    std::unordered_map<GroupKey, std::unique_ptr<AIGroup>, GroupKeyHash> groups_;
};

class AISystem {
public:
    AISystem();
    ~AISystem();

    void Update(entt::registry& registry, float dt);

    uint32_t GenerateMonsterId();
    void ReleaseMonsterId(uint32_t id);

    void LoadAiScript(const std::string& filepath);
    void LoadAiScriptFromData(entt::registry& registry,
                              const std::vector<AIGroupSpawn>& spawns,
                              uint32_t grid_index);

    void Summon(entt::registry& registry, uint32_t grid_index);
    void SummonOnAllChannels(entt::registry& registry, uint32_t channel_count);

    AIGroupManager& GetGroupManager() { return *group_manager_; }

private:
    void UpdateState(entt::registry& registry, entt::entity entity,
                     AIComponent& ai, CharacterStats& stats, float dt);
    void FindNearestTarget(entt::registry& registry, entt::entity entity, AIComponent& ai);
    void Patrol(entt::registry& registry, entt::entity entity,
                AIComponent& ai, CharacterStats& stats, float dt);
    void Chase(entt::registry& registry, entt::entity entity,
               AIComponent& ai, CharacterStats& stats, float dt);
    void Attack(entt::registry& registry, entt::entity entity,
                AIComponent& ai, CharacterStats& stats, float dt);
    void Flee(entt::registry& registry, entt::entity entity,
              AIComponent& ai, CharacterStats& stats, float dt);
    void ReturnToSpawn(entt::registry& registry, entt::entity entity,
                       AIComponent& ai, CharacterStats& stats, float dt);
    void HandleBossPhase(entt::registry& registry, entt::entity entity,
                         AIComponent& ai, CharacterStats& stats);
    void HandleEnrage(entt::registry& registry, entt::entity entity,
                      AIComponent& ai, CharacterStats& stats);

    void SummonGroup(entt::registry& registry, const AIGroupSpawn& spawn, uint32_t grid_index);

    std::unique_ptr<AIGroupManager> group_manager_;
    std::vector<AIGroupSpawn> script_spawns_;
    std::vector<AIGroupSpawn> unique_script_spawns_;
    std::mt19937 rng_;
    uint32_t next_monster_id_ = 1;
    float regen_check_timer_ = 0.0f;
};
