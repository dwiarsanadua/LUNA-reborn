#pragma once
#include <ui/Window.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/InputField.hpp>
#include <vector>

class ChatPanel {
public:
    ChatPanel();
    Window* GetWindow() { return window_; }
    void Update(GameState* state, float dt);
    void Render(UIRenderer& ui, GameState* state);
    bool IsOpen() const { return open_; }
    void SetOpen(bool o) { open_ = o; }
    void Toggle() { open_ = !open_; }
    void AddMessage(const std::string& msg) { messages_.push_back(msg); if (messages_.size() > 50) messages_.erase(messages_.begin()); }
    const std::string& GetInput() const { return input_; }
    void SetInput(const std::string& s) { input_ = s; }
    void AppendChar(char c) { if (input_.length() < 64) input_ += c; }
    void PopChar() { if (!input_.empty()) input_.pop_back(); }
    void ClearInput() { input_.clear(); }

private:
    Window* window_ = nullptr;
    bool open_ = false;
    std::string input_;
    std::vector<std::string> messages_;
    int channel_ = 0; // 0=All, 1=Whisper, 2=Party, 3=Guild, 4=System
    const char* channels_[5] = {"All", "Whisper", "Party", "Guild", "System"};
    uint32_t channel_colors_[5] = {0xffffffff, 0xffff88ff, 0x8888ffff, 0x44ff44ff, 0xffff8844};
};
