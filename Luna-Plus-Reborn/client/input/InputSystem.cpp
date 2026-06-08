#include "InputSystem.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstring>

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

    // Initialize GLFW gamepad mappings from embedded string
    const char* mappings =
        "78696e70757401000000000000000000,Microsoft X-Box 360 pad,"
        "a:b0,b:b1,x:b2,y:b3,leftshoulder:b4,rightshoulder:b5,"
        "back:b6,start:b7,guide:b8,leftthumb:b9,rightthumb:b10,"
        "dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,"
        "leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,"
        "platform:Windows,";
    glfwUpdateGamepadMappings(mappings);
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
    camera_yaw_delta_ = 0;
    camera_pitch_delta_ = 0;

    // Poll gamepad
    if (gamepad_enabled_) {
        gamepad_prev_ = gamepad_;
        PollGamepad();
        MapGamepadToKeys();
    }
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

// --- Gamepad ---

void InputSystem::PollGamepad() {
    int jid = -1;
    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickIsGamepad(i)) {
            jid = i;
            break;
        }
    }

    if (jid < 0) {
        gamepad_.connected = false;
        gamepad_.joystick_id = -1;
        return;
    }

    gamepad_.connected = true;
    gamepad_.joystick_id = jid;

    GLFWgamepadstate state;
    if (!glfwGetGamepadState(jid, &state)) {
        gamepad_.connected = false;
        return;
    }

    // Buttons
    gamepad_.a      = state.buttons[GLFW_GAMEPAD_BUTTON_A] != 0;
    gamepad_.b      = state.buttons[GLFW_GAMEPAD_BUTTON_B] != 0;
    gamepad_.x      = state.buttons[GLFW_GAMEPAD_BUTTON_X] != 0;
    gamepad_.y      = state.buttons[GLFW_GAMEPAD_BUTTON_Y] != 0;
    gamepad_.lb     = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] != 0;
    gamepad_.rb     = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] != 0;
    gamepad_.back   = state.buttons[GLFW_GAMEPAD_BUTTON_BACK] != 0;
    gamepad_.start  = state.buttons[GLFW_GAMEPAD_BUTTON_START] != 0;
    gamepad_.guide  = state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] != 0;
    gamepad_.dpad_up    = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] != 0;
    gamepad_.dpad_down  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] != 0;
    gamepad_.dpad_left  = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] != 0;
    gamepad_.dpad_right = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] != 0;

    // Axes with dead zone
    auto apply_dead = [](float v, float dz) -> float {
        if (std::abs(v) < dz) return 0.0f;
        return (v > 0 ? 1.0f : -1.0f) * ((std::abs(v) - dz) / (1.0f - dz));
    };

    gamepad_.left_x  = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], DEAD_ZONE);
    gamepad_.left_y  = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], DEAD_ZONE);
    gamepad_.right_x = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], DEAD_ZONE);
    gamepad_.right_y = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y], DEAD_ZONE);
    gamepad_.lt = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER], DEAD_ZONE);
    gamepad_.rt = apply_dead(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER], DEAD_ZONE);

    // Right stick drives camera orbit delta
    camera_yaw_delta_   = gamepad_.right_x * 120.0f * dt_;
    camera_pitch_delta_ = gamepad_.right_y * 120.0f * dt_;
}

void InputSystem::MapGamepadToKeys() {
    // Left analog → WASD movement
    bool move_left  = gamepad_.left_x < -DEAD_ZONE;
    bool move_right = gamepad_.left_x >  DEAD_ZONE;
    bool move_up    = gamepad_.left_y < -DEAD_ZONE;
    bool move_down  = gamepad_.left_y >  DEAD_ZONE;

    // Inject key state directly
    keys_down_[GLFW_KEY_A] = move_left;
    keys_down_[GLFW_KEY_D] = move_right;
    keys_down_[GLFW_KEY_W] = move_up;
    keys_down_[GLFW_KEY_S] = move_down;

    // A → Space (attack)
    if (gamepad_.a && !gamepad_prev_.a)
        keys_down_[GLFW_KEY_SPACE] = true;
    if (!gamepad_.a && gamepad_prev_.a)
        keys_down_[GLFW_KEY_SPACE] = false;

    // B → Esc (menu)
    if (gamepad_.b && !gamepad_prev_.b)
        keys_down_[GLFW_KEY_ESCAPE] = true;
    if (!gamepad_.b && gamepad_prev_.b)
        keys_down_[GLFW_KEY_ESCAPE] = false;

    // X → I (inventory)
    if (gamepad_.x && !gamepad_prev_.x)
        keys_down_[GLFW_KEY_I] = true;
    if (!gamepad_.x && gamepad_prev_.x)
        keys_down_[GLFW_KEY_I] = false;

    // Y → K (skills)
    if (gamepad_.y && !gamepad_prev_.y)
        keys_down_[GLFW_KEY_K] = true;
    if (!gamepad_.y && gamepad_prev_.y)
        keys_down_[GLFW_KEY_K] = false;

    // D-pad → UI navigation (arrow keys)
    if (gamepad_.dpad_up && !gamepad_prev_.dpad_up)
        keys_down_[GLFW_KEY_UP] = true;
    if (!gamepad_.dpad_up && gamepad_prev_.dpad_up)
        keys_down_[GLFW_KEY_UP] = false;

    if (gamepad_.dpad_down && !gamepad_prev_.dpad_down)
        keys_down_[GLFW_KEY_DOWN] = true;
    if (!gamepad_.dpad_down && gamepad_prev_.dpad_down)
        keys_down_[GLFW_KEY_DOWN] = false;

    if (gamepad_.dpad_left && !gamepad_prev_.dpad_left)
        keys_down_[GLFW_KEY_LEFT] = true;
    if (!gamepad_.dpad_left && gamepad_prev_.dpad_left)
        keys_down_[GLFW_KEY_LEFT] = false;

    if (gamepad_.dpad_right && !gamepad_prev_.dpad_right)
        keys_down_[GLFW_KEY_RIGHT] = true;
    if (!gamepad_.dpad_right && gamepad_prev_.dpad_right)
        keys_down_[GLFW_KEY_RIGHT] = false;

    gamepad_mapped_ = true;
}

bool InputSystem::IsGamepadButtonDown(int btn) const {
    if (!gamepad_.connected) return false;
    switch (btn) {
        case GLFW_GAMEPAD_BUTTON_A: return gamepad_.a;
        case GLFW_GAMEPAD_BUTTON_B: return gamepad_.b;
        case GLFW_GAMEPAD_BUTTON_X: return gamepad_.x;
        case GLFW_GAMEPAD_BUTTON_Y: return gamepad_.y;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return gamepad_.lb;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return gamepad_.rb;
        case GLFW_GAMEPAD_BUTTON_BACK: return gamepad_.back;
        case GLFW_GAMEPAD_BUTTON_START: return gamepad_.start;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP: return gamepad_.dpad_up;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return gamepad_.dpad_down;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return gamepad_.dpad_left;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return gamepad_.dpad_right;
        default: return false;
    }
}

bool InputSystem::IsGamepadButtonPressed(int btn) const {
    if (!gamepad_.connected) return false;
    bool cur = IsGamepadButtonDown(btn);
    bool prev = false;
    switch (btn) {
        case GLFW_GAMEPAD_BUTTON_A: prev = gamepad_prev_.a; break;
        case GLFW_GAMEPAD_BUTTON_B: prev = gamepad_prev_.b; break;
        case GLFW_GAMEPAD_BUTTON_X: prev = gamepad_prev_.x; break;
        case GLFW_GAMEPAD_BUTTON_Y: prev = gamepad_prev_.y; break;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: prev = gamepad_prev_.lb; break;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: prev = gamepad_prev_.rb; break;
        case GLFW_GAMEPAD_BUTTON_BACK: prev = gamepad_prev_.back; break;
        case GLFW_GAMEPAD_BUTTON_START: prev = gamepad_prev_.start; break;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP: prev = gamepad_prev_.dpad_up; break;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: prev = gamepad_prev_.dpad_down; break;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: prev = gamepad_prev_.dpad_left; break;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: prev = gamepad_prev_.dpad_right; break;
    }
    return cur && !prev;
}

float InputSystem::GetGamepadAxis(int axis) const {
    if (!gamepad_.connected) return 0.0f;
    switch (axis) {
        case GLFW_GAMEPAD_AXIS_LEFT_X: return gamepad_.left_x;
        case GLFW_GAMEPAD_AXIS_LEFT_Y: return gamepad_.left_y;
        case GLFW_GAMEPAD_AXIS_RIGHT_X: return gamepad_.right_x;
        case GLFW_GAMEPAD_AXIS_RIGHT_Y: return gamepad_.right_y;
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return gamepad_.lt;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return gamepad_.rt;
        default: return 0.0f;
    }
}
