#include "PKManagerDlg.hpp"
#include <cstdio>
#include <algorithm>

PKManagerDlg::PKManagerDlg() {}

void PKManagerDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/PKLootDlg.bin.txt",
        "PK Manager", 400, 150, 400, 350);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);
}

void PKManagerDlg::Close() {
    window_ = nullptr;
}

void PKManagerDlg::SetMode(PKMode mode) {
    if (state_.mode == mode) return;

    // Cannot toggle PK off while protection timer is active
    if (mode == PKMode::Peaceful && !CanTogglePKOff()) return;

    state_.mode = mode;
    if (mode == PKMode::Aggressive) {
        state_.is_flagged = true;
        StartPKProtectionTimer();
    } else {
        state_.is_flagged = false;
    }
    if (mode_cb_) mode_cb_(mode);
}

void PKManagerDlg::StartPKProtectionTimer() {
    // Base 20 minutes + extra per bad fame (same as old: 20 min + (bad_fame / 75) * 5 min)
    float base_seconds = 20 * 60.0f;
    float extra_per_75_badfame = (state_.bad_fame / 75.0f) * 5 * 60.0f;
    state_.pk_protection_remaining = base_seconds + extra_per_75_badfame;
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
    // Add bad fame on PK kill (thresholds from old: 100k, 500k, 1M, 5M, 10M, 50M, 100M)
    int bad_fame_increment = 100000;
    AddBadFame(bad_fame_increment);
    // Reset protection timer on kill
    if (state_.is_flagged) {
        StartPKProtectionTimer();
    }
}

void PKManagerDlg::Update(float dt) {
    state_.pk_timer += dt;

    // Decay PK protection timer
    if (state_.pk_protection_remaining > 0.0f) {
        state_.pk_protection_remaining = std::max(0.0f, state_.pk_protection_remaining - dt);
    }

    // Decay PK points over time (1 point per 10 minutes)
    if (state_.pk_points > 0 && state_.pk_timer > 600.0f) {
        state_.pk_points--;
        state_.pk_timer = 0;
        if (state_.pk_points <= 0) {
            state_.is_penalized = false;
        }
    }

    // Decay bad fame slowly (1 point per second when not flagged)
    if (state_.bad_fame > 0 && !state_.is_flagged) {
        state_.bad_fame = std::max(0, state_.bad_fame - 1);
    }
}

void PKManagerDlg::ApplyPenalties() {
    // Bad fame-based penalties
    if (state_.bad_fame >= 100000000) {
        // Extreme: drop 5 items on death
    } else if (state_.bad_fame >= 10000000) {
        // Severe: drop 4 items
    } else if (state_.bad_fame >= 5000000) {
        // Heavy: drop 3 items
    } else if (state_.bad_fame >= 500000) {
        // Moderate: drop 2 items
    } else if (state_.bad_fame > 0) {
        // Light: drop 1 item
    }
}

int CPKManager_GetLootingChance(int bad_fame) {
    if (bad_fame < 100000) return 3;
    else if (bad_fame < 500000) return 4;
    else if (bad_fame < 1000000) return 5;
    else if (bad_fame < 5000000) return 6;
    else if (bad_fame < 10000000) return 7;
    else if (bad_fame < 50000000) return 8;
    else if (bad_fame < 100000000) return 9;
    else return 10;
}

int CPKManager_GetLootingItemNum(int bad_fame) {
    if (bad_fame < 100000000) return 1;
    else if (bad_fame < 400000000) return 2;
    else if (bad_fame < 700000000) return 3;
    else if (bad_fame < 1000000000) return 4;
    else return 5;
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

    // Protection timer display
    if (state_.pk_protection_remaining > 0.0f) {
        int mins = (int)(state_.pk_protection_remaining) / 60;
        int secs = (int)(state_.pk_protection_remaining) % 60;
        ui.DrawText(wx + 10, wy + 76, 0xffffaa44, "Protection: %02d:%02d", mins, secs);
    }

    // Bad fame display
    if (state_.bad_fame > 0) {
        snprintf(buf, sizeof(buf), "Bad Fame: %d", state_.bad_fame);
        ui.DrawText(wx + 10, wy + 94, 0xffff6644, buf);

        int loot_chance = CPKManager_GetLootingChance(state_.bad_fame);
        int loot_items = CPKManager_GetLootingItemNum(state_.bad_fame);
        snprintf(buf, sizeof(buf), "Loot Chance: %d/10  Items: %d", loot_chance, loot_items);
        ui.DrawText(wx + 10, wy + 112, 0xffcccccc, buf);
    }

    wy += 130;
    ui.DrawText(wx, wy, 0xffffcc88, "Penalty Info:");
    ui.DrawText(wx + 10, wy + 20, 0xffcccccc,
        "5+ PK Points: Stat reduction 25%%\n"
        "Vendor prices increased 50%%\n"
        "Cannot enter safe zones\n"
        "PK points decay 1 per 10 min idle");
}
