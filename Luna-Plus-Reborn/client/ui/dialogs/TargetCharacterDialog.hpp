#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <functional>
#include <string>

struct TargetInfo {
    uint32_t entity_id = 0;
    std::string name;
    int level = 1;
    int class_id = 0;
    int hp = 500;
    int max_hp = 500;
    uint16_t map_id = 0;
    std::string guild_name;
    std::string title;
    bool is_player = true;
    bool is_pk = false;
    bool is_in_party = false;
    bool is_in_guild = false;
};

class TargetCharacterDialog {
public:
    Window* GetWindow() { return window_; }

    void Open(WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetTarget(const TargetInfo& info);
    void ClearTarget();

    bool HasTarget() const { return has_target_; }
    const TargetInfo& GetTarget() const { return target_; }

    void SetNetworkCallbacks(
        std::function<void(uint32_t)> whisper,
        std::function<void(uint32_t)> party_invite,
        std::function<void(uint32_t)> add_friend,
        std::function<void(uint32_t)> trade_request);

private:
    Window* window_ = nullptr;
    bool has_target_ = false;
    TargetInfo target_;

    Label* name_label_ = nullptr;
    Label* level_label_ = nullptr;
    Label* class_label_ = nullptr;
    Label* hp_label_ = nullptr;
    Label* guild_label_ = nullptr;
    Label* title_label_ = nullptr;
    Label* hp_bar_bg_ = nullptr;

    std::function<void(uint32_t)> on_whisper_;
    std::function<void(uint32_t)> on_party_invite_;
    std::function<void(uint32_t)> on_add_friend_;
    std::function<void(uint32_t)> on_trade_request_;

    static const char* ClassName(int class_id);
    void UpdateDisplay();
    void CreateUI(WindowManager* wm);
};
