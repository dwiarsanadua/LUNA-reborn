#include "QuestSystem.h"
#include <spdlog/spdlog.h>
#include <algorithm>

QuestSystem::QuestSystem() {
    LoadQuestTemplates();
}

void QuestSystem::LoadQuestTemplates() {
    // Would load from database
    // Example quest:
    QuestCondition kill_condition;
    kill_condition.type = QuestConditionType::KILL_COUNT;
    kill_condition.target_id = 101; // monster template ID
    kill_condition.required_count = 10;
    kill_condition.current_count = 0;

    QuestReward reward;
    reward.exp = 1000;
    reward.gold = 500;

    quest_templates_[1] = {{kill_condition}, reward};
}

void QuestSystem::StartQuest(entt::entity entity, uint32_t quest_id) {
    auto it = quest_templates_.find(quest_id);
    if (it == quest_templates_.end()) {
        spdlog::warn("QuestSystem: unknown quest {}", quest_id);
        return;
    }

    // Would add to QuestComponent
    spdlog::info("QuestSystem: entity {} started quest {}", static_cast<uint32_t>(entity), quest_id);
}

void QuestSystem::CompleteQuest(entt::entity entity, uint32_t quest_id) {
    spdlog::info("QuestSystem: entity {} completed quest {}", static_cast<uint32_t>(entity), quest_id);
}

void QuestSystem::ClaimReward(entt::entity entity, uint32_t quest_id) {
    auto it = quest_templates_.find(quest_id);
    if (it == quest_templates_.end()) return;

    auto& reward = it->second.second;
    // Apply XP, gold, items to entity
    spdlog::info("QuestSystem: entity {} claimed reward for quest {} ({} exp, {} gold)",
                 static_cast<uint32_t>(entity), quest_id, reward.exp, reward.gold);
}

void QuestSystem::UpdateCondition(entt::entity entity, QuestConditionType type, int target_id, int amount) {
    // Find matching condition in entity's active quests and update count
    spdlog::debug("QuestSystem: update condition type={} target={} amount={}",
                  static_cast<int>(type), target_id, amount);
}

bool QuestSystem::CheckConditions(const std::vector<QuestCondition>& conditions) {
    for (auto& c : conditions) {
        if (c.current_count < c.required_count) return false;
    }
    return true;
}

void QuestSystem::Update(entt::registry& registry, float dt) {
    // Check for completed quests, auto-complete if conditions met
}
