#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <string>

class AutoAnswerDlg {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    void SetAutoReply(bool enabled) { auto_reply_ = enabled; }
    bool IsAutoReply() const { return auto_reply_; }
    void SetReplyText(const std::string& text) { reply_text_ = text; }
    std::string GetReplyText() const { return reply_text_; }
private:
    Window* window_ = nullptr;
    bool auto_reply_ = false;
    std::string reply_text_ = "Away from keyboard";
};
