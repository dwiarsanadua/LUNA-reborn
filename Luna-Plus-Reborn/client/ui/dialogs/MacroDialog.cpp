#include "MacroDialog.hpp"
#include <cstdio>

void MacroDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    macros_ = {
        {"Attack Combo", "/attack; /attack", 49},    // 1
        {"Buff Up", "/buff STR; /buff DEF", 50},     // 2
        {"Heal", "/heal; /heal", 51},                // 3
        {"Escape", "/teleport town", 52},             // 4
        {"Summon Pet", "/pet summon", 53},            // 5
    };

    window_ = wm->Open("Macro Manager", 150, 60, 480, 420);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Macro Commands", 10, 4, 0xff88ccff);

    macro_grid_ = window_->AddWidget<Grid>(5, 2, 200, 28, 10, 28);
    macro_grid_->SetPadding(3);
    macro_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = row * 2 + col;
            if (idx >= 0 && idx < (int)macros_.size()) {
                selected_ = idx;
                if (name_input_) name_input_->SetText(macros_[idx].name);
                if (cmd_input_) cmd_input_->SetText(macros_[idx].command);
                if (status_label_) {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "Selected: %s [Key: %d]", macros_[idx].name.c_str(), macros_[idx].keybind);
                    status_label_->SetText(buf);
                }
            }
        }
    });
    Refresh();

    name_input_ = window_->AddWidget<InputField>(220, 28, 240, 20);
    name_input_->SetPlaceholder("Macro name");

    cmd_input_ = window_->AddWidget<InputField>(220, 52, 240, 20);
    cmd_input_->SetPlaceholder("Commands (; separated)");

    auto* save_btn = window_->AddWidget<Button>("Save", 220, 80, 80, 22);
    save_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    save_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && selected_ >= 0 && selected_ < (int)macros_.size()) {
            if (name_input_) macros_[selected_].name = name_input_->GetText();
            if (cmd_input_) macros_[selected_].command = cmd_input_->GetText();
            Refresh();
            if (status_label_) status_label_->SetText("Macro saved!");
        }
    });

    auto* execute_btn = window_->AddWidget<Button>("Execute", 310, 80, 80, 22);
    execute_btn->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    execute_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && selected_ >= 0) {
            ExecuteMacro(state, selected_);
        }
    });

    status_label_ = window_->AddWidget<Label>("Select or create a macro", 10, 400, 0xffcccccc);

    window_->AddWidget<Label>("Tip: Macros execute multiple\ncommands with one key press!", 220, 120, 0xffaaaaaa);
}

void MacroDialog::Refresh() {
    if (!macro_grid_) return;
    macro_grid_->ClearAll();
    for (int i = 0; i < (int)macros_.size() && i < 10; i++) {
        int r = i / 2, c = i % 2;
        GridSlot gs; gs.empty = false;
        gs.text = macros_[i].name + " [" + std::to_string(macros_[i].keybind - 48) + "]";
        macro_grid_->SetSlot(r, c, gs);
    }
}

void MacroDialog::ExecuteMacro(GameState* state, int idx) {
    if (idx < 0 || idx >= (int)macros_.size()) return;
    auto& macro = macros_[idx];
    state->chat_messages.push_back("Macro: " + macro.name + " → " + macro.command);
    // Simulate macro execution
    if (macro.command.find("attack") != std::string::npos) {
        state->chat_messages.push_back("Executing attack combo!");
    }
    if (macro.command.find("heal") != std::string::npos) {
        state->hp = std::min(state->hp + 200, state->max_hp);
        state->chat_messages.push_back("Healed for 200 HP!");
    }
    if (macro.command.find("buff") != std::string::npos) {
        state->attack += 10;
        state->defense += 5;
        state->chat_messages.push_back("Buff activated! +10 ATK, +5 DEF");
    }
    if (macro.command.find("teleport") != std::string::npos) {
        state->chat_messages.push_back("Teleported to town!");
    }
    if (macro.command.find("pet") != std::string::npos) {
        state->chat_messages.push_back("Pet summoned via macro!");
    }
}

void MacroDialog::UpdateFromState(GameState*) {}
