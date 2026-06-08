#pragma once
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <functional>
#include <vector>

class LegacyHudOverlay {
public:
    using HotbarHandler = std::function<void(int slot)>;

    void Init(float screen_w, float screen_h);
    void Shutdown();
    void SetHotbarHandler(HotbarHandler handler) { hotbar_handler_ = std::move(handler); }
    void Update(GameState* state, float screen_w, float screen_h);
    void Render(UIRenderer& ui);
    bool IsActive() const { return active_; }
    void ToggleChat();
    bool IsChatVisible() const { return chat_visible_; }
    void SyncChatInput(GameState* state);

private:
    void WireQuickSlot();
    void SyncChatList(GameState* state);

    WindowManager hud_wm_;
    Window* char_gage_ = nullptr;
    Window* quick_slot_ = nullptr;
    Window* chat_window_ = nullptr;
    HotbarHandler hotbar_handler_;
    bool active_ = false;
    bool chat_visible_ = true;
    bool quickslot_wired_ = false;
    size_t last_chat_count_ = 0;
};
