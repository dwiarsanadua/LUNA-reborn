#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <string>

class ItemPopupDlg {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Open(WindowManager* wm, const std::string& item_name, const std::string& desc);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    std::string GetItemName() const { return item_name_; }
    std::string GetDescription() const { return description_; }
private:
    Window* window_ = nullptr;
    std::string item_name_ = "Item";
    std::string description_ = "An item";
};
