#include "LootSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <random>

static std::mt19937 loot_rng(std::random_device{}());

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
                                 const std::vector<DropResult>& items) {
    for (const auto& item : items) {
        auto entity = reg.create();
        reg.emplace<Transform>(entity, position);
        reg.emplace<TagDroppedItem>(entity);
    }
}
