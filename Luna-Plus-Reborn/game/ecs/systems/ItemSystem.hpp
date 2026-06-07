#pragma once
#include <entt/entt.hpp>

class ItemSystem {
public:
    bool UseItem(entt::registry& reg, entt::entity player, size_t slot_index);
    bool EquipItem(entt::registry& reg, entt::entity player, size_t slot_index);
    bool UnequipItem(entt::registry& reg, entt::entity player, uint8_t equip_slot);
    bool PickupItem(entt::registry& reg, entt::entity player, entt::entity item_entity);
    bool DropItem(entt::registry& reg, entt::entity player, size_t slot_index, uint16_t count);
    bool EnchantItem(entt::registry& reg, entt::entity player, size_t slot_index);
    bool ComposeItem(entt::registry& reg, entt::entity player, uint32_t recipe_id);
};
