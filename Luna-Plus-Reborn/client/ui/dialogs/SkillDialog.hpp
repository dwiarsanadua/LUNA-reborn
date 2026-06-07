#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <functional>

class SkillDialog {
public:
    void Open(GameState* state, WindowManager* wm = nullptr);
    void Close();
    Window* GetWindow() { return window_; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TextureInfo bg_tex_;
    TabPanel* tabs_ = nullptr;
    Grid* skill_grid_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* sp_label_ = nullptr;
    int current_tab_ = 0;
    void Refresh(GameState* state);
    void LearnSkill(GameState* state, int skill_id);
};
