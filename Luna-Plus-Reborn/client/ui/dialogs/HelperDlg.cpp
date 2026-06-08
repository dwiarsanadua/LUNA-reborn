#include "HelperDlg.hpp"
#include <cstdio>
#include <algorithm>

HelperDlg::HelperDlg() {}

void HelperDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->Open("Helper", 880, 400, 240, 260);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(20);
}

void HelperDlg::Close() {
    window_ = nullptr;
}

void HelperDlg::SetCompanionName(const std::string& name) {
    companion_name_ = name;
}

void HelperDlg::SetCompanionLevel(int level) {
    companion_level_ = level;
}

void HelperDlg::SetCompanionHP(int hp, int max_hp) {
    companion_hp_ = hp;
    companion_max_hp_ = max_hp;
}

void HelperDlg::SetCompanionEXP(int exp, int next_exp) {
    companion_exp_ = exp;
    companion_exp_next_ = next_exp;
}

void HelperDlg::SetCompanionMood(float mood) {
    companion_mood_ = std::max(0.0f, std::min(1.0f, mood));
}

void HelperDlg::AddBuff(const HelperBuff& buff) {
    buffs_.push_back(buff);
}

void HelperDlg::ClearBuffs() {
    buffs_.clear();
}

void HelperDlg::Feed() {
    hunger_ = 1.0f;
    companion_mood_ = std::min(1.0f, companion_mood_ + 0.15f);
}

void HelperDlg::Interact() {
    companion_mood_ = std::min(1.0f, companion_mood_ + 0.05f);
}

void HelperDlg::Update(float dt) {
    hunger_ = std::max(0.0f, hunger_ - dt * 0.001f);

    for (auto& b : buffs_) {
        b.remaining -= dt;
    }
    buffs_.erase(std::remove_if(buffs_.begin(), buffs_.end(),
        [](auto& b) { return b.remaining <= 0; }), buffs_.end());
}

void HelperDlg::Render(UIRenderer& ui) {
    if (!window_) return;

    float wx = window_->GetX() + 8;
    float wy = window_->GetY() + window_->GetTitleBarH() + 8;

    char buf[256];

    // Companion name and level
    snprintf(buf, sizeof(buf), "%s  Lv.%d", companion_name_.c_str(), companion_level_);
    ui.DrawText(wx, wy, 0xffffcc88, buf);

    // HP bar
    float hp_pct = (float)companion_hp_ / std::max(1, companion_max_hp_);
    snprintf(buf, sizeof(buf), "HP %d/%d", companion_hp_, companion_max_hp_);
    ui.DrawBar(wx, wy + 22, 220, 12, hp_pct, {255, 60, 60, 255}, {60, 0, 0, 180});
    ui.DrawText(wx + 2, wy + 23, 0xffffffff, buf);

    // EXP bar
    float xp_pct = (float)companion_exp_ / std::max(1, companion_exp_next_);
    ui.DrawBar(wx, wy + 40, 220, 10, xp_pct, {255, 200, 60, 255}, {60, 40, 0, 180});

    // Mood
    const char* mood_str = "Happy";
    if (companion_mood_ < 0.3f) mood_str = "Angry";
    else if (companion_mood_ < 0.6f) mood_str = "Neutral";
    snprintf(buf, sizeof(buf), "Mood: %s (%.0f%%)", mood_str, companion_mood_ * 100);
    ui.DrawText(wx, wy + 56, 0xff88ff88, buf);

    // Hunger
    float hunger_pct = hunger_;
    ui.DrawBar(wx, wy + 74, 120, 10, hunger_pct, {180, 140, 60, 255}, {40, 30, 10, 180});
    ui.DrawText(wx + 124, wy + 73, 0xffcccccc, "Hunger");

    // Buffs
    ui.DrawText(wx, wy + 92, 0xffffcc88, "Active Buffs:");
    float by = wy + 110;
    for (auto& b : buffs_) {
        uint8_t r = (b.color >> 16) & 0xFF;
        uint8_t g = (b.color >> 8) & 0xFF;
        uint8_t bb = b.color & 0xFF;
        ui.DrawRect(wx, by, 8, 8, {r, g, bb, 200});
        snprintf(buf, sizeof(buf), "%s (%.1fs)", b.name.c_str(), b.remaining);
        ui.DrawText(wx + 12, by - 2, b.color, buf);
        by += 16;
    }
    if (buffs_.empty()) {
        ui.DrawText(wx + 10, by, 0xff888888, "No active buffs");
        by += 18;
    }

    // Buttons
    by = std::max(by, wy + 170);
    ui.DrawRect(wx, by, 70, 22, {60, 80, 40, 220});
    ui.DrawBorder(wx, by, 70, 22, {100, 180, 80, 200});
    ui.DrawText(wx + 8, by + 3, 0xffffffff, "Feed");

    ui.DrawRect(wx + 80, by, 70, 22, {40, 60, 80, 220});
    ui.DrawBorder(wx + 80, by, 70, 22, {80, 120, 180, 200});
    ui.DrawText(wx + 84, by + 3, 0xffffffff, "Interact");
}
