#include "LoadingScreen.hpp"
#include <ui/GameState.hpp>
#include <network/NetworkClient.hpp>

static const char* LOADING_TIPS[] = {
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
static const int NUM_TIPS = sizeof(LOADING_TIPS) / sizeof(LOADING_TIPS[0]);

void LoadingScreen::Init(GameState* state, NetworkClient* network) {
    Screen::Init(state, network);
}

void LoadingScreen::Enter() {
    timer_ = 0;
    tip_index_ = rand() % NUM_TIPS;
}

void LoadingScreen::Update(float dt) {
    timer_ += dt;
    if (timer_ > 0.5f && state_->login_ok && state_->characters.empty()) {
        // Still waiting for character list
    }
}

bool LoadingScreen::HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) {
    (void)payload;
    if (type == 0x0202 || type == 0x0208) return true;
    return false;
}

void LoadingScreen::Render(UIRenderer& ui) {
    // Background
    ui.DrawRect(0, 0, 1280, 720, {5, 5, 15, 255});

    // Title
    ui.DrawTextCentered(280, 0xffffcc88, "LUNA Plus Reborn");
    ui.DrawTextCentered(305, 0xff888888, "Loading...");

    // Loading bar
    float pct = fmod(timer_, 3.0f) / 3.0f;
    ui.DrawRect(440, 380, 400, 24, {20, 20, 40, 220});
    ui.DrawRect(440, 380, 400 * pct, 24, {80, 160, 220, 220});
    ui.DrawBorder(440, 380, 400, 24, {100, 180, 255, 180});

    // Loading tip
    int idx = (int)(timer_ / 5.0f) % NUM_TIPS;
    ui.DrawTextCentered(430, 0xff999999, "%s", LOADING_TIPS[idx]);

    // Animated dots
    int dots = (int)(timer_ * 3) % 4;
    char dots_buf[16] = "...";
    for (int i = dots; i < 3; i++) dots_buf[i] = ' ';
    dots_buf[3] = 0;
    ui.DrawTextCentered(350, 0xffaaaaaa, "Connecting%s", dots_buf);
}
