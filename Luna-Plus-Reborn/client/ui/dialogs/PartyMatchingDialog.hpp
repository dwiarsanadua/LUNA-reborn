#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

struct PartyListing {
    uint32_t party_id = 0;
    std::string title;
    std::string leader_name;
    uint16_t min_level = 1;
    uint16_t max_level = 999;
    uint8_t member_count = 1;
    uint8_t max_members = 6;
    std::string description;
};

class PartyMatchingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetListings(const std::vector<PartyListing>& listings);
    void SetApplyCallback(std::function<void(uint32_t)> on_apply);
    void SetCreateCallback(std::function<void(const std::string&, uint16_t, uint16_t)> on_create);

private:
    void RebuildListGrid();
    void DoApply(int row);
    void DoCreate();

    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Grid* list_grid_ = nullptr;
    Label* info_label_ = nullptr;
    std::vector<PartyListing> listings_;
    std::function<void(uint32_t)> on_apply_;
    std::function<void(const std::string&, uint16_t, uint16_t)> on_create_;
};
