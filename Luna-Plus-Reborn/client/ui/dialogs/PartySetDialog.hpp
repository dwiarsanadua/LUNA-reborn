#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/CheckBox.hpp>
#include <ui/widgets/ComboBox.hpp>

class PartySetDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetPartyRole(bool is_leader, bool has_party);

private:
    Window* window_ = nullptr;
    Button* btn_add_member_ = nullptr;
    Button* btn_force_secede_ = nullptr;
    Button* btn_transfer_ = nullptr;
    Button* btn_breakup_ = nullptr;
    Button* btn_war_suggest_ = nullptr;
    Button* btn_secede_ = nullptr;
    CheckBox* chk_damage_ = nullptr;
    CheckBox* chk_random_ = nullptr;
    CheckBox* chk_sequence_ = nullptr;
    CheckBox* chk_skill_icon_ = nullptr;
    ComboBox* cmb_distribute_ = nullptr;
    ComboBox* cmb_dice_grade_ = nullptr;
};
