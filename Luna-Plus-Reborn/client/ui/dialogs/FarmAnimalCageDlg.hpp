#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>

class FarmAnimalCageDlg {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
private:
    Window* window_ = nullptr;
};
