#pragma once
#include <cstdint>

struct Equipment {
    enum Slot : uint8_t {
        Weapon = 0, Shield = 1, Helmet = 2, Body = 3,
        Gloves = 4, Boots = 5, Cape = 6, Necklace = 7,
        Ring1 = 8, Ring2 = 9, Earring1 = 10, Earring2 = 11,
        Belt = 12, Costume = 13, Pet = 14, Vehicle = 15,
        COUNT
    };

    uint32_t items[COUNT] = {0};
    uint32_t item_uids[COUNT] = {0};
    uint8_t enchants[COUNT] = {0};

    bool IsEquipped(Slot slot) const { return items[slot] != 0; }
    void Equip(Slot slot, uint32_t item_id, uint32_t uid);
    void Unequip(Slot slot);
};
