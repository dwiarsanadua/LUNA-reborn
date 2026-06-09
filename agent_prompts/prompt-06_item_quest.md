# Agent-06 — Server Methods: Item/Quest Additional Features

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Implementasi method tambahan untuk ItemSystem dan QuestSystem yang masih missing dari Old.

## Aturan Ketat

1. BACA dulu file yang akan diubah — catat method yang SUDAH ADA
2. ✅ Jika SUDAH ADA → skip. Jangan duplikasi.
3. 🔧 Jika BELUM ADA → tambah dengan style yang sama
4. Build verify setelah selesai

## ItemSystem — Method yang harus dicek

Baca server/map/systems/ItemSystem.cpp:
```bash
grep -n "void\|bool\|int32_t\|uint32_t" server/map/systems/ItemSystem.*
```

Cek method berikut:
- `AddItem` → cek ✅/❌
- `RemoveItem` → cek
- `MoveItem` → cek
- `EquipItem` / `UnequipItem` → cek
- `UseItem` → cek
- `BuyItem` → cek (dari NPC shop)
- `SellItem` → cek (jual ke NPC)
- `ReinforceItem` → cek (Old: MP_ITEM_REINFORCE_SYN)
- `EnchantItem` → cek (Old: MP_ITEM_ENCHANT_SYN)
- `MixItem` → cek (Old: MP_ITEM_MIX_SYN)
- `ComposeItem` → cek (Old: MP_ITEM_COMPOSE_SYN)
- `DissolveItem` → cek (Old: MP_ITEM_DISSOLVE_SYN)
- `DurabilityDegrade` → cek (Old: durability system)
- `GenerateItemOption` → cek (Old: random option on drop)

Untuk setiap yang BELUM ADA, implementasi minimal:
```cpp
bool ItemSystem::SellItem(entt::registry& reg, entt::entity player, uint32_t slot) {
    auto* inv = reg.try_get<Inventory>(player);
    if (!inv || slot >= inv->slots.size() || inv->slots[slot].item_id == 0) return false;
    // Calculate sell price
    uint32_t price = inv->slots[slot].item_id / 10; // simplified
    inv->gold += price;
    inv->slots[slot] = ItemSlot{}; // clear slot
    spdlog::debug("ItemSystem: sold slot {} for {} gold", slot, price);
    return true;
}
```

## QuestSystem — Method yang harus dicek

Baca server/map/systems/QuestSystem.cpp:
- `StartQuest` → cek
- `CompleteQuest` → cek
- `ClaimReward` → cek
- `AbandonQuest` → cek
- `UpdateObjective` → cek (kill count, item collect, etc.)
- `CheckConditions` → cek
- `FailQuest` → cek

## Output

✅ Kembalikan: "Agent-06 done: added [list method item + quest yang ditambah]"
