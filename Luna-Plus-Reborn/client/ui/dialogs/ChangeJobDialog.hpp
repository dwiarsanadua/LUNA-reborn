#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <functional>
#include <string>

class ChangeJobDialog {
public:
    using ConfirmCallback = std::function<void(int target_class_id)>;

    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm,
              int target_class_id, const std::string& class_name,
              const std::string& description, int cost_gold,
              ConfirmCallback on_confirm = nullptr);
    void Close();
    void Refresh(GameState* state);

private:
    Window* window_ = nullptr;
    int target_class_id_ = 0;
    std::string class_name_;
    std::string description_;
    int cost_gold_ = 0;
    ConfirmCallback on_confirm_;

    static std::string ClassNameForId(int id);
    static std::string ClassDescription(int id);
};
