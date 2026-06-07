#pragma once
#include <vector>
#include <cstdint>

struct ItemSlot {
    uint32_t item_id = 0;
    uint32_t item_uid = 0;
    uint16_t count = 0;
    uint8_t enchant = 0;
    uint8_t sockets = 0;
    uint32_t socket_items[4] = {0};
    uint8_t grade = 0;
    bool is_bound = false;
    uint32_t expiration = 0;
};

struct Inventory {
    static constexpr size_t MAX_SLOTS = 64;
    std::vector<ItemSlot> slots{MAX_SLOTS};
    uint32_t gold = 0;
    uint32_t bank_gold = 0;

    bool IsEmpty(size_t idx) const { return idx < slots.size() && slots[idx].item_id == 0; }
    ItemSlot* Get(size_t idx) { return idx < slots.size() ? &slots[idx] : nullptr; }
    int32_t FindItem(uint32_t item_id) const;
    bool AddItem(uint32_t item_id, uint16_t count);
    bool RemoveItem(size_t idx, uint16_t count);
    void SwapItems(size_t a, size_t b);
};
