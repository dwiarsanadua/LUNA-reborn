#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <string>

class ChatRoomCreateDlg {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    std::string GetRoomName() const { return room_name_; }
private:
    Window* window_ = nullptr;
    std::string room_name_ = "New Chat";
};
