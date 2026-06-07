#include "ChatPanel.hpp"
#include <algorithm>
#include <cstdio>

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
    // Chat background panel at bottom-left
    int n = std::min(5, (int)messages_.size());
    float ch = 16.0f * n + 8.0f;
    ui.DrawRect(8, 480 - ch, 500, ch, {10, 15, 30, 200});
    ui.DrawBorder(8, 480 - ch, 500, ch, {60, 80, 120, 100});

    // Channel tabs
    float tx = 10;
    for (int i = 0; i < 5; i++) {
        float tw = strlen(channels_[i]) * 9.0f + 10;
        if (i == channel_) ui.DrawRect(tx, 480 - ch, tw, 14, {40, 50, 80, 200});
        ui.DrawText(tx + 3, 480 - ch + 1, channel_colors_[i], "%s", channels_[i]);
        tx += tw;
    }

    // Messages
    for (int i = 0; i < n; i++) {
        auto& msg = messages_[messages_.size() - n + i];
        ui.DrawText(12, 480 - ch + 16 + i * 16, 0xffffffff, "%s", msg.c_str());
    }

    // Chat input
    if (state->chat_open || open_) {
        ui.DrawRect(8, 494, 500, 20, {10, 15, 30, 240});
        uint32_t input_color = channel_colors_[channel_];
        ui.DrawText(12, 496, input_color, "%s_", state->chat_input.c_str());
    }
}
