#include "MixDialog.hpp"
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

void MixDialog::LoadRecipes() {
    cached_recipes_.clear();

    // Warrior weapon recipes
    cached_recipes_.push_back({1, "Iron Sword", 101, "Iron Sword +1", 10, 500, 90,
        {1001, 3}, {{1002, 1}, {1003, 2}}});
    cached_recipes_.push_back({2, "Steel Blade", 102, "Steel Blade", 20, 1500, 75,
        {1001, 5}, {{1004, 2}}});
    cached_recipes_.push_back({3, "Mithril Sword", 103, "Mithril Sword", 35, 5000, 60,
        {1001, 8}, {{1005, 3}, {1006, 1}}});

    // Mage weapon recipes
    cached_recipes_.push_back({4, "Apprentice Staff", 201, "Apprentice Staff", 10, 500, 90,
        {2001, 3}, {{2002, 1}}});
    cached_recipes_.push_back({5, "Fire Staff", 202, "Fire Staff", 25, 3000, 70,
        {2001, 5}, {{2003, 2}, {2004, 1}}});

    // Archer weapon recipes
    cached_recipes_.push_back({6, "Hunter Bow", 301, "Hunter Bow", 15, 1000, 85,
        {3001, 3}, {{3002, 2}}});
    cached_recipes_.push_back({7, "Longbow", 302, "Longbow", 30, 4000, 65,
        {3001, 6}, {{3003, 3}, {3004, 1}}});

    // Armor recipes
    cached_recipes_.push_back({8, "Leather Armor", 401, "Leather Armor", 15, 800, 88,
        {4001, 4}, {{4002, 2}}});
    cached_recipes_.push_back({9, "Chain Mail", 402, "Chain Mail", 28, 3500, 72,
        {4001, 6}, {{4003, 3}, {4004, 1}}});
    cached_recipes_.push_back({10, "Plate Armor", 403, "Plate Armor", 40, 8000, 55,
        {4001, 10}, {{4005, 4}, {4006, 2}}});

    // Accessory recipes
    cached_recipes_.push_back({11, "Ring of Strength", 501, "Ring of Strength", 20, 2000, 80,
        {5001, 2}, {{5002, 1}}});
    cached_recipes_.push_back({12, "Amulet of Wisdom", 502, "Amulet of Wisdom", 30, 4500, 68,
        {5001, 4}, {{5003, 2}}});

    // Potion recipes
    cached_recipes_.push_back({13, "HP Potion (L)", 601, "Large HP Potion", 5, 100, 95,
        {6001, 3}, {}});
    cached_recipes_.push_back({14, "MP Potion (L)", 602, "Large MP Potion", 5, 100, 95,
        {6002, 3}, {}});
    cached_recipes_.push_back({15, "Elixir", 603, "Elixir", 30, 3000, 70,
        {6001, 5}, {{6002, 5}, {6003, 1}}});
}

void MixDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/MixDialog.bin.txt",
        "Item Mix", 200, 100, 420, 380);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(28);

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(gold_buf, 10, 4, 0xffffcc00);

    // Search
    auto* search_lbl = window_->AddWidget<Label>("Search:", 10, 28, 0xffcccccc);
    (void)search_lbl;
    search_input_ = window_->AddWidget<InputField>(70, 28, 140, 20);
    search_input_->SetPlaceholder("item name...");

    auto* search_btn = window_->AddWidget<Button>("Find", 220, 28, 60, 20);
    search_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    search_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSearch(state);
    });

    // Recipe list
    auto* recipe_lbl = window_->AddWidget<Label>("--- Mix Recipes ---", 10, 54, 0xff88ff88);
    (void)recipe_lbl;
    recipe_list_ = new ListBox(10, 72, 200, 180);
    recipe_list_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int sel = recipe_list_->GetSelected();
            if (sel >= 0) {
                selected_recipe_ = sel;
                ShowRecipeDetail(sel);
            }
        }
    });

    // Material grid
    auto* mat_lbl = window_->AddWidget<Label>("Materials:", 220, 54, 0xff88ff88);
    (void)mat_lbl;
    material_grid_ = window_->AddWidget<Grid>(3, 3, 60, 28, 220, 72);
    material_grid_->SetPadding(3);

    // Detail area
    detail_label_ = window_->AddWidget<Label>("Select a recipe", 220, 180, 0xffcccccc);
    cost_label_ = window_->AddWidget<Label>("", 220, 210, 0xffffcc00);
    success_label_ = window_->AddWidget<Label>("", 220, 230, 0xff88ff88);

    // Protection & Support checkboxes (simplified as buttons)
    auto* protect_btn = window_->AddWidget<Button>("Protection: OFF", 10, 262, 130, 22);
    protect_btn->SetColors({60,40,40,220}, {100,60,60,220}, {40,20,20,220});
    protect_btn->OnEvent([this, protect_btn](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            use_protection_ = !use_protection_;
            protect_btn->SetText(use_protection_ ? "Protection: ON (+20g)" : "Protection: OFF");
        }
    });

    auto* support_btn = window_->AddWidget<Button>("Support: OFF", 150, 262, 120, 22);
    support_btn->SetColors({40,60,40,220}, {60,100,60,220}, {20,40,20,220});
    support_btn->OnEvent([this, support_btn](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            use_support_ = !use_support_;
            support_btn->SetText(use_support_ ? "Support: ON (+10g)" : "Support: OFF");
        }
    });

    // Mix button
    auto* mix_btn = window_->AddWidget<Button>("Mix Items!", 10, 296, 140, 28);
    mix_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    mix_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoMix(state);
    });

    // Cancel button
    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 170, 296, 80, 28);
    cancel_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) window_ = nullptr;
    });

    result_label_ = window_->AddWidget<Label>("Ready", 10, 336, 0xffaaaaaa);

    LoadRecipes();
    Refresh(state);
}

void MixDialog::Refresh(GameState* state) {
    if (!recipe_list_ || !gold_label_) return;

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_->SetText(gold_buf);

    recipe_list_->Clear();
    for (size_t i = 0; i < cached_recipes_.size(); i++) {
        const auto& recipe = cached_recipes_[i];
        bool affordable = state->gold >= recipe.gold_cost;
        bool high_enough = state->level >= recipe.required_level;
        const char* tag;
        if (!high_enough) tag = "LVL";
        else if (!affordable) tag = "GOLD";
        else tag = "OK";

        char line[128];
        snprintf(line, sizeof(line), "[%s] %s → %s", tag, recipe.name.c_str(), recipe.result_name.c_str());
        recipe_list_->AddItem(line);
    }

    if (selected_recipe_ >= 0 && selected_recipe_ < (int)cached_recipes_.size())
        ShowRecipeDetail(selected_recipe_);
}

void MixDialog::ShowRecipeDetail(int index) {
    if (index < 0 || index >= (int)cached_recipes_.size()) return;
    const auto& recipe = cached_recipes_[index];

    // Detail text
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\nRequires: Lv.%d\nBase: Item %u x%d",
             recipe.result_name.c_str(), recipe.required_level,
             recipe.base_material.item_id, recipe.base_material.count);
    if (detail_label_) detail_label_->SetText(buf);

    // Cost
    char cost_buf[64];
    snprintf(cost_buf, sizeof(cost_buf), "Cost: %dg", recipe.gold_cost);
    if (cost_label_) cost_label_->SetText(cost_buf);

    // Success rate
    char success_buf[64];
    int final_rate = recipe.success_rate;
    if (use_support_) final_rate = std::min(100, final_rate + 20);
    snprintf(success_buf, sizeof(success_buf), "Success: %d%%", final_rate);
    if (success_label_) success_label_->SetText(success_buf);

    // Material grid
    if (material_grid_) {
        material_grid_->ClearAll();
        int slot = 0;
        // Base material
        {
            char mat[64];
            snprintf(mat, sizeof(mat), "Base\nx%d", recipe.base_material.count);
            GridSlot gs; gs.empty = false; gs.text = mat;
            material_grid_->SetSlot(0, slot++, gs);
        }
        // Extra materials
        for (const auto& em : recipe.extra_materials) {
            char mat[64];
            snprintf(mat, sizeof(mat), "Mat%d\nx%d", em.item_id, em.count);
            GridSlot gs; gs.empty = false; gs.text = mat;
            material_grid_->SetSlot(0, slot++, gs);
        }
    }
}

void MixDialog::DoMix(GameState* state) {
    if (selected_recipe_ < 0 || selected_recipe_ >= (int)cached_recipes_.size()) {
        if (result_label_) result_label_->SetText("Select a recipe first!");
        return;
    }

    const auto& recipe = cached_recipes_[selected_recipe_];

    if (state->level < recipe.required_level) {
        if (result_label_) result_label_->SetText("Level too low!");
        return;
    }

    int total_cost = recipe.gold_cost;
    if (use_protection_) total_cost += 20;
    if (use_support_) total_cost += 10;

    if (state->gold < total_cost) {
        if (result_label_) result_label_->SetText("Not enough gold!");
        return;
    }

    if (mix_cb_) {
        // Online: delegate to network callback
        uint32_t source_pos = recipe.base_material.item_id;
        mix_cb_(recipe.id, source_pos, use_protection_, use_support_);
        if (result_label_) result_label_->SetText("Mix request sent...");
    } else {
        // Offline: simulate mix result
        state->gold -= total_cost;
        int roll = rand() % 100;
        int final_rate = recipe.success_rate;
        if (use_support_) final_rate = std::min(100, final_rate + 20);

        char buf[128];
        if (roll < final_rate) {
            // Success
            InvItem new_item;
            new_item.id = recipe.result_item_id;
            new_item.name = recipe.result_name;
            new_item.count = 1;
            new_item.slot = static_cast<int>(state->inventory.size());
            state->inventory.push_back(new_item);
            snprintf(buf, sizeof(buf), "Mix success! Created %s! (Cost: %dg)",
                     recipe.result_name.c_str(), total_cost);
        } else {
            snprintf(buf, sizeof(buf), "Mix failed! (Rolled %d%%, needed %d%%)",
                     roll, final_rate);
            if (use_protection_) {
                strcat(buf, " Protection saved your materials.");
            }
        }
        state->chat_messages.push_back(buf);
        if (result_label_) result_label_->SetText(buf);
    }

    Refresh(state);
}

void MixDialog::DoSearch(GameState* state) {
    if (!search_input_) return;
    std::string query = search_input_->GetText();
    if (query.empty()) {
        LoadRecipes();
        Refresh(state);
        return;
    }

    // Filter recipes by name match
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    cached_recipes_.clear();

    // Re-load and filter
    LoadRecipes();
    auto all = cached_recipes_;
    cached_recipes_.clear();

    for (const auto& recipe : all) {
        std::string lower_name = recipe.name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        std::string lower_result = recipe.result_name;
        std::transform(lower_result.begin(), lower_result.end(), lower_result.begin(), ::tolower);

        if (lower_name.find(lower_query) != std::string::npos ||
            lower_result.find(lower_query) != std::string::npos) {
            cached_recipes_.push_back(recipe);
        }
    }

    selected_recipe_ = -1;
    Refresh(state);
}

void MixDialog::UpdateFromState(GameState* state) {
    if (window_ && state->mix_open) Refresh(state);
}
