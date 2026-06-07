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

    // Timers
    float dt_ = 0;
    double time_ = 0;

    // Callbacks
    KeyCallback key_cb_;
    MouseCallback mouse_cb_;
};
