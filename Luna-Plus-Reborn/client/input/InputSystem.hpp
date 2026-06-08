#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

struct GLFWwindow;

enum class InputMode {
    Game,
    Chat,
    UI,
    Field
};

enum class KeyState {
    Up,
    Pressed,
    Held,
    Released
};

struct KeyEvent {
    int key;
    int scancode;
    int action;
    int mods;
    KeyState state = KeyState::Pressed;
};

struct MouseEvent {
    double x, y;
    int button;
    int action;
    int mods;
    bool double_click = false;
};

// ── Gamepad / Controller support ──────────────────────────────────────────

enum class GamepadAxis {
    LeftStickX,
    LeftStickY,
    RightStickX,
    RightStickY,
    LeftTrigger,
    RightTrigger,
    COUNT
};

enum class GamepadButton {
    A, B, X, Y,
    DPadUp, DPadDown, DPadLeft, DPadRight,
    Start, Back, Guide,
    LeftStick, RightStick,
    LeftBumper, RightBumper,
    COUNT
};

struct GamepadState {
    bool connected = false;
    std::string name;
    float axes[6] = {0};           // indexed by GamepadAxis
    bool buttons[14] = {0};        // indexed by GamepadButton
    bool buttons_prev[14] = {0};
    bool buttons_pressed[14] = {0};
    bool buttons_released[14] = {0};

    // Analog stick configuration
    float left_deadzone = 0.15f;
    float right_deadzone = 0.15f;
    float trigger_threshold = 0.1f;
    float sensitivity_x = 1.0f;
    float sensitivity_y = 1.0f;
};

struct GamepadRemapEntry {
    GamepadButton original;
    int remapped_to_key;           // GLFW key to emit (-1 = none)
    std::string custom_label;
};

struct GamepadConfig {
    bool enabled = true;
    int active_gamepad = 0;
    float ui_nav_repeat_delay = 0.3f;
    float ui_nav_repeat_rate = 0.1f;
    float ui_scroll_speed = 1.0f;

    // Per-gamepad remapping
    std::unordered_map<int, std::vector<GamepadRemapEntry>> remap;

    // Sensitivity per gamepad
    std::unordered_map<int, float> sensitivity_x;
    std::unordered_map<int, float> sensitivity_y;
};

// ── Main Input System ─────────────────────────────────────────────────────

class InputSystem {
public:
    void Init(GLFWwindow* window);
    void Update(float dt);
    void Shutdown();

    void SetMode(InputMode mode);
    InputMode GetMode() const { return mode_; }
    bool IsChatMode() const { return mode_ == InputMode::Chat; }
    bool IsUIMode() const { return mode_ == InputMode::UI; }

    // Keyboard
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    bool IsKeyReleased(int key) const;
    bool IsKeyHeld(int key) const;
    bool IsAnyKeyPressed() const;

    // Mouse
    bool IsMouseDown(int button) const;
    bool IsMousePressed(int button) const;
    bool IsMouseDoubleClicked(int button) const;
    double GetMouseX() const { return mouse_x_; }
    double GetMouseY() const { return mouse_y_; }
    double GetMouseDeltaX() const { return mouse_dx_; }
    double GetMouseDeltaY() const { return mouse_dy_; }
    float GetScrollDelta() const { return scroll_; }

    // ── Gamepad API ───────────────────────────────────────────────────────

    // Poll GLFW gamepad states
    void PollGamepads();

    // Query individual gamepad
    const GamepadState& GetGamepadState(int index) const;
    int GetConnectedGamepadCount() const;
    int GetActiveGamepad() const { return gamepad_config_.active_gamepad; }
    void SetActiveGamepad(int index);

    // Gamepad button/axis queries (on active gamepad)
    bool IsGamepadButtonDown(GamepadButton btn) const;
    bool IsGamepadButtonPressed(GamepadButton btn) const;
    bool IsGamepadButtonReleased(GamepadButton btn) const;
    float GetGamepadAxis(GamepadAxis axis) const;
    float GetGamepadAxisRaw(GamepadAxis axis) const;

    // UI navigation helpers
    bool IsUINavUp() const;
    bool IsUINavDown() const;
    bool IsUINavLeft() const;
    bool IsUINavRight() const;
    bool IsUINavConfirm() const;
    bool IsUINavCancel() const;
    bool IsUINavStart() const;
    bool IsUINavSelect() const;
    float GetUINavScroll() const;
    float GetUINavHorizontal() const;

    // Gamepad configuration
    GamepadConfig& GetGamepadConfig() { return gamepad_config_; }
    void SetGamepadEnabled(bool enabled) { gamepad_config_.enabled = enabled; }
    bool IsGamepadEnabled() const { return gamepad_config_.enabled; }
    float GetGamepadSensitivityX() const;
    float GetGamepadSensitivityY() const;
    void SetGamepadSensitivity(int gamepad_idx, float sx, float sy);

    // Button remapping
    void RemapButton(int gamepad_idx, GamepadButton original, int target_key);
    void ClearRemap(int gamepad_idx, GamepadButton original);
    int GetRemappedKey(int gamepad_idx, GamepadButton original) const;

    // Events
    using KeyCallback = std::function<void(const KeyEvent&)>;
    using MouseCallback = std::function<void(const MouseEvent&)>;
    using CharCallback = std::function<void(unsigned int codepoint)>;
    void SetKeyCallback(KeyCallback cb) { key_cb_ = cb; }
    void SetMouseCallback(MouseCallback cb) { mouse_cb_ = cb; }
    void SetCharCallback(CharCallback cb) { char_cb_ = cb; }

    using GamepadConnectionCallback = std::function<void(int index, bool connected)>;
    void SetGamepadConnectionCallback(GamepadConnectionCallback cb) { gamepad_connect_cb_ = cb; }

    // GLFW callbacks
    static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GlfwCharCallback(GLFWwindow* window, unsigned int codepoint);
    static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GlfwCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static InputSystem* GetInstance() { return instance_; }

    static constexpr int MAX_GAMEPADS = 4;  // GLFW supports up to 4

private:
    void UpdateKeyState(int key, int action);
    void CheckDoubleClick(int button);

    static InputSystem* instance_;

    GLFWwindow* window_;
    InputMode mode_ = InputMode::Game;

    // Keyboard
    static constexpr int MAX_KEYS = 512;
    bool keys_down_[MAX_KEYS] = {};
    bool keys_prev_[MAX_KEYS] = {};
    float key_hold_time_[MAX_KEYS] = {};

    // Mouse
    bool mouse_down_[8] = {};
    bool mouse_prev_[8] = {};
    double mouse_x_ = 0, mouse_y_ = 0;
    double mouse_dx_ = 0, mouse_dy_ = 0;
    double last_click_time_[8] = {};
    double last_click_pos_[8][2] = {};
    bool double_click_[8] = {};
    float scroll_ = 0;

    // Timers
    float dt_ = 0;
    double time_ = 0;

    // Callbacks
    KeyCallback key_cb_;
    MouseCallback mouse_cb_;
    CharCallback char_cb_;

    // ── Gamepad state ─────────────────────────────────────────────────────
    GamepadState gamepads_[MAX_GAMEPADS];
    GamepadConfig gamepad_config_;
    GamepadConnectionCallback gamepad_connect_cb_;

    // UI navigation repeat timer
    float ui_nav_timer_ = 0;
    GamepadButton ui_last_nav_btn_ = GamepadButton::COUNT;
    bool ui_nav_repeated_ = false;
};
