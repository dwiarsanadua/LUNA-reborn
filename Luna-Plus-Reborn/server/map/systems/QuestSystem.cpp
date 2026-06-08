#include "QuestSystem.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <ecs/systems/GameDataDB.hpp>

QuestSystem::QuestSystem() {
    LoadQuestTemplates(GAME_DATA_PATH);
}

void QuestSystem::LoadQuestTemplates(const std::string& db_path) {
    // In production, load from game_data.db quests table
    // Example quest templates:
    QuestTemplate qt1;
    qt1.quest_id = 1;
    qt1.name = "Wolf Hunt";
    qt1.description = "Kill 10 wolves";
    qt1.min_level = 1;
    qt1.giver_npc_id = 100;
    qt1.completer_npc_id = 100;
    qt1.reward_exp = 500;
    qt1.reward_gold = 100;
    qt1.reward_items = {1001};
    qt1.reward_item_counts = {5};
    qt1.is_repeatable = false;

    QuestObjective obj1;
    obj1.type = QuestObjective::KillMonster;
    obj1.target_id = 101; // wolf monster ID
    obj1.required_count = 10;
    obj1.current_count = 0;
    obj1.description = "Kill wolves (0/10)";
    qt1.objectives.push_back(obj1);
    quest_templates_[1] = qt1;

    QuestTemplate qt2;
    qt2.quest_id = 2;
    qt2.name = "Gather Herbs";
    qt2.description = "Collect 5 medicinal herbs";
    qt2.min_level = 3;
    qt2.giver_npc_id = 101;
    qt2.completer_npc_id = 101;
    qt2.reward_exp = 1200;
    qt2.reward_gold = 300;
    qt2.reward_items = {2001};
    qt2.reward_item_counts = {1};
    qt2.is_repeatable = true;

    QuestObjective obj2;
    obj2.type = QuestObjective::CollectItem;
    obj2.target_id = 3001; // herb item ID
    obj2.required_count = 5;
    obj2.current_count = 0;
    obj2.description = "Collect herbs (0/5)";
    qt2.objectives.push_back(obj2);
    quest_templates_[2] = qt2;

    spdlog::info("QuestSystem: loaded {} quest templates", quest_templates_.size());
}

bool QuestSystem::CanStartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (!stats) return false;

    auto it = quest_templates_.find(quest_id);
    if (it == quest_templates_.end()) {
        spdlog::warn("QuestSystem: unknown quest {}", quest_id);
        return false;
    }

    auto& qt = it->second;
    if (stats->level < qt.min_level) {
        spdlog::info("QuestSystem: level too low for quest {} (need {})", quest_id, qt.min_level);
        return false;
    }
    if (quest_log->IsCompleted(quest_id) && !qt.is_repeatable) {
        spdlog::info("QuestSystem: quest {} already completed", quest_id);
        return false;
    }
    if (quest_log->GetActive(quest_id)) {
        spdlog::info("QuestSystem: quest {} already active", quest_id);
        return false;
    }
    if (!quest_log->CanAcceptNew()) {
        spdlog::info("QuestSystem: quest log full");
        return false;
    }
    return true;
}

bool QuestSystem::StartQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    if (!CanStartQuest(registry, entity, quest_id)) return false;

    auto& quest_log = registry.get_or_emplace<QuestLog>(entity);
    auto& qt = quest_templates_[quest_id];

    QuestEntry entry;
    entry.quest_id = quest_id;
    entry.giver_npc_id = qt.giver_npc_id;
    entry.completer_npc_id = qt.completer_npc_id;
    entry.accepted_at = static_cast<uint32_t>(std::time(nullptr));

    for (auto& obj : qt.objectives) {
        entry.objectives.push_back(obj);
    }

    quest_log.active_quests.push_back(std::move(entry));
    spdlog::info("QuestSystem: entity {} started quest '{}'", static_cast<uint32_t>(entity), qt.name);
    return true;
}

bool QuestSystem::CompleteQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;

    auto* entry = quest_log->GetActive(quest_id);
    if (!entry) {
        spdlog::warn("QuestSystem: quest {} not active for entity {}", quest_id, static_cast<uint32_t>(entity));
        return false;
    }

    if (!CheckConditions(entry->objectives)) {
        spdlog::info("QuestSystem: quest {} conditions not met", quest_id);
        return false;
    }

    entry->is_completed = true;
    entry->completed_at = static_cast<uint32_t>(std::time(nullptr));
    spdlog::info("QuestSystem: entity {} completed quest {}", static_cast<uint32_t>(entity), quest_id);
    return true;
}

bool QuestSystem::ClaimReward(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;

    auto* entry = quest_log->GetActive(quest_id);
    if (!entry || !entry->is_completed || entry->is_reward_taken) return false;

    auto it = quest_templates_.find(quest_id);
    if (it == quest_templates_.end()) return false;

    GrantRewards(registry, entity, it->second);
    entry->is_reward_taken = true;

    // Move to completed list
    quest_log->completed_quest_ids.push_back(quest_id);
    quest_log->active_quests.erase(
        std::remove_if(quest_log->active_quests.begin(), quest_log->active_quests.end(),
            [quest_id](const QuestEntry& e) { return e.quest_id == quest_id && e.is_reward_taken; }),
        quest_log->active_quests.end());

    spdlog::info("QuestSystem: entity {} claimed reward for quest {}", static_cast<uint32_t>(entity), quest_id);
    return true;
}

void QuestSystem::GrantRewards(entt::registry& registry, entt::entity entity, const QuestTemplate& qt) {
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (stats) {
        stats->exp += qt.reward_exp;
        // Level-up check
        while (stats->exp >= stats->exp_next_level) {
            stats->exp -= stats->exp_next_level;
            stats->level++;
            stats->exp_next_level = static_cast<uint64_t>(stats->level * 100);
            stats->max_hp += 20;
            stats->hp = stats->max_hp;
            spdlog::info("QuestSystem: entity {} leveled up to {}!", static_cast<uint32_t>(entity), stats->level);
        }
    }

    // Add items to inventory
    auto* inv = registry.try_get<Inventory>(entity);
    if (inv) {
        for (size_t i = 0; i < qt.reward_items.size(); i++) {
            ItemSlot reward;
            reward.item_id = qt.reward_items[i];
            reward.count = (i < qt.reward_item_counts.size()) ? qt.reward_item_counts[i] : 1;
            inv->AddItem(reward.item_id, reward.count);
        }
        inv->gold += qt.reward_gold;
    }
}

void QuestSystem::UpdateCondition(entt::registry& registry, entt::entity entity,
                                   QuestObjective::Type type, uint32_t target_id, uint16_t amount) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return;

    for (auto& entry : quest_log->active_quests) {
        if (entry.is_completed) continue;
        for (auto& obj : entry.objectives) {
            if (obj.type == type && obj.target_id == target_id) {
                obj.current_count = std::min(obj.current_count + amount, obj.required_count);
                spdlog::debug("QuestSystem: condition updated for quest {}: {}/{}",
                              entry.quest_id, obj.current_count, obj.required_count);
            }
        }
    }
}

bool QuestSystem::CheckConditions(const std::vector<QuestObjective>& objectives) {
    for (auto& obj : objectives) {
        if (obj.current_count < obj.required_count) return false;
    }
    return true;
}

void QuestSystem::Update(entt::registry& registry, float dt) {
    // Check for auto-completable quests
    auto view = registry.view<QuestLog>();
    for (auto entity : view) {
        auto& quest_log = view.get<QuestLog>(entity);
        for (auto& entry : quest_log.active_quests) {
            if (entry.is_completed) continue;
            if (CheckConditions(entry.objectives)) {
                // Auto-complete if all objectives met
                entry.is_completed = true;
                entry.completed_at = static_cast<uint32_t>(std::time(nullptr));
                // For immediate-reward quests, grant directly
                auto it = quest_templates_.find(entry.quest_id);
                if (it != quest_templates_.end() && it->second.completer_npc_id == 0) {
                    ClaimReward(registry, entity, entry.quest_id);
                }
            }
        }
    }
}
