#include "FishingDialog.hpp"
#include <ui/WindowManager.hpp>
#include <ui/widgets/Button.hpp>
#include <rendering/UIRenderer.hpp>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>

void FishingDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/FishingDlg.bin.txt",
            "FISHING", 440, 260, 400, 200);
    }
    if (!window_) {
        window_ = new Window("FISHING", 440, 260, 400, 200);
        window_->SetClosable(true);
        status_label_ = window_->AddWidget<Label>("Press SPACE to cast line", 20, 30, 0xffffcc88);
        fish_label_ = window_->AddWidget<Label>("", 20, 60, 0xffffffff);
        progress_bar_ = window_->AddWidget<ProgressBar>(20, 100, 360, 20);
    } else {
        window_->SetClosable(true);
        status_label_ = window_->FindWidget<Label>("");
        fish_label_ = window_->FindWidget<Label>("");
        progress_bar_ = window_->FindWidget<ProgressBar>("");
        if (!status_label_)
            status_label_ = window_->AddWidget<Label>("Press SPACE to cast line", 20, 30, 0xffffcc88);
        if (!progress_bar_)
            progress_bar_ = window_->AddWidget<ProgressBar>(20, 100, 360, 20);
    }

    active_ = true;
    casting_ = false;
    catching_ = false;
    caught_ = false;
    progress_ = 0;
    wait_timer_ = 0;

    if (window_ && cast_cb_) {
        auto* cast_btn = window_->AddWidget<Button>("Cast Line", 20, 130, 120, 24);
        cast_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click && cast_cb_) cast_cb_();
        });
    }

    auto* point_btn = window_->AddWidget<Button>("Fishing Point", 160, 130, 120, 24);
    point_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OpenPointDialog();
    });
}

// ─── Fishing Point System ─────────────────────────────────

void FishingDialog::OpenPointDialog() {
    point_active_ = true;
    add_points_ = 0;
    fill_slot_count_ = 0;
    for (int i = 0; i < MAX_FISH_SLOTS; i++) {
        fish_slots_[i] = FishItemInfo();
    }
}

void FishingDialog::ClosePointDialog() {
    point_active_ = false;
    ClearFishSlots();
}

void FishingDialog::SetCurrentPoints(uint32_t points) {
    current_points_ = points;
}

bool FishingDialog::AddFishToSlot(const FishItemInfo& fish) {
    if (fill_slot_count_ >= MAX_FISH_SLOTS) return false;
    if (fish.supply_value <= 0) return false;

    fish_slots_[fill_slot_count_] = fish;
    fish_slots_[fill_slot_count_].point_value = fish.supply_value * fish.durability;
    fill_slot_count_++;
    RecalculatePoints();
    return true;
}

void FishingDialog::ClearFishSlots() {
    for (int i = 0; i < MAX_FISH_SLOTS; i++) {
        fish_slots_[i] = FishItemInfo();
    }
    fill_slot_count_ = 0;
    add_points_ = 0;
}

void FishingDialog::RecalculatePoints() {
    add_points_ = 0;
    for (int i = 0; i < fill_slot_count_; i++) {
        add_points_ += fish_slots_[i].point_value;
    }
}

void FishingDialog::SendConvertPoints() {
    if (add_points_ == 0) return;
    if (current_points_ + add_points_ > MAX_FISH_POINT) return;
}

void FishingDialog::OnConvertAck(uint32_t added_points) {
    current_points_ += added_points;
    ClearFishSlots();
    point_active_ = false;
}

void FishingDialog::OnConvertNack() {
    ClearFishSlots();
}

const FishItemInfo* FishingDialog::GetFishSlot(int idx) const {
    if (idx < 0 || idx >= MAX_FISH_SLOTS) return nullptr;
    return &fish_slots_[idx];
}

void FishingDialog::RenderPointDialog(UIRenderer& ui) {
    if (!point_active_) return;

    float dlg_x = 80, dlg_y = 140;
    float dlg_w = 360, dlg_h = 300;

    ui.DrawRect(dlg_x, dlg_y, dlg_w, dlg_h, UIColor{20, 20, 35, 230});
    ui.DrawBorder(dlg_x, dlg_y, dlg_w, dlg_h, UIColor{100, 120, 180, 200}, 1.0f);

    ui.DrawText(dlg_x + 10, dlg_y + 6, 0xFFFFCC88, "Fishing Point Exchange");

    char buf[64];
    snprintf(buf, sizeof(buf), "Current Points: %u", current_points_);
    ui.DrawText(dlg_x + 10, dlg_y + 30, 0xFF88CCFF, "%s", buf);

    // Fish slots grid (4x3 = 12 slots)
    float slot_size = 48;
    float gap = 6;
    float start_x = dlg_x + 12;
    float start_y = dlg_y + 56;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int idx = row * 4 + col;
            float sx = start_x + col * (slot_size + gap);
            float sy = start_y + row * (slot_size + gap);

            if (idx < fill_slot_count_ && fish_slots_[idx].item_id > 0) {
                ui.DrawRect(sx, sy, slot_size, slot_size, UIColor{50, 80, 50, 200});
                ui.DrawBorder(sx, sy, slot_size, slot_size, UIColor{100, 180, 100, 220}, 1.0f);

                char slot_text[16];
                snprintf(slot_text, sizeof(slot_text), "%d", fish_slots_[idx].point_value);
                float tw = ui.MeasureText(slot_text);
                ui.DrawText(sx + (slot_size - tw) * 0.5f, sy + slot_size - 14, 0xFFFFFF88, "%s", slot_text);
            } else {
                ui.DrawRect(sx, sy, slot_size, slot_size, UIColor{30, 30, 50, 180});
                ui.DrawBorder(sx, sy, slot_size, slot_size, UIColor{60, 60, 100, 150}, 1.0f);
            }
        }
    }

    // Points to add
    snprintf(buf, sizeof(buf), "Points to add: %u", add_points_);
    ui.DrawText(dlg_x + 10, start_y + 3 * (slot_size + gap) + 8, 0xFFFFFF88, "%s", buf);

    // Buttons
    float btn_y = start_y + 3 * (slot_size + gap) + 30;
    float btn_w = 100, btn_h = 24;

    ui.DrawRect(dlg_x + 20, btn_y, btn_w, btn_h, UIColor{50, 100, 50, 220});
    ui.DrawBorder(dlg_x + 20, btn_y, btn_w, btn_h, UIColor{100, 200, 100, 255}, 1.0f);
    float tw1 = ui.MeasureText("Convert");
    ui.DrawText(dlg_x + 20 + (btn_w - tw1) * 0.5f, btn_y + 3, 0xFFFFFFFF, "Convert");

    ui.DrawRect(dlg_x + 130, btn_y, btn_w, btn_h, UIColor{80, 60, 40, 220});
    ui.DrawBorder(dlg_x + 130, btn_y, btn_w, btn_h, UIColor{180, 140, 100, 255}, 1.0f);
    float tw2 = ui.MeasureText("Clear");
    ui.DrawText(dlg_x + 130 + (btn_w - tw2) * 0.5f, btn_y + 3, 0xFFFFFFFF, "Clear");

    ui.DrawRect(dlg_x + 240, btn_y, btn_w, btn_h, UIColor{60, 60, 80, 220});
    ui.DrawBorder(dlg_x + 240, btn_y, btn_w, btn_h, UIColor{140, 140, 180, 255}, 1.0f);
    float tw3 = ui.MeasureText("Close");
    ui.DrawText(dlg_x + 240 + (btn_w - tw3) * 0.5f, btn_y + 3, 0xFFFFFFFF, "Close");
}

void FishingDialog::Update(float dt) {
    if (!active_) return;

    if (casting_) {
        cast_timer_ += dt;
        if (cast_timer_ > 1.0f) {
            casting_ = false;
            wait_timer_ = 0;
            if (status_label_) status_label_->SetText("Waiting for bite...");
        }
    } else if (!catching_ && !caught_) {
        wait_timer_ += dt;
        if (wait_timer_ > MAX_WAIT) {
            if (status_label_) status_label_->SetText("No bite. Try again.");
            active_ = false;
        } else if (wait_timer_ > 2.0f && (rand() % 100) < 3) {
            catching_ = true;
            bite_timer_ = 0;
            catch_window_ = CATCH_WINDOW;
            if (status_label_) status_label_->SetText("BITE! Press SPACE!");
        }
    } else if (catching_) {
        bite_timer_ += dt;
        catch_window_ -= dt;
        progress_ += progress_dir_ * dt * 2.0f;
        if (progress_ >= 1.0f) { progress_ = 1.0f; progress_dir_ = -1.0f; }
        if (progress_ <= 0.0f) { progress_ = 0.0f; progress_dir_ = 1.0f; }
        if (progress_bar_) progress_bar_->SetProgress(progress_);
        if (catch_window_ <= 0) {
            catching_ = false;
            if (status_label_) status_label_->SetText("Fish got away!");
            active_ = false;
        }
    }
}

void FishingDialog::Render(UIRenderer& ui) {
    if (active_ && window_) window_->Render(ui);
    if (point_active_) RenderPointDialog(ui);
}
