#include "SkillTrnDialog.hpp"
#include <gameobjects/ClassAdvancement.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void SkillTrnDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/SkillTrn.bin.txt");

    if (!window_) {
        spdlog::warn("SkillTrnDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("Skill Training", 260, 122, 280, 360);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetTitleBarH(28);
    }

    char sp_buf[64];
    snprintf(sp_buf, sizeof(sp_buf), "SP: %d", state->skill_points);
    sp_label_ = window_->AddWidget<Label>(sp_buf, 10, 4, 0xff88ff88);

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(gold_buf, 120, 4, 0xffffcc00);

    // Skill list
    skill_list_ = new ListBox(10, 30, 250, 180);
    skill_list_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int sel = skill_list_->GetSelected();
            if (sel >= 0) {
                selected_skill_ = sel;
                ShowSkillDetail(sel);
            }
        }
    });

    // Detail area
    detail_label_ = window_->AddWidget<Label>("Select a skill to train", 10, 218, 0xffcccccc);

    // Cost display
    cost_sp_label_ = window_->AddWidget<Label>("SP: -", 10, 250, 0xff88ff88);
    cost_gold_label_ = window_->AddWidget<Label>("Gold: -", 120, 250, 0xffffcc00);

    // Train button
    auto* train_btn = window_->AddWidget<Button>("Train Skill", 10, 280, 120, 28);
    train_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    train_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) TrainSkill(state);
    });

    // Cancel button
    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 150, 280, 80, 28);
    cancel_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) window_ = nullptr;
    });

    BuildSkillList(state);
    Refresh(state);
}

void SkillTrnDialog::BuildSkillList(GameState* state) {
    cached_skills_.clear();
    auto skills = ClassAdvancement::GetSkillsForClass(state->class_id);

    for (const auto& sk : skills) {
        SkillTrnEntry entry;
        entry.skill_id = sk.skill_id;
        entry.name = sk.name;
        entry.description = sk.description;
        entry.required_level = sk.required_level;
        entry.required_skill_id = sk.required_skill_id;
        entry.max_level = sk.max_level;
        entry.current_level = sk.current_level;

        // Check if already learned
        entry.is_learned = std::find(state->learned_skills.begin(),
            state->learned_skills.end(), sk.skill_id) != state->learned_skills.end();

        // Check prerequisite
        entry.has_prereq = true;
        if (sk.required_skill_id > 0) {
            entry.has_prereq = std::find(state->learned_skills.begin(),
                state->learned_skills.end(), sk.required_skill_id) != state->learned_skills.end();
        }

        // SP cost: base 1 + level tier
        entry.sp_cost = 1 + (sk.required_level / 10);
        entry.gold_cost = 50 + sk.required_level * 10;

        entry.can_learn = ClassAdvancement::CanLearn(sk, state->level, state->learned_skills)
                          && state->skill_points >= entry.sp_cost
                          && state->gold >= entry.gold_cost;

        cached_skills_.push_back(entry);
    }
}

void SkillTrnDialog::Refresh(GameState* state) {
    if (!skill_list_ || !sp_label_ || !gold_label_) return;

    BuildSkillList(state);

    char sp_buf[64];
    snprintf(sp_buf, sizeof(sp_buf), "SP: %d", state->skill_points);
    sp_label_->SetText(sp_buf);

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_->SetText(gold_buf);

    // Populate list
    skill_list_->Clear();
    for (const auto& entry : cached_skills_) {
        char line[128];
        const char* status;
        if (entry.is_learned) status = "LEARNED";
        else if (entry.can_learn) status = "TRAIN";
        else if (!entry.has_prereq) status = "LOCKED";
        else if (state->level < entry.required_level) status = "LVL";
        else status = "NEED SP";

        snprintf(line, sizeof(line), "[%s] %s (Lv.%d/%d)",
                 status, entry.name.c_str(), entry.current_level, entry.max_level);
        skill_list_->AddItem(line);
    }

    if (selected_skill_ >= 0 && selected_skill_ < (int)cached_skills_.size())
        ShowSkillDetail(selected_skill_);
}

void SkillTrnDialog::ShowSkillDetail(int index) {
    if (index < 0 || index >= (int)cached_skills_.size()) return;
    const auto& entry = cached_skills_[index];

    char buf[256];
    snprintf(buf, sizeof(buf), "%s (Lv.%d)\n%s\n"
             "Required: Lv.%d | Level: %d/%d\n%s",
             entry.name.c_str(), entry.required_level,
             entry.description.c_str(),
             entry.required_level, entry.current_level, entry.max_level,
             entry.is_learned ? "Learned" :
             entry.can_learn ? "Ready to train" :
             !entry.has_prereq ? "Need prerequisite" : "Not enough SP/Gold");
    if (detail_label_) detail_label_->SetText(buf);

    char sp_buf[64];
    snprintf(sp_buf, sizeof(sp_buf), "SP: %d", entry.sp_cost);
    if (cost_sp_label_) cost_sp_label_->SetText(sp_buf);

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", entry.gold_cost);
    if (cost_gold_label_) cost_gold_label_->SetText(gold_buf);
}

void SkillTrnDialog::TrainSkill(GameState* state) {
    if (selected_skill_ < 0 || selected_skill_ >= (int)cached_skills_.size()) return;
    const auto& entry = cached_skills_[selected_skill_];

    if (!entry.can_learn) {
        state->chat_messages.push_back("Cannot train this skill yet!");
        return;
    }

    if (train_cb_) {
        train_cb_(entry.skill_id, entry.sp_cost, entry.gold_cost);
    } else {
        // Offline training
        state->learned_skills.push_back(entry.skill_id);
        state->skill_points -= entry.sp_cost;
        state->gold -= entry.gold_cost;
        state->chat_messages.push_back("Skill trained: " + entry.name);
    }

    Refresh(state);
}

void SkillTrnDialog::UpdateFromState(GameState* state) {
    if (window_) Refresh(state);
}

void SkillTrnDialog::Close() {
    window_ = nullptr;
}
