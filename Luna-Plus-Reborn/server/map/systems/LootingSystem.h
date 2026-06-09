#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <random>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Inventory.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/components/Transform.hpp>

struct LootItemEntry {
    ItemSlot item;
    bool is_wear_item = false;
};

class LootingSystem {
public:
    LootingSystem();

    bool ShouldLoot(const CharacterStats& victim, const CharacterStats& attacker);
    void CreateLootRoom(entt::entity victim, entt::entity attacker, entt::registry& registry);
    bool TakeLootItem(entt::entity looter, uint32_t loot_room_id, uint32_t item_slot, entt::registry& registry);
    void AutoLoot(entt::entity victim, entt::entity attacker, entt::registry& registry);
    void Process(entt::registry& registry, float dt);

private:
    struct LootRoom {
        uint32_t id = 0;
        entt::entity victim = entt::null;
        entt::entity attacker = entt::null;
        std::vector<LootItemEntry> items;
        float time_remaining = 0.0f;
    };

    std::mt19937 rng_;
    std::unordered_map<uint32_t, LootRoom> loot_rooms_;
    uint32_t next_room_id_ = 1;

    float GetLootChance(int32_t bad_fame);
    int GetLootItemCount(int32_t bad_fame);
    float GetWearItemLootRatio(int32_t bad_fame);
};
