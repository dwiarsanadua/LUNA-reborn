#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct QuestDef {
    uint32_t id;
    std::string name;
    std::string description;
    std::string objective;
    int xp_reward;
    int gold_reward;
    std::string item_reward;
    int required_level;
    std::string category; // Main, Side, Daily, Event
};

class QuestDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    static std::vector<QuestDef> GetAvailableQuests(int level);
    static bool TryCompleteQuest(GameState* state, const QuestDef& quest);
    
private:
    Window* window_ = nullptr;
    ListBox* quest_list_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* reward_label_ = nullptr;
    TextureInfo bg_tex_;
    std::vector<QuestDef> current_quests_;
    void ShowQuestDetail(GameState* state, int index);
};
