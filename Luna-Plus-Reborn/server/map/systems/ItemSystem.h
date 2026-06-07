// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <entt/entt.hpp>

struct ItemInstance {
    uint32_t uid = 0;
    uint32_t template_id = 0;
    uint32_t count = 1;
    int slot = -1;
    int enchant = 0;
    bool equipped = false;
};

struct InventoryComponent {
    std::vector<ItemInstance> items;
    uint32_t gold = 0;
    int max_slots = 40;
};

struct EquipmentComponent {
    ItemInstance weapon;
    ItemInstance offhand;
    ItemInstance armor;
    ItemInstance helmet;
    ItemInstance gloves;
    ItemInstance boots;
    ItemInstance ring1;
    ItemInstance ring2;
    ItemInstance necklace;
};

class ItemSystem {
public:
    ItemSystem();

    // Inventory
    bool AddItem(entt::entity entity, const ItemInstance& item);
    bool RemoveItem(entt::entity entity, int slot, uint32_t count = 1);
    bool MoveItem(entt::entity entity, int from_slot, int to_slot);
    bool SplitStack(entt::entity entity, int slot, uint32_t count);

    // Equipment
    bool EquipItem(entt::entity entity, int slot);
    bool UnequipItem(entt::entity entity, int equip_slot);

    // Drop
    ItemInstance GenerateDrop(int template_id, int monster_level);
    void SpawnLoot(glm::vec3 position, const std::vector<ItemInstance>& items);

    // Upgrade
    bool UpgradeItem(ItemInstance& item);
    int GetUpgradeSuccessRate(int current_enchant);

    void Update(entt::registry& registry, float dt);
};
