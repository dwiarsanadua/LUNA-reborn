#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

struct KeyBinding {
    enum Mod { NONE = 0, CTRL = 1, SHIFT = 2, ALT = 4 };
    int key = 0;
    int mods = NONE;
    std::string action;
    std::string display_name;
};

class Keyboard {
public:
    static void Init();
    static void Update();

    // Key bindings
    static void Bind(const std::string& action, int key, int mods = KeyBinding::NONE);
    static bool IsActionPressed(const std::string& action);
    static const KeyBinding* GetBinding(const std::string& action);
    static std::vector<KeyBinding> GetAllBindings();

    // Combo detection
    static void AddCombo(const std::vector<int>& keys, std::function<void()> cb, float timeout = 0.5f);
    static void UpdateCombos(float dt);

    // Key name conversion
    static std::string KeyName(int key);
    static int NameToKey(const std::string& name);

    // Default bindings
    static void LoadDefaults();

    static constexpr int KEY_SPACE = 32;
    static constexpr int KEY_ENTER = 257;
    static constexpr int KEY_ESC = 256;
    static constexpr int KEY_BACKSPACE = 259;
    static constexpr int KEY_TAB = 258;
    static constexpr int KEY_DELETE = 261;
    static constexpr int KEY_UP = 265;
    static constexpr int KEY_DOWN = 264;
    static constexpr int KEY_LEFT = 263;
    static constexpr int KEY_RIGHT = 262;
    static constexpr int KEY_I = 73;
    static constexpr int KEY_K = 75;
    static constexpr int KEY_J = 74;
    static constexpr int KEY_N = 78;
    static constexpr int KEY_T = 84;
    static constexpr int KEY_P = 80;
    static constexpr int KEY_B = 66;
    static constexpr int KEY_D = 68;
    static constexpr int KEY_W = 87;
    static constexpr int KEY_A = 65;
    static constexpr int KEY_S = 83;
    static constexpr int KEY_LCTRL = 341;
    static constexpr int KEY_LSHIFT = 340;
    static constexpr int KEY_LALT = 342;

private:
    static std::unordered_map<std::string, KeyBinding> bindings_;

    struct Combo {
        std::vector<int> keys;
        int mods;
        float timeout;
        float timer;
        bool active;
        std::function<void()> callback;
        int step = 0;
    };
    static std::vector<Combo> combos_;
};
