#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct QuickQuestEntry {
    uint32_t quest_id = 0;
    std::string name;
    std::string objective;
    uint16_t progress = 0;
    uint16_t required = 1;
    bool completed = false;
};

class QuestQuickViewDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetQuests(const std::vector<QuickQuestEntry>& quests);
    bool IsActive() const { return window_ != nullptr; }

private:
    void RebuildDisplay();

    Window* window_ = nullptr;
    std::vector<QuickQuestEntry> quests_;
    std::vector<Label*> quest_labels_;
};
