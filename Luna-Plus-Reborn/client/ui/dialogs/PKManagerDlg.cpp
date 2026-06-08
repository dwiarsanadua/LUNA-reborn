#include "PKManagerDlg.hpp"
#include <cstdio>
#include <algorithm>

PKManagerDlg::PKManagerDlg() {}

void PKManagerDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->Open("PK Manager", 400, 150, 400, 350);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);
}

void PKManagerDlg::Close() {
    window_ = nullptr;
}

void PKManagerDlg::SetMode(PKMode mode) {
    if (state_.mode != mode) {
        state_.mode = mode;
        if (mode == PKMode::Aggressive) {
            state_.is_flagged = true;
        } else {
            state_.is_flagged = false;
        }
        if (mode_cb_) mode_cb_(mode);
    }
}

void PKManagerDlg::SetLootMode(LootMode mode) {
    state_.loot_mode = mode;
}

void PKManagerDlg::AddPKPoint(int pts) {
    state_.pk_points += pts;
    if (state_.pk_points >= 5) {
        state_.is_penalized = true;
    }
}

void PKManagerDlg::ResetPKPoints() {
    state_.pk_points = 0;
    state_.is_penalized = false;
}

bool PKManagerDlg::CanAttack(bool is_player) const {
    if (!is_player) return true; // always can attack monsters
    return state_.mode != PKMode::Peaceful;
}

void PKManagerDlg::OnKill() {
    state_.kills++;
    AddPKPoint(1);
    state_.pk_timer = 0;
}

void PKManagerDlg::Update(float dt) {
    state_.pk_timer += dt;

    // Decay PK points over time (1 point per 10 minutes)
    if (state_.pk_points > 0 && state_.pk_timer > 600.0f) {
        state_.pk_points--;
        state_.pk_timer = 0;
        if (state_.pk_points <= 0) {
            state_.is_penalized = false;
        }
    }
}

void PKManagerDlg::ApplyPenalties() {
    // For future: stat reductions, vendor price increases, etc.
}

void PKManagerDlg::Render(UIRenderer& ui) {
    if (!window_) return;

    const char* mode_names[] = { "Peaceful", "Aggressive", "Defense" };
    const char* loot_names[] = { "None", "Party Only", "All" };
    char buf[256];

    float wx = window_->GetX() + 10;
    float wy = window_->GetY() + window_->GetTitleBarH() + 10;

    ui.DrawText(wx, wy, 0xffffcc88, "PK Mode:");
    for (int i = 0; i < 3; i++) {
        uint32_t c = (i == (int)state_.mode) ? 0xff88ff88 : 0xffcccccc;
        const char* indicator = (i == (int)state_.mode) ? ">" : " ";
        ui.DrawText(wx + 10 + i * 120, wy + 20, c, "%s%s", indicator, mode_names[i]);
    }

    wy += 50;
    ui.DrawText(wx, wy, 0xffffcc88, "Loot Mode:");
    ui.DrawText(wx + 10, wy + 20, 0xffffffff, "Current: %s", loot_names[(int)state_.loot_mode]);

    wy += 50;
    ui.DrawText(wx, wy, 0xffffcc88, "PK Status:");
    snprintf(buf, sizeof(buf), "PK Points: %d  |  Kills: %d", state_.pk_points, state_.kills);
    ui.DrawText(wx + 10, wy + 20, 0xffffffff, buf);

    if (state_.is_flagged) {
        ui.DrawText(wx + 10, wy + 40, 0xffff4444, ">> PK FLAGGED <<");
    }
    if (state_.is_penalized) {
        ui.DrawText(wx + 10, wy + 58, 0xffff6644, ">> PENALTY ACTIVE <<");
    }

    wy += 80;
    ui.DrawText(wx, wy, 0xffffcc88, "Penalty Info:");
    ui.DrawText(wx + 10, wy + 20, 0xffcccccc,
        "5+ PK Points: Stat reduction 25%%\n"
        "Vendor prices increased 50%%\n"
        "Cannot enter safe zones\n"
        "PK points decay 1 per 10 min idle");
}
