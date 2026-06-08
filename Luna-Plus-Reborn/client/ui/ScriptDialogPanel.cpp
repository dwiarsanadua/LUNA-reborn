#include "ScriptDialogPanel.hpp"
#include <algorithm>

bool ScriptDialogPanel::IsEntryOpen(const std::string& key, const ScriptDialogEntry& e) const {
    if (e.open_flag) return *e.open_flag;
    auto it = open_states_.find(key);
    return it != open_states_.end() && it->second;
}

void ScriptDialogPanel::Register(const std::string& key, ScriptDialogEntry entry) {
    entries_[key] = std::move(entry);
    open_states_[key] = false;
}

void ScriptDialogPanel::Open(const std::string& key, WindowManager* wm) {
    if (!wm) return;
    auto it = entries_.find(key);
    if (it == entries_.end()) return;
    auto& e = it->second;
    if (e.open_flag) *e.open_flag = true;
    else open_states_[key] = true;
    if (!windows_[key]) {
        windows_[key] = wm->LoadFromScriptOrOpen(
            e.script_path, e.fallback_title, e.x, e.y, e.w, e.h);
        if (windows_[key]) {
            windows_[key]->SetClosable(true);
            windows_[key]->SetMovable(true);
            windows_[key]->OnClose([this, key, &e]() {
                if (e.open_flag) *e.open_flag = false;
                else open_states_[key] = false;
            });
        }
    } else {
        windows_[key]->SetVisible(true);
    }
}

void ScriptDialogPanel::Close(const std::string& key) {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
        if (it->second.open_flag) *it->second.open_flag = false;
        else open_states_[key] = false;
    }
    if (auto w = windows_.find(key); w != windows_.end() && w->second)
        w->second->SetVisible(false);
}

void ScriptDialogPanel::Toggle(const std::string& key, WindowManager* wm) {
    auto it = entries_.find(key);
    if (it == entries_.end()) return;
    if (IsEntryOpen(key, it->second)) Close(key);
    else Open(key, wm);
}

void ScriptDialogPanel::Render(UIRenderer& ui, GameState* state) {
    (void)state;
    for (auto& [key, win] : windows_) {
        auto it = entries_.find(key);
        if (it == entries_.end() || !IsEntryOpen(key, it->second)) continue;
        if (win && win->IsVisible()) win->Render(ui);
    }
}

Window* ScriptDialogPanel::Get(const std::string& key) {
    auto it = windows_.find(key);
    return it != windows_.end() ? it->second : nullptr;
}

bool ScriptDialogPanel::IsOpen(const std::string& key) const {
    auto it = entries_.find(key);
    if (it == entries_.end()) return false;
    return IsEntryOpen(key, it->second);
}

std::vector<std::string> ScriptDialogPanel::ListKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    for (const auto& [k, _] : entries_) keys.push_back(k);
    std::sort(keys.begin(), keys.end());
    return keys;
}
