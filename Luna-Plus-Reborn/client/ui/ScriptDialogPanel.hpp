#pragma once
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct ScriptDialogEntry {
    std::string script_path;
    std::string fallback_title;
    float x = 100, y = 80, w = 400, h = 300;
    bool* open_flag = nullptr;
};

class ScriptDialogPanel {
public:
    void Register(const std::string& key, ScriptDialogEntry entry);
    void Open(const std::string& key, WindowManager* wm);
    void Close(const std::string& key);
    void Toggle(const std::string& key, WindowManager* wm);
    void Render(UIRenderer& ui, GameState* state);
    Window* Get(const std::string& key);
    bool IsOpen(const std::string& key) const;
    int RegisteredCount() const { return static_cast<int>(entries_.size()); }
    std::vector<std::string> ListKeys() const;

private:
    bool IsEntryOpen(const std::string& key, const ScriptDialogEntry& e) const;

    std::unordered_map<std::string, ScriptDialogEntry> entries_;
    std::unordered_map<std::string, Window*> windows_;
    std::unordered_map<std::string, bool> open_states_;
};
