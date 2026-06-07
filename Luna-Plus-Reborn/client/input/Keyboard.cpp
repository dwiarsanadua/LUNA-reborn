#include "Keyboard.hpp"
#include "InputSystem.hpp"
#include <algorithm>
#include <cstdio>

std::unordered_map<std::string, KeyBinding> Keyboard::bindings_;
std::vector<Keyboard::Combo> Keyboard::combos_;

void Keyboard::Init() {
    LoadDefaults();
}

void Keyboard::Update() {
    // Combo detection happens in UpdateCombos
}

void Keyboard::LoadDefaults() {
    bindings_.clear();
    Bind("move_forward", KEY_W);
    Bind("move_back", KEY_S);
    Bind("move_left", KEY_A);
    Bind("move_right", KEY_D);
    Bind("inventory", KEY_I);
    Bind("skills", KEY_K);
    Bind("quests", KEY_J);
    Bind("npc_dialog", KEY_N);
    Bind("chat", KEY_T);
    Bind("pk_toggle", KEY_P);
    Bind("boss_spawn", KEY_B);
    Bind("dungeon", KEY_D);
    Bind("screenshot", Keyboard::KEY_DELETE, KeyBinding::CTRL);
    Bind("fullscreen", Keyboard::KEY_ENTER, KeyBinding::ALT);
}

void Keyboard::Bind(const std::string& action, int key, int mods) {
    KeyBinding kb;
    kb.key = key;
    kb.mods = mods;
    kb.action = action;
    kb.display_name = KeyName(key);
    if (mods & KeyBinding::CTRL) kb.display_name = "Ctrl+" + kb.display_name;
    if (mods & KeyBinding::SHIFT) kb.display_name = "Shift+" + kb.display_name;
    if (mods & KeyBinding::ALT) kb.display_name = "Alt+" + kb.display_name;
    bindings_[action] = kb;
}

bool Keyboard::IsActionPressed(const std::string& action) {
    auto it = bindings_.find(action);
    if (it == bindings_.end()) return false;

    auto* input = InputSystem::GetInstance();
    if (!input) return false;

    auto& kb = it->second;
    // Check mods
    bool mod_ok = true;
    if (kb.mods & KeyBinding::CTRL) mod_ok = mod_ok && input->IsKeyDown(KEY_LCTRL);
    if (kb.mods & KeyBinding::SHIFT) mod_ok = mod_ok && input->IsKeyDown(KEY_LSHIFT);
    if (kb.mods & KeyBinding::ALT) mod_ok = mod_ok && input->IsKeyDown(KEY_LALT);

    return mod_ok && input->IsKeyPressed(kb.key);
}

const KeyBinding* Keyboard::GetBinding(const std::string& action) {
    auto it = bindings_.find(action);
    if (it != bindings_.end()) return &it->second;
    return nullptr;
}

std::vector<KeyBinding> Keyboard::GetAllBindings() {
    std::vector<KeyBinding> result;
    for (auto& [_, kb] : bindings_) result.push_back(kb);
    return result;
}

void Keyboard::AddCombo(const std::vector<int>& keys, std::function<void()> cb, float timeout) {
    Combo c;
    c.keys = keys;
    c.timeout = timeout;
    c.callback = cb;
    c.step = 0;
    c.timer = 0;
    combos_.push_back(c);
}

void Keyboard::UpdateCombos(float dt) {
    auto* input = InputSystem::GetInstance();
    if (!input) return;

    for (auto& c : combos_) {
        if (c.step >= (int)c.keys.size()) {
            // Combo complete
            if (c.callback) c.callback();
            c.step = 0;
            continue;
        }

        c.timer += dt;
        if (c.timer > c.timeout) {
            c.step = 0;
            c.timer = 0;
            continue;
        }

        if (input->IsKeyPressed(c.keys[c.step])) {
            c.step++;
            c.timer = 0;
        }
    }
}

std::string Keyboard::KeyName(int key) {
    switch (key) {
    case KEY_SPACE: return "Space";
    case KEY_ENTER: return "Enter";
    case KEY_ESC: return "Esc";
    case KEY_BACKSPACE: return "Backspace";
    case KEY_TAB: return "Tab";
    case KEY_DELETE: return "Delete";
    case KEY_UP: return "Up";
    case KEY_DOWN: return "Down";
    case KEY_LEFT: return "Left";
    case KEY_RIGHT: return "Right";
    case KEY_LCTRL: return "Ctrl";
    case KEY_LSHIFT: return "Shift";
    case KEY_LALT: return "Alt";
    default:
        if (key >= 32 && key <= 126) {
            char buf[2] = {(char)key, 0};
            return buf;
        }
        char buf[16]; snprintf(buf, sizeof(buf), "Key_%d", key);
        return buf;
    }
}

int Keyboard::NameToKey(const std::string& name) {
    if (name == "Space") return KEY_SPACE;
    if (name == "Enter") return KEY_ENTER;
    if (name == "Esc") return KEY_ESC;
    if (name == "Backspace") return KEY_BACKSPACE;
    if (name == "Tab") return KEY_TAB;
    if (name == "Delete") return KEY_DELETE;
    if (name == "Up") return KEY_UP;
    if (name == "Down") return KEY_DOWN;
    if (name == "Left") return KEY_LEFT;
    if (name == "Right") return KEY_RIGHT;
    if (name.length() == 1) return (int)name[0];
    return 0;
}
