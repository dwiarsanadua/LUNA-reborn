#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <vector>
#include <string>
#include <array>
#include <cstdint>

struct GuildWareItem {
    uint32_t id = 0;
    std::string name;
    int count = 0;
    int slot = 0;
};

class GuildWarehouseDialog {
public:
    static constexpr int TABS = 2;
    static constexpr int SLOTS_PER_TAB = 36;
    static constexpr int TOTAL_SLOTS = TABS * SLOTS_PER_TAB;

    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close();
    void UpdateFromState(GameState* state);
    void SetItems(int tab, const std::vector<GuildWareItem>& items);
    void SetGold(uint32_t gold);
    void SetWarehouseRank(uint8_t rank);
    void SetMinRank(uint8_t rank);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    std::array<Grid*, TABS> grids_{};
    Label* gold_label_ = nullptr;
    Label* rank_label_ = nullptr;
    std::array<std::vector<GuildWareItem>, TABS> items_;
    uint32_t gold_ = 0;
    uint8_t min_rank_ = 1;
};
