#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <cstdint>

class CharDelDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetDeleteCallback(std::function<void(const std::string&)> on_delete);

private:
    void DoDelete();

    Window* window_ = nullptr;
    Label* info_label_ = nullptr;
    InputField* password_input_ = nullptr;
    Button* confirm_btn_ = nullptr;
    std::function<void(const std::string&)> on_delete_;
};
