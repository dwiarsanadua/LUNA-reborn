// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include <ecs/components/QuestLog.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Tag.hpp>

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
};

#define GAMEDATADB_QUEST_TEMPLATE_DEFINED

class QuestSystem {
public:
    QuestSystem();

    void LoadQuestTemplates(const std::string& db_path);

    bool StartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool CompleteQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    bool ClaimReward(entt::registry& registry, entt::entity entity, uint32_t quest_id);

    void UpdateCondition(entt::registry& registry, entt::entity entity,
                         QuestObjective::Type type, uint32_t target_id, uint16_t amount = 1);
    bool CheckConditions(const std::vector<QuestObjective>& objectives);

    bool CanStartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id);
    void Update(entt::registry& registry, float dt);
    const std::unordered_map<uint32_t, QuestTemplate>& GetTemplates() const { return quest_templates_; }

private:
    std::unordered_map<uint32_t, QuestTemplate> quest_templates_;
    void GrantRewards(entt::registry& registry, entt::entity entity, const QuestTemplate& qt);
};
