#include "HousingDialog.hpp"
#include <cstdio>

void HousingDialog::Open(GameState* state, WindowManager* wm) {
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/HousingMainPointDlg.bin.txt",
        "Housing", 100, 60, 480, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    info_label_ = window_->AddWidget<Label>("", 10, 4, 0xff88ff88);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 460, 320);

    // Tab 0: Overview
    auto* overview_lbl = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 1: Furniture
    furniture_grid_ = window_->AddWidget<Grid>(4, 4, 100, 28, 14, 80);
    furniture_grid_->SetPadding(3);

    // Tab 2: Decoration mode
    auto* decor_lbl = window_->AddWidget<Label>("Decoration Mode\n\nPlace furniture by clicking\nan empty slot.\n\n[Coming Soon: Full 3D preview]", 14, 56, 0xffcccccc);

    tabs_->OnEvent([this, overview_lbl, decor_lbl](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            overview_lbl->SetVisible(e.int_value == 0);
            furniture_grid_->SetVisible(e.int_value == 1);
            decor_lbl->SetVisible(e.int_value == 2);
            decor_lbl->SetVisible(e.int_value == 2);
        }
    });
    furniture_grid_->SetVisible(false);
    decor_lbl->SetVisible(false);

    // Action buttons
    auto* buy_btn = window_->AddWidget<Button>("Buy House (10,000g)", 10, 360, 160, 24);
    buy_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (state->gold >= 10000) {
                state->gold -= 10000;
                state->chat_messages.push_back("Congratulations! You bought a house!");
                if (info_label_) info_label_->SetText("You own: Cozy Cottage (ID: 1)");
            } else {
                state->chat_messages.push_back("Not enough gold! Need 10,000g.");
            }
        }
    });

    auto* enter_btn = window_->AddWidget<Button>("Enter House", 180, 360, 120, 24);
    enter_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (info_label_) info_label_->SetText("Entered your house.");
        }
    });

    Refresh(state);
}

void HousingDialog::Refresh(GameState* state) {
    (void)state;
    if (info_label_) info_label_->SetText("Housing System\n\nBuy a house to start decorating!\nPlace furniture in your rooms.");
    if (furniture_grid_) {
        furniture_grid_->ClearAll();
        // Sample furniture
        const char* sample_furniture[] = {"Wooden Table", "Comfy Chair", "Lamp", "Rug", "Bookshelf", "Bed", "Desk", "Cabinet"};
        for (int i = 0; i < 8 && i < 16; i++) {
            int r = i / 4;
            int c = i % 4;
            GridSlot gs;
            gs.empty = false;
            gs.text = sample_furniture[i];
            furniture_grid_->SetSlot(r, c, gs);
        }
    }
}

void HousingDialog::UpdateFromState(GameState* state) {
    (void)state;
}
