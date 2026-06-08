#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <string>
#include <vector>
#include <functional>

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
    void SetNetworkCallbacks(std::function<void(uint32_t)> start_fn,
                             std::function<void(uint32_t)> complete_fn,
                             std::function<void()> refresh_fn);
    static std::vector<QuestDef> GetAvailableQuests(int level);
    static bool TryCompleteQuest(GameState* state, const QuestDef& quest);
    
private:
    Window* window_ = nullptr;
    ListBox* quest_list_ = nullptr;
    ListBox* avail_list_ = nullptr;
    ListBox* done_list_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* reward_label_ = nullptr;
    TextureInfo bg_tex_;
    std::vector<QuestDef> current_quests_;
    std::function<void(uint32_t)> on_start_quest_;
    std::function<void(uint32_t)> on_complete_quest_;
    std::function<void()> on_refresh_;
    void RefreshLists(GameState* state);
    void ShowQuestDetail(GameState* state, int index);
    void ShowNetworkQuestDetail(const GameState::NetworkQuestEntry& q);
};
