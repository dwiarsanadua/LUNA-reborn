# Agent C01 — Remaining Critical Dialogs

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Konteks

Dari 213 .bin dialog files Old, 59 sudah di-port. Berikut 6 dialog CRITICAL yang masih missing. Referensi .bin asli di `/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LunaPlus/Data/Interface/Windows/`.

Baca file .bin.txt yang sudah di-convert di Reborn assets untuk referensi layout.

## File yang harus dibuat/diubah

### 1. client/ui/dialogs/NPCShopDialog.cpp [BARU] — 🔴 NPCShop

Old .bin: NPCShop.bin — NPC shop buy/sell layout
Deskripsi: Dialog untuk membeli dan menjual item dari NPC. NPCDialog.cpp yang sekarang hanya punya shop tab basic.

Pseudo-code:
```cpp
class NPCShopDialog {
    Window* window_ = nullptr;
    Grid* shop_grid_;
    Grid* sell_grid_;
    Label* gold_label_;
    ListBox* category_list_;
    InputField* search_input_;
    
    void Open(uint32_t npc_id) {
        // Load dari asset/interface/Windows/NPCShop.bin.txt
        window_ = wm_->LoadFromScript("NPCShop.bin.txt");
        if (!window_) CreateFallback();
        // Kirim packet request shop data
        network_->Send(NpcRequest(npc_id, NpcAction::Shop));
    }
    
    void OnShopResponse(NpcResponse* resp) {
        // Tampilkan item di shop_grid_
        for (auto& item : *resp->shop_items()) {
            shop_grid_->AddItem(item->item_id(), item->price());
        }
    }
    
    void OnBuy(int slot, uint16_t count) {
        network_->Send(NpcShopBuyRequest(npc_id_, slot, count));
    }
    
    void OnSell(int inv_slot, uint16_t count) {
        network_->Send(NpcShopSellRequest(npc_id_, inv_slot, count));
    }
};
```

### 2. client/ui/dialogs/ItemMallWarehouseDialog.cpp [BARU] — 🔴 ItemMallWarehouse

Old .bin: ItemMallWarehouse.bin — Cash shop item storage
Deskripsi: Warehouse khusus untuk item yang dibeli dari cash shop.

### 3. client/ui/dialogs/IdentificationDialog.cpp [BARU] — 🔴 Identification

Old .bin: IdentificationDlg.bin — Item identification/appraisal
Deskripsi: Dialog untuk mengidentifikasi item unidentified yang di-drop monster.

### 4. client/ui/dialogs/EnchantDialog.cpp [BARU] — 🟡 Enchant

Old .bin: EnchantDialog.bin — Equipment enchanting terpisah dari reinforce
Catatan: UpgradeDialog sekarang handle ReinforceDialog.bin. EnchantDialog.bin adalah dialog terpisah untuk enchant via scroll.

### 5. client/ui/dialogs/SiegeWarFlagDialog.cpp [BARU] — 🟡 SiegeWarFlag

Old .bin: SiegeWarFlagDlg.bin — Siege war flag placement
Deskripsi: UI untuk plant/remove flag di siege war zone.

### 6. client/ui/dialogs/ProgressDialog.cpp [BARU] — 🟡 Progress

Old .bin: ProgressDialog.bin — Progress/wait bar
Deskripsi: Dialog progress bar untuk crafting, moving, item use yang butuh waktu.

## Aturan

1. IKUTI pattern dialog yang sudah ada (lihat HousingDialog.cpp atau HelpDialog.cpp)
2. Setiap dialog harus punya: .hpp + .cpp
3. Daftarkan di WindowManager atau ScreenManager
4. BACA .bin.txt yang sudah di-convert untuk referensi layout
5. JANGAN build atau compile
6. ✅ Kembalikan "Agent C01 done: 6 critical dialogs created"
