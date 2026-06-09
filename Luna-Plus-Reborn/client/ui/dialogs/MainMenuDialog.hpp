#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <functional>
#include <string>
#include <vector>

struct MenuButtonDef {
    std::string label;
    std::string tooltip;
    bool* toggle_state = nullptr;
    std::function<void()> on_click;
    uint32_t color_normal = 0;
    uint32_t color_active = 0;
    uint32_t color_hover = 0;
};

class MainMenuDialog {
public:
    MainMenuDialog();

    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return window_ != nullptr; }

    void RegisterDefaultButtons(GameState* state,
                                std::function<void()> on_inv,
                                std::function<void()> on_skill,
                                std::function<void()> on_char,
                                std::function<void()> on_quest,
                                std::function<void()> on_party,
                                std::function<void()> on_guild,
                                std::function<void()> on_friend,
                                std::function<void()> on_options,
                                std::function<void()> on_help);

    void AddCustomButton(const MenuButtonDef& def);
    void Update(GameState* state);
    void Render(UIRenderer& ui);

private:
    Window* window_ = nullptr;
    std::vector<MenuButtonDef> buttons_;
    int hovered_idx_ = -1;
    float bar_x_ = 0, bar_y_ = 0, bar_w_ = 0, bar_h_ = 32;
    bool registered_ = false;

    void RenderButton(UIRenderer& ui, const MenuButtonDef& def, float x, float y, float w, float h, bool hovered);
};
