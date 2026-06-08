#include "LootSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include "../components/CharacterStats.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <fstream>
#include <sstream>

static std::mt19937 loot_rng(std::random_device{}());

void LootSystem::LoadDropTables(const std::string& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        spdlog::warn("LoadDropTables: cannot open {}", json_path);
        return;
    }
    std::stringstream buf;
    buf << file.rdbuf();
    std::string content = buf.str();
    file.close();

    // Minimal JSON parser for monster drop data
    // Format: [{"monster_id":N,"drops":[{"item_id":N,"count":N,"rate":F}],"gold_min":N,"gold_max":N,"exp_mult":F}]
    size_t pos = 0;
    while ((pos = content.find("\"monster_id\"", pos)) != std::string::npos) {
        DropTable dt{};
        pos = content.find(':', pos) + 1;
        dt.monster_id = static_cast<uint32_t>(std::stoul(&content[pos]));

        auto gold_min_pos = content.find("\"gold_min\"", pos);
        if (gold_min_pos != std::string::npos) {
            auto cpos = content.find(':', gold_min_pos) + 1;
            dt.gold_min = std::stoi(&content[cpos]);
        }
        auto gold_max_pos = content.find("\"gold_max\"", pos);
        if (gold_max_pos != std::string::npos) {
            auto cpos = content.find(':', gold_max_pos) + 1;
            dt.gold_max = std::stoi(&content[cpos]);
        }
        auto exp_pos = content.find("\"exp_mult\"", pos);
        if (exp_pos != std::string::npos) {
            auto cpos = content.find(':', exp_pos) + 1;
            dt.exp_mult = std::stof(&content[cpos]);
        }

        auto drops_start = content.find("\"drops\"", pos);
        if (drops_start != std::string::npos) {
            auto array_start = content.find('[', drops_start);
            if (array_start != std::string::npos) {
                auto array_end = content.find(']', array_start);
                std::string array_content = content.substr(array_start + 1, array_end - array_start - 1);
                size_t dp = 0;
                while ((dp = array_content.find("\"item_id\"", dp)) != std::string::npos) {
                    LootEntry le{};
                    auto cpos = array_content.find(':', dp) + 1;
                    le.item_id = static_cast<uint32_t>(std::stoul(&array_content[cpos]));
                    auto cnt_pos = array_content.find("\"count\"", dp);
                    if (cnt_pos != std::string::npos) {
                        cpos = array_content.find(':', cnt_pos) + 1;
                        le.count = std::stoi(&array_content[cpos]);
                    }
                    auto rate_pos = array_content.find("\"rate\"", dp);
                    if (rate_pos != std::string::npos) {
                        cpos = array_content.find(':', rate_pos) + 1;
                        le.rate = std::stof(&array_content[cpos]);
                    }
                    dt.entries.push_back(le);
                    dp = array_content.find('}', dp) + 1;
                }
            }
        }

        drop_tables_[dt.monster_id] = dt;
        spdlog::info("Loaded drop table for monster {}", dt.monster_id);
        pos = content.find('}', pos) + 1;
    }
    spdlog::info("LoadDropTables: loaded {} drop tables", drop_tables_.size());
}

std::vector<LootEntry> LootSystem::RollLoot(int monster_id) {
    std::vector<LootEntry> results;
    auto it = drop_tables_.find(static_cast<uint32_t>(monster_id));
    if (it == drop_tables_.end()) {
        spdlog::warn("RollLoot: no drop table for monster {}", monster_id);
        return results;
    }
    const DropTable& dt = it->second;
    for (const auto& entry : dt.entries) {
        if (std::uniform_real_distribution<float>(0, 1)(loot_rng) < entry.rate) {
            int final_count = entry.count;
            if (final_count > 1) {
                final_count = 1 + static_cast<int>(loot_rng() % final_count);
            }
            results.push_back({entry.item_id, final_count, entry.rate});
            spdlog::info("Drop: item {} x{} ({:.1f}%)", entry.item_id, final_count, entry.rate * 100);
        }
    }
    return results;
}

std::vector<LootSystem::DropResult> LootSystem::RollLoot(uint32_t monster_id) const {
    std::vector<DropResult> results;
    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return results;
    auto drops = db.GetDrops(monster_id);
    db.Close();
    for (auto& drop : drops) {
        if (std::uniform_real_distribution<float>(0, 1)(loot_rng) < drop.probability) {
            uint16_t count = drop.min_count;
            if (drop.max_count > drop.min_count)
                count += static_cast<uint16_t>(loot_rng() % (drop.max_count - drop.min_count + 1));
            results.push_back({drop.item_id, count});
            spdlog::info("Loot: item {} x{} ({:.0f}%)", drop.item_id, count, drop.probability * 100);
        }
    }
    return results;
}

std::vector<LootSystem::DropResult> LootSystem::RollGold(uint32_t monster_id) const {
    std::vector<DropResult> results;
    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return results;
    auto m = db.GetMonster(monster_id);
    db.Close();
    if (m.monster_id && m.gold_max > 0) {
        uint32_t gold = m.gold_min;
        if (m.gold_max > m.gold_min)
            gold += static_cast<uint32_t>(loot_rng() % (m.gold_max - m.gold_min + 1));
        results.push_back({0, static_cast<uint16_t>(gold)});
    }
    return results;
}

void LootSystem::SpawnLootItems(entt::registry& reg, const glm::vec3& position,
                                 const std::vector<LootEntry>& items) {
    for (const auto& item : items) {
        auto entity = reg.create();
        reg.emplace<Transform>(entity, position);
        reg.emplace<DroppedItemData>(entity, item.item_id, static_cast<uint16_t>(item.count), 0, 60.0f);
        reg.emplace<TagDroppedItem>(entity);
        spdlog::info("Spawned loot item {} x{} at ({:.1f},{:.1f},{:.1f})",
                     item.item_id, item.count, position.x, position.y, position.z);
    }
}

void LootSystem::SpawnLootItems(entt::registry& reg, const glm::vec3& position,
                                 const std::vector<DropResult>& items) {
    for (const auto& item : items) {
        auto entity = reg.create();
        reg.emplace<Transform>(entity, position);
        reg.emplace<DroppedItemData>(entity, item.item_id, item.count, 0, 60.0f);
        reg.emplace<TagDroppedItem>(entity);
    }
}

bool LootSystem::CanLoot(entt::registry& reg, entt::entity player, entt::entity item) {
    if (!reg.valid(player) || !reg.valid(item)) return false;
    auto* player_pos = reg.try_get<Transform>(player);
    auto* item_pos = reg.try_get<Transform>(item);
    if (!player_pos || !item_pos) return false;
    float dist = glm::distance(player_pos->position, item_pos->position);
    if (dist > 3.0f) return false;
    auto* data = reg.try_get<DroppedItemData>(item);
    if (!data) return false;
    if (data->owner_id != 0) {
        auto* owner = reg.try_get<CharacterStats>(player);
        if (!owner || data->owner_id != static_cast<uint32_t>(player)) return false;
    }
    return true;
}
