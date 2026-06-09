#include "LoadingScreen.hpp"
#include <ui/GameState.hpp>
#include <network/NetworkClient.hpp>
#include <fstream>
#include <spdlog/spdlog.h>

void LoadingScreen::Init(GameState* state, NetworkClient* network) {
    Screen::Init(state, network);
    if (!progress_bar_) {
        progress_bar_ = new ProgressBar(540, 440, 200, 16);
        progress_bar_->SetColors({60, 200, 60, 255}, {20, 20, 40, 220});
    }
    LoadTips();
}

void LoadingScreen::Enter() {
    timer_ = 0;
    tip_timer_ = 0.0f;
    current_tip_ = 0;
    ShowRandomTip();
    if (progress_bar_) progress_bar_->SetProgress(0.0f);
}

void LoadingScreen::Update(float dt) {
    timer_ += dt;
    tip_timer_ += dt;
    if (tip_timer_ > 3.0f) {
        tip_timer_ = 0.0f;
        current_tip_ = (current_tip_ + 1) % tips_.size();
        ShowRandomTip();
    }
    if (progress_bar_) {
        float pct = fmod(timer_, 3.0f) / 3.0f;
        progress_bar_->SetProgress(pct);
    }
    if (timer_ > 0.5f && state_->login_ok && state_->characters.empty()) {
        // Still waiting for character list
    }
}

bool LoadingScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    (void)payload;
    if (type == 0x0202 || type == 0x0208) return true;
    return false;
}

void LoadingScreen::LoadTips() {
    tips_.clear();
    std::ifstream f("assets/interface/LoadingMsg.bin.txt");
    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) tips_.push_back(line);
        }
        spdlog::info("LoadingScreen: loaded {} tips from file", tips_.size());
    }
    // Fallback defaults
    if (tips_.empty()) {
        tips_ = {
            "Tip: Press F1 for controls and help!",
            "Tip: Enchant items up to +15 at the Blacksmith (N key)!",
            "Tip: Repair your weapons before they break!",
            "Tip: Plant seeds with [H] then 1-9 on your farm!",
            "Tip: Press O to adjust video and audio settings.",
            "Tip: Party up with friends using [P] for more XP!",
            "Tip: Store items in your bank with [B].",
            "Tip: WASD to move, mouse drag to rotate camera.",
            "Tip: Press I for inventory, K for skills, J for quests.",
        };
    }
}

void LoadingScreen::ShowRandomTip() {
    if (tips_.empty()) return;
    current_tip_ = rand() % tips_.size();
}

void LoadingScreen::Render(UIRenderer& ui) {
    // Background
    ui.DrawRect(0, 0, 1280, 720, {5, 5, 15, 255});

    // Title
    ui.DrawTextCentered(280, 0xffffcc88, "LUNA Plus Reborn");
    ui.DrawTextCentered(305, 0xff888888, "Loading...");

    // Progress bar widget
    if (progress_bar_) {
        progress_bar_->Render(ui);
    }

    // Loading tip (rotating every 3s)
    if (!tips_.empty()) {
        int idx = current_tip_ % tips_.size();
        ui.DrawTextCentered(470, 0xff999999, "%s", tips_[idx].c_str());
    }

    // Animated dots
    int dots = (int)(timer_ * 3) % 4;
    char dots_buf[16] = "...";
    for (int i = dots; i < 3; i++) dots_buf[i] = ' ';
    dots_buf[3] = 0;
    ui.DrawTextCentered(350, 0xffaaaaaa, "Connecting%s", dots_buf);
}
