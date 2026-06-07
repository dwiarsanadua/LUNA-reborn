#include "DurabilitySystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void DurabilitySystem::Init() {
    durabilities_.clear();
    spdlog::info("DurabilitySystem: initialized");
}

ItemDurability* DurabilitySystem::FindOrCreate(uint32_t item_id) {
    for (auto& d : durabilities_) if (d.item_id == item_id) return &d;
    ItemDurability d;
    d.item_id = item_id;
    d.max_durability = MAX_DURABILITY;
    d.current_durability = MAX_DURABILITY;
    durabilities_.push_back(d);
    return &durabilities_.back();
}

ItemDurability* DurabilitySystem::GetDurability(uint32_t item_id) {
    return FindOrCreate(item_id);
}

void DurabilitySystem::SetDurability(uint32_t item_id, int current, int max_dur) {
    auto* d = FindOrCreate(item_id);
    d->current_durability = std::min(max_dur, std::max(0, current));
    d->max_durability = max_dur;
}

void DurabilitySystem::OnAttack(uint32_t weapon_id) {
    auto* d = FindOrCreate(weapon_id);
    d->current_durability = std::max(0, d->current_durability - ATTACK_DRAIN);
    if (d->current_durability <= 0) d->destroyed = true;
}

void DurabilitySystem::OnHit(uint32_t armor_id) {
    auto* d = FindOrCreate(armor_id);
    d->current_durability = std::max(0, d->current_durability - HIT_DRAIN);
    if (d->current_durability <= 0) d->destroyed = true;
}

void DurabilitySystem::OnSkillUse(uint32_t item_id) {
    auto* d = FindOrCreate(item_id);
    d->current_durability = std::max(0, d->current_durability - ATTACK_DRAIN);
}

bool DurabilitySystem::Repair(uint32_t item_id) {
    auto* d = FindOrCreate(item_id);
    if (d->current_durability >= d->max_durability) return false;
    if (d->destroyed) return false;
    d->current_durability = d->max_durability;
    return true;
}

bool DurabilitySystem::RepairAll(std::vector<uint32_t> item_ids) {
    bool any = false;
    for (auto id : item_ids) { if (Repair(id)) any = true; }
    return any;
}

uint32_t DurabilitySystem::GetRepairCost(uint32_t item_id) const {
    for (auto& d : durabilities_) {
        if (d.item_id == item_id) {
            int missing = d.max_durability - d.current_durability;
            return (uint32_t)(missing * REPAIR_COST_PER_POINT);
        }
    }
    return 0;
}

uint32_t DurabilitySystem::GetRepairCostAll(const std::vector<uint32_t>& item_ids) const {
    uint32_t total = 0;
    for (auto id : item_ids) total += GetRepairCost(id);
    return total;
}

bool DurabilitySystem::IsBroken(uint32_t item_id) const {
    for (auto& d : durabilities_) if (d.item_id == item_id) return d.current_durability <= 0;
    return false;
}

bool DurabilitySystem::IsDestroyed(uint32_t item_id) const {
    for (auto& d : durabilities_) if (d.item_id == item_id) return d.destroyed;
    return false;
}
