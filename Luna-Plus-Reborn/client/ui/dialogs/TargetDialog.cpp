#include "TargetDialog.hpp"
#include <rendering/UIRenderer.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstdio>

TargetDialog* g_target_dlg = nullptr;

static constexpr float LIFEPOINT_FIVE   = 1.00f;
static constexpr float LIFEPOINT_FORE   = 0.80f;
static constexpr float LIFEPOINT_THREE  = 0.60f;
static constexpr float LIFEPOINT_TWO    = 0.40f;
static constexpr float LIFEPOINT_ONE    = 0.20f;

TargetDialog::TargetDialog() {
    g_target_dlg = this;
}

TargetDialog::~TargetDialog() {
    g_target_dlg = nullptr;
}

void TargetDialog::SetTarget(const TargetInfo& info) {
    target_ = info;
    target_.has_target = true;
    has_target_ = true;
    anim_timer_ = 0.0f;
    fade_alpha_ = 1.0f;
    context_menu_open_ = false;
}

void TargetDialog::ClearTarget() {
    has_target_ = false;
    target_.has_target = false;
    context_menu_open_ = false;
}

void TargetDialog::SetSize(float w, float h) {
    width_ = w;
    height_ = h;
}

void TargetDialog::SetPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void TargetDialog::RenderTargetName(class UIRenderer& ui) {
    if (target_.name.empty()) return;
    uint32_t color = 0xFFFFFFFF;
    if (target_.is_boss)        color = 0xFFFF4444;
    else if (target_.is_pk)     color = 0xFFFF8888;
    else if (!target_.is_player) color = 0xFFFFFF88;

    float tw = ui.MeasureText(target_.name.c_str());
    float tx = x_ + (width_ - tw) * 0.5f;
    ui.DrawText(tx, y_ + 4, color, "%s", target_.name.c_str());
}

void TargetDialog::RenderTargetLevel(class UIRenderer& ui) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Lv.%d", target_.level);

    uint32_t color = 0xFFFFFFFF;
    int diff = target_.level - 50;
    if (diff >= 10)      color = 0xFFFF4444;
    else if (diff >= 5)  color = 0xFFFF8844;
    else if (diff >= 0)  color = 0xFFFFFF44;
    else                 color = 0xFF88FF88;

    ui.DrawText(x_ + 8, y_ + 24, color, "%s", buf);
}

void TargetDialog::RenderHpBar(class UIRenderer& ui) {
    float bar_x = x_ + 8;
    float bar_y = y_ + 44;
    float bar_w = width_ - 16;
    float bar_h = 14;

    uint32_t hp_bg  = 0xFF222222;
    uint32_t hp_fg  = 0xFF44CC44;
    if (target_.hp_pct < 0.3f) hp_fg = 0xFFCC4444;
    else if (target_.hp_pct < 0.6f) hp_fg = 0xFFCCCC44;

    ui.DrawRect(bar_x, bar_y, bar_w, bar_h, UIColor{34,34,34,200});

    float fill = std::min(1.0f, std::max(0.0f, target_.hp_pct));
    if (fill > 0.0f) {
        ui.DrawRect(bar_x, bar_y, bar_w * fill, bar_h,
            UIColor{
                static_cast<uint8_t>((hp_fg >> 16) & 0xFF),
                static_cast<uint8_t>((hp_fg >> 8) & 0xFF),
                static_cast<uint8_t>(hp_fg & 0xFF),
                220
            });
    }

    ui.DrawBorder(bar_x, bar_y, bar_w, bar_h, UIColor{100,100,120,180}, 1.0f);

    char hp_text[32];
    snprintf(hp_text, sizeof(hp_text), "%d / %d", target_.hp, target_.max_hp);
    float tw = ui.MeasureText(hp_text);
    ui.DrawText(bar_x + (bar_w - tw) * 0.5f, bar_y + 1, 0xFFFFFFFF, "%s", hp_text);
}

void TargetDialog::RenderLifePoints(class UIRenderer& ui) {
    int points = 0;
    if (target_.hp_pct >= LIFEPOINT_FIVE)       points = 5;
    else if (target_.hp_pct >= LIFEPOINT_FORE)  points = 4;
    else if (target_.hp_pct >= LIFEPOINT_THREE) points = 3;
    else if (target_.hp_pct >= LIFEPOINT_TWO)   points = 2;
    else if (target_.hp_pct >= LIFEPOINT_ONE)   points = 1;

    float lx = x_ + 8;
    float ly = y_ + 62;
    float dot_size = 10;
    float gap = 4;

    for (int i = 0; i < 5; i++) {
        uint32_t dot_color = (i < points) ? 0xFF44FF44 : 0xFF333333;
        ui.DrawRect(lx + i * (dot_size + gap), ly, dot_size, dot_size,
            UIColor{
                static_cast<uint8_t>((dot_color >> 16) & 0xFF),
                static_cast<uint8_t>((dot_color >> 8) & 0xFF),
                static_cast<uint8_t>(dot_color & 0xFF),
                200
            });
    }
}

void TargetDialog::RenderBuffIcons(class UIRenderer& ui) {
    if (target_.buff_flags == 0) return;

    float bx = x_ + 8;
    float by = y_ + 78;
    float icon_size = 20;
    float gap = 2;

    for (int i = 0; i < 8; i++) {
        if (target_.buff_flags & (1u << i)) {
            ui.DrawRect(bx + i * (icon_size + gap), by, icon_size, icon_size,
                UIColor{60, 160, 60, 180});
            ui.DrawBorder(bx + i * (icon_size + gap), by, icon_size, icon_size,
                UIColor{100, 200, 100, 220}, 1.0f);
        }
    }
}

void TargetDialog::RenderContextMenu(class UIRenderer& ui) {
    if (!context_menu_open_) return;

    float mx = x_ + width_;
    float my = y_;
    float mw = 140;
    float mh = 100;

    ui.DrawRect(mx, my, mw, mh, UIColor{30, 30, 40, 230});
    ui.DrawBorder(mx, my, mw, mh, UIColor{100, 100, 150, 200}, 1.0f);

    ui.DrawText(mx + 6, my + 4, 0xFFFFFFFF, "Deselect");
}

void TargetDialog::Render(class UIRenderer& ui, float dt) {
    if (!has_target_) return;

    anim_timer_ += dt;

    RenderTargetName(ui);
    RenderTargetLevel(ui);
    RenderHpBar(ui);
    RenderLifePoints(ui);
    RenderBuffIcons(ui);
    RenderContextMenu(ui);
}
