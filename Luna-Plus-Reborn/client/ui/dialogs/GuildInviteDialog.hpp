#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <string>
#include <cstdint>

class GuildInviteDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetInviteCallback(std::function<void(const std::string&)> on_invite);

private:
    void DoInvite();

    Window* window_ = nullptr;
    Label* info_label_ = nullptr;
    InputField* name_input_ = nullptr;
    Button* invite_btn_ = nullptr;
    std::function<void(const std::string&)> on_invite_;
};
