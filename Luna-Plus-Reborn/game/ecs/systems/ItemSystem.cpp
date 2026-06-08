#include "ItemSystem.hpp"
#include "GameDataDB.hpp"
#include "../components/Inventory.hpp"
#include "../components/Equipment.hpp"
#include "../components/CharacterStats.hpp"
#include <spdlog/spdlog.h>
#include <random>

static std::mt19937 item_rng(std::random_device{}());

bool ItemSystem::UseItem(entt::registry& reg, entt::entity player, size_t slot_index) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto* slot = inv.Get(slot_index);
    if (!slot || slot->item_id == 0) return false;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto itemData = db.GetItem(slot->item_id);
    db.Close();

    if (itemData.item_type == 1) { // consumable
        auto& stats = reg.get<CharacterStats>(player);
        stats.hp = std::min(stats.max_hp, stats.hp + 100);
        stats.mp = std::min(stats.max_mp, stats.mp + 50);
        inv.RemoveItem(slot_index, 1);
        spdlog::info("Used {}: HP +100 MP +50", itemData.name);
    }
    return true;
}

bool ItemSystem::EquipItem(entt::registry& reg, entt::entity player, size_t inv_slot) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto& equip = reg.get<Equipment>(player);
    auto* slot = inv.Get(inv_slot);
    if (!slot || slot->item_id == 0) return false;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto itemData = db.GetItem(slot->item_id);
    db.Close();

    Equipment::Slot eSlot = static_cast<Equipment::Slot>(itemData.item_subtype);
    if (eSlot >= Equipment::COUNT) return false;

    uint32_t old_item = equip.items[eSlot];
    equip.items[eSlot] = slot->item_id;
    slot->item_id = old_item;

    auto& stats = reg.get<CharacterStats>(player);
    stats.physic_attack += itemData.attack;
    stats.physic_defense += itemData.defense;
    spdlog::info("Equipped {} (ATK+{} DEF+{})", itemData.name, itemData.attack, itemData.defense);
    return true;
}

bool ItemSystem::UnequipItem(entt::registry& reg, entt::entity player, uint8_t equip_slot) {
    if (!reg.valid(player)) return false;
    auto& equip = reg.get<Equipment>(player);
    auto& inv = reg.get<Inventory>(player);
    auto slot = static_cast<Equipment::Slot>(equip_slot);
    if (slot >= Equipment::COUNT || equip.items[slot] == 0) return false;

    GameDataDB db;
    if (!db.Open(GAME_DATA_PATH)) return false;
    auto itemData = db.GetItem(equip.items[slot]);
    db.Close();

    if (!inv.AddItem(equip.items[slot], 1)) return false;
    equip.items[slot] = 0;
    auto& stats = reg.get<CharacterStats>(player);
    stats.physic_attack -= itemData.attack;
    stats.physic_defense -= itemData.defense;
    spdlog::info("Unequipped {}", itemData.name);
    return true;
}

bool ItemSystem::PickupItem(entt::registry& reg, entt::entity player, entt::entity item_entity) {
    if (!reg.valid(player) || !reg.valid(item_entity)) return false;
    spdlog::info("Player picked up item");
    reg.destroy(item_entity);
    return true;
}

bool ItemSystem::DropItem(entt::registry& reg, entt::entity player, size_t slot_index, uint16_t count) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto* slot = inv.Get(slot_index);
    if (!slot || slot->item_id == 0) return false;
    inv.RemoveItem(slot_index, count);
    spdlog::info("Dropped item {}", slot->item_id);
    return true;
}

bool ItemSystem::UpgradeItem(entt::registry& reg, entt::entity player, int inventory_slot) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto* slot = inv.Get(static_cast<size_t>(inventory_slot));
    if (!slot || slot->item_id == 0) return false;

    static const float upgrade_rates[10] = {
        0.95f, 0.85f, 0.70f, 0.55f, 0.40f,
        0.30f, 0.20f, 0.12f, 0.07f, 0.03f
    };

    int cur = slot->enchant;
    if (cur >= 10) {
        spdlog::info("Item already max upgrade (+{})", cur);
        return false;
    }

    uint32_t cost = 500 + cur * 300;
    if (inv.gold < cost) {
        spdlog::info("Not enough gold (need {}, have {})", cost, inv.gold);
        return false;
    }
    inv.gold -= cost;

    bool success = std::uniform_real_distribution<float>(0, 1)(item_rng) < upgrade_rates[cur];
    if (success) {
        slot->enchant++;
        spdlog::info("Upgrade success! Item is now +{}", slot->enchant);

        if (slot->enchant == 7) {
            spdlog::info("Item glows with power! +7 effect activated");
        } else if (slot->enchant == 10) {
            spdlog::info("MAX upgrade! Item stats are maximized, name color changed");
        }
    } else {
        if (cur >= 6) {
            slot->item_id = 0;
            slot->enchant = 0;
            slot->count = 0;
            spdlog::info("Upgrade failed! Item destroyed");
        } else {
            if (slot->enchant > 0) slot->enchant--;
            spdlog::info("Upgrade failed! Downgraded to +{}", slot->enchant);
        }
    }
    return success;
}

bool ItemSystem::EnchantItem(entt::registry& reg, entt::entity player, size_t slot_index) {
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    auto* slot = inv.Get(slot_index);
    if (!slot || slot->item_id == 0) return false;
    if (slot->enchant >= 15) { spdlog::info("Item already max enchant"); return false; }
    float prob = 1.0f - slot->enchant * 0.05f;
    bool success = std::uniform_real_distribution<float>(0, 1)(item_rng) < prob;
    uint32_t cost = 100 + slot->enchant * 50;
    if (inv.gold < cost) { spdlog::info("Not enough gold"); return false; }
    inv.gold -= cost;
    if (success) {
        slot->enchant++;
        spdlog::info("Enchant success! +{}", slot->enchant);
    } else {
        slot->item_id = 0;
        spdlog::info("Enchant failed! Item destroyed");
    }
    return success;
}

bool ItemSystem::ComposeItem(entt::registry& reg, entt::entity player, uint32_t recipe_id) {
    (void)recipe_id;
    if (!reg.valid(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    // Simple compose: combine 3 identical items to make a higher tier
    int found_first = -1;
    uint32_t target_item = 0;
    int count = 0;
    for (size_t i = 0; i < inv.slots.size(); i++) {
        if (inv.slots[i].item_id > 0) {
            if (found_first < 0) { found_first = (int)i; target_item = inv.slots[i].item_id; count = inv.slots[i].count; }
            else if (inv.slots[i].item_id == target_item) { count += inv.slots[i].count; }
        }
    }
    if (count < 3) { spdlog::info("Need 3 of same item to compose"); return false; }
    // Remove 3, add 1 upgraded
    int to_remove = 3;
    for (size_t i = 0; i < inv.slots.size() && to_remove > 0; i++) {
        if (inv.slots[i].item_id == target_item) {
            uint16_t take = std::min((uint16_t)to_remove, inv.slots[i].count);
            inv.RemoveItem(i, take);
            to_remove -= take;
        }
    }
    inv.AddItem(target_item + 1000, 1); // Higher tier item
    spdlog::info("Composed item {} from 3x{}", target_item + 1000, target_item);
    return true;
}
