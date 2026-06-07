#include "KeyBindings.hpp"
#include "ConfigManager.hpp"
#include <input/InputSystem.hpp>
#include <algorithm>
#include <spdlog/spdlog.h>

std::vector<KeyBindings::Binding> KeyBindings::bindings_;
std::unordered_map<std::string, int> KeyBindings::binding_index_;

void KeyBindings::Init() {
    bindings_.clear();
    binding_index_.clear();

    // Movement
    Register("move_forward", "Move Forward", 87); // W
    Register("move_back", "Move Back", 83); // S
    Register("move_left", "Move Left", 65); // A
    Register("move_right", "Move Right", 68); // D

    // Actions
    Register("inventory", "Inventory", 73); // I
    Register("skills", "Skills", 75); // K
    Register("quests", "Quests", 74); // J
    Register("chat", "Chat", 84); // T
    Register("npc_dialog", "NPC Dialog", 78); // N
    Register("pk_toggle", "PK Toggle", 80); // P
    Register("boss_spawn", "Spawn Boss", 66); // B
    Register("dungeon", "Dungeon", 68); // D (same as move_right, uses context)

    // System
    Register("screenshot", "Screenshot", 261, KeyBindings::BindingMod::CTRL); // Ctrl+Delete
    Register("fullscreen", "Fullscreen", 257, KeyBindings::BindingMod::ALT); // Alt+Enter

    LoadFromConfig();
}

void KeyBindings::Register(const std::string& action, const std::string& display_name,
                            int default_key, int default_mods, const std::string& category) {
    Binding b;
    b.action = action;
    b.display_name = display_name;
    b.primary_key = default_key;
    b.primary_mods = default_mods;
    b.category = category;

    binding_index_[action] = (int)bindings_.size();
    bindings_.push_back(b);
}

void KeyBindings::LoadFromConfig() {
    for (auto& b : bindings_) {
        std::string key_str = ConfigManager::GetString("keys." + b.action, "");
        if (!key_str.empty()) {
            // Parse key from config (e.g., "Ctrl+W" or "Space")
            int mods = 0;
            std::string k = key_str;
            if (k.find("Ctrl+") == 0) { mods |= KeyBindings::BindingMod::CTRL; k = k.substr(5); }
            if (k.find("Shift+") == 0) { mods |= KeyBindings::BindingMod::SHIFT; k = k.substr(6); }
            if (k.find("Alt+") == 0) { mods |= KeyBindings::BindingMod::ALT; k = k.substr(4); }

            // Convert key name to GLFW key code (simplified)
            if (k.length() == 1) b.primary_key = (int)k[0];
            else if (k == "Space") b.primary_key = 32;
            else if (k == "Enter") b.primary_key = 257;
            else if (k == "Esc") b.primary_key = 256;
            // ... more keys

            b.primary_mods = mods;
        }
    }
}

void KeyBindings::SaveToConfig() {
    for (auto& b : bindings_) {
        std::string key_str;
        if (b.primary_mods & KeyBindings::BindingMod::CTRL) key_str += "Ctrl+";
        if (b.primary_mods & KeyBindings::BindingMod::SHIFT) key_str += "Shift+";
        if (b.primary_mods & KeyBindings::BindingMod::ALT) key_str += "Alt+";

        char single = (char)b.primary_key;
        if (b.primary_key >= 32 && b.primary_key <= 126) key_str += single;
        else if (b.primary_key == 32) key_str += "Space";
        else if (b.primary_key == 257) key_str += "Enter";
        else key_str += "Key" + std::to_string(b.primary_key);

        ConfigManager::SetString("keys." + b.action, key_str);
    }
    ConfigManager::Save();
}

bool KeyBindings::IsPressed(const std::string& action) {
    auto* input = InputSystem::GetInstance();
    if (!input) return false;

    int idx = GetActionIndex(action);
    if (idx < 0 || idx >= (int)bindings_.size()) return false;

    auto& b = bindings_[idx];
    bool mod_ok = true;
    if (b.primary_mods & KeyBindings::BindingMod::CTRL) mod_ok = mod_ok && input->IsKeyDown(341);
    if (b.primary_mods & KeyBindings::BindingMod::SHIFT) mod_ok = mod_ok && input->IsKeyDown(340);
    if (b.primary_mods & KeyBindings::BindingMod::ALT) mod_ok = mod_ok && input->IsKeyDown(342);

    return mod_ok && input->IsKeyPressed(b.primary_key);
}

const KeyBindings::Binding* KeyBindings::GetBinding(const std::string& action) {
    int idx = GetActionIndex(action);
    if (idx >= 0 && idx < (int)bindings_.size()) return &bindings_[idx];
    return nullptr;
}

std::vector<KeyBindings::Binding> KeyBindings::GetBindingsByCategory(const std::string& category) {
    std::vector<Binding> result;
    for (auto& b : bindings_) {
        if (b.category == category) result.push_back(b);
    }
    return result;
}

std::vector<std::string> KeyBindings::GetCategories() {
    std::vector<std::string> cats;
    for (auto& b : bindings_) {
        if (std::find(cats.begin(), cats.end(), b.category) == cats.end()) {
            cats.push_back(b.category);
        }
    }
    return cats;
}

void KeyBindings::SetHandler(const std::string& action, std::function<void()> handler) {
    int idx = GetActionIndex(action);
    if (idx >= 0 && idx < (int)bindings_.size()) {
        bindings_[idx].handler = handler;
    }
}

int KeyBindings::GetActionIndex(const std::string& action) {
    auto it = binding_index_.find(action);
    if (it != binding_index_.end()) return it->second;
    return -1;
}

void KeyBindings::Bind(int key, int mods) {
    // Used for rebinding at runtime
    (void)key; (void)mods;
}

void KeyBindings::Unbind(const std::string& action) {
    int idx = GetActionIndex(action);
    if (idx >= 0 && idx < (int)bindings_.size()) {
        bindings_[idx].primary_key = 0;
        bindings_[idx].primary_mods = 0;
    }
}
