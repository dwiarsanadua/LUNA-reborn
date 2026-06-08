#include "QuestLog.hpp"
#include <algorithm>

bool QuestEntry::IsObjectiveComplete() const {
    for (const auto& obj : objectives) {
        if (obj.current_count < obj.required_count) return false;
    }
    return !objectives.empty();
}

void QuestEntry::UpdateObjective(QuestObjective::Type type, uint32_t target_id, uint16_t count) {
    for (auto& obj : objectives) {
        if (obj.type == type && obj.target_id == target_id) {
            uint32_t next = obj.current_count + count;
            obj.current_count = static_cast<uint16_t>(std::min<uint32_t>(next, obj.required_count));
        }
    }
}

QuestEntry* QuestLog::GetActive(uint32_t quest_id) {
    for (auto& entry : active_quests) {
        if (entry.quest_id == quest_id) return &entry;
    }
    return nullptr;
}

bool QuestLog::IsCompleted(uint32_t quest_id) const {
    return std::find(completed_quest_ids.begin(), completed_quest_ids.end(), quest_id)
           != completed_quest_ids.end();
}
