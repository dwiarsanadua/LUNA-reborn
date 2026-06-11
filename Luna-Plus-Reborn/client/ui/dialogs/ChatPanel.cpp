#include "ChatPanel.hpp"
#include <rendering/UIRenderer.hpp>
#include <algorithm>
#include <cstdio>
#include <cstring>

ChatPanel::ChatPanel() {}

void ChatPanel::Update(GameState* state, float dt) {
    (void)state;
    (void)dt;
    // Copy messages from GameState
    if (!state->chat_messages.empty()) {
        for (auto& m : state->chat_messages) {
            bool found = false;
            for (auto& em : messages_) { if (em == m) { found = true; break; } }
            if (!found) messages_.push_back(m);
        }
        if (messages_.size() > 50) messages_.erase(messages_.begin(), messages_.begin() + (messages_.size() - 50));
    }
}

void ChatPanel::Render(UIRenderer& ui, GameState* state) {
    // Chat panel anchored to bottom-left, above the hotbar (Old Luna layout)
    float panel_bottom = ui.logicalHeight - 64.0f;
    int n = std::min(6, (int)messages_.size());
    float ch = 16.0f * n + 24.0f;
    float py = panel_bottom - ch;
    ui.DrawRect(8, py, 500, ch, {10, 15, 30, 170});
    ui.DrawBorder(8, py, 500, ch, {60, 80, 120, 100});

    // Channel tabs
    float tx = 10;
    for (int i = 0; i < 5; i++) {
        float tw = strlen(channels_[i]) * 9.0f + 10;
        if (i == channel_) ui.DrawRect(tx, py, tw, 14, {40, 50, 80, 200});
        ui.DrawText(tx + 3, py + 1, channel_colors_[i], "%s", channels_[i]);
        tx += tw;
    }

    // Messages, colored by channel prefix like Old Luna
    for (int i = 0; i < n; i++) {
        auto& msg = messages_[messages_.size() - n + i];
        uint32_t col = 0xffffffff;
        if (msg.rfind("[Party]", 0) == 0)        col = 0xff88ff88;
        else if (msg.rfind("[Guild]", 0) == 0)   col = 0xff88ffff;
        else if (msg.rfind("[Whisper]", 0) == 0) col = 0xffff88ff;
        else if (msg.find("Level Up") != std::string::npos ||
                 msg.find("defeated") != std::string::npos) col = 0xff66ddff;
        else if (msg.find("died") != std::string::npos ||
                 msg.find("broken") != std::string::npos)   col = 0xff6666ff;
        ui.DrawText(12, py + 18 + i * 16, col, "%s", msg.c_str());
    }

    // Chat input line, directly under the message panel
    if (state->chat_open || open_) {
        ui.DrawRect(8, panel_bottom + 2, 500, 20, {10, 15, 30, 240});
        ui.DrawBorder(8, panel_bottom + 2, 500, 20, {100, 150, 220, 180});
        uint32_t input_color = channel_colors_[channel_];
        ui.DrawText(12, panel_bottom + 4, input_color, "%s_", state->chat_input.c_str());
    }
}
