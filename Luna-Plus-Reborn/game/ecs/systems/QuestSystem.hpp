#pragma once
#include <entt/entt.hpp>
#include "../components/QuestLog.hpp"
#include <unordered_map>
#include <vector>
#include <cstdint>

class GameDataDB;
class FSMEngine;

class QuestSystem {
public:
    void Initialize(GameDataDB& db);

    bool CanAcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void AcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void UpdateQuestProgress(entt::registry& reg, entt::entity player,
                             QuestObjective::Type type, uint32_t target_id, uint16_t count = 1);
    bool TryCompleteQuest(entt::registry& reg, entt::entity player, uint32_t quest_id);
    void GiveQuestReward(entt::registry& reg, entt::entity player, uint32_t quest_id);
    std::vector<uint32_t> GetAvailableQuests(entt::registry& reg,
                                              entt::entity player, uint32_t npc_id);

    // Event handlers — called by combat/item/interaction systems
    void OnKillMonster(entt::registry& reg, entt::entity player, uint32_t monster_id);
    void OnCollectItem(entt::registry& reg, entt::entity player, uint32_t item_id, uint16_t count = 1);
    void OnNPCTalk(entt::registry& reg, entt::entity player, uint32_t npc_id);
    void OnLevelUp(entt::registry& reg, entt::entity player, uint16_t new_level);

    // FSM integration
    void SetFSMEngine(FSMEngine* fsm) { fsm_ = fsm; }

private:
    // NPC → quest IDs cache (built during Initialize)
    std::unordered_map<uint32_t, std::vector<uint32_t>> npc_quest_map_;
    FSMEngine* fsm_ = nullptr;
};
