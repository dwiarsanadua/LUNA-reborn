#include "ShopSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/Inventory.hpp"
#include <spdlog/spdlog.h>

void ShopSystem::OpenShop(entt::registry& reg, entt::entity player, uint32_t npc_id) {
    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return;
    auto items = db.GetShopItems(npc_id);
    db.Close();
    spdlog::info("NPC shop {} opened with {} items", npc_id, items.size());
    for (auto& item : items) {
        spdlog::info("  - Item {}: {} gold", item.item_id, item.price);
    }
}

bool ShopSystem::BuyItem(entt::registry& reg, entt::entity player, uint32_t item_id, uint16_t count) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto itemData = db.GetItem(item_id);
    db.Close();
    if (itemData.item_id == 0) return false;

    uint32_t cost = itemData.price_buy * count;
    if (cost == 0) cost = item_id * count;
    if (inv.gold < cost) return false;
    inv.gold -= cost;
    inv.AddItem(item_id, count);
    spdlog::info("Bought {} x{} for {}", itemData.name, count, cost);
    return true;
}

bool ShopSystem::SellItem(entt::registry& reg, entt::entity player, size_t slot_index, uint16_t count) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto* slot = inv.Get(slot_index);
    if (!slot || slot->item_id == 0 || slot->count < count) return false;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto itemData = db.GetItem(slot->item_id);
    db.Close();

    uint32_t price = itemData.price_sell > 0 ? itemData.price_sell * count : slot->item_id * count / 2;
    inv.gold += price;
    inv.RemoveItem(slot_index, count);
    spdlog::info("Sold {} x{} for {}", itemData.name, count, price);
    return true;
}
