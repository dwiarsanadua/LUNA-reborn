#include "MountDialog.hpp"
#include <cstdio>

static const char* mount_names[] = {
    "Brown Horse", "White Horse", "Black Stallion",
    "Elk", "Tiger", "Dragonling",
    "Magic Broom", "Flying Carpet", "Chocobo"
};

void MountDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/RideOptionDlg.bin.txt",
        "Mounts", 200, 80, 360, 380);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Mount Collection", 10, 4, 0xff88ff88);

    auto* mount_grid = window_->AddWidget<Grid>(3, 3, 100, 28, 10, 28);
    mount_grid->SetPadding(3);
    mount_grid->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            current_mount_ = row * 3 + col;
            if (info_label_) {
                char buf[128];
                snprintf(buf, sizeof(buf), "Selected: %s\nPress 'Summon' to ride!",
                    mount_names[current_mount_ % 9]);
                info_label_->SetText(buf);
            }
        }
    });

    for (int i = 0; i < 9; i++) {
        int r = i / 3, c = i % 3;
        GridSlot gs; gs.empty = false; gs.text = mount_names[i];
        mount_grid->SetSlot(r, c, gs);
    }

    info_label_ = window_->AddWidget<Label>("Select a mount to ride.", 10, 140, 0xffffffff);

    auto* summon_btn = window_->AddWidget<Button>("Summon Mount", 10, 280, 130, 24);
    summon_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    summon_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (current_mount_ >= 0 && current_mount_ < 9) {
                state->chat_messages.push_back("You summon your " + std::string(mount_names[current_mount_ % 9]) + "!");
                if (info_label_) info_label_->SetText("Mount summoned! +50% move speed");
            }
        }
    });

    auto* dismiss_btn = window_->AddWidget<Button>("Dismiss", 150, 280, 100, 24);
    dismiss_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    dismiss_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (info_label_) info_label_->SetText("Mount dismissed.");
        }
    });

    auto* buy_btn = window_->AddWidget<Button>("Buy Selected (500g)", 10, 320, 160, 24);
    buy_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (state->gold >= 500) {
                state->gold -= 500;
                state->chat_messages.push_back("Mount purchased!");
                if (info_label_) info_label_->SetText("Mount unlocked permanently!");
            } else {
                if (info_label_) info_label_->SetText("Need 500g to buy this mount!");
            }
        }
    });
}

void MountDialog::UpdateFromState(GameState*) {}
