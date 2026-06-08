#include "StallSystem.h"
#include <ecs/components/Inventory.hpp>
#include <ecs/components/Tag.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

StallSystem::StallSystem() {}

bool StallSystem::OpenStall(entt::registry& reg, entt::entity owner,
                            const std::string& title, float x, float y, float z) {
    if (!reg.valid(owner)) return false;
    if (owner_to_stall_.find(static_cast<uint32_t>(owner)) != owner_to_stall_.end()) {
        spdlog::warn("StallSystem: player already has a stall open");
        return false;
    }

    Stall stall;
    stall.stall_id = next_stall_id_++;
    stall.owner_id = static_cast<uint32_t>(owner);
    stall.title = title;
    stall.is_open = true;
    stall.pos_x = x;
    stall.pos_y = y;
    stall.pos_z = z;

    stalls_[stall.stall_id] = stall;
    owner_to_stall_[static_cast<uint32_t>(owner)] = stall.stall_id;

    spdlog::info("StallSystem: stall '{}' opened by entity {}", title, static_cast<uint32_t>(owner));
    return true;
}

bool StallSystem::CloseStall(entt::registry& reg, entt::entity owner) {
    auto it = owner_to_stall_.find(static_cast<uint32_t>(owner));
    if (it == owner_to_stall_.end()) return false;

    stalls_.erase(it->second);
    owner_to_stall_.erase(it);
    spdlog::info("StallSystem: stall closed by entity {}", static_cast<uint32_t>(owner));
    return true;
}

bool StallSystem::AddItem(entt::registry& reg, entt::entity owner,
                          uint32_t item_id, uint16_t count, uint32_t price) {
    auto it = owner_to_stall_.find(static_cast<uint32_t>(owner));
    if (it == owner_to_stall_.end()) return false;

    auto& stall = stalls_[it->second];
    if (stall.items.size() >= 20) {
        spdlog::warn("StallSystem: stall item limit reached");
        return false;
    }

    if (!reg.valid(owner) || !reg.all_of<Inventory>(owner)) return false;
    auto& inv = reg.get<Inventory>(owner);
    int32_t slot = inv.FindItem(item_id);
    if (slot < 0) return false;
    if (!inv.RemoveItem(static_cast<size_t>(slot), count)) return false;

    StallItem si;
    si.item_id = item_id;
    si.count = count;
    si.price_per_unit = price;
    si.slot_index = static_cast<uint8_t>(stall.items.size());
    stall.items.push_back(si);
    return true;
}

bool StallSystem::RemoveItem(entt::registry& reg, entt::entity owner, uint8_t slot_index) {
    auto it = owner_to_stall_.find(static_cast<uint32_t>(owner));
    if (it == owner_to_stall_.end()) return false;

    auto& stall = stalls_[it->second];
    if (slot_index >= stall.items.size()) return false;

    auto& si = stall.items[slot_index];
    if (reg.valid(owner) && reg.all_of<Inventory>(owner)) {
        reg.get<Inventory>(owner).AddItem(si.item_id, si.count);
    }

    stall.items.erase(stall.items.begin() + slot_index);
    return true;
}

std::vector<StallItem> StallSystem::BrowseStall(entt::registry& reg, uint32_t stall_id) {
    auto it = stalls_.find(stall_id);
    if (it == stalls_.end()) return {};
    return it->second.items;
}

bool StallSystem::BuyItem(entt::registry& reg, entt::entity buyer,
                          uint32_t stall_id, uint8_t slot_index, uint16_t count) {
    auto it = stalls_.find(stall_id);
    if (it == stalls_.end()) return false;

    auto& stall = it->second;
    if (slot_index >= stall.items.size()) return false;
    auto& si = stall.items[slot_index];
    if (si.count < count) return false;

    if (!reg.valid(buyer) || !reg.all_of<Inventory>(buyer)) return false;
    auto& buyer_inv = reg.get<Inventory>(buyer);

    uint32_t total_cost = si.price_per_unit * count;
    if (buyer_inv.gold < total_cost) return false;

    uint32_t tax = static_cast<uint32_t>(total_cost * stall.tax_rate);
    uint32_t seller_revenue = total_cost - tax;

    buyer_inv.gold -= total_cost;
    buyer_inv.AddItem(si.item_id, count);

    si.count -= count;

    entt::entity seller_entity = static_cast<entt::entity>(stall.owner_id);
    if (reg.valid(seller_entity) && reg.all_of<Inventory>(seller_entity)) {
        reg.get<Inventory>(seller_entity).gold += seller_revenue;
    }

    if (si.count == 0) {
        stall.items.erase(stall.items.begin() + slot_index);
    }

    spdlog::info("StallSystem: buyer {} purchased {}x{} from stall {} for {} gold",
                 static_cast<uint32_t>(buyer), count, si.item_id, stall_id, total_cost);
    return true;
}

Stall* StallSystem::GetStallByOwner(entt::entity owner) {
    auto it = owner_to_stall_.find(static_cast<uint32_t>(owner));
    if (it == owner_to_stall_.end()) return nullptr;
    return &stalls_[it->second];
}

Stall* StallSystem::GetStallById(uint32_t stall_id) {
    auto it = stalls_.find(stall_id);
    return it != stalls_.end() ? &it->second : nullptr;
}
