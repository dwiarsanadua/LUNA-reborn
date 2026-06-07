#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <string>
#include <vector>

class ChatRoomDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* channels_ = nullptr;
    ListBox* msg_display_ = nullptr;
    InputField* chat_input_ = nullptr;
    std::vector<std::string> channel_names_;
    int active_channel_ = 0;
    void SendMessage(GameState* state);
};
