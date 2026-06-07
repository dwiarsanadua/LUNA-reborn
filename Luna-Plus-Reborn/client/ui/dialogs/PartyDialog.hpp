#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <vector>

class PartyDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
private:
    Window* window_ = nullptr;
};
