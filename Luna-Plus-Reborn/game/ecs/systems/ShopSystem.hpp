#pragma once
#include <entt/entt.hpp>

class ShopSystem {
public:
    void OpenShop(entt::registry& reg, entt::entity player, uint32_t npc_id);
    bool BuyItem(entt::registry& reg, entt::entity player, uint32_t item_id, uint16_t count);
    bool SellItem(entt::registry& reg, entt::entity player, size_t slot_index, uint16_t count);
};
