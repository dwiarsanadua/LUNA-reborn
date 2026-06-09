#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/ComboBox.hpp>
#include <gameobjects/ConsignmentSystem.hpp>
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

struct Category1Entry {
    int id;
    std::string name;
};

struct Category2Entry {
    int cat1_id;
    int cat2_id;
    std::string name;
    int item_type_detail;
};

struct ConsignmentNetworkCallbacks {
    bool enabled = false;
    std::function<void(const std::string&)> on_search;
    std::function<void(bool mine, bool bids)> on_refresh;
    std::function<void(uint64_t id, bool buyout, uint32_t amount)> on_trade;
    std::function<void(uint8_t inv_slot, uint16_t count, uint32_t bid, uint32_t buyout)> on_list;
    std::function<void(uint64_t id)> on_cancel;
    std::function<void(const std::string& title)> on_stall_open;
    std::function<void(uint8_t inv_slot, uint32_t price)> on_stall_add;
    std::function<void(uint32_t owner_id, uint8_t stall_slot)> on_stall_buy;
    std::function<void()> on_stall_close;
    std::function<void()> on_stall_list;
};

class ConsignmentDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, ConsignmentSystem* consignment);
    void SetNetworkCallbacks(const ConsignmentNetworkCallbacks& cb) { net_cb_ = cb; }
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Grid* browse_grid_ = nullptr;
    Grid* my_grid_ = nullptr;
    Grid* bid_grid_ = nullptr;
    ListBox* stall_list_ = nullptr;
    InputField* search_input_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* status_label_ = nullptr;
    ComboBox* cat1_combo_ = nullptr;
    ComboBox* cat2_combo_ = nullptr;
    ConsignmentSystem* consignment_ = nullptr;
    ConsignmentNetworkCallbacks net_cb_;
    std::vector<AuctionListing> cached_listings_;
    std::vector<AuctionListing> cached_my_listings_;
    std::vector<AuctionListing> cached_my_bids_;
    int selected_row_ = -1;
    int selected_stall_owner_ = 0;
    uint8_t selected_stall_slot_ = 0;

    std::vector<Category1Entry> cat1_list_;
    std::vector<Category2Entry> cat2_list_;
    std::vector<int> cat2_visible_indices_;

    void LoadCategories(const std::string& path);
    void RefreshCat2Combo(int cat1_id);
    void RefreshBrowse(GameState* state);
    void RefreshMyListings(GameState* state);
    void RefreshMyBids(GameState* state);
    void RefreshStall(GameState* state);
    void DoBuyout(GameState* state, const AuctionListing& listing);
    void DoBid(GameState* state, const AuctionListing& listing);
    void DoListItem(GameState* state);
    void DoCancelListing(GameState* state, uint64_t listing_id);
    void PopulateGrid(Grid* grid, const std::vector<AuctionListing>& listings);
};
