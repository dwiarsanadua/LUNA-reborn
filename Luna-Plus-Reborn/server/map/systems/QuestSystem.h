// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>

enum class QuestState : uint8_t {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    REWARDED
};

enum class QuestConditionType : uint8_t {
    KILL_COUNT,
    ITEM_COLLECT,
    LEVEL_CHECK,
    NPC_TALK,
    REACH_LOCATION
};

struct QuestCondition {
    QuestConditionType type;
    int target_id;
    int required_count;
    int current_count = 0;
};

struct QuestReward {
    uint64_t exp = 0;
    uint32_t gold = 0;
    std::vector<uint32_t> item_ids;
    int reputation = 0;
};

struct QuestObjective {
    uint32_t quest_id;
    std::string name;
    std::string description;
    QuestState state = QuestState::NOT_STARTED;
    std::vector<QuestCondition> conditions;
    QuestReward reward;
};

struct QuestComponent {
    std::vector<QuestObjective> active_quests;
    std::vector<uint32_t> completed_quests;
};

class QuestSystem {
public:
    QuestSystem();

    void StartQuest(entt::entity entity, uint32_t quest_id);
    void CompleteQuest(entt::entity entity, uint32_t quest_id);
    void ClaimReward(entt::entity entity, uint32_t quest_id);

    void UpdateCondition(entt::entity entity, QuestConditionType type, int target_id, int amount = 1);
    bool CheckConditions(const std::vector<QuestCondition>& conditions);

    void Update(entt::registry& registry, float dt);

private:
    std::unordered_map<uint32_t, std::pair<std::vector<QuestCondition>, QuestReward>> quest_templates_;
    void LoadQuestTemplates();
};
