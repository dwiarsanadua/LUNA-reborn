#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

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
    std::vector<DropResult> RollLoot(uint32_t monster_id) const;
    std::vector<DropResult> RollGold(uint32_t monster_id) const;
    void SpawnLootItems(entt::registry& reg, const glm::vec3& position,
                        const std::vector<DropResult>& items);
};
