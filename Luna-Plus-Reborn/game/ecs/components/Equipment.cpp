#include "Equipment.hpp"
#include <cstring>

void Equipment::Equip(Slot slot, uint32_t item_id, uint32_t uid) {
    if (slot >= COUNT) return;
    items[slot] = item_id;
    item_uids[slot] = uid;
}

void Equipment::Unequip(Slot slot) {
    if (slot >= COUNT) return;
    items[slot] = 0;
    item_uids[slot] = 0;
}
