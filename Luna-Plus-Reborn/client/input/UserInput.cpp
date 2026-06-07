#include "UserInput.hpp"
#include "InputSystem.hpp"

InputMode UserInput::current_mode_ = InputMode::Game;
InputMode UserInput::previous_mode_ = InputMode::Game;
bool UserInput::chat_active_ = false;
std::string UserInput::chat_buffer_;
InputMode UserInput::mode_stack_[MAX_MODE_STACK] = {};
int UserInput::mode_stack_depth_ = 0;

void UserInput::Init() {
    current_mode_ = InputMode::Game;
    chat_active_ = false;
    chat_buffer_.clear();
    mode_stack_depth_ = 0;
}

void UserInput::Update(float dt) {
    (void)dt;
    auto* input = InputSystem::GetInstance();
    if (!input) return;

    // Auto-switch to game mode when ESC is pressed in UI/chat mode
    if (input->IsKeyPressed(256)) { // ESC
        if (IsChatMode()) {
            OnChatClose();
        } else if (IsUIMode()) {
            OnUIClose();
        }
    }

    input->SetMode(current_mode_);
}

void UserInput::SetMode(InputMode mode) {
    previous_mode_ = current_mode_;
    current_mode_ = mode;
}

void UserInput::OnChatOpen() {
    previous_mode_ = current_mode_;
    current_mode_ = InputMode::Chat;
    chat_active_ = true;
}

void UserInput::OnChatClose() {
    current_mode_ = previous_mode_;
    chat_active_ = false;
}

void UserInput::OnUIOpen() {
    PushMode(InputMode::UI);
}

void UserInput::OnUIClose() {
    PopMode();
}

void UserInput::OnDialogOpen() {
    PushMode(InputMode::UI);
}

void UserInput::OnDialogClose() {
    PopMode();
}

void UserInput::PushMode(InputMode mode) {
    if (mode_stack_depth_ < MAX_MODE_STACK) {
        mode_stack_[mode_stack_depth_++] = current_mode_;
    }
    current_mode_ = mode;
}

void UserInput::PopMode() {
    if (mode_stack_depth_ > 0) {
        current_mode_ = mode_stack_[--mode_stack_depth_];
    } else {
        current_mode_ = InputMode::Game;
    }
}

bool UserInput::CanProcessGameInput() {
    return current_mode_ == InputMode::Game;
}

bool UserInput::CanProcessUIInput() {
    return current_mode_ == InputMode::UI || current_mode_ == InputMode::Game;
}
