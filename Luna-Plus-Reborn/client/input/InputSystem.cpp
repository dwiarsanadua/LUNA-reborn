#include "InputSystem.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

InputSystem* InputSystem::instance_ = nullptr;

void InputSystem::Init(GLFWwindow* window) {
    window_ = window;
    instance_ = this;

    for (int i = 0; i < MAX_KEYS; i++) {
        keys_down_[i] = false;
        keys_prev_[i] = false;
        key_hold_time_[i] = 0;
    }
    for (int i = 0; i < 8; i++) {
        mouse_down_[i] = false;
        mouse_prev_[i] = false;
        last_click_time_[i] = 0;
        double_click_[i] = false;
    }
    mouse_x_ = mouse_y_ = 0;
    mouse_dx_ = mouse_dy_ = 0;
    scroll_ = 0;
    time_ = glfwGetTime();
}

void InputSystem::Update(float dt) {
    dt_ = dt;
    time_ = glfwGetTime();

    // Copy current state to previous
    for (int i = 0; i < MAX_KEYS; i++) {
        keys_prev_[i] = keys_down_[i];
        if (keys_down_[i]) key_hold_time_[i] += dt;
    }
    for (int i = 0; i < 8; i++) {
        mouse_prev_[i] = mouse_down_[i];
    }

    // Reset per-frame values
    mouse_dx_ = 0;
    mouse_dy_ = 0;
    scroll_ = 0;
    for (int i = 0; i < 8; i++) double_click_[i] = false;
}

void InputSystem::Shutdown() {
    instance_ = nullptr;
}

void InputSystem::SetMode(InputMode mode) {
    mode_ = mode;
}

bool InputSystem::IsKeyDown(int key) const {
    if (key < 0 || key >= MAX_KEYS) return false;
    return keys_down_[key];
}

bool InputSystem::IsKeyPressed(int key) const {
    if (key < 0 || key >= MAX_KEYS) return false;
    return keys_down_[key] && !keys_prev_[key];
}

bool InputSystem::IsKeyReleased(int key) const {
    if (key < 0 || key >= MAX_KEYS) return false;
    return !keys_down_[key] && keys_prev_[key];
}

bool InputSystem::IsKeyHeld(int key) const {
    if (key < 0 || key >= MAX_KEYS) return false;
    return keys_down_[key] && key_hold_time_[key] > 0.3f;
}

bool InputSystem::IsAnyKeyPressed() const {
    for (int i = 0; i < MAX_KEYS; i++) {
        if (keys_down_[i] && !keys_prev_[i]) return true;
    }
    return false;
}

bool InputSystem::IsMouseDown(int button) const {
    if (button < 0 || button >= 8) return false;
    return mouse_down_[button];
}

bool InputSystem::IsMousePressed(int button) const {
    if (button < 0 || button >= 8) return false;
    return mouse_down_[button] && !mouse_prev_[button];
}

bool InputSystem::IsMouseDoubleClicked(int button) const {
    if (button < 0 || button >= 8) return false;
    return double_click_[button];
}

void InputSystem::UpdateKeyState(int key, int action) {
    if (key < 0 || key >= MAX_KEYS) return;

    if (action == 1) { // PRESS
        keys_down_[key] = true;
        key_hold_time_[key] = 0;
    } else if (action == 0) { // RELEASE
        keys_down_[key] = false;
        key_hold_time_[key] = 0;
    }
    // action == 2 (REPEAT) — key stays down
}

void InputSystem::CheckDoubleClick(int button) {
    if (button < 0 || button >= 8) return;
    double now = time_;
    double dt_click = now - last_click_time_[button];
    double dx = std::abs(mouse_x_ - last_click_pos_[button][0]);
    double dy = std::abs(mouse_y_ - last_click_pos_[button][1]);

    if (dt_click < 0.4 && dx < 10 && dy < 10) {
        double_click_[button] = true;
        last_click_time_[button] = 0; // Reset to prevent triple-click
    } else {
        last_click_time_[button] = now;
        last_click_pos_[button][0] = mouse_x_;
        last_click_pos_[button][1] = mouse_y_;
    }
}

// --- Static GLFW callbacks ---

void InputSystem::GlfwKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
    auto* sys = GetInstance();
    if (!sys) return;
    sys->UpdateKeyState(key, action);

    KeyEvent e;
    e.key = key;
    e.scancode = scancode;
    e.action = action;
    e.mods = mods;
    e.state = (action == 1) ? KeyState::Pressed :
              (action == 0) ? KeyState::Released : KeyState::Held;
    if (sys->key_cb_) sys->key_cb_(e);
}

void InputSystem::GlfwCharCallback(GLFWwindow*, unsigned int codepoint) {
    // Char input is handled directly by chat/input field
    (void)codepoint;
}

void InputSystem::GlfwMouseButtonCallback(GLFWwindow*, int button, int action, int mods) {
    auto* sys = GetInstance();
    if (!sys) return;
    if (button < 0 || button >= 8) return;

    if (action == 1) { // PRESS
        sys->mouse_down_[button] = true;
        sys->CheckDoubleClick(button);
    } else { // RELEASE
        sys->mouse_down_[button] = false;
    }

    MouseEvent e;
    e.x = sys->mouse_x_;
    e.y = sys->mouse_y_;
    e.button = button;
    e.action = action;
    e.mods = mods;
    e.double_click = sys->double_click_[button];
    if (sys->mouse_cb_) sys->mouse_cb_(e);
}

void InputSystem::GlfwCursorPosCallback(GLFWwindow*, double x, double y) {
    auto* sys = GetInstance();
    if (!sys) return;
    sys->mouse_dx_ = x - sys->mouse_x_;
    sys->mouse_dy_ = y - sys->mouse_y_;
    sys->mouse_x_ = x;
    sys->mouse_y_ = y;
}

void InputSystem::GlfwScrollCallback(GLFWwindow*, double, double yoffset) {
    auto* sys = GetInstance();
    if (!sys) return;
    sys->scroll_ = (float)yoffset;
}
