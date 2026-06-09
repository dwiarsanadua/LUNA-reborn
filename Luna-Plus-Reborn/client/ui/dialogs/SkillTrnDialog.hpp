#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/ListBox.hpp>
#include <functional>
#include <cstdint>

struct SkillTrnEntry {
    int skill_id;
    std::string name;
    std::string description;
    int required_level;
    int required_skill_id;
    int max_level;
    int current_level;
    int sp_cost;
    int gold_cost;
    bool can_learn;
    bool is_learned;
    bool has_prereq;
};

class SkillTrnDialog {
public:
    void Open(GameState* state, WindowManager* wm = nullptr);
    void Close();
    Window* GetWindow() { return window_; }
    void UpdateFromState(GameState* state);

    using TrainCallback = std::function<void(int skill_id, int sp_cost, int gold_cost)>;
    void SetTrainCallback(TrainCallback cb) { train_cb_ = cb; }

private:
    Window* window_ = nullptr;
    ListBox* skill_list_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* sp_label_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* cost_sp_label_ = nullptr;
    Label* cost_gold_label_ = nullptr;
    int selected_skill_ = -1;
    TrainCallback train_cb_;
    std::vector<SkillTrnEntry> cached_skills_;

    void Refresh(GameState* state);
    void TrainSkill(GameState* state);
    void ShowSkillDetail(int index);
    void BuildSkillList(GameState* state);
};
