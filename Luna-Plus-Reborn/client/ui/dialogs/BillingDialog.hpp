#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <string>

class BillingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
};
