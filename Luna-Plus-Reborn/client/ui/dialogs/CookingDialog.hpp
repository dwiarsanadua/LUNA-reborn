#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct Recipe {
    uint32_t id;
    std::string name;
    std::string result;
    int result_count;
    std::string ingredient1;
    int ingredient1_count;
    std::string ingredient2;
    int ingredient2_count;
    int required_level;
    int cooking_time; // seconds
    int xp_reward;
};

class CookingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    Label* recipe_label_ = nullptr;
    Label* status_label_ = nullptr;
    Grid* recipe_grid_ = nullptr;
    std::vector<Recipe> recipes_;
    int selected_idx_ = -1;
    void Refresh();
    void Cook(GameState* state, const Recipe& recipe);
};
