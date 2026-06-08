#include "QuestSystem.hpp"
#include "GameDataDB.hpp"
#include "FSMEngine.hpp"
#include "../components/Tag.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Inventory.hpp"
#include <spdlog/spdlog.h>
#include <ctime>
#include <algorithm>

static QuestObjective::Type CondTypeFromString(const std::string& type) {
    if (type == "kill") return QuestObjective::KillMonster;
    if (type == "collect") return QuestObjective::CollectItem;
    if (type == "talk") return QuestObjective::TalkToNPC;
    if (type == "use_item") return QuestObjective::UseItem;
    if (type == "level_up" || type == "level") return QuestObjective::ReachLevel;
    return QuestObjective::KillMonster;
}

void QuestSystem::Initialize(GameDataDB& db) {
    spdlog::info("QuestSystem: initializing...");
    int count = 0;
    for (uint32_t i = 1; i <= 5000; ++i) {
        auto* q = db.GetQuestTemplate(i);
        if (q) {
            npc_quest_map_[q->npc_start_id].push_back(i);
            count++;
        }
    }
    spdlog::info("QuestSystem: cached {} quests across {} NPCs", count, npc_quest_map_.size());
}

bool QuestSystem::CanAcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return false;
    auto& db = GameDataDB::Instance();
    auto* qt = db.GetQuestTemplate(quest_id);
    if (!qt) return false;

    auto& ql = reg.get<QuestLog>(player);
    if (!ql.CanAcceptNew()) return false;
    if (ql.IsCompleted(quest_id)) return false;
    if (ql.GetActive(quest_id) != nullptr) return false;

    auto& stats = reg.get<CharacterStats>(player);
    if (qt->level_required > 0 && stats.level < qt->level_required) return false;

    for (auto prereq_id : qt->prerequisites) {
        if (!ql.IsCompleted(prereq_id)) return false;
    }
    return true;
}

void QuestSystem::AcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!CanAcceptQuest(reg, player, quest_id)) return;
    auto& db = GameDataDB::Instance();
    auto* qt = db.GetQuestTemplate(quest_id);
    if (!qt) return;

    auto& ql = reg.get<QuestLog>(player);
    QuestEntry entry;
    entry.quest_id = quest_id;
    entry.giver_npc_id = qt->npc_start_id;
    entry.completer_npc_id = qt->npc_complete_id;
    entry.accepted_at = static_cast<uint32_t>(time(nullptr));

    for (auto& ct : qt->conditions) {
        QuestObjective obj;
        obj.type = CondTypeFromString(ct.type);
        obj.target_id = ct.target_id;
        obj.required_count = ct.count;
        obj.current_count = 0;
        entry.objectives.push_back(obj);
    }

    ql.active_quests.push_back(entry);
    spdlog::info("QuestSystem: accepted quest '{}' (id={})", qt->name, quest_id);
}

void QuestSystem::UpdateQuestProgress(entt::registry& reg, entt::entity player,
                                       QuestObjective::Type type, uint32_t target_id, uint16_t count) {
    if (!reg.valid(player)) return;
    auto& ql = reg.get<QuestLog>(player);
    for (auto& entry : ql.active_quests) {
        if (entry.is_completed) continue;
        for (auto& obj : entry.objectives) {
            if (obj.type == type && obj.target_id == target_id) {
                obj.current_count = std::min<uint16_t>(
                    static_cast<uint16_t>(obj.current_count + count), obj.required_count);
            }
        }
    }
}

bool QuestSystem::TryCompleteQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return false;
    auto& ql = reg.get<QuestLog>(player);
    auto* quest = ql.GetActive(quest_id);
    if (!quest || quest->is_completed) return false;
    if (!quest->IsObjectiveComplete()) return false;
    quest->is_completed = true;
    quest->completed_at = static_cast<uint32_t>(time(nullptr));
    spdlog::info("QuestSystem: quest {} completed", quest_id);
    return true;
}

void QuestSystem::GiveQuestReward(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return;
    auto& ql = reg.get<QuestLog>(player);
    auto* quest = ql.GetActive(quest_id);
    if (!quest || !quest->is_completed || quest->is_reward_taken) return;

    auto& db = GameDataDB::Instance();
    auto* qt = db.GetQuestTemplate(quest_id);
    if (!qt) return;

    quest->is_reward_taken = true;
    ql.completed_quest_ids.push_back(quest_id);

    auto& stats = reg.get<CharacterStats>(player);
    auto& inv = reg.get<Inventory>(player);

    if (qt->reward_exp > 0) {
        stats.exp += qt->reward_exp;
    }
    if (qt->reward_gold > 0) {
        inv.gold += qt->reward_gold;
    }
    for (auto& ri : qt->reward_items) {
        if (ri.item_id > 0 && ri.count > 0) {
            inv.AddItem(ri.item_id, ri.count);
        }
    }

    auto it = std::remove_if(ql.active_quests.begin(), ql.active_quests.end(),
        [quest_id](const QuestEntry& e) { return e.quest_id == quest_id && e.is_reward_taken; });
    if (it != ql.active_quests.end()) ql.active_quests.erase(it);

    spdlog::info("QuestSystem: rewards given for quest {} (exp={}, gold={}, items={})",
                 quest_id, qt->reward_exp, qt->reward_gold, qt->reward_items.size());
}

std::vector<uint32_t> QuestSystem::GetAvailableQuests(entt::registry& reg,
                                                       entt::entity player, uint32_t npc_id) {
    std::vector<uint32_t> result;
    if (!reg.valid(player)) return result;
    auto it = npc_quest_map_.find(npc_id);
    if (it == npc_quest_map_.end()) return result;
    for (auto quest_id : it->second) {
        if (CanAcceptQuest(reg, player, quest_id)) result.push_back(quest_id);
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════
//  Event handlers
// ═══════════════════════════════════════════════════════════════════════

void QuestSystem::OnKillMonster(entt::registry& reg, entt::entity player, uint32_t monster_id) {
    UpdateQuestProgress(reg, player, QuestObjective::KillMonster, monster_id, 1);
}

void QuestSystem::OnCollectItem(entt::registry& reg, entt::entity player, uint32_t item_id, uint16_t count) {
    UpdateQuestProgress(reg, player, QuestObjective::CollectItem, item_id, count);
}

void QuestSystem::OnNPCTalk(entt::registry& reg, entt::entity player, uint32_t npc_id) {
    UpdateQuestProgress(reg, player, QuestObjective::TalkToNPC, npc_id, 1);
    if (!reg.valid(player)) return;
    auto& ql = reg.get<QuestLog>(player);
    for (auto& entry : ql.active_quests) {
        if (entry.is_completed) continue;
        if (entry.completer_npc_id != npc_id) continue;
        if (!entry.IsObjectiveComplete()) continue;
        TryCompleteQuest(reg, player, entry.quest_id);
    }
}

void QuestSystem::OnLevelUp(entt::registry& reg, entt::entity player, uint16_t new_level) {
    UpdateQuestProgress(reg, player, QuestObjective::ReachLevel, new_level, 1);
}
