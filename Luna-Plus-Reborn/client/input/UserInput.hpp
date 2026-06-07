#pragma once
#include "InputSystem.hpp"
#include <string>

class UserInput {
public:
    static void Init();
    static void Update(float dt);

    // Mode switching
    static void SetMode(InputMode mode);
    static InputMode GetMode() { return current_mode_; }
    static bool IsGameMode() { return current_mode_ == InputMode::Game; }
    static bool IsChatMode() { return current_mode_ == InputMode::Chat; }
    static bool IsUIMode() { return current_mode_ == InputMode::UI; }
    static bool IsFieldMode() { return current_mode_ == InputMode::Field; }

    // Auto-switching
    static void OnChatOpen();
    static void OnChatClose();
    static void OnUIOpen();
    static void OnUIClose();
    static void OnDialogOpen();
    static void OnDialogClose();

    // Chat text buffer
    static std::string& GetChatBuffer() { return chat_buffer_; }
    static void SetChatBuffer(const std::string& s) { chat_buffer_ = s; }
    static void AppendChar(char c) { if (chat_buffer_.size() < 64) chat_buffer_ += c; }
    static void PopChar() { if (!chat_buffer_.empty()) chat_buffer_.pop_back(); }
    static void ClearChatBuffer() { chat_buffer_.clear(); }
    static bool IsChatActive() { return chat_active_; }
    static void SetChatActive(bool a) { chat_active_ = a; }

    // Mode stack (for nested dialogs)
    static void PushMode(InputMode mode);
    static void PopMode();

    // Queries
    static bool CanProcessGameInput();
    static bool CanProcessUIInput();

private:
    static InputMode current_mode_;
    static InputMode previous_mode_;
    static bool chat_active_;
    static std::string chat_buffer_;

    // Mode stack for nested UI
    static constexpr int MAX_MODE_STACK = 16;
    static InputMode mode_stack_[MAX_MODE_STACK];
    static int mode_stack_depth_;
};
