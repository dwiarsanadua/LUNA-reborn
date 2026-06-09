#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <functional>
#include <string>
#include <cstdint>

class ProgressDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void OpenWithMessage(GameState* state, WindowManager* wm, const std::string& message);
    void Close();
    void UpdateFromState(GameState* state);
    void SetProgress(float pct);
    void SetMessage(const std::string& msg);
    void SetCancelCallback(std::function<void()> on_cancel);
    bool IsActive() const { return window_ != nullptr; }

private:
    Window* window_ = nullptr;
    Label* message_label_ = nullptr;
    Label* progress_text_ = nullptr;
    float progress_ = 0.0f;
    std::function<void()> on_cancel_;
};

class ProgressDialog;

struct ProgressBarUtil {
    static void Draw(float x, float y, float w, float h, float pct);
};
