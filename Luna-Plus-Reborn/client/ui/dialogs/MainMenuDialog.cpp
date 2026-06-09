#include "MainMenuDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <algorithm>
#include <cstdio>

MainMenuDialog::MainMenuDialog() {}

void MainMenuDialog::Open(WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/MainMenu.bin.txt",
        "Main Menu", 0, 0, 0, 0);
    if (window_) {
        window_->SetTitleBarH(0);
        window_->SetClosable(false);
        window_->SetMovable(false);
        window_->SetDrawChrome(false);
    }
}

void MainMenuDialog::Close() {
    window_ = nullptr;
}

void MainMenuDialog::RegisterDefaultButtons(GameState* state,
    std::function<void()> on_inv, std::function<void()> on_skill,
    std::function<void()> on_char, std::function<void()> on_quest,
    std::function<void()> on_party, std::function<void()> on_guild,
    std::function<void()> on_friend, std::function<void()> on_options,
    std::function<void()> on_help) {
    if (registered_) return;
    registered_ = true;

    auto make = [&](const std::string& lbl, bool* toggle, std::function<void()> cb,
                    uint32_t col_n, uint32_t col_a) {
        MenuButtonDef def;
        def.label = lbl;
        def.toggle_state = toggle;
        def.on_click = cb;
        def.color_normal = col_n;
        def.color_active = col_a;
        def.color_hover = 0;
        buttons_.push_back(def);
    };

    make("Inv",  &state->inv_open,      on_inv,   0x55667788, 0x7788aaee);
    make("Skill", &state->skill_open,    on_skill,  0x55667788, 0x7788aaee);
    make("Char",  &state->charinfo_open, on_char,   0x55667788, 0x7788aaee);
    make("Quest", &state->quest_open,    on_quest,  0x55667788, 0x7788aaee);
    make("Party", &state->party_open,    on_party,  0x55667788, 0x7788aaee);
    make("Guild", &state->guild_open,    on_guild,  0x55667788, 0x7788aaee);
    make("Friend",&state->friend_open,   on_friend, 0x55667788, 0x7788aaee);
    make("Opts",  &state->options_open,  on_options, 0x55667788, 0x7788aaee);
    make("Help",  &state->help_open,     on_help,   0x55667788, 0x7788aaee);

    // Extra toggles from GameState
    auto addToggle = [&](const std::string& lbl, bool* toggle) {
        MenuButtonDef def;
        def.label = lbl;
        def.toggle_state = toggle;
        def.on_click = [toggle]() { if (toggle) *toggle = !*toggle; };
        def.color_normal = 0x55667788;
        def.color_active = 0x7788aaee;
        buttons_.push_back(def);
    };

    addToggle("Pet",    &state->pet_open);
    addToggle("Mount",  &state->mount_open);
    addToggle("Farm",   &state->farm_open);
    addToggle("Mail",   &state->mail_open);
    addToggle("Stall",  &state->stall_sell_open);
    addToggle("Family", &state->family_open);
    addToggle("Housing",&state->housing_open);
    addToggle("Dungeon",&state->dungeon_open);
    addToggle("Costume",&state->costume_open);
    addToggle("Cash",   &state->cashshop_open);
    addToggle("Fishing",&state->fishing_open);
    addToggle("Cooking",&state->cooking_open);
    addToggle("Upgrade",&state->upgrade_open);
    addToggle("Avatar", &state->avatar_open);
}

void MainMenuDialog::AddCustomButton(const MenuButtonDef& def) {
    buttons_.push_back(def);
}

void MainMenuDialog::Update(GameState* state) {
    (void)state;
    // Position is calculated in Render based on screen size
}

void MainMenuDialog::RenderButton(UIRenderer& ui, const MenuButtonDef& def,
                                   float x, float y, float w, float h, bool hovered) {
    bool active = def.toggle_state && *def.toggle_state;
    uint32_t bg = active ? def.color_active : (hovered ? def.color_hover : def.color_normal);
    if (bg == 0) bg = active ? 0x667799cc : (hovered ? 0x445566aa : 0x33445588);

    ui.DrawRect(x, y, w, h, UIColor::FromRGBA(bg));
    if (active || hovered) {
        uint32_t border = active ? 0x88aaccff : 0x667788cc;
        ui.DrawBorder(x, y, w, h, UIColor::FromRGBA(border));
    }
    ui.DrawTextCenteredInRect(x, y, w, h, active ? 0xffffffff : 0xffcccccc, "%s", def.label.c_str());
}

void MainMenuDialog::Render(UIRenderer& ui) {
    if (buttons_.empty()) return;

    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    float btn_w = 48;
    float btn_h = 24;
    float gap = 2;
    int cols = 10;
    int rows = (int)buttons_.size() / cols + ((int)buttons_.size() % cols ? 1 : 0);
    float total_w = cols * (btn_w + gap) - gap;
    float total_h = rows * (btn_h + gap) - gap;

    bar_x_ = (lw - total_w) * 0.5f;
    bar_y_ = lh - total_h - 8;
    bar_w_ = total_w;
    bar_h_ = total_h;

    // Background bar
    ui.DrawRect(bar_x_ - 4, bar_y_ - 4, bar_w_ + 8, bar_h_ + 8, UIColor{20, 25, 35, 200});
    ui.DrawBorder(bar_x_ - 4, bar_y_ - 4, bar_w_ + 8, bar_h_ + 8, UIColor{60, 80, 120, 100});

    // Buttons
    for (size_t i = 0; i < buttons_.size(); i++) {
        int row = (int)i / cols;
        int col = (int)i % cols;
        float bx = bar_x_ + col * (btn_w + gap);
        float by = bar_y_ + row * (btn_h + gap);

        bool hovered = (i == (size_t)hovered_idx_);
        RenderButton(ui, buttons_[i], bx, by, btn_w, btn_h, hovered);

        // Click detection via mouse coordinates
        float mx = ui.GetMouseX();
        float my = ui.GetMouseY();
        if (mx >= bx && mx <= bx + btn_w && my >= by && my <= by + btn_h) {
            hovered_idx_ = (int)i;
            if (ui.IsMouseClicked()) {
                if (buttons_[i].on_click) {
                    buttons_[i].on_click();
                } else if (buttons_[i].toggle_state) {
                    *buttons_[i].toggle_state = !*buttons_[i].toggle_state;
                }
            }
        }
    }

    // Reset hover if mouse leaves bar
    float mx = ui.GetMouseX();
    float my = ui.GetMouseY();
    if (my < bar_y_ - 10 || my > bar_y_ + bar_h_ + 10 ||
        mx < bar_x_ - 10 || mx > bar_x_ + bar_w_ + 10) {
        hovered_idx_ = -1;
    }

    // Tooltip for hovered button
    if (hovered_idx_ >= 0 && hovered_idx_ < (int)buttons_.size()) {
        auto& def = buttons_[hovered_idx_];
        if (!def.tooltip.empty()) {
            float tx = bar_x_;
            float ty = bar_y_ - 22;
            float tw = ui.CalculateTextWidth(def.tooltip.c_str()) + 12;
            ui.DrawRect(tx, ty, tw, 20, UIColor{10, 15, 25, 220});
            ui.DrawText(tx + 4, ty + 2, 0xffcccccc, "%s", def.tooltip.c_str());
        }
    }
}
