#include "KeyBindDialog.hpp"
#include <cstdio>

[[maybe_unused]] static std::vector<std::pair<std::string, std::string>> GetDefaultBindings() {
    return {
        {"I", "Inventory"}, {"K", "Skills"}, {"J", "Quests"}, {"C", "Character Info"},
        {"P", "Party"}, {"G", "Guild"}, {"F", "Friends"}, {"L", "Family"},
        {"H", "Farm"}, {"R", "Housing"}, {"O", "Options"}, {"B", "Bank"},
        {"M", "World Map"}, {"Y", "Auction"}, {"U", "Trade"}, {"V", "Fishing"},
        {"X", "Dungeons"}, {"Z", "Costume"}, {"N", "NPC"}, {"T", "Chat"},
        {"Q", "Cash Shop"}, {"F1", "Help"}, {"Space", "Spawn"},
        {"1-0", "Hotbar Slots"}
    };
}

static std::vector<std::string> GetCategories() {
    return {"Gameplay", "Social", "World", "Actions"};
}

static std::vector<std::pair<std::string, std::string>> GetBindingsByCategory(const std::string& cat) {
    std::vector<std::pair<std::string, std::string>> result;
    if (cat == "Gameplay") {
        result = {{"I", "Inventory"}, {"K", "Skills"}, {"J", "Quests"}, {"C", "Character"},
                  {"O", "Options"}, {"B", "Bank/Bank"}, {"N", "NPC Interaction"}};
    } else if (cat == "Social") {
        result = {{"P", "Party"}, {"G", "Guild"}, {"F", "Friends"}, {"L", "Family"},
                  {"U", "Trade"}, {"T", "Chat"}};
    } else if (cat == "World") {
        result = {{"M", "World Map"}, {"H", "Farm"}, {"R", "Housing"}, {"Y", "Auction"},
                  {"V", "Fishing"}, {"X", "Dungeons"}};
    } else if (cat == "Actions") {
        result = {{"Q", "Cash Shop"}, {"Z", "Costume"}, {"F1", "Help"},
                  {"Space", "Spawn"}, {"1-0", "Hotbar"}};
    }
    return result;
}

void KeyBindDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    window_ = wm->Open("Key Bindings", 150, 60, 500, 420);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Customize Hotkeys", 10, 4, 0xff88ccff);
    (void)title;

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 480, 340);

    auto cats = GetCategories();
    for (int i = 0; i < (int)cats.size(); i++) {
        auto* grid = new Grid(8, 2, 220, 24, 0, 0);
        grid->SetPadding(2);
        tabs_->AddTab(cats[i], grid);
        category_grids_.push_back(grid);
    }

    // Status label
    status_label_ = window_->AddWidget<Label>("Key bindings are currently read-only. (configurable via KeyBindings.hpp)", 10, 380, 0xffaaaaaa);

    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) RefreshCategory(e.int_value);
    });

    RefreshCategory(0);
}

void KeyBindDialog::RefreshCategory(int cat_idx) {
    if (cat_idx < 0 || cat_idx >= (int)category_grids_.size()) return;
    auto cats = GetCategories();
    if (cat_idx >= (int)cats.size()) return;
    
    auto grid = category_grids_[cat_idx];
    grid->ClearAll();
    
    auto bindings = GetBindingsByCategory(cats[cat_idx]);
    for (int i = 0; i < (int)bindings.size() && i < 16; i++) {
        int r = i;
        GridSlot key_gs, action_gs;
        key_gs.empty = false; key_gs.text = bindings[i].first;
        action_gs.empty = false; action_gs.text = bindings[i].second;
        grid->SetSlot(r, 0, key_gs);
        grid->SetSlot(r, 1, action_gs);
    }
    if (status_label_) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Category: %s — %zu bindings", cats[cat_idx].c_str(), bindings.size());
        status_label_->SetText(buf);
    }
}

void KeyBindDialog::UpdateFromState(GameState* state) {
    (void)state;
}
