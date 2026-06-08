#pragma once
#include <cstdint>
#include <functional>

struct GLFWwindow;

enum class InputMode {
    Game,     // Normal gameplay: WASD, hotkeys, etc.
    Chat,     // Chat input: text entry
    UI,       // UI navigation: tab, arrow keys, enter
    Field     // Text field: IME text entry
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

struct GamepadState {
    bool connected = false;
    float left_x = 0, left_y = 0;
    float right_x = 0, right_y = 0;
    float lt = 0, rt = 0;
    bool a = false, b = false, x = false, y = false;
    bool lb = false, rb = false;
    bool back = false, start = false;
    bool dpad_up = false, dpad_down = false, dpad_left = false, dpad_right = false;
    bool guide = false;
    int joystick_id = -1;
};

class InputSystem {
public:
    void Init(GLFWwindow* window);
    void Update(float dt);
    void Shutdown();

    // Mode
    void SetMode(InputMode mode);
    InputMode GetMode() const { return mode_; }
    bool IsChatMode() const { return mode_ == InputMode::Chat; }
    bool IsUIMode() const { return mode_ == InputMode::UI; }

    // Keyboard state
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    bool IsKeyReleased(int key) const;
    bool IsKeyHeld(int key) const;
    bool IsAnyKeyPressed() const;

    // Mouse state
    bool IsMouseDown(int button) const;
    bool IsMousePressed(int button) const;
    bool IsMouseDoubleClicked(int button) const;
    double GetMouseX() const { return mouse_x_; }
    double GetMouseY() const { return mouse_y_; }
    double GetMouseDeltaX() const { return mouse_dx_; }
    double GetMouseDeltaY() const { return mouse_dy_; }
    float GetScrollDelta() const { return scroll_; }

    // Gamepad state
    const GamepadState& GetGamepadState() const { return gamepad_; }
    bool IsGamepadButtonDown(int btn) const;
    bool IsGamepadButtonPressed(int btn) const;
    float GetGamepadAxis(int axis) const;
    void SetGamepadEnabled(bool e) { gamepad_enabled_ = e; }
    bool IsGamepadEnabled() const { return gamepad_enabled_; }

    // Camera orbit from right stick (per frame)
    float GetCameraYawDelta() const { return camera_yaw_delta_; }
    float GetCameraPitchDelta() const { return camera_pitch_delta_; }

    // Events
    using KeyCallback = std::function<void(const KeyEvent&)>;
    using MouseCallback = std::function<void(const MouseEvent&)>;
    void SetKeyCallback(KeyCallback cb) { key_cb_ = cb; }
    void SetMouseCallback(MouseCallback cb) { mouse_cb_ = cb; }

    // GLFW callbacks (to be registered in main.cpp)
    static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GlfwCharCallback(GLFWwindow* window, unsigned int codepoint);
    static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GlfwCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static InputSystem* GetInstance() { return instance_; }

private:
    void UpdateKeyState(int key, int action);
    void CheckDoubleClick(int button);
    void PollGamepad();
    void MapGamepadToKeys();

    static InputSystem* instance_;

    GLFWwindow* window_ = nullptr;
    InputMode mode_ = InputMode::Game;

    // Keyboard state
    static constexpr int MAX_KEYS = 512;
    bool keys_down_[MAX_KEYS] = {};
    bool keys_prev_[MAX_KEYS] = {};
    float key_hold_time_[MAX_KEYS] = {};

    // Mouse state
    bool mouse_down_[8] = {};
    bool mouse_prev_[8] = {};
    double mouse_x_ = 0, mouse_y_ = 0;
    double mouse_dx_ = 0, mouse_dy_ = 0;
    double last_click_time_[8] = {};
    double last_click_pos_[8][2] = {};
    bool double_click_[8] = {};
    float scroll_ = 0;

    // Gamepad state
    GamepadState gamepad_;
    GamepadState gamepad_prev_;
    bool gamepad_enabled_ = true;
    bool gamepad_mapped_ = false;
    static constexpr float DEAD_ZONE = 0.2f;

    // Camera orbit from right stick
    float camera_yaw_delta_ = 0;
    float camera_pitch_delta_ = 0;

    // Timers
    float dt_ = 0;
    double time_ = 0;

    // Callbacks
    KeyCallback key_cb_;
    MouseCallback mouse_cb_;
};
