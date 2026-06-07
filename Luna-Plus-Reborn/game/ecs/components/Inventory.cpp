#include "Inventory.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

int32_t Inventory::FindItem(uint32_t item_id) const {
    for (size_t i = 0; i < slots.size(); i++) {
        if (slots[i].item_id == item_id) return static_cast<int32_t>(i);
    }
    return -1;
}

bool Inventory::AddItem(uint32_t item_id, uint16_t count) {
    for (auto& slot : slots) {
        if (slot.item_id == item_id) {
            slot.count += count;
            return true;
        }
    }
    for (auto& slot : slots) {
        if (slot.item_id == 0) {
            slot.item_id = item_id;
            slot.count = count;
            slot.item_uid = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&slot));
            return true;
        }
    }
    spdlog::warn("Inventory full, cannot add item {}", item_id);
    return false;
}

bool Inventory::RemoveItem(size_t idx, uint16_t count) {
    if (idx >= slots.size() || slots[idx].item_id == 0) return false;
    if (slots[idx].count < count) return false;
    slots[idx].count -= count;
    if (slots[idx].count == 0) slots[idx] = ItemSlot{};
    return true;
}

void Inventory::SwapItems(size_t a, size_t b) {
    if (a >= slots.size() || b >= slots.size()) return;
    std::swap(slots[a], slots[b]);
}
