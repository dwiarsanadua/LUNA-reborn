#include "ConsignmentDialog.hpp"
#include <cstdio>
#include <algorithm>

void ConsignmentDialog::Open(GameState* state, WindowManager* wm, ConsignmentSystem* cs) {
    consignment_ = cs;
    window_ = wm->Open("Auction House", 80, 40, 600, 480);
    if (window_) { /* already exists */ }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    // Gold display at top
    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(gold_buf, 10, 4, 0xffffcc00);

    // Search area (Browse tab)
    auto* search_lbl = window_->AddWidget<Label>("Search:", 10, 28, 0xffcccccc);
    (void)search_lbl;
    search_input_ = window_->AddWidget<InputField>(70, 28, 180, 20);
    search_input_->SetPlaceholder("item name...");
    auto* search_btn = window_->AddWidget<Button>("Search", 260, 28, 70, 20);
    search_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    search_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) RefreshBrowse();
    });
    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 340, 28, 70, 20);
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) { RefreshBrowse(); RefreshMyListings(); RefreshMyBids(); }
    });

    // TabPanel
    tabs_ = window_->AddWidget<TabPanel>(10, 52, 580, 340);
    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            browse_grid_->SetVisible(e.int_value == 0);
            my_grid_->SetVisible(e.int_value == 1);
            bid_grid_->SetVisible(e.int_value == 2);
        }
    });

    // Tab 0: Browse grid
    browse_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    browse_grid_->SetPadding(3);
    browse_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        (void)col;
        if (e.type == UIEvent::Click) {
            int idx = row * 4 + col;
            if (idx >= 0 && idx < (int)cached_listings_.size()) {
                selected_row_ = row;
                auto& listing = cached_listings_[idx];
                char buf[256];
                snprintf(buf, sizeof(buf), "Selected: %s (x%d) | Enchant +%d | Bid: %dg | Buyout: %dg",
                    listing.item_name.c_str(), listing.item_count, listing.enchant_level,
                    listing.bid_price, listing.buyout_price);
                if (status_label_) status_label_->SetText(buf);
            }
        }
    });

    // Tab 1: My listings grid
    my_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    my_grid_->SetPadding(3);
    my_grid_->SetVisible(false);

    // Tab 2: My bids grid
    bid_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    bid_grid_->SetPadding(3);
    bid_grid_->SetVisible(false);

    // Bottom action buttons
    auto* buyout_btn = window_->AddWidget<Button>("Buyout", 10, 410, 80, 24);
    buyout_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    buyout_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() != 0) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_listings_.size()) DoBuyout(state, cached_listings_[idx]);
    });

    auto* bid_btn = window_->AddWidget<Button>("Bid", 100, 410, 70, 24);
    bid_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() != 0) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_listings_.size()) DoBid(state, cached_listings_[idx]);
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 200, 410, 80, 24);
    cancel_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    cancel_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() != 1) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_my_listings_.size())
            DoCancelListing(state, cached_my_listings_[idx].id);
    });

    auto* list_btn = window_->AddWidget<Button>("List Item", 440, 410, 90, 24);
    list_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    list_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoListItem(state);
    });

    // Status / info label
    status_label_ = window_->AddWidget<Label>("Select a listing or list an item", 10, 442, 0xffaaaaaa);

    // Initial data load
    RefreshBrowse();
    RefreshMyListings();
    RefreshMyBids();
}

void ConsignmentDialog::PopulateGrid(Grid* grid, const std::vector<AuctionListing>& listings) {
    grid->ClearAll();
    int rows = grid->GetRows();
    int cols = grid->GetCols();
    for (int i = 0; i < (int)listings.size() && i < rows * cols; i++) {
        int r = i / cols;
        int c = i % cols;
        auto& l = listings[i];
        char buf[96];
        if (l.enchant_level > 0)
            snprintf(buf, sizeof(buf), "%s +%d", l.item_name.c_str(), l.enchant_level);
        else
            snprintf(buf, sizeof(buf), "%s", l.item_name.c_str());
        GridSlot gs;
        gs.empty = false;
        gs.text = buf;
        gs.count = l.item_count;
        gs.userdata = (int)(l.id & 0xFFFFFFFF);
        grid->SetSlot(r, c, gs);
    }
}

void ConsignmentDialog::RefreshBrowse() {
    if (!consignment_ || !browse_grid_) return;
    std::string q = search_input_ ? search_input_->GetText() : "";
    cached_listings_ = consignment_->SearchListings(q);
    PopulateGrid(browse_grid_, cached_listings_);
    if (status_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d listings found", (int)cached_listings_.size());
        status_label_->SetText(buf);
    }
}

void ConsignmentDialog::RefreshMyListings() {
    if (!consignment_ || !my_grid_) return;
    cached_my_listings_ = consignment_->GetMyListings(1);
    PopulateGrid(my_grid_, cached_my_listings_);
}

void ConsignmentDialog::RefreshMyBids() {
    if (!consignment_ || !bid_grid_) return;
    cached_my_bids_ = consignment_->GetMyBids(1);
    PopulateGrid(bid_grid_, cached_my_bids_);
}

void ConsignmentDialog::DoBuyout(GameState* state, const AuctionListing& listing) {
    if (!consignment_ || listing.sold || !listing.active) {
        if (status_label_) status_label_->SetText("Already sold!");
        return;
    }
    if ((uint32_t)state->gold < listing.buyout_price) {
        if (status_label_) status_label_->SetText("Not enough gold!");
        return;
    }
    if (consignment_->Buyout(listing.id, 1)) {
        state->gold -= (int)listing.buyout_price;
        char buf[128];
        snprintf(buf, sizeof(buf), "Purchased %s for %dg!", listing.item_name.c_str(), listing.buyout_price);
        state->chat_messages.push_back(buf);
        if (status_label_) status_label_->SetText(buf);
        if (gold_label_) {
            char gb[64];
            snprintf(gb, sizeof(gb), "Gold: %d", state->gold);
            gold_label_->SetText(gb);
        }
        RefreshBrowse();
        RefreshMyBids();
    }
}

void ConsignmentDialog::DoBid(GameState* state, const AuctionListing& listing) {
    if (!consignment_ || listing.sold || !listing.active) {
        if (status_label_) status_label_->SetText("Already sold!");
        return;
    }
    uint32_t min_bid = listing.current_bid > 0 ? listing.current_bid + 1 : listing.bid_price;
    if ((uint32_t)state->gold < min_bid) {
        if (status_label_) status_label_->SetText("Not enough gold to bid!");
        return;
    }
    if (consignment_->PlaceBid(listing.id, 1, min_bid)) {
        state->gold -= (int)min_bid;
        char buf[128];
        snprintf(buf, sizeof(buf), "Bid %dg on %s!", min_bid, listing.item_name.c_str());
        state->chat_messages.push_back(buf);
        if (status_label_) status_label_->SetText(buf);
        if (gold_label_) {
            char gb[64];
            snprintf(gb, sizeof(gb), "Gold: %d", state->gold);
            gold_label_->SetText(gb);
        }
        RefreshBrowse();
        RefreshMyBids();
    }
}

void ConsignmentDialog::DoListItem(GameState* state) {
    if (!consignment_) return;
    if (state->inventory.empty()) {
        if (status_label_) status_label_->SetText("No items to list!");
        return;
    }
    auto& item = state->inventory[0];
    uint32_t price = 50;
    uint32_t buyout = 100;
    consignment_->ListItem(1, item.id, item.count, price, buyout);
    char buf[128];
    snprintf(buf, sizeof(buf), "Listed %s x%d (bid: %dg / buyout: %dg)", item.name.c_str(), item.count, price, buyout);
    state->chat_messages.push_back(buf);
    if (status_label_) status_label_->SetText(buf);
    RefreshBrowse();
    RefreshMyListings();
}

void ConsignmentDialog::DoCancelListing(GameState* state, uint64_t listing_id) {
    if (!consignment_) return;
    if (consignment_->CancelListing(listing_id)) {
        state->chat_messages.push_back("Listing cancelled.");
        if (status_label_) status_label_->SetText("Listing cancelled.");
        RefreshBrowse();
        RefreshMyListings();
    }
}

void ConsignmentDialog::UpdateFromState(GameState* state) {
    if (!window_) return;
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
}
