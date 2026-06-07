#include "Mouse.hpp"
#include "InputSystem.hpp"
#include <rendering/UIRenderer.hpp>
#include <cmath>
#include <algorithm>

Mouse::CursorType Mouse::current_cursor_ = Mouse::CURSOR_DEFAULT;
bool Mouse::cursor_visible_ = true;
bool Mouse::use_custom_cursor_ = false;
bool Mouse::dragging_[8] = {};
float Mouse::drag_start_x_[8] = {}, Mouse::drag_start_y_[8] = {};
float Mouse::drag_current_x_[8] = {}, Mouse::drag_current_y_[8] = {};
float Mouse::drag_threshold_ = 5.0f;
std::string Mouse::cursor_texture_path_;
int Mouse::cursor_hot_x_ = 0, Mouse::cursor_hot_y_ = 0;
uint32_t Mouse::cursor_textures_[Mouse::CURSOR_COUNT] = {};
bool Mouse::cursor_textures_loaded_ = false;

void Mouse::Init() {
    for (int i = 0; i < 8; i++) dragging_[i] = false;
    ShowCursor(true);
}

void Mouse::Update() {
    auto* input = InputSystem::GetInstance();
    if (!input) return;

    for (int i = 0; i < 8; i++) {
        if (input->IsMouseDown(i)) {
            if (!dragging_[i]) {
                // Start drag
                dragging_[i] = true;
                drag_start_x_[i] = (float)input->GetMouseX();
                drag_start_y_[i] = (float)input->GetMouseY();
            }
            drag_current_x_[i] = (float)input->GetMouseX();
            drag_current_y_[i] = (float)input->GetMouseY();
        } else {
            dragging_[i] = false;
        }
    }
}

void Mouse::SetCursor(CursorType type) {
    current_cursor_ = type;
}

void Mouse::ShowCursor(bool show) {
    cursor_visible_ = show;
}

bool Mouse::IsDoubleClick(int button) {
    auto* input = InputSystem::GetInstance();
    return input && input->IsMouseDoubleClicked(button);
}

bool Mouse::IsClickHeld(int button, float duration) {
    auto* input = InputSystem::GetInstance();
    return input && input->IsMouseDown(button);
}

bool Mouse::IsDragging(int button) {
    if (button < 0 || button >= 8) return false;
    if (!dragging_[button]) return false;
    float dx = drag_current_x_[button] - drag_start_x_[button];
    float dy = drag_current_y_[button] - drag_start_y_[button];
    return (dx * dx + dy * dy) >= (drag_threshold_ * drag_threshold_);
}

float Mouse::GetDragDistance(int button) {
    if (button < 0 || button >= 8) return 0;
    float dx = drag_current_x_[button] - drag_start_x_[button];
    float dy = drag_current_y_[button] - drag_start_y_[button];
    return std::sqrt(dx * dx + dy * dy);
}

void Mouse::SetCursorTexture(const std::string& texture_path, int hot_x, int hot_y) {
    cursor_texture_path_ = texture_path;
    cursor_hot_x_ = hot_x;
    cursor_hot_y_ = hot_y;
}

void Mouse::RenderCursor(UIRenderer& ui) {
    if (!use_custom_cursor_ || !cursor_visible_) return;
    auto* input = InputSystem::GetInstance();
    if (!input) return;
    float x = (float)input->GetMouseX();
    float y = (float)input->GetMouseY();
    // Draw cursor texture at mouse position
    ui.DrawRect(x, y, 24, 24, {255, 255, 255, 200});
}
