#include "RevivalDialog.hpp"
#include <rendering/UIRenderer.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

RevivalDialog* g_revival_dlg = nullptr;

static constexpr float BTN_W = 180.0f;
static constexpr float BTN_H = 30.0f;

RevivalDialog::RevivalDialog() {
    g_revival_dlg = this;
}

RevivalDialog::~RevivalDialog() {
    g_revival_dlg = nullptr;
}

void RevivalDialog::Open() {
    open_ = true;
    show_confirm_ = false;
    confirm_target_ = 0;
    spdlog::debug("RevivalDialog: opened");
}

void RevivalDialog::Close() {
    open_ = false;
    show_confirm_ = false;
    confirm_target_ = 0;
}

void RevivalDialog::SetCanReviveHere(bool can) {
    can_revive_here_ = can;
}

void RevivalDialog::SetCanReviveTown(bool can) {
    can_revive_town_ = can;
}

bool RevivalDialog::HandleConfirmClick(float mx, float my) {
    if (!show_confirm_) return false;

    float cx = x_ + (width_ - 260.0f) * 0.5f;
    float cy = y_ + 120.0f;

    float yes_x = cx + 20.0f;
    float no_x = cx + 140.0f;
    float btn_y = cy + 40.0f;
    float btn_w = 100.0f;
    float btn_h = 26.0f;

    if (mx >= yes_x && mx <= yes_x + btn_w && my >= btn_y && my <= btn_y + btn_h) {
        show_confirm_ = false;
        if (confirm_target_ == 1 && on_revive_here_) on_revive_here_();
        else if (confirm_target_ == 2 && on_revive_town_) on_revive_town_();
        return true;
    }

    if (mx >= no_x && mx <= no_x + btn_w && my >= btn_y && my <= btn_y + btn_h) {
        show_confirm_ = false;
        confirm_target_ = 0;
        return true;
    }

    return false;
}

bool RevivalDialog::HandleClick(float mx, float my) {
    if (!open_) return false;

    if (show_confirm_) return HandleConfirmClick(mx, my);

    float cx = x_ + (width_ - BTN_W) * 0.5f;

    // Revive Here button
    float here_y = y_ + 60.0f;
    if (can_revive_here_ && mx >= cx && mx <= cx + BTN_W && my >= here_y && my <= here_y + BTN_H) {
        if (player_level_ >= 10) {
            show_confirm_ = true;
            confirm_target_ = 1;
        } else {
            if (on_revive_here_) on_revive_here_();
        }
        return true;
    }

    // Revive Town button
    float town_y = here_y + BTN_H + 10.0f;
    if (can_revive_town_ && mx >= cx && mx <= cx + BTN_W && my >= town_y && my <= town_y + BTN_H) {
        if (player_level_ >= 10) {
            show_confirm_ = true;
            confirm_target_ = 2;
        } else {
            if (on_revive_town_) on_revive_town_();
        }
        return true;
    }

    return false;
}

void RevivalDialog::RenderConfirm(UIRenderer& ui) {
    float cx = x_ + (width_ - 260.0f) * 0.5f;
    float cy = y_ + 120.0f;
    float cw = 260.0f;
    float ch = 100.0f;

    ui.DrawRect(cx, cy, cw, ch, UIColor{20, 20, 35, 240});
    ui.DrawBorder(cx, cy, cw, ch, UIColor{150, 150, 200, 220}, 1.0f);

    const char* msg = (confirm_target_ == 1)
        ? "Revive at current location?"
        : "Revive at town? (Exp penalty)";
    float tw = ui.MeasureText(msg);
    ui.DrawText(cx + (cw - tw) * 0.5f, cy + 12, 0xFFFFFFFF, "%s", msg);

    float btn_w = 100.0f;
    float btn_h = 26.0f;
    float btn_y = cy + 50.0f;

    ui.DrawRect(cx + 20, btn_y, btn_w, btn_h, UIColor{50, 100, 50, 220});
    ui.DrawBorder(cx + 20, btn_y, btn_w, btn_h, UIColor{100, 200, 100, 255}, 1.0f);
    float ty = ui.MeasureText("Yes");
    ui.DrawText(cx + 20 + (btn_w - ty) * 0.5f, btn_y + 4, 0xFFFFFFFF, "Yes");

    ui.DrawRect(cx + 140, btn_y, btn_w, btn_h, UIColor{80, 60, 60, 220});
    ui.DrawBorder(cx + 140, btn_y, btn_w, btn_h, UIColor{200, 100, 100, 255}, 1.0f);
    float tn = ui.MeasureText("No");
    ui.DrawText(cx + 140 + (btn_w - tn) * 0.5f, btn_y + 4, 0xFFFFFFFF, "No");
}

void RevivalDialog::Render(UIRenderer& ui) {
    if (!open_) return;

    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    x_ = (lw - width_) * 0.5f;
    y_ = lh * 0.3f;

    ui.DrawRect(x_, y_, width_, height_, UIColor{15, 15, 30, 220});
    ui.DrawBorder(x_, y_, width_, height_, UIColor{100, 120, 180, 200}, 1.0f);

    ui.DrawText(x_ + 12, y_ + 8, 0xFFFFCC88, "You have died.");

    ui.DrawText(x_ + 12, y_ + 32, 0xFFAAAAAA, "Choose a revival option:");

    float cx = x_ + (width_ - BTN_W) * 0.5f;

    if (can_revive_here_) {
        ui.DrawRect(cx, y_ + 60, BTN_W, BTN_H, UIColor{50, 80, 50, 220});
        ui.DrawBorder(cx, y_ + 60, BTN_W, BTN_H, UIColor{100, 180, 100, 255}, 1.0f);
        float th = ui.MeasureText("Revive Here");
        ui.DrawText(cx + (BTN_W - th) * 0.5f, y_ + 65, 0xFFFFFFFF, "Revive Here");
    } else {
        ui.DrawRect(cx, y_ + 60, BTN_W, BTN_H, UIColor{40, 40, 50, 150});
        ui.DrawBorder(cx, y_ + 60, BTN_W, BTN_H, UIColor{60, 60, 80, 150}, 1.0f);
        float th = ui.MeasureText("Revive Here");
        ui.DrawText(cx + (BTN_W - th) * 0.5f, y_ + 65, 0xFF666666, "Revive Here");
    }

    if (can_revive_town_) {
        ui.DrawRect(cx, y_ + 100, BTN_W, BTN_H, UIColor{60, 60, 100, 220});
        ui.DrawBorder(cx, y_ + 100, BTN_W, BTN_H, UIColor{120, 120, 200, 255}, 1.0f);
        float tt = ui.MeasureText("Revive at Town");
        ui.DrawText(cx + (BTN_W - tt) * 0.5f, y_ + 105, 0xFFFFFFFF, "Revive at Town");
    } else {
        ui.DrawRect(cx, y_ + 100, BTN_W, BTN_H, UIColor{40, 40, 50, 150});
        ui.DrawBorder(cx, y_ + 100, BTN_W, BTN_H, UIColor{60, 60, 80, 150}, 1.0f);
        float tt = ui.MeasureText("Revive at Town");
        ui.DrawText(cx + (BTN_W - tt) * 0.5f, y_ + 105, 0xFF666666, "Revive at Town");
    }

    ui.DrawText(x_ + 12, y_ + 150, 0xFF666688, "XP penalty may apply for town revive.");

    // Close hint
    float tc = ui.MeasureText("Press ESC to close");
    ui.DrawText(x_ + (width_ - tc) * 0.5f, y_ + height_ - 24, 0xFF444466, "Press ESC to close");

    if (show_confirm_) RenderConfirm(ui);
}
