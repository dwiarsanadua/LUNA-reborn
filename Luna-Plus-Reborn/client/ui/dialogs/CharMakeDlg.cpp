#include "CharMakeDlg.hpp"
#include <ui/GameState.hpp>
#include <cstdio>
#include <cstring>
#include <algorithm>

static const char* RACE_NAMES[] = { "Human", "Elf", "Dwarf", "Orc" };
static const char* GENDER_NAMES[] = { "Male", "Female" };
static const char* CLASS_NAMES[] = { "Warrior", "Mage", "Archer", "Assassin" };
static const uint32_t RACE_COLORS[] = { 0xffe8c090, 0xffc8d8a0, 0xffc0a878, 0xff90a060 };
static const char* RACE_MODELS[] = {
    "models/char/human_male.glb", "models/char/elf_female.glb",
    "models/char/dwarf_male.glb", "models/char/orc_male.glb"
};

CharMakeDlg::CharMakeDlg() {}

CharMakeDlg::~CharMakeDlg() {
    if (preview_created_) {
        CharRenderer_Remove(preview_instance_);
    }
}

void CharMakeDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->Open("Character Creation", 40, 20, 1200, 680);
    window_->SetClosable(false);
    window_->SetMovable(false);
    window_->SetTitleBarH(0);
    window_->SetModal(true);
}

void CharMakeDlg::Close() {
    if (preview_created_) {
        CharRenderer_Remove(preview_instance_);
        preview_created_ = false;
    }
    window_ = nullptr;
}

void CharMakeDlg::UpdatePreview() {
    if (!preview_created_) {
        preview_instance_ = 0;
        CharRenderer_Spawn(preview_instance_,
            RACE_MODELS[race_ % 4], 0, 0, 0, RACE_COLORS[race_ % 4]);
        preview_created_ = true;
    } else {
        CharRenderer_Remove(preview_instance_);
        preview_instance_ = 0;
        CharRenderer_Spawn(preview_instance_,
            RACE_MODELS[race_ % 4], 0, 0, 0, RACE_COLORS[race_ % 4]);
    }
}

GameState* CharMakeDlg::CreateCharacter() {
    auto* state = new GameState();
    state->name = name_;
    state->race = race_;
    state->gender = gender_;
    state->class_id = class_;
    state->level = 1;
    state->hp = 100;
    state->max_hp = 100;
    state->mp = 50;
    state->max_mp = 50;
    state->exp = 0;
    state->exp_next = 200;
    state->gold = 100;
    state->attack = 10;
    state->defense = 5;
    state->hair_style = appearance_.hair_style;
    state->face_style = appearance_.face_style;
    state->hair_color = appearance_.hair_color;
    state->skin_color = appearance_.skin_color;
    state->eye_color = appearance_.eye_color;
    complete_ = true;
    return state;
}

void CharMakeDlg::OnConfirm() {
    if (name_.empty()) return;
    complete_ = true;
}

void CharMakeDlg::Render(UIRenderer& ui) {
    if (!window_) return;
    float wx = window_->GetX();
    float wy = window_->GetY();
    float ww = window_->GetW();
    float wh = window_->GetH();

    // Dark overlay
    ui.DrawRect(0, 0, 1280, 720, {0, 0, 0, 180});
    ui.DrawRect(wx, wy, ww, wh, {20, 16, 30, 240});
    ui.DrawBorder(wx, wy, ww, wh, {120, 180, 255, 200}, 2.0f);

    ui.DrawTextCentered(wy + 12, 0xffffcc88, "CHARACTER CREATION");

    // Tab buttons
    const char* tabs[] = { "Class", "Appearance", "Preview" };
    float tab_x = wx + 30;
    float tab_y = wy + 40;
    for (int i = 0; i < 3; i++) {
        uint32_t tc = (i == selected_tab_) ? 0xff88ff88 : 0xff888888;
        if (i > 0) tab_x += 110;
        ui.DrawText(tab_x, tab_y, tc, "[%s]", tabs[i]);
    }

    switch (selected_tab_) {
    case 0: RenderClassSelection(ui, wx + 20, wy + 70); break;
    case 1: RenderAppearance(ui, wx + 20, wy + 70); break;
    case 2: RenderPreview(ui, wx + 20, wy + 70); break;
    }

    // Name input area
    ui.DrawText(wx + 20, wy + wh - 60, 0xffffffff, "Name:");
    ui.DrawBorder(wx + 80, wy + wh - 62, 200, 24, {80, 80, 120, 200});
    if (!name_.empty()) {
        ui.DrawText(wx + 84, wy + wh - 58, 0xffffffff, name_.c_str());
    }

    // Confirm button
    bool can_confirm = !name_.empty();
    ui.DrawRect(wx + ww - 160, wy + wh - 45, 130, 30,
        can_confirm ? UIColor{40, 80, 40, 220} : UIColor{40, 40, 40, 220});
    ui.DrawBorder(wx + ww - 160, wy + wh - 45, 130, 30,
        can_confirm ? UIColor{80, 180, 80, 200} : UIColor{80, 80, 80, 200});
    ui.DrawTextCentered(wy + wh - 38, can_confirm ? 0xffffffff : 0xff666666, "CREATE");
}

void CharMakeDlg::RenderClassSelection(UIRenderer& ui, float x, float y) {
    ui.DrawText(x, y, 0xffffcc88, "Race:");
    for (int i = 0; i < 4; i++) {
        uint32_t c = (i == race_) ? 0xff88ff88 : 0xffcccccc;
        ui.DrawText(x + 10 + i * 90, y + 24, c, "[%s]", RACE_NAMES[i]);
    }

    ui.DrawText(x, y + 50, 0xffffcc88, "Gender:");
    for (int i = 0; i < 2; i++) {
        uint32_t c = (i == gender_) ? 0xff88ff88 : 0xffcccccc;
        ui.DrawText(x + 10 + i * 90, y + 74, c, "[%s]", GENDER_NAMES[i]);
    }

    ui.DrawText(x, y + 100, 0xffffcc88, "Class:");
    for (int i = 0; i < 4; i++) {
        uint32_t c = (i == class_) ? 0xff88ff88 : 0xffcccccc;
        ui.DrawText(x + 10 + i * 90, y + 124, c, "[%s]", CLASS_NAMES[i]);
    }
}

void CharMakeDlg::RenderAppearance(UIRenderer& ui, float x, float y) {
    char buf[128];

    ui.DrawText(x, y, 0xffffcc88, "Hair Style");
    snprintf(buf, sizeof(buf), "Style %d", appearance_.hair_style);
    ui.DrawText(x + 10, y + 24, 0xffffffff, buf);
    ui.DrawText(x + 10, y + 44, 0xff888888, "< prev    next >");

    ui.DrawText(x + 200, y, 0xffffcc88, "Face Style");
    snprintf(buf, sizeof(buf), "Style %d", appearance_.face_style);
    ui.DrawText(x + 210, y + 24, 0xffffffff, buf);
    ui.DrawText(x + 210, y + 44, 0xff888888, "< prev    next >");

    ui.DrawText(x, y + 70, 0xffffcc88, "Colors:");
    ui.DrawText(x + 10, y + 94, 0xffffffff, "Hair:  ");
    ui.DrawRect(x + 70, y + 94, 40, 16, {
        (uint8_t)((appearance_.hair_color >> 16) & 0xFF),
        (uint8_t)((appearance_.hair_color >> 8) & 0xFF),
        (uint8_t)(appearance_.hair_color & 0xFF), 255});

    ui.DrawText(x + 120, y + 94, 0xffffffff, "Skin:  ");
    ui.DrawRect(x + 175, y + 94, 40, 16, {
        (uint8_t)((appearance_.skin_color >> 16) & 0xFF),
        (uint8_t)((appearance_.skin_color >> 8) & 0xFF),
        (uint8_t)(appearance_.skin_color & 0xFF), 255});

    ui.DrawText(x + 10, y + 120, 0xffffffff, "Eyes:  ");
    ui.DrawRect(x + 70, y + 120, 40, 16, {
        (uint8_t)((appearance_.eye_color >> 16) & 0xFF),
        (uint8_t)((appearance_.eye_color >> 8) & 0xFF),
        (uint8_t)(appearance_.eye_color & 0xFF), 255});
}

void CharMakeDlg::RenderPreview(UIRenderer& ui, float x, float y) {
    ui.DrawText(x, y, 0xffffcc88, "Character Preview");
    ui.DrawBorder(x, y + 20, 240, 320, {60, 60, 100, 200});

    char buf[256];
    snprintf(buf, sizeof(buf), "%s %s %s",
        RACE_NAMES[race_], GENDER_NAMES[gender_], CLASS_NAMES[class_]);
    ui.DrawText(x + 4, y + 350, 0xffffffff, buf);
    ui.DrawText(x + 4, y + 370, 0xffcccccc, "Level 1");
}
