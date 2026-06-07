#include "CookingDialog.hpp"
#include <cstdio>

static std::vector<Recipe> GenerateRecipes() {
    std::vector<Recipe> r;
    r.push_back({1, "Apple Juice", "HP Potion (S)", 3, "Apple", 5, "Water", 2, 1, 5, 50});
    r.push_back({2, "Herbal Tea", "MP Potion (S)", 3, "Herb", 5, "Water", 2, 3, 5, 50});
    r.push_back({3, "Meat Stew", "HP Potion (M)", 2, "Meat", 3, "Herb", 3, 5, 10, 100});
    r.push_back({4, "Energy Drink", "MP Potion (M)", 2, "Fruit", 4, "Honey", 2, 8, 10, 100});
    r.push_back({5, "Bread", "Bread", 5, "Flour", 3, "Water", 1, 1, 8, 40});
    r.push_back({6, "Fish Soup", "EXP Boost", 1, "Fish", 3, "Herb", 2, 10, 15, 200});
    r.push_back({7, "Fruit Cake", "Gold Boost", 1, "Fruit", 5, "Flour", 3, 15, 20, 300});
    r.push_back({8, "Elixir", "Full Recovery", 1, "Herb", 10, "Honey", 5, 20, 30, 500});
    r.push_back({9, "Dragon Stew", "ATK Boost", 1, "Meat", 8, "Dragon Scale", 2, 30, 45, 1000});
    r.push_back({10, "Ambrosia", "DEF Boost", 1, "Honey", 10, "Herb", 8, 40, 60, 2000});
    return r;
}

void CookingDialog::Open(GameState* state, WindowManager* wm) {
    recipes_ = GenerateRecipes();
    window_ = wm->Open("Cooking", 150, 60, 500, 420);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Cooking Station", 10, 4, 0xff88ff88);

    recipe_grid_ = window_->AddWidget<Grid>(5, 2, 220, 28, 10, 30);
    recipe_grid_->SetPadding(3);
    recipe_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = row * 2 + col;
            if (idx >= 0 && idx < (int)recipes_.size()) {
                selected_idx_ = idx;
                auto& r = recipes_[idx];
                char buf[256];
                snprintf(buf, sizeof(buf), "%s (Lv.%d)\n%s x%d\nNeeds: %s x%d + %s x%d\nCooking: %ds | XP: %d",
                    r.name.c_str(), r.required_level,
                    r.result.c_str(), r.result_count,
                    r.ingredient1.c_str(), r.ingredient1_count,
                    r.ingredient2.c_str(), r.ingredient2_count,
                    r.cooking_time, r.xp_reward);
                if (recipe_label_) recipe_label_->SetText(buf);
            }
        }
    });

    // Refresh grid
    for (int i = 0; i < (int)recipes_.size() && i < 10; i++) {
        int r = i / 2, c = i % 2;
        GridSlot gs; gs.empty = false; gs.text = recipes_[i].name;
        recipe_grid_->SetSlot(r, c, gs);
    }

    recipe_label_ = window_->AddWidget<Label>("Select a recipe", 250, 30, 0xffffffff);
    status_label_ = window_->AddWidget<Label>("", 250, 200, 0xffcccccc);

    auto* cook_btn = window_->AddWidget<Button>("Cook!", 250, 240, 100, 24);
    cook_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    cook_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && selected_idx_ >= 0 && selected_idx_ < (int)recipes_.size()) {
            Cook(state, recipes_[selected_idx_]);
        }
    });

    auto* level_info = window_->AddWidget<Label>("Cooking Lv." + std::to_string(state->level), 400, 4, 0xffffcc88);
    (void)level_info;
}

void CookingDialog::Cook(GameState* state, const Recipe& recipe) {
    if (state->level < recipe.required_level) {
        if (status_label_) status_label_->SetText("Level too low for this recipe!");
        return;
    }
    if (state->gold < recipe.cooking_time) {
        if (status_label_) status_label_->SetText("Not enough gold for ingredients!");
        return;
    }
    state->gold -= recipe.cooking_time;
    state->exp = std::min(state->exp + recipe.xp_reward, state->exp_next);
    char buf[128];
    snprintf(buf, sizeof(buf), "Cooked %s x%d! +%d XP", recipe.result.c_str(), recipe.result_count, recipe.xp_reward);
    state->chat_messages.push_back(buf);
    if (status_label_) status_label_->SetText(buf);
}

void CookingDialog::Refresh() {}
void CookingDialog::UpdateFromState(GameState* state) { (void)state; }
