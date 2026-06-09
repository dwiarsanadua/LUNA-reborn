#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct KillTrackEntry {
    uint32_t monster_id = 0;
    std::string name;
    uint16_t killed = 0;
    uint16_t required = 1;
};

class MonsterKillDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetKillTracking(const std::vector<KillTrackEntry>& entries);
    bool IsActive() const { return window_ != nullptr; }

private:
    void RebuildDisplay();

    Window* window_ = nullptr;
    std::vector<KillTrackEntry> entries_;
    std::vector<Label*> entry_labels_;
    Label* title_label_ = nullptr;
};
