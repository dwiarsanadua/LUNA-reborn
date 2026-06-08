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

    // Initialize gamepad states
    for (int i = 0; i < MAX_GAMEPADS; i++) {
        gamepads_[i].connected = false;
        gamepads_[i].name = "Gamepad " + std::to_string(i + 1);
    }

    // Default sensitivity
    for (int i = 0; i < MAX_GAMEPADS; i++) {
        gamepad_config_.sensitivity_x[i] = 1.0f;
        gamepad_config_.sensitivity_y[i] = 1.0f;
    }
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

    // Poll gamepads
    PollGamepads();

    // UI navigation repeat logic
    if (mode_ == InputMode::UI || mode_ == InputMode::Field) {
        GamepadButton nav_btns[] = {
            GamepadButton::DPadUp, GamepadButton::DPadDown,
            GamepadButton::DPadLeft, GamepadButton::DPadRight
        };
        bool any_nav_down = false;
        for (auto btn : nav_btns) {
            if (IsGamepadButtonDown(btn)) {
                any_nav_down = true;
                if (btn == ui_last_nav_btn_) {
                    ui_nav_timer_ += dt;
                    ui_nav_repeated_ = (ui_nav_timer_ >= gamepad_config_.ui_nav_repeat_delay);
                } else {
                    ui_last_nav_btn_ = btn;
                    ui_nav_timer_ = 0;
                    ui_nav_repeated_ = false;
                }
                break;
            }
        }
        if (!any_nav_down) {
            ui_last_nav_btn_ = GamepadButton::COUNT;
            ui_nav_timer_ = 0;
            ui_nav_repeated_ = false;
        }
    }
}

void InputSystem::Shutdown() {
    instance_ = nullptr;
}

void InputSystem::SetMode(InputMode mode) {
    mode_ = mode;
}

// ── Keyboard ───────────────────────────────────────────────────────────────

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

// ── Mouse ──────────────────────────────────────────────────────────────────

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

    if (action == 1) {
        keys_down_[key] = true;
        key_hold_time_[key] = 0;
    } else if (action == 0) {
        keys_down_[key] = false;
        key_hold_time_[key] = 0;
    }
}

void InputSystem::CheckDoubleClick(int button) {
    if (button < 0 || button >= 8) return;
    double now = time_;
    double dt_click = now - last_click_time_[button];
    double dx = std::abs(mouse_x_ - last_click_pos_[button][0]);
    double dy = std::abs(mouse_y_ - last_click_pos_[button][1]);

    if (dt_click < 0.4 && dx < 10 && dy < 10) {
        double_click_[button] = true;
        last_click_time_[button] = 0;
    } else {
        last_click_time_[button] = now;
        last_click_pos_[button][0] = mouse_x_;
        last_click_pos_[button][1] = mouse_y_;
    }
}

// ── Gamepad API ────────────────────────────────────────────────────────────

void InputSystem::PollGamepads() {
    if (!gamepad_config_.enabled) return;

    for (int i = 0; i < MAX_GAMEPADS; i++) {
        bool was_connected = gamepads_[i].connected;

        if (glfwJoystickIsGamepad(i)) {
            gamepads_[i].connected = true;
            gamepads_[i].name = glfwGetGamepadName(i) ? glfwGetGamepadName(i) : ("Gamepad " + std::to_string(i + 1));

            GLFWgamepadstate state;
            if (glfwGetGamepadState(i, &state)) {
                // Save previous button states
                for (int b = 0; b < 14; b++) {
                    gamepads_[i].buttons_prev[b] = gamepads_[i].buttons[b];
                }

                // Axes mapping: GLFW axes -> our enum
                // GLFW_GAMEPAD_AXIS_LEFT_X = 0
                // GLFW_GAMEPAD_AXIS_LEFT_Y = 1
                // GLFW_GAMEPAD_AXIS_RIGHT_X = 2
                // GLFW_GAMEPAD_AXIS_RIGHT_Y = 3
                // GLFW_GAMEPAD_AXIS_LEFT_TRIGGER = 4
                // GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER = 5

                // Apply deadzone to left stick
                float lx = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
                float ly = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
                float lmag = std::sqrt(lx * lx + ly * ly);
                float ldz = gamepads_[i].left_deadzone;
                if (lmag < ldz) { lx = 0; ly = 0; }
                else {
                    float ladj = (lmag - ldz) / (1.0f - ldz);
                    lx = (lx / lmag) * ladj;
                    ly = (ly / lmag) * ladj;
                }

                float rx = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
                float ry = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
                float rmag = std::sqrt(rx * rx + ry * ry);
                float rdz = gamepads_[i].right_deadzone;
                if (rmag < rdz) { rx = 0; ry = 0; }
                else {
                    float radj = (rmag - rdz) / (1.0f - rdz);
                    rx = (rx / rmag) * radj;
                    ry = (ry / rmag) * radj;
                }

                // Apply sensitivity
                lx *= GetGamepadSensitivityX();
                ly *= GetGamepadSensitivityY();
                rx *= GetGamepadSensitivityX();
                ry *= GetGamepadSensitivityY();

                gamepads_[i].axes[(int)GamepadAxis::LeftStickX] = lx;
                gamepads_[i].axes[(int)GamepadAxis::LeftStickY] = ly;
                gamepads_[i].axes[(int)GamepadAxis::RightStickX] = rx;
                gamepads_[i].axes[(int)GamepadAxis::RightStickY] = ry;

                // Triggers
                float lt = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
                float rt = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
                gamepads_[i].axes[(int)GamepadAxis::LeftTrigger] = lt;
                gamepads_[i].axes[(int)GamepadAxis::RightTrigger] = rt;

                // Buttons mapping: GLFW_GAMEPAD_BUTTON_* -> our enum
                static const int glfw_to_our[14] = {
                    GLFW_GAMEPAD_BUTTON_A,           // A
                    GLFW_GAMEPAD_BUTTON_B,           // B
                    GLFW_GAMEPAD_BUTTON_X,           // X
                    GLFW_GAMEPAD_BUTTON_Y,           // Y
                    GLFW_GAMEPAD_BUTTON_DPAD_UP,     // DPadUp
                    GLFW_GAMEPAD_BUTTON_DPAD_DOWN,   // DPadDown
                    GLFW_GAMEPAD_BUTTON_DPAD_LEFT,   // DPadLeft
                    GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,  // DPadRight
                    GLFW_GAMEPAD_BUTTON_START,       // Start
                    GLFW_GAMEPAD_BUTTON_BACK,        // Back
                    GLFW_GAMEPAD_BUTTON_GUIDE,       // Guide
                    GLFW_GAMEPAD_BUTTON_LEFT_THUMB,  // LeftStick
                    GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, // RightStick
                    GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, // LeftBumper
                };

                for (int b = 0; b < 14; b++) {
                    bool is_down = state.buttons[glfw_to_our[b]] == GLFW_PRESS;
                    gamepads_[i].buttons[b] = is_down;
                    gamepads_[i].buttons_pressed[b] = is_down && !gamepads_[i].buttons_prev[b];
                    gamepads_[i].buttons_released[b] = !is_down && gamepads_[i].buttons_prev[b];
                }

                // Right bumper (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER is index 7, not in our direct mapping)
                bool rb = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS;
                int rb_idx = (int)GamepadButton::RightBumper;
                gamepads_[i].buttons_prev[rb_idx] = gamepads_[i].buttons[rb_idx];
                gamepads_[i].buttons[rb_idx] = rb;
                gamepads_[i].buttons_pressed[rb_idx] = rb && !gamepads_[i].buttons_prev[rb_idx];
                gamepads_[i].buttons_released[rb_idx] = !rb && gamepads_[i].buttons_prev[rb_idx];
            }
        } else {
            gamepads_[i].connected = false;
        }

        // Connection callback
        if (gamepads_[i].connected != was_connected && gamepad_connect_cb_) {
            gamepad_connect_cb_(i, gamepads_[i].connected);
        }
    }
}

const GamepadState& InputSystem::GetGamepadState(int index) const {
    static GamepadState default_state{};
    if (index < 0 || index >= MAX_GAMEPADS) return default_state;
    return gamepads_[index];
}

int InputSystem::GetConnectedGamepadCount() const {
    int count = 0;
    for (int i = 0; i < MAX_GAMEPADS; i++) {
        if (gamepads_[i].connected) count++;
    }
    return count;
}

void InputSystem::SetActiveGamepad(int index) {
    if (index >= 0 && index < MAX_GAMEPADS) {
        gamepad_config_.active_gamepad = index;
    }
}

bool InputSystem::IsGamepadButtonDown(GamepadButton btn) const {
    if (!gamepad_config_.enabled) return false;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return false;
    int b = (int)btn;
    if (b < 0 || b >= 14) return false;
    return gamepads_[idx].buttons[b];
}

bool InputSystem::IsGamepadButtonPressed(GamepadButton btn) const {
    if (!gamepad_config_.enabled) return false;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return false;
    int b = (int)btn;
    if (b < 0 || b >= 14) return false;
    return gamepads_[idx].buttons_pressed[b];
}

bool InputSystem::IsGamepadButtonReleased(GamepadButton btn) const {
    if (!gamepad_config_.enabled) return false;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return false;
    int b = (int)btn;
    if (b < 0 || b >= 14) return false;
    return gamepads_[idx].buttons_released[b];
}

float InputSystem::GetGamepadAxis(GamepadAxis axis) const {
    if (!gamepad_config_.enabled) return 0;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return 0;
    int a = (int)axis;
    if (a < 0 || a >= 6) return 0;
    return gamepads_[idx].axes[a];
}

float InputSystem::GetGamepadAxisRaw(GamepadAxis axis) const {
    if (!gamepad_config_.enabled) return 0;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return 0;
    int a = (int)axis;
    if (a < 0 || a >= 6) return 0;

    // Return raw axis value without deadzone/sensitivity
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(idx, &state)) return 0;
    return state.axes[a];
}

// ── UI Navigation ──────────────────────────────────────────────────────────

bool InputSystem::IsUINavUp() const {
    if (!gamepad_config_.enabled) return false;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return false;

    // D-Pad up or left stick up
    bool dpad = IsGamepadButtonPressed(GamepadButton::DPadUp);
    bool stick = gamepads_[idx].axes[(int)GamepadAxis::LeftStickY] > 0.5f;
    bool repeat = (ui_last_nav_btn_ == GamepadButton::DPadUp && ui_nav_repeated_
                   && ui_nav_timer_ >= gamepad_config_.ui_nav_repeat_rate);
    return dpad || (stick && IsGamepadButtonPressed(GamepadButton::DPadUp)) || repeat;
}

bool InputSystem::IsUINavDown() const {
    if (!gamepad_config_.enabled) return false;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return false;

    bool dpad = IsGamepadButtonPressed(GamepadButton::DPadDown);
    bool stick = gamepads_[idx].axes[(int)GamepadAxis::LeftStickY] < -0.5f;
    bool repeat = (ui_last_nav_btn_ == GamepadButton::DPadDown && ui_nav_repeated_
                   && ui_nav_timer_ >= gamepad_config_.ui_nav_repeat_rate);
    return dpad || (stick && IsGamepadButtonPressed(GamepadButton::DPadDown)) || repeat;
}

bool InputSystem::IsUINavLeft() const {
    if (!gamepad_config_.enabled) return false;
    bool dpad = IsGamepadButtonPressed(GamepadButton::DPadLeft);
    bool repeat = (ui_last_nav_btn_ == GamepadButton::DPadLeft && ui_nav_repeated_
                   && ui_nav_timer_ >= gamepad_config_.ui_nav_repeat_rate);
    return dpad || repeat;
}

bool InputSystem::IsUINavRight() const {
    if (!gamepad_config_.enabled) return false;
    bool dpad = IsGamepadButtonPressed(GamepadButton::DPadRight);
    bool repeat = (ui_last_nav_btn_ == GamepadButton::DPadRight && ui_nav_repeated_
                   && ui_nav_timer_ >= gamepad_config_.ui_nav_repeat_rate);
    return dpad || repeat;
}

bool InputSystem::IsUINavConfirm() const {
    return IsGamepadButtonPressed(GamepadButton::A) || IsGamepadButtonPressed(GamepadButton::Start);
}

bool InputSystem::IsUINavCancel() const {
    return IsGamepadButtonPressed(GamepadButton::B) || IsGamepadButtonPressed(GamepadButton::Back);
}

bool InputSystem::IsUINavStart() const {
    return IsGamepadButtonPressed(GamepadButton::Start);
}

bool InputSystem::IsUINavSelect() const {
    return IsGamepadButtonPressed(GamepadButton::Back);
}

float InputSystem::GetUINavScroll() const {
    if (!gamepad_config_.enabled) return 0;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return 0;

    // Right stick Y or left stick Y for scroll
    float ry = gamepads_[idx].axes[(int)GamepadAxis::RightStickY];
    if (std::abs(ry) > 0.3f) return ry * gamepad_config_.ui_scroll_speed;
    return 0;
}

float InputSystem::GetUINavHorizontal() const {
    if (!gamepad_config_.enabled) return 0;
    int idx = gamepad_config_.active_gamepad;
    if (idx < 0 || idx >= MAX_GAMEPADS) return 0;
    return gamepads_[idx].axes[(int)GamepadAxis::LeftStickX];
}

// ── Gamepad configuration ──────────────────────────────────────────────────

float InputSystem::GetGamepadSensitivityX() const {
    int idx = gamepad_config_.active_gamepad;
    auto it = gamepad_config_.sensitivity_x.find(idx);
    return (it != gamepad_config_.sensitivity_x.end()) ? it->second : 1.0f;
}

float InputSystem::GetGamepadSensitivityY() const {
    int idx = gamepad_config_.active_gamepad;
    auto it = gamepad_config_.sensitivity_y.find(idx);
    return (it != gamepad_config_.sensitivity_y.end()) ? it->second : 1.0f;
}

void InputSystem::SetGamepadSensitivity(int gamepad_idx, float sx, float sy) {
    if (gamepad_idx < 0 || gamepad_idx >= MAX_GAMEPADS) return;
    gamepad_config_.sensitivity_x[gamepad_idx] = std::max(0.1f, std::min(10.0f, sx));
    gamepad_config_.sensitivity_y[gamepad_idx] = std::max(0.1f, std::min(10.0f, sy));
    if (gamepad_idx < MAX_GAMEPADS) {
        gamepads_[gamepad_idx].sensitivity_x = gamepad_config_.sensitivity_x[gamepad_idx];
        gamepads_[gamepad_idx].sensitivity_y = gamepad_config_.sensitivity_y[gamepad_idx];
    }
}

void InputSystem::RemapButton(int gamepad_idx, GamepadButton original, int target_key) {
    if (gamepad_idx < 0 || gamepad_idx >= MAX_GAMEPADS) return;
    auto& remap_list = gamepad_config_.remap[gamepad_idx];

    // Check if already remapped
    for (auto& entry : remap_list) {
        if (entry.original == original) {
            entry.remapped_to_key = target_key;
            return;
        }
    }

    GamepadRemapEntry entry;
    entry.original = original;
    entry.remapped_to_key = target_key;
    remap_list.push_back(entry);
}

void InputSystem::ClearRemap(int gamepad_idx, GamepadButton original) {
    auto it = gamepad_config_.remap.find(gamepad_idx);
    if (it == gamepad_config_.remap.end()) return;
    auto& list = it->second;
    list.erase(std::remove_if(list.begin(), list.end(),
        [&](const GamepadRemapEntry& e) { return e.original == original; }), list.end());
}

int InputSystem::GetRemappedKey(int gamepad_idx, GamepadButton original) const {
    auto it = gamepad_config_.remap.find(gamepad_idx);
    if (it == gamepad_config_.remap.end()) return -1;
    for (auto& entry : it->second) {
        if (entry.original == original) return entry.remapped_to_key;
    }
    return -1;
}

// ── Static GLFW callbacks ──────────────────────────────────────────────────

void InputSystem::GlfwKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
    auto* sys = GetInstance();
    if (!sys) return;
    sys->UpdateKeyState(key, action);

    // Also check gamepad remapping
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
    auto* sys = GetInstance();
    if (sys && sys->char_cb_) sys->char_cb_(codepoint);
}

void InputSystem::GlfwMouseButtonCallback(GLFWwindow*, int button, int action, int mods) {
    auto* sys = GetInstance();
    if (!sys) return;
    if (button < 0 || button >= 8) return;

    if (action == 1) {
        sys->mouse_down_[button] = true;
        sys->CheckDoubleClick(button);
    } else {
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
