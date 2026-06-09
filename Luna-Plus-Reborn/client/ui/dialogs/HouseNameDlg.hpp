#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <string>

class HouseNameDlg {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    std::string GetHouseName() const { return house_name_; }
    void SetHouseName(const std::string& name) { house_name_ = name; }
private:
    Window* window_ = nullptr;
    std::string house_name_ = "My House";
};
