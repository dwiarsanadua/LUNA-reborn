#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/InputField.hpp>
#include <vector>
#include <string>

struct MacroSlot {
    std::string name;
    std::string command;
    int keybind;
};

class MacroDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    Grid* macro_grid_ = nullptr;
    InputField* name_input_ = nullptr;
    InputField* cmd_input_ = nullptr;
    Label* status_label_ = nullptr;
    std::vector<MacroSlot> macros_;
    int selected_ = -1;
    void Refresh();
    void ExecuteMacro(GameState* state, int idx);
};
