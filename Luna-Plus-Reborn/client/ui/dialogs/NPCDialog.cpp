#include "NPCDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>

NPCDialog::NPCDialog() {}

void NPCDialog::Open(GameState* state, uint32_t npc_id, const std::string& npc_name, WindowManager* wm) {
    if (window_) return;
    state_ = state;
    current_npc_id_ = npc_id;
    current_npc_name_ = npc_name;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/NpcImage.bin.txt");
    if (!window_) {
        window_ = new Window("", 300, 300, 680, 240);
        window_->SetMovable(true);
        window_->SetClosable(true);
        window_->SetTitleBarH(0);
    }

    window_->SetCustomBackground([this](UIRenderer& ui, float x, float y, float w, float h) {
        if (!bgfx::isValid(bg_tex_.handle)) {
            bg_tex_ = ui.LoadTexture("ui_npc_dialog", "ui_npctopbottom.png");
        }
        if (bgfx::isValid(bg_tex_.handle)) {
            ui.DrawImage(x, y, w, h, bg_tex_.handle);
        } else {
            ui.DrawRect(x, y, w, h, {20, 20, 30, 230});
            ui.DrawBorder(x, y, w, h, {100, 100, 150, 200});
        }
    });

    text_label_ = window_->AddWidget<Label>("", 120, 40, ColorPalette::TEXT_NORMAL);
    
    BuildDialogTree();
    SetNode(0);
}

void NPCDialog::SetNode(int node_id) {
    if (node_id < 0 || node_id >= (int)current_tree_.size()) {
        Close();
        return;
    }

    const auto& node = current_tree_[node_id];
    text_label_->SetText(node.text);

    // Clear old options
    for (auto* btn : option_btns_) {
        btn->SetVisible(false);
        // Note: Actual removal would be better but requires more engine work
    }
    option_btns_.clear();

    float bx = 120, by = 120;
    for (size_t i = 0; i < node.options.size(); i++) {
        const auto& opt = node.options[i];
        auto* btn = window_->AddWidget<Button>(opt.text, bx, by + i * 30, 400, 25);
        btn->OnEvent([this, opt](const UIEvent& e) {
            if (e.type == UIEvent::Click) {
                if (opt.next_node == -1) Close();
                else if (opt.next_node == -2) { /* Open Shop */ Close(); }
                else if (opt.next_node == -3) { /* Open Quest */ Close(); }
                else SetNode(opt.next_node);
            }
        });
        option_btns_.push_back(btn);
    }
}

void NPCDialog::BuildDialogTree() {
    current_tree_.clear();
    // Default tree for any NPC (later load from DB/Script)
    NPCDialogNode root;
    root.text = "Hello! I am " + current_npc_name_ + ". How can I help you today?";
    root.options.push_back({"Trade", -2});
    root.options.push_back({"Quest", -3});
    root.options.push_back({"Goodbye", -1});
    current_tree_.push_back(root);
}

void NPCDialog::Close() {
    window_ = nullptr;
    text_label_ = nullptr;
    option_btns_.clear();
}

void NPCDialog::UpdateFromState(GameState* state) {
    (void)state;
}
