// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <entt/entt.hpp>
#include <ecs/components/QuestLog.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Tag.hpp>

enum class QuestState : uint8_t {
    Inactive = 0,
    Active,
    Completed,
    Failed,
    RewardTaken,
    Abandoned,
};

struct QuestTemplate {
    uint32_t quest_id;
    std::string name;
    std::string description;
    int min_level;
    std::vector<QuestObjective> objectives;
    uint64_t reward_exp;
    uint32_t reward_gold;
    std::vector<uint32_t> reward_items;
    std::vector<uint32_t> reward_item_counts;
    int reward_reputation;
    uint32_t giver_npc_id;
    uint32_t completer_npc_id;
    bool is_repeatable = false;
    uint32_t cooldown_seconds = 0;
    std::vector<uint32_t> prerequisite_quest_ids;
    uint32_t next_quest_id = 0;
    uint32_t time_limit_seconds = 0;
    int max_level = 0;
};

#define GAMEDATADB_QUEST_TEMPLATE_DEFINED

class QuestSystem {
public:
    QuestSystem();

    void LoadQuestTemplates(const std::string& db_path);

    bool StartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool CompleteQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool ClaimReward(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool AbandonQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool FailQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);

    void UpdateCondition(entt::registry& registry, entt::entity entity,
                         QuestObjective::Type type, uint32_t target_id, uint16_t amount = 1);
    bool CheckConditions(const std::vector<QuestObjective>& objectives);

    bool CanStartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    void Update(entt::registry& registry, float dt);
    const std::unordered_map<uint32_t, QuestTemplate>& GetTemplates() const { return quest_templates_; }
    bool LoadQuestTemplatesFromDatabase(class Database& db);

    QuestState GetQuestState(entt::registry& registry, entt::entity entity, uint32_t quest_id);

private:
    std::unordered_map<uint32_t, QuestTemplate> quest_templates_;
    void GrantRewards(entt::registry& registry, entt::entity entity, const QuestTemplate& qt);
    bool HasPrerequisites(entt::registry& registry, entt::entity entity, const QuestTemplate& qt);
    bool CheckTimeLimits(entt::registry& registry, entt::entity entity, QuestEntry& entry,
                         const QuestTemplate& qt);
};
