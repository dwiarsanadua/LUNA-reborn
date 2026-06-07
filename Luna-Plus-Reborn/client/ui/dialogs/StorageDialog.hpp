#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <vector>
#include <string>

struct StoredItem {
    uint32_t id = 0;
    std::string name;
    int count = 0;
    int slot = 0;
};

class StorageDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
private:
    Window* window_ = nullptr;
    Grid* storage_grid_ = nullptr;
    Label* gold_label_ = nullptr;
    std::vector<StoredItem> items_;
};
