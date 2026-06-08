#include "ItemSystem.h"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>
#include <game/ecs/components/Tag.hpp>

ItemSystem::ItemSystem() : rng_(std::random_device{}()) {}

void ItemSystem::LoadLootTables(const std::string& db_path) {
    // In production, loads from game_data.db
    // Example loot table:
    LootTable wolf_table;
    wolf_table.table_id = 101;
    wolf_table.entries = {
        {1001, 50.0f, 1, 2, 0, 0, 1, 99}, // Wolf Fang: 50%
        {1002, 30.0f, 1, 1, 0, 0, 1, 99}, // Wolf Hide: 30%
        {2001, 5.0f,  1, 1, 0, 0, 1, 99}, // Bronze Sword: 5%
    };
    wolf_table.gold_min = 5;
    wolf_table.gold_max = 20;
    loot_tables_[101] = wolf_table;

    LootTable boss_table;
    boss_table.table_id = 201;
    boss_table.entries = {
        {3001, 100.0f, 1, 1, 0, 0, 1, 99}, // Boss Core: 100%
        {4001, 20.0f,  1, 1, 0, 3, 1, 99}, // Rare weapon: 20%, +0~+3
        {5001, 10.0f,  1, 1, 0, 5, 1, 99}, // Epic accessory: 10%, +0~+5
    };
    boss_table.gold_min = 100;
    boss_table.gold_max = 500;
    loot_tables_[201] = boss_table;

    spdlog::info("ItemSystem: loaded {} loot tables", loot_tables_.size());
}

bool ItemSystem::AddItem(entt::registry& registry, entt::entity entity, const ItemSlot& item) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv) return false;

    // Try to stack first
    if (item.count > 1) {
        for (size_t i = 0; i < inv->slots.size(); i++) {
            if (inv->slots[i].item_id == item.item_id &&
                inv->slots[i].count + item.count <= 999) {
                inv->slots[i].count += item.count;
                spdlog::debug("ItemSystem: stacked item {} in slot {}", item.item_id, i);
                return true;
            }
        }
    }

    // Find empty slot
    for (size_t i = 0; i < inv->slots.size(); i++) {
        if (inv->slots[i].item_id == 0) {
            inv->slots[i] = item;
            spdlog::debug("ItemSystem: added item {} to slot {}", item.item_id, i);
            return true;
        }
    }

    spdlog::warn("ItemSystem: inventory full for entity {}", static_cast<uint32_t>(entity));
    return false;
}

bool ItemSystem::RemoveItem(entt::registry& registry, entt::entity entity, size_t slot, uint16_t count) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv || slot >= inv->slots.size()) return false;

    auto& s = inv->slots[slot];
    if (s.item_id == 0 || s.count < count) return false;

    s.count -= count;
    if (s.count == 0) {
        s.item_id = 0;
        s.enchant = 0;
        s.grade = 0;
    }
    return true;
}

bool ItemSystem::MoveItem(entt::registry& registry, entt::entity entity, size_t from_slot, size_t to_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv || from_slot >= inv->slots.size() || to_slot >= inv->slots.size()) return false;
    std::swap(inv->slots[from_slot], inv->slots[to_slot]);
    return true;
}

bool ItemSystem::SplitStack(entt::registry& registry, entt::entity entity, size_t slot, uint16_t count) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv || slot >= inv->slots.size()) return false;
    auto& src = inv->slots[slot];
    if (src.item_id == 0 || src.count <= count) return false;

    ItemSlot split_item = src;
    split_item.count = count;
    src.count -= count;

    return AddItem(registry, entity, split_item);
}

bool ItemSystem::EquipItem(entt::registry& registry, entt::entity entity, size_t inv_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    auto* equip = registry.try_get<Equipment>(entity);
    if (!inv || !equip || inv_slot >= inv->slots.size()) return false;

    auto& slot = inv->slots[inv_slot];
    if (slot.item_id == 0) return false;

    // Determine equipment slot from item subtype (simplified)
    Equipment::Slot eSlot = Equipment::Slot::Weapon;
    if (slot.item_id >= 2000 && slot.item_id < 3000) eSlot = Equipment::Slot::Body;
    else if (slot.item_id >= 3000 && slot.item_id < 4000) eSlot = Equipment::Slot::Helmet;
    else if (slot.item_id >= 4000 && slot.item_id < 5000) eSlot = Equipment::Slot::Gloves;
    else if (slot.item_id >= 5000 && slot.item_id < 6000) eSlot = Equipment::Slot::Boots;

    if (static_cast<size_t>(eSlot) >= Equipment::COUNT) return false;

    // Swap with currently equipped item
    uint32_t old_id = equip->items[static_cast<size_t>(eSlot)];
    equip->items[static_cast<size_t>(eSlot)] = slot.item_id;
    slot.item_id = old_id;
    if (old_id == 0) slot.count = 0;

    // Update stats from equipped item
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (stats) {
        stats->physic_attack += 10; // placeholder stat bonus
        stats->physic_defense += 5;
    }

    spdlog::info("ItemSystem: equipped item {} in slot {}", slot.item_id, static_cast<int>(eSlot));
    return true;
}

bool ItemSystem::UnequipItem(entt::registry& registry, entt::entity entity, uint8_t equip_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    auto* equip = registry.try_get<Equipment>(entity);
    if (!inv || !equip) return false;

    auto slot = static_cast<Equipment::Slot>(equip_slot);
    if (static_cast<size_t>(slot) >= Equipment::COUNT) return false;
    if (equip->items[static_cast<size_t>(slot)] == 0) return false;

    // Find empty inventory slot
    for (size_t i = 0; i < inv->slots.size(); i++) {
        if (inv->slots[i].item_id == 0) {
            inv->slots[i].item_id = equip->items[static_cast<size_t>(slot)];
            inv->slots[i].count = 1;
            equip->items[static_cast<size_t>(slot)] = 0;

            auto* stats = registry.try_get<CharacterStats>(entity);
            if (stats) {
                stats->physic_attack -= 10;
                stats->physic_defense -= 5;
            }
            return true;
        }
    }
    return false;
}

std::vector<ItemSlot> ItemSystem::RollLootTable(uint32_t table_id, int monster_level) {
    std::vector<ItemSlot> drops;
    auto it = loot_tables_.find(table_id);
    if (it == loot_tables_.end()) return drops;

    auto& table = it->second;
    for (auto& entry : table.entries) {
        if (monster_level < entry.min_level || monster_level > entry.max_level) continue;

        float roll = std::uniform_real_distribution<float>(0, 100)(rng_);
        if (roll <= entry.drop_chance) {
            ItemSlot item;
            item.item_id = entry.item_id;
            item.count = static_cast<uint16_t>(
                std::uniform_int_distribution<int>(entry.min_count, entry.max_count)(rng_));
            item.enchant = static_cast<uint8_t>(
                std::uniform_int_distribution<int>(entry.min_enchant, entry.max_enchant)(rng_));
            item.grade = 0;
            drops.push_back(item);
        }
    }
    return drops;
}

std::vector<ItemSlot> ItemSystem::GenerateDrop(int monster_template_id, int monster_level) {
    // Map monster template to loot table
    uint32_t table_id = (monster_template_id < 200) ? 101 : 201; // simple mapping
    auto drops = RollLootTable(table_id, monster_level);

    // Add gold
    ItemSlot gold;
    gold.item_id = 0; // gold marker
    gold.count = static_cast<uint16_t>(
        std::uniform_int_distribution<int>(5, 50)(rng_) * monster_level);
    drops.push_back(gold);

    return drops;
}

void ItemSystem::SpawnLoot(entt::registry& registry, glm::vec3 position, const std::vector<ItemSlot>& items) {
    for (auto& item : items) {
        auto loot_entity = registry.create();
        registry.emplace<ItemSlot>(loot_entity, item);
        registry.emplace<Transform>(loot_entity, position);
        registry.emplace<TagMonster>(loot_entity); // reuse for dropped items
        spdlog::debug("ItemSystem: spawned loot item {} at ({:.1f},{:.1f},{:.1f})",
                      item.item_id, position.x, position.y, position.z);
    }
}

bool ItemSystem::UpgradeItem(ItemSlot& item) {
    int rate = GetUpgradeSuccessRate(item.enchant);
    int roll = std::uniform_int_distribution<int>(1, 100)(rng_);
    if (roll <= rate) {
        item.enchant++;
        spdlog::info("ItemSystem: upgrade success to +{}", item.enchant);
        return true;
    }
    spdlog::info("ItemSystem: upgrade failed at +{}, item destroyed", item.enchant);
    item.item_id = 0;
    item.count = 0;
    return false;
}

int ItemSystem::GetUpgradeSuccessRate(int current_enchant) {
    static const int rates[] = {100, 90, 80, 70, 50, 35, 20, 10, 5, 1};
    if (current_enchant < 0 || current_enchant >= 10) return 0;
    return rates[current_enchant];
}

void ItemSystem::UseItem(entt::registry& registry, entt::entity entity, size_t slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv || slot >= inv->slots.size()) return;

    auto& item = inv->slots[slot];
    if (item.item_id == 0) return;

    auto* stats = registry.try_get<CharacterStats>(entity);
    if (!stats) return;

    // Simple consumables
    if (item.item_id >= 1000 && item.item_id < 2000) {
        if (item.item_id == 1001) { // HP Potion
            stats->hp = std::min(stats->max_hp, stats->hp + 100);
            RemoveItem(registry, entity, slot, 1);
            spdlog::info("ItemSystem: used HP potion, HP now {}/{}", stats->hp, stats->max_hp);
        } else if (item.item_id == 1002) { // MP Potion
            stats->mp = std::min(stats->max_mp, stats->mp + 50);
            RemoveItem(registry, entity, slot, 1);
            spdlog::info("ItemSystem: used MP potion, MP now {}/{}", stats->mp, stats->max_mp);
        }
    }
}

void ItemSystem::Update(entt::registry& registry, float dt) {
    // Process timed item effects, expire buff items, remove expired items
    // For example: check durability, expire event items, etc.
}
