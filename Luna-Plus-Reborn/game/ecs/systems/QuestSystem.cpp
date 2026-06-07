#include "QuestSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/Tag.hpp"
#include "../components/CharacterStats.hpp"
#include <spdlog/spdlog.h>
#include <ctime>

bool QuestSystem::CanAcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return false;
    auto& quest_log = reg.get<QuestLog>(player);
    return quest_log.CanAcceptNew() && !quest_log.IsCompleted(quest_id);
}

void QuestSystem::AcceptQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!CanAcceptQuest(reg, player, quest_id)) return;
    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return;
    auto qd = db.GetQuest(quest_id);
    db.Close();
    if (qd.quest_id == 0) return;

    auto& quest_log = reg.get<QuestLog>(player);
    QuestEntry entry;
    entry.quest_id = quest_id;
    entry.accepted_at = static_cast<uint32_t>(time(nullptr));
    QuestObjective obj;
    obj.type = QuestObjective::KillMonster;
    obj.target_id = quest_id;
    obj.required_count = 10;
    obj.current_count = 0;
    entry.objectives.push_back(obj);
    quest_log.active_quests.push_back(entry);
    spdlog::info("Quest accepted: {} (id={})", qd.title, quest_id);
}

void QuestSystem::UpdateQuestProgress(entt::registry& reg, entt::entity player,
                                       QuestObjective::Type type, uint32_t target_id, uint16_t count) {
    if (!reg.valid(player)) return;
    auto& quest_log = reg.get<QuestLog>(player);
    for (auto& quest : quest_log.active_quests) {
        for (auto& obj : quest.objectives) {
            if (obj.type == type && obj.target_id == target_id) {
                obj.current_count = std::min<uint16_t>(obj.current_count + count, obj.required_count);
            }
        }
    }
}

bool QuestSystem::TryCompleteQuest(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return false;
    auto& quest_log = reg.get<QuestLog>(player);
    auto* quest = quest_log.GetActive(quest_id);
    if (!quest || quest->is_completed) return false;
    if (!quest->IsObjectiveComplete()) return false;
    quest->is_completed = true;
    quest->completed_at = static_cast<uint32_t>(time(nullptr));
    return true;
}

void QuestSystem::GiveQuestReward(entt::registry& reg, entt::entity player, uint32_t quest_id) {
    if (!reg.valid(player)) return;
    auto& quest_log = reg.get<QuestLog>(player);
    auto* quest = quest_log.GetActive(quest_id);
    if (!quest || !quest->is_completed || quest->is_reward_taken) return;
    quest->is_reward_taken = true;
    quest_log.completed_quest_ids.push_back(quest_id);

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return;
    auto qd = db.GetQuest(quest_id);
    db.Close();

    if (qd.quest_id) {
        auto& stats = reg.get<CharacterStats>(player);
        stats.exp += qd.reward_exp;
        stats.exp_next_level = (stats.level + 1) * 500;
        spdlog::info("Quest reward: +{} XP, +{} gold", qd.reward_exp, qd.reward_gold);
    }
}

std::vector<uint32_t> QuestSystem::GetAvailableQuests(entt::registry& reg,
                                                       entt::entity player, uint32_t npc_id) {
    return {};
}
