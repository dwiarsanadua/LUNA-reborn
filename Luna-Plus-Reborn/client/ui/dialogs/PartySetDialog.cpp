#include "PartySetDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/UiScriptParser.hpp>
#include <spdlog/spdlog.h>

void PartySetDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/PartySet.bin.txt");

    if (!window_) {
        spdlog::warn("PartySetDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("PartySet", 0, 71, 212, 52);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetDrawChrome(true);
        window_->SetTitleBarH(0);
        window_->SetRect(0, 71, 212, 52);

        btn_add_member_ = window_->AddWidget<Button>("", 7, 6, 32, 19);
        btn_add_member_->SetID("PA_ADDPARTYMEMBERBTN");
        btn_add_member_->SetColors({50,70,120,220}, {80,110,160,220}, {30,50,90,220});

        btn_force_secede_ = window_->AddWidget<Button>("", 44, 6, 32, 19);
        btn_force_secede_->SetID("PA_FORCEDSECEDEBTN");

        btn_transfer_ = window_->AddWidget<Button>("", 81, 6, 32, 19);
        btn_transfer_->SetID("PA_TRANSFERBTN");

        btn_breakup_ = window_->AddWidget<Button>("", 118, 6, 32, 19);
        btn_breakup_->SetID("PA_BREAKUPBTN");

        btn_war_suggest_ = window_->AddWidget<Button>("", 112, 6, 27, 19);
        btn_war_suggest_->SetID("PA_WARSUGGESTBTN");

        btn_secede_ = window_->AddWidget<Button>("", 155, 6, 32, 19);
        btn_secede_->SetID("PA_SECEDEBTN");

        chk_damage_ = window_->AddWidget<CheckBox>("DMG", 170, 14);
        chk_damage_->SetID("PA_OP_DAMAGE");

        chk_random_ = window_->AddWidget<CheckBox>("RND", 185, 14);
        chk_random_->SetID("PA_OP_RANDOM");

        chk_sequence_ = window_->AddWidget<CheckBox>("SEQ", 200, 14);
        chk_sequence_->SetID("PA_OP_SEQUENCE");

        chk_skill_icon_ = window_->AddWidget<CheckBox>("", 200, 8);
        chk_skill_icon_->SetID("PA_SKILL_ICON_BUTTON");
        chk_skill_icon_->SetChecked(true);

        cmb_distribute_ = window_->AddWidget<ComboBox>(122, 28, 80, 19);
        cmb_distribute_->SetID("PA_DISTRIBUTE");
        cmb_distribute_->AddItem("Equal");
        cmb_distribute_->AddItem("Random");
        cmb_distribute_->AddItem("Sequential");

        cmb_dice_grade_ = window_->AddWidget<ComboBox>(7, 28, 110, 19);
        cmb_dice_grade_->SetID("PA_DICEGRADE");
        cmb_dice_grade_->AddItem("Grade A");
        cmb_dice_grade_->AddItem("Grade B");
        cmb_dice_grade_->AddItem("Grade C");
        cmb_dice_grade_->AddItem("Grade D");
        cmb_dice_grade_->AddItem("Grade E");
    }

    if (window_) {
        btn_add_member_ = window_->FindWidget<Button>("PA_ADDPARTYMEMBERBTN");
        btn_force_secede_ = window_->FindWidget<Button>("PA_FORCEDSECEDEBTN");
        btn_transfer_ = window_->FindWidget<Button>("PA_TRANSFERBTN");
        btn_breakup_ = window_->FindWidget<Button>("PA_BREAKUPBTN");
        btn_war_suggest_ = window_->FindWidget<Button>("PA_WARSUGGESTBTN");
        btn_secede_ = window_->FindWidget<Button>("PA_SECEDEBTN");
        chk_damage_ = window_->FindWidget<CheckBox>("PA_OP_DAMAGE");
        chk_random_ = window_->FindWidget<CheckBox>("PA_OP_RANDOM");
        chk_sequence_ = window_->FindWidget<CheckBox>("PA_OP_SEQUENCE");
        chk_skill_icon_ = window_->FindWidget<CheckBox>("PA_SKILL_ICON_BUTTON");
        cmb_distribute_ = window_->FindWidget<ComboBox>("PA_DISTRIBUTE");
        cmb_dice_grade_ = window_->FindWidget<ComboBox>("PA_DICEGRADE");
    }

    SetPartyRole(false, false);
}

void PartySetDialog::SetPartyRole(bool is_leader, bool has_party) {
    if (!window_) return;
    if (btn_add_member_) btn_add_member_->SetVisible(is_leader);
    if (btn_force_secede_) btn_force_secede_->SetVisible(is_leader && has_party);
    if (btn_transfer_) btn_transfer_->SetVisible(is_leader && has_party);
    if (btn_breakup_) btn_breakup_->SetVisible(is_leader && has_party);
    if (btn_war_suggest_) btn_war_suggest_->SetVisible(is_leader && has_party);
    if (btn_secede_) btn_secede_->SetVisible(has_party);
}

void PartySetDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    bool has_party = !state->party_members.empty();
    bool is_leader = has_party && state->party_members[0].is_leader;
    SetPartyRole(is_leader, has_party);
}
