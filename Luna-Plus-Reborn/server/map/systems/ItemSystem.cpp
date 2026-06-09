#include "ItemSystem.h"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <game/ecs/components/Tag.hpp>
#include <game/ecs/components/Inventory.hpp>
#include <game/ecs/components/Equipment.hpp>
#include <game/ecs/components/CharacterStats.hpp>
#include <game/ecs/components/Transform.hpp>

// ---- Item definition (loaded from DB) ----

struct ItemDefinition {
    uint32_t item_id = 0;
    std::string name;
    uint8_t type = 0;       // 0=consumable, 1=weapon, 2=armor, 3=accessory, 4=material, 5=quest
    uint8_t subtype = 0;    // weapon type, armor slot, etc.
    uint16_t max_stack = 99;
    uint16_t max_durability = 0;  // 0 = indestructible
    int32_t bonus_str = 0;
    int32_t bonus_dex = 0;
    int32_t bonus_int = 0;
    int32_t bonus_con = 0;
    int32_t bonus_atk = 0;
    int32_t bonus_def = 0;
    int32_t bonus_matk = 0;
    int32_t bonus_mdef = 0;
    int32_t bonus_hp = 0;
    int32_t bonus_mp = 0;
    float bonus_hp_regen = 0.0f;
    float bonus_mp_regen = 0.0f;
    float bonus_crit = 0.0f;
    float bonus_evasion = 0.0f;
    float bonus_accuracy = 0.0f;
    float bonus_move_speed = 0.0f;
    uint32_t required_level = 0;
    uint8_t required_class = 0;  // 0=all
    bool is_bound_on_pickup = false;
    bool is_bound_on_equip = false;
    bool is_tradable = true;
    bool is_sellable = true;
    bool is_destroyable = true;
    uint32_t sell_price = 0;
    uint32_t buy_price = 0;
};

static std::unordered_map<uint32_t, ItemDefinition> s_item_defs;
static bool s_item_defs_loaded = false;

ItemSystem::ItemSystem() : rng_(std::random_device{}()) {}

void ItemSystem::LoadLootTables(const std::string& db_path) {
    // Load item definitions from JSON or DB
    if (!s_item_defs_loaded) {
        std::ifstream f(db_path + "/item_defs.json");
        if (f) {
            try {
                nlohmann::json j;
                f >> j;
                for (auto& item : j) {
                    ItemDefinition def;
                    def.item_id = item["id"].get<uint32_t>();
                    def.name = item.value("name", "Unknown");
                    def.type = item.value("type", 0);
                    def.subtype = item.value("subtype", 0);
                    def.max_stack = item.value("max_stack", 99);
                    def.max_durability = item.value("max_durability", 0);
                    def.bonus_str = item.value("bonus_str", 0);
                    def.bonus_dex = item.value("bonus_dex", 0);
                    def.bonus_int = item.value("bonus_int", 0);
                    def.bonus_con = item.value("bonus_con", 0);
                    def.bonus_atk = item.value("bonus_atk", 0);
                    def.bonus_def = item.value("bonus_def", 0);
                    def.bonus_matk = item.value("bonus_matk", 0);
                    def.bonus_mdef = item.value("bonus_mdef", 0);
                    def.bonus_hp = item.value("bonus_hp", 0);
                    def.bonus_mp = item.value("bonus_mp", 0);
                    def.bonus_hp_regen = item.value("bonus_hp_regen", 0.0f);
                    def.bonus_mp_regen = item.value("bonus_mp_regen", 0.0f);
                    def.bonus_crit = item.value("bonus_crit", 0.0f);
                    def.bonus_evasion = item.value("bonus_evasion", 0.0f);
                    def.bonus_accuracy = item.value("bonus_accuracy", 0.0f);
                    def.bonus_move_speed = item.value("bonus_move_speed", 0.0f);
                    def.required_level = item.value("required_level", 0);
                    def.required_class = item.value("required_class", 0);
                    def.is_bound_on_pickup = item.value("is_bound_on_pickup", false);
                    def.is_bound_on_equip = item.value("is_bound_on_equip", false);
                    def.is_tradable = item.value("is_tradable", true);
                    def.is_sellable = item.value("is_sellable", true);
                    def.is_destroyable = item.value("is_destroyable", true);
                    def.sell_price = item.value("sell_price", 0);
                    def.buy_price = item.value("buy_price", 0);
                    s_item_defs[def.item_id] = def;
                }
                s_item_defs_loaded = true;
                spdlog::info("ItemSystem: loaded {} item definitions", s_item_defs.size());
            } catch (const std::exception& e) {
                spdlog::warn("ItemSystem: failed to parse item_defs.json: {}", e.what());
            }
        } else {
            spdlog::info("ItemSystem: no item_defs.json found, using defaults");
        }
        s_item_defs_loaded = true;
    }

    // Load loot tables from JSON
    std::ifstream f(db_path + "/loot_tables.json");
    if (f) {
        try {
            nlohmann::json j;
            f >> j;
            for (auto& entry : j) {
                LootTable table;
                table.table_id = entry["table_id"].get<uint32_t>();
                for (auto& e : entry["entries"]) {
                    LootTableEntry lte;
                    lte.item_id = e["item_id"].get<uint32_t>();
                    lte.drop_chance = e.value("drop_chance", 0.0f);
                    lte.min_count = e.value("min_count", 1);
                    lte.max_count = e.value("max_count", 1);
                    lte.min_enchant = e.value("min_enchant", 0);
                    lte.max_enchant = e.value("max_enchant", 0);
                    lte.min_level = e.value("min_level", 1);
                    lte.max_level = e.value("max_level", 999);
                    table.entries.push_back(lte);
                }
                table.gold_min = entry.value("gold_min", 0);
                table.gold_max = entry.value("gold_max", 0);
                loot_tables_[table.table_id] = table;
            }
            spdlog::info("ItemSystem: loaded {} loot tables from JSON", loot_tables_.size());
        } catch (const std::exception& e) {
            spdlog::warn("ItemSystem: failed to parse loot_tables.json: {}", e.what());
        }
    }

    // Fallback: add default loot tables
    if (loot_tables_.empty()) {
        LootTable wolf_table;
        wolf_table.table_id = 101;
        wolf_table.entries = {
            {1001, 50.0f, 1, 2, 0, 0, 1, 99},
            {1002, 30.0f, 1, 1, 0, 0, 1, 99},
            {2001, 5.0f,  1, 1, 0, 0, 1, 99},
        };
        wolf_table.gold_min = 5;
        wolf_table.gold_max = 20;
        loot_tables_[101] = wolf_table;

        LootTable boss_table;
        boss_table.table_id = 201;
        boss_table.entries = {
            {3001, 100.0f, 1, 1, 0, 0, 1, 99},
            {4001, 20.0f,  1, 1, 0, 3, 1, 99},
            {5001, 10.0f,  1, 1, 0, 5, 1, 99},
        };
        boss_table.gold_min = 100;
        boss_table.gold_max = 500;
        loot_tables_[201] = boss_table;

        spdlog::info("ItemSystem: using default loot tables");
    }

    spdlog::info("ItemSystem: loaded {} total loot tables", loot_tables_.size());
}

bool ItemSystem::AddItem(entt::registry& registry, entt::entity entity, const ItemSlot& item) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv) return false;

    auto it = s_item_defs.find(item.item_id);
    uint16_t max_stack = (it != s_item_defs.end()) ? it->second.max_stack : 999;

    // Try to stack first
    if (item.count > 1 && max_stack > 1) {
        for (size_t i = 0; i < inv->slots.size(); i++) {
            if (inv->slots[i].item_id == item.item_id &&
                inv->slots[i].count + item.count <= max_stack &&
                inv->slots[i].enchant == item.enchant) {
                inv->slots[i].count += item.count;
                spdlog::debug("ItemSystem: stacked item {} x{} in slot {}", item.item_id, item.count, i);
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
        s = ItemSlot{};
    }
    return true;
}

bool ItemSystem::MoveItem(entt::registry& registry, entt::entity entity, size_t from_slot, size_t to_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv) {
        spdlog::warn("ItemSystem::MoveItem: null inventory for entity {}", static_cast<uint32_t>(entity));
        return false;
    }
    if (from_slot >= inv->slots.size()) {
        spdlog::warn("ItemSystem::MoveItem: from_slot {} out of bounds ({})", from_slot, inv->slots.size());
        return false;
    }
    if (to_slot >= inv->slots.size()) {
        spdlog::warn("ItemSystem::MoveItem: to_slot {} out of bounds ({})", to_slot, inv->slots.size());
        return false;
    }

    // If target has same item type, try to stack
    auto& src = inv->slots[from_slot];
    auto& dst = inv->slots[to_slot];

    if (dst.item_id == src.item_id && dst.enchant == src.enchant && dst.item_id != 0) {
        auto it = s_item_defs.find(src.item_id);
        uint16_t max_stack = (it != s_item_defs.end()) ? it->second.max_stack : 999;
        uint16_t space = max_stack - dst.count;
        uint16_t move_count = std::min(src.count, space);
        if (move_count > 0) {
            dst.count += move_count;
            src.count -= move_count;
            if (src.count == 0) src = ItemSlot{};
            return true;
        }
    }

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

static void ApplyEquipmentStats(CharacterStats* stats, const ItemDefinition& def, bool add) {
    int sign = add ? 1 : -1;
    stats->strength += sign * def.bonus_str;
    stats->dexterity += sign * def.bonus_dex;
    stats->intelligence += sign * def.bonus_int;
    stats->constitution += sign * def.bonus_con;
    stats->physic_attack += sign * def.bonus_atk;
    stats->physic_defense += sign * def.bonus_def;
    stats->magic_attack += sign * def.bonus_matk;
    stats->magic_defense += sign * def.bonus_mdef;
    stats->max_hp += sign * def.bonus_hp;
    stats->max_mp += sign * def.bonus_mp;
    stats->hp_regen += sign * def.bonus_hp_regen;
    stats->mp_regen += sign * def.bonus_mp_regen;
    stats->critical_rate += sign * def.bonus_crit;
    stats->evasion += sign * def.bonus_evasion;
    stats->accuracy += sign * def.bonus_accuracy;
    stats->move_speed += sign * def.bonus_move_speed;
}

bool ItemSystem::EquipItem(entt::registry& registry, entt::entity entity, size_t inv_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    auto* equip = registry.try_get<Equipment>(entity);
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (!inv || !equip || !stats || inv_slot >= inv->slots.size()) return false;

    auto& slot = inv->slots[inv_slot];
    if (slot.item_id == 0) return false;

    auto it = s_item_defs.find(slot.item_id);
    if (it == s_item_defs.end()) {
        spdlog::warn("ItemSystem: unknown item {}", slot.item_id);
        return false;
    }
    auto& def = it->second;

    // Check level requirement
    if (stats->level < def.required_level) {
        spdlog::warn("ItemSystem: level {} < required {} for item {}", stats->level, def.required_level, slot.item_id);
        return false;
    }

    // Determine equipment slot from item subtype
    Equipment::Slot eSlot = Equipment::Slot::Weapon;
    switch (def.type) {
        case 1: // Weapon
            eSlot = Equipment::Slot::Weapon;
            break;
        case 2: // Armor
            switch (def.subtype) {
                case 0: eSlot = Equipment::Slot::Body; break;
                case 1: eSlot = Equipment::Slot::Helmet; break;
                case 2: eSlot = Equipment::Slot::Gloves; break;
                case 3: eSlot = Equipment::Slot::Boots; break;
                case 4: eSlot = Equipment::Slot::Shield; break;
                case 5: eSlot = Equipment::Slot::Cape; break;
                default: eSlot = Equipment::Slot::Body; break;
            }
            break;
        case 3: // Accessory
            switch (def.subtype) {
                case 0: eSlot = Equipment::Slot::Necklace; break;
                case 1: eSlot = Equipment::Slot::Ring1; break;
                case 2: eSlot = Equipment::Slot::Earring1; break;
                case 3: eSlot = Equipment::Slot::Belt; break;
                default: eSlot = Equipment::Slot::Necklace; break;
            }
            break;
        case 13: // Costume
            eSlot = Equipment::Slot::Costume;
            break;
        default:
            eSlot = Equipment::Slot::Weapon;
            break;
    }

    size_t eSlotIdx = static_cast<size_t>(eSlot);
    if (eSlotIdx >= Equipment::COUNT) return false;

    // Bind on equip
    if (def.is_bound_on_equip) {
        slot.is_bound = true;
    }

    // Unequip current item in that slot
    uint32_t old_id = equip->items[eSlotIdx];
    if (old_id != 0) {
        auto old_it = s_item_defs.find(old_id);
        if (old_it != s_item_defs.end()) {
            ApplyEquipmentStats(stats, old_it->second, false);
        }
    }

    // Equip new item
    equip->items[eSlotIdx] = slot.item_id;
    equip->item_uids[eSlotIdx] = slot.item_uid;
    equip->enchants[eSlotIdx] = slot.enchant;
    ApplyEquipmentStats(stats, def, true);

    // Clear inventory slot
    slot = ItemSlot{};

    spdlog::info("ItemSystem: equipped item {} ({}) in slot {}",
                 slot.item_id, def.name, static_cast<int>(eSlot));
    return true;
}

bool ItemSystem::UnequipItem(entt::registry& registry, entt::entity entity, uint8_t equip_slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    auto* equip = registry.try_get<Equipment>(entity);
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (!inv || !equip || !stats) return false;

    auto slot = static_cast<Equipment::Slot>(equip_slot);
    size_t slotIdx = static_cast<size_t>(slot);
    if (slotIdx >= Equipment::COUNT) return false;
    if (equip->items[slotIdx] == 0) return false;

    auto it = s_item_defs.find(equip->items[slotIdx]);
    if (it != s_item_defs.end()) {
        ApplyEquipmentStats(stats, it->second, false);
    }

    // Find empty inventory slot
    for (size_t i = 0; i < inv->slots.size(); i++) {
        if (inv->slots[i].item_id == 0) {
            inv->slots[i].item_id = equip->items[slotIdx];
            inv->slots[i].item_uid = equip->item_uids[slotIdx];
            inv->slots[i].enchant = equip->enchants[slotIdx];
            inv->slots[i].count = 1;

            equip->items[slotIdx] = 0;
            equip->item_uids[slotIdx] = 0;
            equip->enchants[slotIdx] = 0;
            return true;
        }
    }

    // No empty slot, restore stats
    if (it != s_item_defs.end()) {
        ApplyEquipmentStats(stats, it->second, true);
    }
    spdlog::warn("ItemSystem: inventory full, cannot unequip");
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

            // Bind on pickup
            auto def_it = s_item_defs.find(item.item_id);
            if (def_it != s_item_defs.end() && def_it->second.is_bound_on_pickup) {
                item.is_bound = true;
            }

            drops.push_back(item);
        }
    }
    return drops;
}

std::vector<ItemSlot> ItemSystem::GenerateDrop(int monster_template_id, int monster_level) {
    uint32_t table_id = (monster_template_id < 200) ? 101 : 201;
    auto drops = RollLootTable(table_id, monster_level);

    // Add gold (scaled by level)
    ItemSlot gold;
    gold.item_id = 0;
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
        registry.emplace<TagMonster>(loot_entity);
        spdlog::debug("ItemSystem: spawned loot item {} x{} at ({:.1f},{:.1f},{:.1f})",
                      item.item_id, item.count, position.x, position.y, position.z);
    }
}

bool ItemSystem::UpgradeItem(ItemSlot& item) {
    int rate = GetUpgradeSuccessRate(item.enchant);
    int roll = std::uniform_int_distribution<int>(1, 100)(rng_);

    if (roll <= rate) {
        item.enchant++;
        spdlog::info("ItemSystem: upgrade success to +{} (rate={}%)", item.enchant, rate);
        return true;
    }

    // Enchant protection: items +5 or lower are destroyed, +6+ just lose 1 enchant level
    if (item.enchant >= 6) {
        item.enchant--;
        spdlog::warn("ItemSystem: upgrade failed at +{}, degraded to +{}", item.enchant + 1, item.enchant);
    } else {
        spdlog::warn("ItemSystem: upgrade failed at +{}, item destroyed", item.enchant);
        item = ItemSlot{};
    }
    return false;
}

int ItemSystem::GetUpgradeSuccessRate(int current_enchant) {
    // Standard Luna upgrade rates
    static const int rates[] = {100, 90, 80, 70, 50, 35, 20, 10, 5, 1};
    if (current_enchant < 0 || current_enchant >= 10) return 0;
    return rates[current_enchant];
}

void ItemSystem::UseItem(entt::registry& registry, entt::entity entity, size_t slot) {
    auto* inv = registry.try_get<Inventory>(entity);
    if (!inv) {
        spdlog::warn("ItemSystem::UseItem: null inventory for entity {}", static_cast<uint32_t>(entity));
        return;
    }
    if (slot >= inv->slots.size()) {
        spdlog::warn("ItemSystem::UseItem: slot {} out of bounds ({})", slot, inv->slots.size());
        return;
    }

    auto& item = inv->slots[slot];
    if (item.item_id == 0) {
        spdlog::warn("ItemSystem::UseItem: empty slot {}", slot);
        return;
    }

    auto* stats = registry.try_get<CharacterStats>(entity);
    if (!stats) {
        spdlog::warn("ItemSystem::UseItem: null CharacterStats for entity {}", static_cast<uint32_t>(entity));
        return;
    }

    auto it = s_item_defs.find(item.item_id);
    uint8_t item_type = (it != s_item_defs.end()) ? it->second.type : 0;

    switch (item_type) {
        case 0: { // Consumable
            switch (item.item_id) {
                case 1001: { // HP Potion (small)
                    int heal = 100 + (item.enchant * 25);
                    stats->hp = std::min(stats->max_hp, stats->hp + heal);
                    RemoveItem(registry, entity, slot, 1);
                    spdlog::info("ItemSystem: used HP potion, +{} HP -> {}/{}", heal, stats->hp, stats->max_hp);
                    break;
                }
                case 1002: { // MP Potion (small)
                    int heal = 50 + (item.enchant * 15);
                    stats->mp = std::min(stats->max_mp, stats->mp + heal);
                    RemoveItem(registry, entity, slot, 1);
                    spdlog::info("ItemSystem: used MP potion, +{} MP -> {}/{}", heal, stats->mp, stats->max_mp);
                    break;
                }
                case 1003: { // Full Recovery
                    stats->hp = stats->max_hp;
                    stats->mp = stats->max_mp;
                    RemoveItem(registry, entity, slot, 1);
                    spdlog::info("ItemSystem: used Full Recovery");
                    break;
                }
                case 1004: { // Scroll of Escape
                    // Teleport to bind point — handled by MapServer
                    RemoveItem(registry, entity, slot, 1);
                    spdlog::info("ItemSystem: used Scroll of Escape");
                    break;
                }
                default: {
                    if (it != s_item_defs.end()) {
                        stats->hp = std::min(stats->max_hp, stats->hp + it->second.bonus_hp);
                        stats->mp = std::min(stats->max_mp, stats->mp + it->second.bonus_mp);
                        RemoveItem(registry, entity, slot, 1);
                        spdlog::info("ItemSystem: used consumable {}", item.item_id);
                    } else {
                        spdlog::warn("ItemSystem::UseItem: item {} not found in definitions", item.item_id);
                    }
                    break;
                }
            }
            break;
        }
        default:
            spdlog::debug("ItemSystem: item {} type {} cannot be used directly", item.item_id, item_type);
            break;
    }
}

void ItemSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<Inventory, CharacterStats>();
    for (auto entity : view) {
        auto& inv = view.get<Inventory>(entity);
        auto& stats = view.get<CharacterStats>(entity);

        for (size_t i = 0; i < inv.slots.size(); i++) {
            auto& slot = inv.slots[i];
            if (slot.item_id == 0) continue;

            auto it = s_item_defs.find(slot.item_id);
            if (it == s_item_defs.end()) continue;
            auto& def = it->second;

            // Check expiration
            if (slot.expiration > 0) {
                if (slot.expiration <= static_cast<uint32_t>(dt)) {
                    slot = ItemSlot{};
                    spdlog::debug("ItemSystem: expired item {} removed from slot {}", slot.item_id, i);
                    continue;
                }
                slot.expiration -= static_cast<uint32_t>(dt);
            }
        }
    }
}
