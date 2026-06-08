#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>

struct LootEntry {
    uint32_t item_id;
    int count;
    float rate;
};

struct DropTable {
    uint32_t monster_id;
    std::vector<LootEntry> entries;
    int gold_min;
    int gold_max;
    float exp_mult;
};

struct DroppedItemData {
    uint32_t item_id;
    uint16_t count;
    uint32_t owner_id = 0;
    float despawn_timer = 60.0f;
};

struct LootItem {
    uint32_t item_id;
    uint16_t min_count;
    uint16_t max_count;
    float probability;
    bool is_guaranteed;
};

class LootSystem {
public:
    struct DropResult { uint32_t item_id; uint16_t count; };

    void LoadDropTables(const std::string& json_path);
    std::vector<LootEntry> RollLoot(int monster_id);
    std::vector<DropResult> RollLoot(uint32_t monster_id) const;
    std::vector<DropResult> RollGold(uint32_t monster_id) const;
    void SpawnLootItems(entt::registry& reg, const glm::vec3& position,
                        const std::vector<LootEntry>& items);
    void SpawnLootItems(entt::registry& reg, const glm::vec3& position,
                        const std::vector<DropResult>& items);
    bool CanLoot(entt::registry& reg, entt::entity player, entt::entity item);

private:
    std::unordered_map<uint32_t, DropTable> drop_tables_;
};
