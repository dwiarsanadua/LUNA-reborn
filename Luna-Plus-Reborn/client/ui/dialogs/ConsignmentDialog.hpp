#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <gameobjects/ConsignmentSystem.hpp>
#include <cstdint>

class ConsignmentDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, ConsignmentSystem* consignment);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Grid* browse_grid_ = nullptr;
    Grid* my_grid_ = nullptr;
    Grid* bid_grid_ = nullptr;
    InputField* search_input_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* status_label_ = nullptr;
    ConsignmentSystem* consignment_ = nullptr;
    std::vector<AuctionListing> cached_listings_;
    std::vector<AuctionListing> cached_my_listings_;
    std::vector<AuctionListing> cached_my_bids_;
    int selected_row_ = -1;

    void RefreshBrowse();
    void RefreshMyListings();
    void RefreshMyBids();
    void DoBuyout(GameState* state, const AuctionListing& listing);
    void DoBid(GameState* state, const AuctionListing& listing);
    void DoListItem(GameState* state);
    void DoCancelListing(GameState* state, uint64_t listing_id);
    void PopulateGrid(Grid* grid, const std::vector<AuctionListing>& listings);
};
