#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <functional>

class InventoryDialog {
public:
    InventoryDialog();
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm = nullptr);
    void Close();
    void UpdateFromState(GameState* state, UIRenderer& ui);
    void Refresh(GameState* state);
    void DrawEquipmentPreview(UIRenderer& ui, GameState* state, float x, float y);


private:
    Window* window_ = nullptr;
    Grid* inv_grid_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* tooltip_label_ = nullptr;
    TextureInfo bg_tex_;
};
