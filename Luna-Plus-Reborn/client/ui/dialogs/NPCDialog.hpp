#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <vector>
#include <string>

struct NPCDialogNode {
    std::string text;
    struct Option {
        std::string text;
        int next_node = -1; // -1 to close, -2 for shop, -3 for quest
    };
    std::vector<Option> options;
};

class NPCDialog {
public:
    NPCDialog();
    Window* GetWindow() { return window_; }
    void Open(GameState* state, uint32_t npc_id, const std::string& npc_name, WindowManager* wm = nullptr);
    void Close();
    void SetNode(int node_id);
    void UpdateFromState(GameState* state);

private:
    void BuildDialogTree();

    Window* window_ = nullptr;
    Label* text_label_ = nullptr;
    std::vector<Button*> option_btns_;
    TextureInfo bg_tex_;

    uint32_t current_npc_id_ = 0;
    std::string current_npc_name_;
    std::vector<NPCDialogNode> current_tree_;
    GameState* state_ = nullptr;
};
