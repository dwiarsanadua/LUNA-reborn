#include "QuestSystem.h"
#include "server/shared/Database.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <ecs/components/Inventory.hpp>

namespace {

QuestObjective::Type MapConditionType(const std::string& type) {
    if (type == "kill") return QuestObjective::KillMonster;
    if (type == "collect") return QuestObjective::CollectItem;
    if (type == "talk") return QuestObjective::TalkToNPC;
    if (type == "use") return QuestObjective::UseItem;
    if (type == "level") return QuestObjective::ReachLevel;
    if (type == "dungeon") return QuestObjective::ClearDungeon;
    return QuestObjective::KillMonster;
}

bool LoadQuestTemplatesFromFullJson(const std::string& path,
                                    std::unordered_map<uint32_t, QuestTemplate>& out) {
    std::ifstream in(path);
    if (!in) return false;
    try {
        nlohmann::json doc;
        in >> doc;
        if (!doc.contains("quests") || !doc["quests"].is_array()) return false;

        out.clear();
        for (const auto& q : doc["quests"]) {
            QuestTemplate qt;
            qt.quest_id = q.value("id", 0u);
            if (qt.quest_id == 0) continue;
            qt.name = q.value("name", "Quest");
            qt.description = q.value("description", "");
            qt.min_level = q.value("level_required", 1);
            qt.max_level = q.value("level_max", 0);
            qt.giver_npc_id = q.value("npc_start_id", 0u);
            qt.completer_npc_id = q.value("npc_complete_id", qt.giver_npc_id);
            qt.is_repeatable = q.value("repeatable", false);
            qt.cooldown_seconds = q.value("cooldown", 0u);
            qt.next_quest_id = q.value("next_quest_id", 0u);
            qt.time_limit_seconds = q.value("time_limit", 0u);

            if (q.contains("prerequisites") && q["prerequisites"].is_array()) {
                for (const auto& pre : q["prerequisites"]) {
                    qt.prerequisite_quest_ids.push_back(pre.get<uint32_t>());
                }
            }

            if (q.contains("conditions") && q["conditions"].is_array()) {
                for (const auto& c : q["conditions"]) {
                    std::string ctype = c.value("type", "");
                    if (ctype == "talk") continue;
                    QuestObjective obj;
                    obj.type = MapConditionType(ctype);
                    if (ctype == "kill")
                        obj.target_id = c.value("target_id", 0u);
                    else if (ctype == "collect")
                        obj.target_id = c.value("item_id", 0u);
                    else if (ctype == "use")
                        obj.target_id = c.value("item_id", 0u);
                    else
                        continue;
                    obj.required_count = static_cast<uint16_t>(c.value("count", 1));
                    obj.current_count = 0;
                    obj.description = ctype + " objective";
                    if (obj.target_id == 0 || obj.required_count == 0) continue;
                    qt.objectives.push_back(obj);
                }
            }

            if (q.contains("rewards") && q["rewards"].is_object()) {
                const auto& r = q["rewards"];
                qt.reward_exp = r.value("exp", 0ull);
                qt.reward_gold = r.value("gold", 0u);
                if (r.contains("items") && r["items"].is_array()) {
                    for (const auto& item : r["items"]) {
                        uint32_t item_id = item.value("item_id", 0u);
                        if (!item_id) continue;
                        qt.reward_items.push_back(item_id);
                        qt.reward_item_counts.push_back(item.value("count", 1u));
                    }
                }
            }

            if (qt.objectives.empty()) continue;
            out[qt.quest_id] = std::move(qt);
        }
        return !out.empty();
    } catch (const std::exception& e) {
        spdlog::warn("QuestSystem: failed to parse {} ({})", path, e.what());
        return false;
    }
}

} // namespace

QuestSystem::QuestSystem() {
    LoadQuestTemplates("assets/data/game_data.db");
}

void QuestSystem::LoadQuestTemplates(const std::string& db_path) {
    (void)db_path;
    quest_templates_.clear();

    if (LoadQuestTemplatesFromFullJson("assets/data/quests_full.json", quest_templates_)) {
        spdlog::info("QuestSystem: loaded {} quests from quests_full.json", quest_templates_.size());
        return;
    }

    spdlog::info("QuestSystem: quests_full.json unavailable, using seed fallback");

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
            qt.max_level = num("max_level", 0);
            qt.giver_npc_id = static_cast<uint32_t>(num("giver_npc_id"));
            qt.completer_npc_id = static_cast<uint32_t>(num("completer_npc_id", qt.giver_npc_id));
            qt.reward_exp = static_cast<uint64_t>(num("reward_exp"));
            qt.reward_gold = static_cast<uint32_t>(num("reward_gold"));
            qt.is_repeatable = num("repeatable") != 0;
            qt.cooldown_seconds = static_cast<uint32_t>(num("cooldown"));
            qt.next_quest_id = static_cast<uint32_t>(num("next_quest_id"));
            qt.time_limit_seconds = static_cast<uint32_t>(num("time_limit"));

            int prereq = num("prerequisite");
            if (prereq > 0) qt.prerequisite_quest_ids.push_back(static_cast<uint32_t>(prereq));

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
        qt1.next_quest_id = 2;
        QuestObjective obj1;
        obj1.type = QuestObjective::KillMonster;
        obj1.target_id = 101;
        obj1.required_count = 10;
        obj1.description = "Kill wolves (0/10)";
        qt1.objectives.push_back(obj1);
        quest_templates_[1] = qt1;

        QuestTemplate qt2;
        qt2.quest_id = 2;
        qt2.name = "Wolf Bounty";
        qt2.description = "Collect 5 wolf pelts";
        qt2.min_level = 2;
        qt2.giver_npc_id = 100;
        qt2.completer_npc_id = 100;
        qt2.reward_exp = 1000;
        qt2.reward_gold = 200;
        qt2.prerequisite_quest_ids = {1};
        QuestObjective obj2;
        obj2.type = QuestObjective::CollectItem;
        obj2.target_id = 2001;
        obj2.required_count = 5;
        obj2.description = "Collect wolf pelts (0/5)";
        qt2.objectives.push_back(obj2);
        quest_templates_[2] = qt2;
    }

    spdlog::info("QuestSystem: loaded {} quest templates", quest_templates_.size());
}

bool QuestSystem::LoadQuestTemplatesFromDatabase(Database& db) {
    if (!quest_templates_.empty()) return true;

    auto rows = db.Query(
        "SELECT id,title,description,level_required,level_max,giver_npc_id,completer_npc_id,"
        "reward_exp,reward_gold,reward_item_id,reward_item_count,repeatable,cooldown,"
        "next_quest_id,time_limit FROM quest_templates");
    if (rows.empty()) return false;

    quest_templates_.clear();
    for (const auto& row : rows) {
        if (row.size() < 12) continue;
        QuestTemplate qt;
        qt.quest_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        if (!qt.quest_id) continue;
        qt.name = row[1];
        qt.description = row[2];
        qt.min_level = std::atoi(row[3].c_str());
        qt.max_level = row.size() > 4 ? std::atoi(row[4].c_str()) : 0;
        qt.giver_npc_id = static_cast<uint32_t>(std::atoi(row[5].c_str()));
        qt.completer_npc_id = static_cast<uint32_t>(std::atoi(row[6].c_str()));
        qt.reward_exp = static_cast<uint64_t>(std::strtoull(row[7].c_str(), nullptr, 10));
        qt.reward_gold = static_cast<uint32_t>(std::atoi(row[8].c_str()));
        uint32_t reward_item = static_cast<uint32_t>(std::atoi(row[9].c_str()));
        if (reward_item) {
            qt.reward_items.push_back(reward_item);
            qt.reward_item_counts.push_back(static_cast<uint32_t>(std::atoi(row[10].c_str())));
        }
        qt.is_repeatable = row.size() > 11 && std::atoi(row[11].c_str()) != 0;
        qt.cooldown_seconds = row.size() > 12 ? static_cast<uint32_t>(std::atoi(row[12].c_str())) : 0;
        qt.next_quest_id = row.size() > 13 ? static_cast<uint32_t>(std::atoi(row[13].c_str())) : 0;
        qt.time_limit_seconds = row.size() > 14 ? static_cast<uint32_t>(std::atoi(row[14].c_str())) : 0;
        quest_templates_[qt.quest_id] = std::move(qt);
    }

    auto prereq_rows = db.Query(
        "SELECT quest_id,prerequisite_id FROM quest_prerequisites");
    for (const auto& row : prereq_rows) {
        if (row.size() < 2) continue;
        uint32_t qid = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        auto it = quest_templates_.find(qid);
        if (it == quest_templates_.end()) continue;
        it->second.prerequisite_quest_ids.push_back(
            static_cast<uint32_t>(std::atoi(row[1].c_str())));
    }

    auto cond_rows = db.Query(
        "SELECT quest_id,condition_type,target_id,target_count FROM quest_conditions");
    for (const auto& row : cond_rows) {
        if (row.size() < 4) continue;
        uint32_t qid = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        auto it = quest_templates_.find(qid);
        if (it == quest_templates_.end()) continue;
        int ctype = std::atoi(row[1].c_str());
        if (ctype == 2) continue;
        QuestObjective obj;
        obj.type = (ctype == 1) ? QuestObjective::CollectItem : QuestObjective::KillMonster;
        obj.target_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
        obj.required_count = static_cast<uint16_t>(std::atoi(row[3].c_str()));
        obj.current_count = 0;
        obj.description = (ctype == 1) ? "Collect items" : "Kill monsters";
        if (obj.target_id && obj.required_count)
            it->second.objectives.push_back(obj);
    }

    for (auto it = quest_templates_.begin(); it != quest_templates_.end();) {
        if (it->second.objectives.empty()) it = quest_templates_.erase(it);
        else ++it;
    }

    spdlog::info("QuestSystem: loaded {} quests from SQLite", quest_templates_.size());
    return !quest_templates_.empty();
}

bool QuestSystem::HasPrerequisites(entt::registry& registry, entt::entity entity,
                                    const QuestTemplate& qt) {
    if (qt.prerequisite_quest_ids.empty()) return true;
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;
    for (auto pre_id : qt.prerequisite_quest_ids) {
        if (!quest_log->IsCompleted(pre_id)) {
            spdlog::info("QuestSystem: prerequisite quest {} not completed for quest {}",
                         pre_id, qt.quest_id);
            return false;
        }
    }
    return true;
}

bool QuestSystem::CheckTimeLimits(entt::registry& registry, entt::entity entity,
                                   QuestEntry& entry, const QuestTemplate& qt) {
    if (qt.time_limit_seconds == 0) return false;
    uint32_t now = static_cast<uint32_t>(std::time(nullptr));
    if (entry.accepted_at > 0 && (now - entry.accepted_at) > qt.time_limit_seconds) {
        spdlog::info("QuestSystem: quest {} timed out for entity {}",
                     qt.quest_id, static_cast<uint32_t>(entity));
        return true;
    }
    return false;
}

QuestState QuestSystem::GetQuestState(entt::registry& registry, entt::entity entity,
                                       uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return QuestState::Inactive;

    for (auto& entry : quest_log->active_quests) {
        if (entry.quest_id == quest_id) {
            if (entry.is_reward_taken) return QuestState::RewardTaken;
            if (entry.is_completed) return QuestState::Completed;
            return QuestState::Active;
        }
    }

    if (quest_log->IsCompleted(quest_id)) return QuestState::Completed;

    return QuestState::Inactive;
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
    if (qt.max_level > 0 && stats->level > qt.max_level) {
        spdlog::info("QuestSystem: level too high for quest {} (max {})", quest_id, qt.max_level);
        return false;
    }
    if (quest_log->IsCompleted(quest_id) && !qt.is_repeatable) {
        spdlog::info("QuestSystem: quest {} already completed", quest_id);
        return false;
    }
    auto* entry = quest_log->GetActive(quest_id);
    if (entry && entry->is_reward_taken && !qt.is_repeatable) {
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
    if (!HasPrerequisites(registry, entity, qt)) {
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

    if (entry->is_completed || entry->is_reward_taken) {
        spdlog::info("QuestSystem: quest {} already completed/rewarded", quest_id);
        return false;
    }

    if (!CheckConditions(entry->objectives)) {
        spdlog::info("QuestSystem: quest {} conditions not met", quest_id);
        return false;
    }

    entry->is_completed = true;
    entry->completed_at = static_cast<uint32_t>(std::time(nullptr));
    spdlog::info("QuestSystem: entity {} completed quest {}", static_cast<uint32_t>(entity), quest_id);

    // Auto-start next quest in chain if configured
    auto it = quest_templates_.find(quest_id);
    if (it != quest_templates_.end() && it->second.next_quest_id > 0) {
        uint32_t next_id = it->second.next_quest_id;
        if (CanStartQuest(registry, entity, next_id)) {
            StartQuest(registry, entity, next_id);
        }
    }

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
    if (!quest_log->IsCompleted(quest_id))
        quest_log->completed_quest_ids.push_back(quest_id);

    quest_log->active_quests.erase(
        std::remove_if(quest_log->active_quests.begin(), quest_log->active_quests.end(),
            [quest_id](const QuestEntry& e) { return e.quest_id == quest_id && e.is_reward_taken; }),
        quest_log->active_quests.end());

    spdlog::info("QuestSystem: entity {} claimed reward for quest {}", static_cast<uint32_t>(entity), quest_id);
    return true;
}

bool QuestSystem::AbandonQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;

    auto it = std::find_if(quest_log->active_quests.begin(), quest_log->active_quests.end(),
        [quest_id](const QuestEntry& e) { return e.quest_id == quest_id && !e.is_reward_taken; });
    if (it == quest_log->active_quests.end()) return false;

    if (it->is_completed) return false;

    quest_log->active_quests.erase(it);
    spdlog::info("QuestSystem: entity {} abandoned quest {}", static_cast<uint32_t>(entity), quest_id);
    return true;
}

bool QuestSystem::FailQuest(entt::registry& registry, entt::entity entity, uint32_t quest_id) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return false;

    auto* entry = quest_log->GetActive(quest_id);
    if (!entry || entry->is_completed || entry->is_reward_taken) return false;

    quest_log->active_quests.erase(
        std::remove_if(quest_log->active_quests.begin(), quest_log->active_quests.end(),
            [quest_id](const QuestEntry& e) { return e.quest_id == quest_id; }),
        quest_log->active_quests.end());

    spdlog::info("QuestSystem: entity {} failed quest {}", static_cast<uint32_t>(entity), quest_id);
    return true;
}

void QuestSystem::GrantRewards(entt::registry& registry, entt::entity entity, const QuestTemplate& qt) {
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (stats) {
        stats->exp += qt.reward_exp;
        while (stats->exp >= stats->exp_next_level) {
            stats->exp -= stats->exp_next_level;
            stats->level++;
            stats->exp_next_level = static_cast<uint64_t>(stats->level * 100);
            stats->max_hp += 20;
            stats->hp = stats->max_hp;
            spdlog::info("QuestSystem: entity {} leveled up to {}!", static_cast<uint32_t>(entity), stats->level);
        }
    }

    auto* inv = registry.try_get<Inventory>(entity);
    if (inv) {
        for (size_t i = 0; i < qt.reward_items.size(); i++) {
            uint32_t item_id = qt.reward_items[i];
            uint32_t count = (i < qt.reward_item_counts.size()) ? qt.reward_item_counts[i] : 1;
            inv->AddItem(item_id, count);
        }
        inv->gold += qt.reward_gold;
    }
}

void QuestSystem::UpdateCondition(entt::registry& registry, entt::entity entity,
                                   QuestObjective::Type type, uint32_t target_id, uint16_t amount) {
    auto* quest_log = registry.try_get<QuestLog>(entity);
    if (!quest_log) return;

    for (auto& entry : quest_log->active_quests) {
        if (entry.is_completed || entry.is_reward_taken) continue;
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
    auto view = registry.view<QuestLog>();
    for (auto entity : view) {
        auto& quest_log = view.get<QuestLog>(entity);

        for (auto it = quest_log.active_quests.begin(); it != quest_log.active_quests.end();) {
            auto& entry = *it;

            if (entry.is_reward_taken) {
                ++it;
                continue;
            }

            auto tpl_it = quest_templates_.find(entry.quest_id);
            if (tpl_it != quest_templates_.end()) {
                if (CheckTimeLimits(registry, entity, entry, tpl_it->second)) {
                    it = quest_log.active_quests.erase(it);
                    continue;
                }
            }

            // Auto-complete if all objectives met
            if (!entry.is_completed && CheckConditions(entry.objectives)) {
                entry.is_completed = true;
                entry.completed_at = static_cast<uint32_t>(std::time(nullptr));
                spdlog::info("QuestSystem: auto-completed quest {} for entity {}",
                             entry.quest_id, static_cast<uint32_t>(entity));

                // Immediate-reward quests (no NPC completer)
                if (tpl_it != quest_templates_.end() && tpl_it->second.completer_npc_id == 0) {
                    ClaimReward(registry, entity, entry.quest_id);
                }
            }

            ++it;
        }
    }
}
