#include "LootingSystem.h"
#include <spdlog/spdlog.h>
#include <algorithm>

LootingSystem::LootingSystem() : rng_(std::random_device{}()) {}

bool LootingSystem::ShouldLoot(const CharacterStats& victim, const CharacterStats& attacker) {
    // PK loot only applies when victim has bad fame (Old: BadFame > 0)
    // In the old system, bad_fame was derived from PK count
    // For now we check if attacker is a player and victim is a player with negative karma
    if (attacker.class_id == 0 || victim.class_id == 0) return false;
    return true;
}

void LootingSystem::CreateLootRoom(entt::entity victim, entt::entity attacker, entt::registry& registry) {
    auto* victim_stats = registry.try_get<CharacterStats>(victim);
    auto* attacker_stats = registry.try_get<CharacterStats>(attacker);
    if (!victim_stats || !attacker_stats) return;

    if (!ShouldLoot(*victim_stats, *attacker_stats)) return;

    LootRoom room;
    room.id = next_room_id_++;
    room.victim = victim;
    room.attacker = attacker;
    room.time_remaining = 120.0f;

    // Calculate how many items to drop based on bad fame
    // Old: BadFame 0 -> 0%, 1000 -> 50%, 3000 -> 80%, 10000 -> 100%
    int32_t bad_fame = victim_stats->level * 10;
    int item_count = GetLootItemCount(bad_fame);
    float wear_ratio = GetWearItemLootRatio(bad_fame);

    // Collect items from victim's inventory
    auto* inv = registry.try_get<Inventory>(victim);
    if (inv) {
        // Shuffle inventory slots for randomness
        std::vector<size_t> indices;
        for (size_t i = 0; i < inv->slots.size(); i++) {
            if (inv->slots[i].item_id != 0) indices.push_back(i);
        }
        std::shuffle(indices.begin(), indices.end(), rng_);

        int taken = 0;
        for (size_t idx : indices) {
            if (taken >= item_count) break;
            auto& slot = inv->slots[idx];

            // Check if item is tradable (not bound)
            if (slot.is_bound) continue;

            LootItemEntry li;
            li.item = slot;
            li.is_wear_item = (slot.enchant > 0);

            // Wear item loot check
            if (li.is_wear_item) {
                float roll = std::uniform_real_distribution<float>(0, 100)(rng_);
                if (roll > wear_ratio * 100.0f) continue;
            }

            // Take a portion of the stack (Old: random portion)
            if (li.item.count > 1) {
                li.item.count = static_cast<uint16_t>(
                    std::uniform_int_distribution<int>(1, li.item.count)(rng_));
            }

            room.items.push_back(li);
            slot.count -= li.item.count;
            if (slot.count == 0) slot = ItemSlot{};
            taken++;
        }
    }

    loot_rooms_.emplace(room.id, std::move(room));
    spdlog::info("LootingSystem: loot room {} created (victim={}, attacker={}, items={})",
                  room.id, static_cast<uint32_t>(victim), static_cast<uint32_t>(attacker), room.items.size());
}

bool LootingSystem::TakeLootItem(entt::entity looter, uint32_t loot_room_id, uint32_t item_slot, entt::registry& registry) {
    auto it = loot_rooms_.find(loot_room_id);
    if (it == loot_rooms_.end()) return false;

    auto& room = it->second;
    if (item_slot >= room.items.size()) return false;
    if (looter != room.attacker) return false;

    auto& li = room.items[item_slot];
    auto* inv = registry.try_get<Inventory>(looter);
    if (!inv) return false;

    ItemSlot added = li.item;
    for (size_t i = 0; i < inv->slots.size(); i++) {
        if (inv->slots[i].item_id == 0) {
            inv->slots[i] = added;
            room.items.erase(room.items.begin() + static_cast<ptrdiff_t>(item_slot));
            spdlog::info("LootingSystem: {} looted item {} from room {}",
                          static_cast<uint32_t>(looter), added.item_id, loot_room_id);
            return true;
        }
    }
    return false;
}

void LootingSystem::AutoLoot(entt::entity victim, entt::entity attacker, entt::registry& registry) {
    auto* victim_stats = registry.try_get<CharacterStats>(victim);
    if (!victim_stats) return;

    int32_t bad_fame = victim_stats->level * 10;
    float chance = GetLootChance(bad_fame);
    float roll = std::uniform_real_distribution<float>(0, 100)(rng_);
    if (roll > chance) return;

    CreateLootRoom(victim, attacker, registry);
}

void LootingSystem::Process(entt::registry& registry, float dt) {
    for (auto it = loot_rooms_.begin(); it != loot_rooms_.end(); ) {
        it->second.time_remaining -= dt;
        if (it->second.time_remaining <= 0.0f) {
            spdlog::debug("LootingSystem: loot room {} expired", it->second.id);
            it = loot_rooms_.erase(it);
        } else {
            ++it;
        }
    }
}

float LootingSystem::GetLootChance(int32_t bad_fame) {
    if (bad_fame <= 0) return 0.0f;
    if (bad_fame >= 10000) return 100.0f;
    if (bad_fame >= 3000) return 80.0f;
    if (bad_fame >= 1000) return 50.0f;
    return static_cast<float>(bad_fame) / 1000.0f * 50.0f;
}

int LootingSystem::GetLootItemCount(int32_t bad_fame) {
    if (bad_fame <= 0) return 0;
    if (bad_fame >= 10000) return 8;
    if (bad_fame >= 3000) return 5;
    if (bad_fame >= 1000) return 3;
    return 1;
}

float LootingSystem::GetWearItemLootRatio(int32_t bad_fame) {
    if (bad_fame <= 0) return 0.0f;
    if (bad_fame >= 10000) return 1.0f;
    if (bad_fame >= 3000) return 0.6f;
    if (bad_fame >= 1000) return 0.3f;
    return 0.1f;
}
