#pragma once
#include <vector>
#include <cstdint>
#include <string>

struct QuestObjective {
    enum Type : uint8_t {
        KillMonster = 0, CollectItem = 1, TalkToNPC = 2,
        UseItem = 3, ReachLevel = 4, ClearDungeon = 5,
    };
    Type type;
    uint32_t target_id;
    uint16_t required_count;
    uint16_t current_count;
    std::string description;
};

struct QuestEntry {
    uint32_t quest_id;
    bool is_completed = false;
    bool is_reward_taken = false;
    std::vector<QuestObjective> objectives;
    uint32_t giver_npc_id = 0;
    uint32_t completer_npc_id = 0;
    uint32_t accepted_at = 0;
    uint32_t completed_at = 0;
    bool IsObjectiveComplete() const;
    void UpdateObjective(QuestObjective::Type type, uint32_t target_id, uint16_t count = 1);
};

struct QuestLog {
    static constexpr size_t MAX_ACTIVE_QUESTS = 20;
    std::vector<QuestEntry> active_quests;
    std::vector<uint32_t> completed_quest_ids;
    QuestEntry* GetActive(uint32_t quest_id);
    bool IsCompleted(uint32_t quest_id) const;
    bool CanAcceptNew() const { return active_quests.size() < MAX_ACTIVE_QUESTS; }
};
