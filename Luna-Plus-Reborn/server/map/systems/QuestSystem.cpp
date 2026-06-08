#include "QuestSystem.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ecs/components/Inventory.hpp>

QuestSystem::QuestSystem() {
    LoadQuestTemplates("assets/data/game_data.db");
}

void QuestSystem::LoadQuestTemplates(const std::string& db_path) {
    (void)db_path;
    quest_templates_.clear();

    auto load_seed = [this](const std::string& path) {
        std::ifstream in(path);
        if (!in) return false;
        std::stringstream ss;
        ss << in.rdbuf();
        std::string blob = ss.str();
        size_t pos = 0;
        int loaded = 0;
        while ((pos = blob.find("\"quest_id\"", pos)) != std::string::npos) {
            size_t obj_start = blob.rfind('{', pos);
            size_t obj_end = blob.find('}', pos);
            if (obj_start == std::string::npos || obj_end == std::string::npos) break;
            std::string obj = blob.substr(obj_start, obj_end - obj_start + 1);

            auto num = [&](const char* key, int fallback = 0) {
                std::string pat = std::string("\"") + key + "\"";
                size_t p = obj.find(pat);
                if (p == std::string::npos) return fallback;
                p = obj.find(':', p);
                return p == std::string::npos ? fallback : std::atoi(obj.c_str() + p + 1);
            };

            QuestTemplate qt;
            qt.quest_id = static_cast<uint32_t>(num("quest_id"));
            if (qt.quest_id == 0) { pos = obj_end + 1; continue; }
            size_t name_p = obj.find("\"name\"");
            if (name_p != std::string::npos) {
                name_p = obj.find('"', obj.find(':', name_p) + 1);
                size_t name_e = obj.find('"', name_p + 1);
                if (name_p != std::string::npos && name_e != std::string::npos)
                    qt.name = obj.substr(name_p + 1, name_e - name_p - 1);
            }
            qt.min_level = num("min_level", 1);
            qt.giver_npc_id = static_cast<uint32_t>(num("giver_npc_id"));
            qt.completer_npc_id = static_cast<uint32_t>(num("completer_npc_id", qt.giver_npc_id));
            qt.reward_exp = static_cast<uint64_t>(num("reward_exp"));
            qt.reward_gold = static_cast<uint32_t>(num("reward_gold"));
            qt.is_repeatable = num("repeatable") != 0;

            QuestObjective obj_kill;
            obj_kill.type = QuestObjective::KillMonster;
            obj_kill.target_id = static_cast<uint32_t>(num("kill_monster_id", 101));
            obj_kill.required_count = static_cast<uint16_t>(num("kill_count", 10));
            obj_kill.current_count = 0;
            char buf[64];
            snprintf(buf, sizeof(buf), "Kill (%d/%d)", 0, obj_kill.required_count);
            obj_kill.description = buf;
            qt.objectives.push_back(obj_kill);

            if (num("collect_item_id") > 0) {
                QuestObjective obj_col;
                obj_col.type = QuestObjective::CollectItem;
                obj_col.target_id = static_cast<uint32_t>(num("collect_item_id"));
                obj_col.required_count = static_cast<uint16_t>(num("collect_count", 1));
                obj_col.description = "Collect items";
                qt.objectives.push_back(obj_col);
            }

            uint32_t reward_item = static_cast<uint32_t>(num("reward_item_id"));
            if (reward_item) {
                qt.reward_items.push_back(reward_item);
                qt.reward_item_counts.push_back(static_cast<uint32_t>(num("reward_item_count", 1)));
            }

            quest_templates_[qt.quest_id] = std::move(qt);
            ++loaded;
            pos = obj_end + 1;
        }
        return loaded > 0;
    };

    if (!load_seed("assets/data/quest_templates_seed.json")) {
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
        QuestObjective obj1;
        obj1.type = QuestObjective::KillMonster;
        obj1.target_id = 101;
        obj1.required_count = 10;
        obj1.description = "Kill wolves (0/10)";
        qt1.objectives.push_back(obj1);
        quest_templates_[1] = qt1;
    }

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
                uint16_t new_count = obj.current_count + amount;
                obj.current_count = (new_count < obj.required_count) ? new_count : obj.required_count;
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
