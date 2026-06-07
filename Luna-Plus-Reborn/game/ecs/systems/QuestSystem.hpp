#pragma once
#include <entt/entt.hpp>
#include "../components/QuestLog.hpp"

class QuestSystem {
public:
    bool CanAcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void AcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void UpdateQuestProgress(entt::registry& reg, entt::entity player,
                             QuestObjective::Type type, uint32_t target_id, uint16_t count = 1);
    bool TryCompleteQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void GiveQuestReward(entt::registry& reg, entt::entity player, uint32_t quest_id);
    std::vector<uint32_t> GetAvailableQuests(entt::registry& reg, entt::entity player, uint32_t npc_id);
};
