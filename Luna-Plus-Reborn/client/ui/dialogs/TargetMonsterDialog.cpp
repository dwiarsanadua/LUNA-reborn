#include "TargetMonsterDialog.hpp"
#include <rendering/UIRenderer.hpp>
#include <cstdio>
#include <algorithm>

void TargetMonsterDialog::UpdateFromState(GameState* state, const std::vector<Monster>& monsters) {
    if (!state) return;

    if (state->target_entity < 0) {
        target_entity_ = -1;
        target_type_ = TargetType::None;
        return;
    }

    target_entity_ = state->target_entity;

    // Search in monsters
    for (const auto& m : monsters) {
        if ((int32_t)m.GetID() == target_entity_ && m.IsAlive()) {
            target_name_ = m.GetName();
            target_level_ = m.GetLevel();
            target_hp_ = m.GetHP();
            target_max_hp_ = m.GetMaxHP();
            target_hp_pct_ = target_max_hp_ > 0
                ? (target_hp_ * 100) / target_max_hp_ : 0;
            is_boss_ = (m.GetType() == MonsterType::Boss);
            target_type_ = is_boss_ ? TargetType::Boss : TargetType::Monster;
            return;
        }
    }

    // Search in remote entities (players/NPCs)
    for (const auto& e : state->entities) {
        if (e.id == (uint32_t)target_entity_) {
            target_name_ = e.name;
            target_level_ = e.level;
            target_hp_pct_ = e.hp_pct;
            target_max_hp_ = 100;
            target_hp_ = (target_hp_pct_ * target_max_hp_) / 100;
            is_boss_ = false;
            target_type_ = TargetType::Player;
            return;
        }
    }

    // Target not found
    target_entity_ = -1;
    target_type_ = TargetType::None;
}

void TargetMonsterDialog::Render(UIRenderer& ui, float x, float y) {
    if (target_type_ == TargetType::None) return;

    float w = 240.0f;
    float h = 85.0f;

    // Background
    ui.DrawRect(x, y, w, h, {20, 20, 30, 200});
    ui.DrawBorder(x, y, w, h, {80, 80, 100, 200});

    // Title
    const char* type_tag = "";
    if (target_type_ == TargetType::Boss) type_tag = "[BOSS] ";
    else if (target_type_ == TargetType::Player) type_tag = "[P] ";
    else if (target_type_ == TargetType::NPC) type_tag = "[NPC] ";

    uint32_t title_color = 0xffffffff;
    if (target_type_ == TargetType::Boss) title_color = 0xffff4444;
    else if (target_type_ == TargetType::Player) title_color = 0xff44ff44;

    char title_buf[128];
    snprintf(title_buf, sizeof(title_buf), "%s%s Lv.%d", type_tag, target_name_.c_str(), target_level_);
    ui.DrawText(x + 8, y + 6, title_color, "%s", title_buf);

    // HP Bar background
    ui.DrawRect(x + 8, y + 28, w - 16, 18, {60, 0, 0, 180});

    // HP Bar fill
    float hp_fill = (float)target_hp_pct_ / 100.0f;
    UIColor bar_color = {60, 200, 60, 255};
    if (hp_fill < 0.5f) bar_color = {255, 200, 60, 255};
    if (hp_fill < 0.25f) bar_color = {255, 60, 60, 255};
    if (is_boss_) bar_color = {255, 40, 40, 255};

    float bar_w = (w - 16) * hp_fill;
    if (bar_w > 0)
        ui.DrawRect(x + 8, y + 28, bar_w, 18, bar_color);

    // HP Text
    char hp_buf[64];
    snprintf(hp_buf, sizeof(hp_buf), "%d / %d (%d%%)", target_hp_, target_max_hp_, target_hp_pct_);
    ui.DrawText(x + 12, y + 29, 0xffffffff, "%s", hp_buf);

    // Level signal indicator (based on level difference)
    if (target_level_ > 0) {
        int level_diff = target_level_ - 1; // compare to player level (simplified)
        UIColor signal_color = {180, 180, 180, 200};
        if (level_diff >= 10) signal_color = {255, 60, 60, 200};
        else if (level_diff >= 5) signal_color = {255, 200, 60, 200};
        else if (level_diff >= 0) signal_color = {200, 200, 200, 200};
        else signal_color = {60, 200, 60, 200};

        ui.DrawRect(x + w - 40, y + 6, 6, 18, signal_color);
    }

    // HP % text on right side
    char pct_buf[16];
    snprintf(pct_buf, sizeof(pct_buf), "%d%%", target_hp_pct_);
    uint32_t pct_color = 0xffffffff;
    if (target_hp_pct_ < 25) pct_color = 0xffff4444;
    else if (target_hp_pct_ < 50) pct_color = 0xffffcc00;
    ui.DrawText(x + w - 50, y + 29, pct_color, "%s", pct_buf);
}
