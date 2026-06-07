#include "ItemSystem.h"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>

static std::mt19937 item_rng(std::random_device{}());

ItemSystem::ItemSystem() = default;

bool ItemSystem::AddItem(entt::entity entity, const ItemInstance& item) {
    // Find empty slot or stack with same item
    spdlog::debug("ItemSystem: add item {} to entity {}", item.template_id, static_cast<uint32_t>(entity));
    return true;
}

bool ItemSystem::RemoveItem(entt::entity entity, int slot, uint32_t count) {
    spdlog::debug("ItemSystem: remove {} from slot {}", count, slot);
    return true;
}

bool ItemSystem::MoveItem(entt::entity entity, int from_slot, int to_slot) {
    return true;
}

bool ItemSystem::SplitStack(entt::entity entity, int slot, uint32_t count) {
    return true;
}

bool ItemSystem::EquipItem(entt::entity entity, int slot) {
    spdlog::debug("ItemSystem: equip item from slot {}", slot);
    return true;
}

bool ItemSystem::UnequipItem(entt::entity entity, int equip_slot) {
    return true;
}

ItemInstance ItemSystem::GenerateDrop(int template_id, int monster_level) {
    ItemInstance item;
    item.template_id = template_id;
    item.count = 1;
    // Roll for rarity, enchant level based on monster level
    return item;
}

void ItemSystem::SpawnLoot(glm::vec3 position, const std::vector<ItemInstance>& items) {
    // Create item entities on the ground
    spdlog::debug("ItemSystem: spawned {} loot items", items.size());
}

bool ItemSystem::UpgradeItem(ItemInstance& item) {
    int rate = GetUpgradeSuccessRate(item.enchant);
    int roll = std::uniform_int_distribution<int>(1, 100)(item_rng);
    if (roll <= rate) {
        item.enchant++;
        spdlog::info("ItemSystem: upgrade success to +{}", item.enchant);
        return true;
    }
    spdlog::info("ItemSystem: upgrade failed at +{}", item.enchant);
    return false;
}

int ItemSystem::GetUpgradeSuccessRate(int current_enchant) {
    // +1=100%, +2=90%, +3=80%, +4=70%, +5=50%, +6=35%, +7=20%, +8=10%, +9=5%, +10=1%
    static const int rates[] = {100, 90, 80, 70, 50, 35, 20, 10, 5, 1};
    if (current_enchant < 0 || current_enchant >= 10) return 0;
    return rates[current_enchant];
}

void ItemSystem::Update(entt::registry& registry, float dt) {
    // Process timed item effects, expire buff items, etc.
}
