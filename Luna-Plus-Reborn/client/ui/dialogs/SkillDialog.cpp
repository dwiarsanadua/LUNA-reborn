#include "SkillDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <gameobjects/ClassAdvancement.hpp>
#include <cstdio>
#include <algorithm>

void SkillDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Skill.bin.txt");

    if (!window_) {
        window_ = new Window("", 300, 40, 520, 420);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetTitleBarH(30);
    }

    window_->SetCustomBackground([this](UIRenderer& ui, float x, float y, float w, float h) {
        if (!bgfx::isValid(bg_tex_.handle)) {
            bg_tex_ = ui.LoadTexture("ui_atlas3", "b3.png");
        }
        if (bgfx::isValid(bg_tex_.handle)) {
            ui.DrawImageUV(x, y, w, h, bg_tex_.handle, 261.0f/1024.0f, 0.0f/1024.0f, 521.0f/1024.0f, 350.0f/1024.0f);
        } else {
            ui.DrawRect(x, y, w, h, {40,40,60,200});
            ui.DrawBorder(x, y, w, h, {100,100,150,200});
        }
    });

    char sp_buf[64];
    snprintf(sp_buf, sizeof(sp_buf), "Skill Points: %d", state->skill_points);
    sp_label_ = window_->AddWidget<Label>(sp_buf, 10, 4, ColorPalette::NAME_NPC);

    const char* class_names[] = {"Warrior", "Mage", "Archer"};
    char class_buf[64];
    snprintf(class_buf, sizeof(class_buf), "Class: %s (Lv.%d)", 
        class_names[state->class_id % 3], state->level);
    window_->AddWidget<Label>(class_buf, 300, 4, ColorPalette::TEXT_GOLD);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 310, 340);

    skill_grid_ = window_->AddWidget<Grid>(6, 2, 150, 28, 10, 50);
    skill_grid_->SetPadding(3);
    skill_grid_->OnSlotEvent([this, state](int row, int col, const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        auto skills = ClassAdvancement::GetSkillsForClass(state->class_id);
        int idx = row * 2 + col;
        if (idx >= 0 && idx < (int)skills.size()) {
            auto& sk = skills[idx];
            if (detail_label_) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s (Lv.%d)\n%s\nLevel: %d/%d\nRequired: Lv.%d\n%s",
                    sk.name.c_str(), sk.required_level, sk.description.c_str(),
                    sk.current_level, sk.max_level, sk.required_level,
                    sk.unlocked ? "UNLOCKED" : "Locked");
                detail_label_->SetText(buf);
            }
        }
    });

    detail_label_ = window_->AddWidget<Label>("Select a skill to learn", 330, 50, ColorPalette::TEXT_NORMAL);

    auto* learn_btn = window_->AddWidget<Button>("Learn Skill", 330, 250, 120, 24);
    learn_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    learn_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        auto skills = ClassAdvancement::GetSkillsForClass(state->class_id);
        for (auto& sk : skills) {
            if (ClassAdvancement::CanLearn(sk, state->level, state->learned_skills) && state->skill_points > 0) {
                LearnSkill(state, sk.skill_id);
                break;
            }
        }
    });

    auto* reset_btn = window_->AddWidget<Button>("Reset Skills", 330, 280, 120, 24);
    reset_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    reset_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            state->learned_skills.clear();
            state->skill_points = std::max(5, (int)state->learned_skills.size() + 5);
            state->chat_messages.push_back("Skills reset! SP refunded.");
            Refresh(state);
        }
    });

    Refresh(state);
}

void SkillDialog::LearnSkill(GameState* state, int skill_id) {
    state->learned_skills.push_back(skill_id);
    state->skill_points--;
    state->chat_messages.push_back("Learned new skill!");
    Refresh(state);
}

void SkillDialog::Refresh(GameState* state) {
    if (!skill_grid_ || !sp_label_) return;

    char sp_buf[64];
    snprintf(sp_buf, sizeof(sp_buf), "Skill Points: %d", state->skill_points);
    sp_label_->SetText(sp_buf);

    skill_grid_->ClearAll();
    auto skills = ClassAdvancement::GetSkillsForClass(state->class_id);
    int cols = skill_grid_->GetCols();
    for (int i = 0; i < (int)skills.size() && i < 12; i++) {
        int r = i / cols;
        int c = i % cols;
        auto& sk = skills[i];
        bool learned = std::find(state->learned_skills.begin(), state->learned_skills.end(), sk.skill_id) != state->learned_skills.end();
        bool can_learn = ClassAdvancement::CanLearn(sk, state->level, state->learned_skills);
        
        char buf[64];
        snprintf(buf, sizeof(buf), "%s%s",
            learned ? "✓ " : (can_learn ? "● " : "○ "),
            sk.name.c_str());
        GridSlot gs;
        gs.empty = false;
        gs.text = buf;
        gs.color = learned ? ColorPalette::NAME_NPC : (can_learn ? ColorPalette::TEXT_HIGHLIGHT : ColorPalette::TEXT_DISABLED);
        skill_grid_->SetSlot(r, c, gs);
    }

    if (detail_label_) {
        detail_label_->SetText("Select a skill from the tree.\nClick 'Learn Skill' to unlock.");
    }
}

void SkillDialog::UpdateFromState(GameState* state) {
    if (window_ && state->skill_open) Refresh(state);
}

void SkillDialog::Close() {
    window_ = nullptr;
}
