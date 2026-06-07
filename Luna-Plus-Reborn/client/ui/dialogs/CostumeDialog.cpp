#include "CostumeDialog.hpp"
#include <cstdio>

void CostumeDialog::Open(GameState* state, WindowManager* wm, CostumeSystem* cs) {
    costume_ = cs;
    window_ = wm->Open("Costume & Equipment", 250, 100, 420, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    info_label_ = window_->AddWidget<Label>("Equipment & Fashion", 10, 4, 0xff88ccff);

    // Equipment grid (4x5 - 20 slots)
    auto* equip_lbl = window_->AddWidget<Label>("Equipment:", 10, 28, 0xffffcc88);
    (void)equip_lbl;
    equip_grid_ = window_->AddWidget<Grid>(4, 5, 72, 24, 10, 46);
    equip_grid_->SetPadding(2);

    // Fashion grid (2x2 - 4 slots)
    auto* fashion_lbl = window_->AddWidget<Label>("Fashion/Costume:", 10, 192, 0xff88ff88);
    (void)fashion_lbl;
    fashion_grid_ = window_->AddWidget<Grid>(1, 4, 90, 24, 10, 210);
    fashion_grid_->SetPadding(2);

    // Toggle fashion active
    auto* toggle_btn = window_->AddWidget<Button>("Toggle Fashion", 10, 250, 120, 24);
    toggle_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && costume_) {
            costume_->SetFashionActive(!costume_->IsFashionActive());
            Refresh();
        }
    });

    // Apply sample fashion
    auto* apply_btn = window_->AddWidget<Button>("Equip Sample", 140, 250, 120, 24);
    apply_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    apply_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && costume_) {
            costume_->EquipItem(EquipSlot::Weapon, 1001, "assets_converted/mod_objs/d_man.glb");
            costume_->EquipItem(EquipSlot::Armor, 1002, "assets_converted/mod_objs/d_man.glb");
            costume_->ApplyFashion(EquipSlot::FashionHead, "assets_converted/mod_objs/n030.glb");
            Refresh();
        }
    });

    auto* unequip_btn = window_->AddWidget<Button>("Unequip All", 270, 250, 120, 24);
    unequip_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    unequip_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && costume_) {
            for (int i = 0; i < EquipSlot::COUNT; i++) {
                costume_->UnequipItem(i);
                costume_->RemoveFashion(i);
            }
            Refresh();
        }
    });

    // Status
    auto* status = window_->AddWidget<Label>("Fashion shows/hides equipment visuals.", 10, 290, 0xffaaaaaa);
    (void)status;

    Refresh();
}

void CostumeDialog::Refresh() {
    if (!costume_ || !equip_grid_ || !fashion_grid_) return;
    equip_grid_->ClearAll();
    fashion_grid_->ClearAll();

    char buf[128];
    int equip_count = 0;
    for (int i = 0; i < EquipSlot::COUNT && equip_count < 20; i++) {
        if (costume_->IsEquipped(i)) {
            int r = equip_count / 5;
            int c = equip_count % 5;
            snprintf(buf, sizeof(buf), "%s", EquipSlot::GetName(i));
            GridSlot gs; gs.empty = false; gs.text = buf;
            equip_grid_->SetSlot(r, c, gs);
            equip_count++;
        }
    }
    if (equip_count == 0) {
        GridSlot gs; gs.empty = false; gs.text = "(Empty)";
        equip_grid_->SetSlot(0, 0, gs);
    }

    int fashion_count = 0;
    for (int i = EquipSlot::FashionHead; i < EquipSlot::COUNT && fashion_count < 4; i++) {
        if (costume_->HasFashion(i)) {
            int c = fashion_count % 4;
            snprintf(buf, sizeof(buf), "%s", EquipSlot::GetName(i));
            GridSlot gs; gs.empty = false; gs.text = buf;
            fashion_grid_->SetSlot(0, c, gs);
            fashion_count++;
        }
    }
    if (fashion_count == 0) {
        GridSlot gs; gs.empty = false; gs.text = "(No fashion)";
        fashion_grid_->SetSlot(0, 0, gs);
    }

    if (info_label_) {
        snprintf(buf, sizeof(buf), "Equipment: %d  |  Fashion: %d  |  Active: %s",
            costume_->GetEquipCount(), costume_->GetFashionCount(),
            costume_->IsFashionActive() ? "Yes" : "No");
        info_label_->SetText(buf);
    }
}

void CostumeDialog::UpdateFromState(GameState* state) {
    (void)state;
}
