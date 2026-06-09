#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/ListBox.hpp>
#include <functional>
#include <cstdint>
#include <vector>

struct MixIngredient {
    uint32_t item_id;
    int count;
};

struct MixRecipe {
    int id;
    std::string name;
    uint32_t result_item_id;
    std::string result_name;
    int required_level;
    int gold_cost;
    int success_rate;
    MixIngredient base_material;
    std::vector<MixIngredient> extra_materials;
};

class MixDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

    using MixCallback = std::function<void(int recipe_id, uint32_t source_item_pos,
                                            bool use_protection, bool use_support)>;
    void SetMixCallback(MixCallback cb) { mix_cb_ = cb; }

private:
    Window* window_ = nullptr;
    ListBox* recipe_list_ = nullptr;
    Grid* material_grid_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* cost_label_ = nullptr;
    Label* success_label_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* result_label_ = nullptr;
    InputField* search_input_ = nullptr;
    int selected_recipe_ = -1;
    bool use_protection_ = false;
    bool use_support_ = false;
    MixCallback mix_cb_;
    std::vector<MixRecipe> cached_recipes_;

    void LoadRecipes();
    void Refresh(GameState* state);
    void ShowRecipeDetail(int index);
    void DoMix(GameState* state);
    void DoSearch(GameState* state);
};
