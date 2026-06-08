// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include <ecs/components/Inventory.hpp>
#include <ecs/components/Equipment.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Transform.hpp>

struct LootTableEntry {
    uint32_t item_id;
    float drop_chance; // 0.0 - 100.0
    int min_count = 1;
    int max_count = 1;
    int min_enchant = 0;
    int max_enchant = 0;
    int min_level = 1;
    int max_level = 999;
};

struct LootTable {
    uint32_t table_id;
    std::vector<LootTableEntry> entries;
    int gold_min = 0;
    int gold_max = 0;
};

class ItemSystem {
public:
    ItemSystem();
    void LoadLootTables(const std::string& db_path);

    // Inventory
    bool AddItem(entt::registry& registry, entt::entity entity, const ItemSlot& item);
    bool RemoveItem(entt::registry& registry, entt::entity entity, size_t slot, uint16_t count = 1);
    bool MoveItem(entt::registry& registry, entt::entity entity, size_t from_slot, size_t to_slot);
    bool SplitStack(entt::registry& registry, entt::entity entity, size_t slot, uint16_t count);

    // Equipment
    bool EquipItem(entt::registry& registry, entt::entity entity, size_t inv_slot);
    bool UnequipItem(entt::registry& registry, entt::entity entity, uint8_t equip_slot);

    // Drop / Loot
    std::vector<ItemSlot> RollLootTable(uint32_t table_id, int monster_level);
    std::vector<ItemSlot> GenerateDrop(int monster_template_id, int monster_level);
    void SpawnLoot(entt::registry& registry, glm::vec3 position, const std::vector<ItemSlot>& items);

    // Upgrade
    bool UpgradeItem(ItemSlot& item);
    int GetUpgradeSuccessRate(int current_enchant);

    // Effects
    void UseItem(entt::registry& registry, entt::entity entity, size_t slot);

    void Update(entt::registry& registry, float dt);

private:
    std::mt19937 rng_;
    std::unordered_map<uint32_t, LootTable> loot_tables_;
};
