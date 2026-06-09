#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <string>

struct StallOptionCallbacks {
    std::function<void(const std::string&)> set_greeting;
    std::function<void(const std::string&)> set_title;
    std::function<void()> open_stall;
    std::function<void()> close_stall;
    std::function<void(uint32_t)> set_tax_rate;
    std::function<void(uint8_t)> set_decoration;
};

class StallOptionDialog {
public:
    Window* GetWindow() { return window_; }

    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetCallbacks(const StallOptionCallbacks& cb) { callbacks_ = cb; }

private:
    Window* window_ = nullptr;
    InputField* title_input_ = nullptr;
    InputField* greeting_input_ = nullptr;
    InputField* tax_input_ = nullptr;
    Label* status_label_ = nullptr;
    StallOptionCallbacks callbacks_;

    void DoOpenStall();
    void DoCloseStall();
    void DoSetGreeting();
    void DoSetTitle();
    void DoSetTax();
    void DoSetDecoration(int idx);
    void RefreshStatus(GameState* state);
};
