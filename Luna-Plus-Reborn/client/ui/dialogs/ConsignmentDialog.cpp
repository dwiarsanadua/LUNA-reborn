#include "ConsignmentDialog.hpp"
#include <cstdio>
#include <algorithm>

void ConsignmentDialog::Open(GameState* state, WindowManager* wm, ConsignmentSystem* cs) {
    consignment_ = cs;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Consignment.bin.txt",
        "Auction House", 80, 40, 600, 480);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(gold_buf, 10, 4, 0xffffcc00);

    auto* search_lbl = window_->AddWidget<Label>("Search:", 10, 28, 0xffcccccc);
    (void)search_lbl;
    search_input_ = window_->AddWidget<InputField>(70, 28, 180, 20);
    search_input_->SetPlaceholder("item name...");
    auto* search_btn = window_->AddWidget<Button>("Search", 260, 28, 70, 20);
    search_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    search_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) RefreshBrowse(state);
    });
    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 340, 28, 70, 20);
    refresh_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            RefreshBrowse(state);
            RefreshMyListings(state);
            RefreshMyBids(state);
            RefreshStall(state);
        }
    });

    tabs_ = window_->AddWidget<TabPanel>(10, 52, 580, 340);
    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            browse_grid_->SetVisible(e.int_value == 0);
            my_grid_->SetVisible(e.int_value == 1);
            bid_grid_->SetVisible(e.int_value == 2);
            if (stall_list_) stall_list_->SetVisible(e.int_value == 3);
        }
    });

    browse_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    browse_grid_->SetPadding(3);
    browse_grid_->OnSlotEvent([this, state](int row, int col, const UIEvent& e) {
        (void)col;
        if (e.type == UIEvent::Click) {
            selected_row_ = row;
            int idx = row * 4 + col;
            if (idx >= 0 && idx < (int)cached_listings_.size()) {
                auto& listing = cached_listings_[idx];
                char buf[256];
                snprintf(buf, sizeof(buf), "Selected: %s (x%d) | Bid: %dg | Buyout: %dg",
                    listing.item_name.c_str(), listing.item_count,
                    listing.bid_price, listing.buyout_price);
                if (status_label_) status_label_->SetText(buf);
            }
        }
    });

    my_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    my_grid_->SetPadding(3);
    my_grid_->SetVisible(false);

    bid_grid_ = window_->AddWidget<Grid>(6, 4, 135, 30, 14, 80);
    bid_grid_->SetPadding(3);
    bid_grid_->SetVisible(false);

    stall_list_ = new ListBox(0, 0, 560, 300);
    stall_list_->AddItem("Street stalls (online sync)");
    tabs_->AddTab("Stall", stall_list_);
    stall_list_->SetVisible(false);

    auto* buyout_btn = window_->AddWidget<Button>("Buyout", 10, 410, 80, 24);
    buyout_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    buyout_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() == 3 && net_cb_.enabled && net_cb_.on_stall_buy) {
            net_cb_.on_stall_buy(static_cast<uint32_t>(selected_stall_owner_), selected_stall_slot_);
            return;
        }
        if (tabs_->GetActive() != 0) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_listings_.size())
            DoBuyout(state, cached_listings_[idx]);
    });

    auto* bid_btn = window_->AddWidget<Button>("Bid", 100, 410, 70, 24);
    bid_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click || tabs_->GetActive() != 0) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_listings_.size())
            DoBid(state, cached_listings_[idx]);
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 200, 410, 80, 24);
    cancel_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    cancel_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() == 3 && net_cb_.enabled && net_cb_.on_stall_close) {
            net_cb_.on_stall_close();
            return;
        }
        if (tabs_->GetActive() != 1) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_my_listings_.size())
            DoCancelListing(state, cached_my_listings_[idx].id);
    });

    auto* list_btn = window_->AddWidget<Button>("List Item", 440, 410, 90, 24);
    list_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    list_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() == 3 && net_cb_.enabled && net_cb_.on_stall_open) {
            net_cb_.on_stall_open("My Stall");
            return;
        }
        DoListItem(state);
    });

    auto* stall_add_btn = window_->AddWidget<Button>("Stall Add", 340, 410, 80, 24);
    stall_add_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click || tabs_->GetActive() != 3) return;
        if (net_cb_.enabled && net_cb_.on_stall_add && !state->inventory.empty())
            net_cb_.on_stall_add(static_cast<uint8_t>(state->inventory[0].slot), 50);
    });

    status_label_ = window_->AddWidget<Label>("Select a listing or list an item", 10, 442, 0xffaaaaaa);
    RefreshBrowse(state);
    RefreshMyListings(state);
    RefreshMyBids(state);
    RefreshStall(state);
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
        snprintf(buf, sizeof(buf), "%s", l.item_name.c_str());
        GridSlot gs;
        gs.empty = false;
        gs.text = buf;
        gs.count = l.item_count;
        gs.userdata = (int)(l.id & 0xFFFFFFFF);
        grid->SetSlot(r, c, gs);
    }
}

void ConsignmentDialog::RefreshBrowse(GameState* state) {
    if (!browse_grid_) return;
    if (net_cb_.enabled && net_cb_.on_search) {
        std::string q = search_input_ ? search_input_->GetText() : "";
        net_cb_.on_search(q);
        cached_listings_ = state->consignment_browse;
    } else if (consignment_) {
        std::string q = search_input_ ? search_input_->GetText() : "";
        cached_listings_ = consignment_->SearchListings(q);
    }
    PopulateGrid(browse_grid_, cached_listings_);
    if (status_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d listings found", (int)cached_listings_.size());
        status_label_->SetText(buf);
    }
}

void ConsignmentDialog::RefreshMyListings(GameState* state) {
    if (!my_grid_) return;
    if (net_cb_.enabled && net_cb_.on_refresh) {
        net_cb_.on_refresh(true, false);
        cached_my_listings_ = state->consignment_mine;
    } else if (consignment_) {
        cached_my_listings_ = consignment_->GetMyListings(1);
    }
    PopulateGrid(my_grid_, cached_my_listings_);
}

void ConsignmentDialog::RefreshMyBids(GameState* state) {
    if (!bid_grid_) return;
    if (net_cb_.enabled && net_cb_.on_refresh) {
        net_cb_.on_refresh(false, true);
        cached_my_bids_ = state->consignment_bids;
    } else if (consignment_) {
        cached_my_bids_ = consignment_->GetMyBids(1);
    }
    PopulateGrid(bid_grid_, cached_my_bids_);
}

void ConsignmentDialog::RefreshStall(GameState* state) {
    if (!stall_list_ || !state) return;
    if (net_cb_.enabled && net_cb_.on_stall_list)
        net_cb_.on_stall_list();
    stall_list_->Clear();
    if (state->my_stall.open) {
        stall_list_->AddItem("[My Stall] " + state->my_stall.title);
        for (const auto& it : state->my_stall.items) {
            char line[128];
            snprintf(line, sizeof(line), "  slot %u: %s x%u @ %ug",
                it.slot, it.item_name.c_str(), it.count, it.price);
            stall_list_->AddItem(line);
        }
    }
    for (const auto& stall : state->nearby_stalls) {
        if (!stall.open) continue;
        stall_list_->AddItem("[" + stall.owner_name + "] " + stall.title);
        for (const auto& it : stall.items) {
            char line[128];
            snprintf(line, sizeof(line), "  slot %u: %s x%u @ %ug (owner %u)",
                it.slot, it.item_name.c_str(), it.count, it.price, stall.owner_id);
            stall_list_->AddItem(line);
            selected_stall_owner_ = static_cast<int>(stall.owner_id);
            selected_stall_slot_ = it.slot;
        }
    }
    if (stall_list_->GetCount() == 0)
        stall_list_->AddItem("(No stalls — List Item opens yours)");
}

void ConsignmentDialog::DoBuyout(GameState* state, const AuctionListing& listing) {
    if (listing.sold || !listing.active) {
        if (status_label_) status_label_->SetText("Already sold!");
        return;
    }
    if (net_cb_.enabled && net_cb_.on_trade) {
        net_cb_.on_trade(listing.id, true, listing.buyout_price);
        return;
    }
    if (!consignment_) return;
    if ((uint32_t)state->gold < listing.buyout_price) {
        if (status_label_) status_label_->SetText("Not enough gold!");
        return;
    }
    if (consignment_->Buyout(listing.id, 1)) {
        state->gold -= (int)listing.buyout_price;
        if (status_label_) status_label_->SetText("Purchased!");
        RefreshBrowse(state);
        RefreshMyBids(state);
    }
}

void ConsignmentDialog::DoBid(GameState* state, const AuctionListing& listing) {
    if (listing.sold || !listing.active) {
        if (status_label_) status_label_->SetText("Already sold!");
        return;
    }
    uint32_t min_bid = listing.current_bid > 0 ? listing.current_bid + 1 : listing.bid_price;
    if (net_cb_.enabled && net_cb_.on_trade) {
        net_cb_.on_trade(listing.id, false, min_bid);
        return;
    }
    if (!consignment_) return;
    if ((uint32_t)state->gold < min_bid) {
        if (status_label_) status_label_->SetText("Not enough gold to bid!");
        return;
    }
    if (consignment_->PlaceBid(listing.id, 1, min_bid)) {
        state->gold -= (int)min_bid;
        if (status_label_) status_label_->SetText("Bid placed!");
        RefreshBrowse(state);
        RefreshMyBids(state);
    }
}

void ConsignmentDialog::DoListItem(GameState* state) {
    if (state->inventory.empty()) {
        if (status_label_) status_label_->SetText("No items to list!");
        return;
    }
    auto& item = state->inventory[0];
    uint32_t price = 50;
    uint32_t buyout = 100;
    if (net_cb_.enabled && net_cb_.on_list) {
        net_cb_.on_list(static_cast<uint8_t>(item.slot),
            static_cast<uint16_t>(item.count), price, buyout);
        return;
    }
    if (!consignment_) return;
    consignment_->ListItem(1, item.id, item.count, price, buyout);
    if (status_label_) status_label_->SetText("Listed item on auction house.");
    RefreshBrowse(state);
    RefreshMyListings(state);
}

void ConsignmentDialog::DoCancelListing(GameState* state, uint64_t listing_id) {
    if (net_cb_.enabled && net_cb_.on_cancel) {
        net_cb_.on_cancel(listing_id);
        return;
    }
    if (!consignment_) return;
    if (consignment_->CancelListing(listing_id)) {
        if (status_label_) status_label_->SetText("Listing cancelled.");
        RefreshBrowse(state);
        RefreshMyListings(state);
    }
}

void ConsignmentDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
    if (net_cb_.enabled) {
        cached_listings_ = state->consignment_browse;
        cached_my_listings_ = state->consignment_mine;
        cached_my_bids_ = state->consignment_bids;
        if (browse_grid_) PopulateGrid(browse_grid_, cached_listings_);
        if (my_grid_) PopulateGrid(my_grid_, cached_my_listings_);
        if (bid_grid_) PopulateGrid(bid_grid_, cached_my_bids_);
        RefreshStall(state);
    }
}
