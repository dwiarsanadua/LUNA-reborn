#include "ConsignmentDialog.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <sstream>

void ConsignmentDialog::LoadCategories(const std::string& path) {
    cat1_list_.clear();
    cat2_list_.clear();

    std::string resolved = VFS::Find(path);
    if (resolved.empty()) resolved = path;

    std::ifstream in(resolved);
    if (!in.is_open()) {
        spdlog::warn("ConsignmentCategory: cannot open '{}'", resolved);
        return;
    }

    std::string line;
    int section = 0;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '/' || line[0] == '{' || line[0] == '}') continue;

        if (line.find("$Category1") != std::string::npos) { section = 1; continue; }
        if (line.find("$Category2") != std::string::npos) { section = 2; continue; }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "#AddList" && section == 1) {
            int id; std::string name;
            if (iss >> id) {
                std::getline(iss, name);
                name.erase(0, name.find_first_not_of(" \t"));
                Category1Entry e;
                e.id = id;
                e.name = name;
                cat1_list_.push_back(e);
            }
        } else if (cmd == "#AddList" && section == 2) {
            int c1, c2, detail; std::string name;
            if (iss >> c1 >> c2) {
                std::getline(iss, name);
                name.erase(0, name.find_first_not_of(" \t"));
                size_t tab = name.find('\t');
                if (tab != std::string::npos) {
                    std::string rest = name.substr(tab + 1);
                    name = name.substr(0, tab);
                    detail = std::atoi(rest.c_str());
                } else {
                    detail = 0;
                }
                Category2Entry e;
                e.cat1_id = c1;
                e.cat2_id = c2;
                e.name = name;
                e.item_type_detail = detail;
                cat2_list_.push_back(e);
            }
        }
    }

    spdlog::info("ConsignmentCategory: loaded {} cat1, {} cat2 entries",
                 cat1_list_.size(), cat2_list_.size());
}

void ConsignmentDialog::RefreshCat2Combo(int cat1_id) {
    if (!cat2_combo_) return;
    cat2_combo_->Clear();
    cat2_visible_indices_.clear();
    for (size_t i = 0; i < cat2_list_.size(); i++) {
        if (cat2_list_[i].cat1_id == cat1_id) {
            cat2_combo_->AddItem(cat2_list_[i].name);
            cat2_visible_indices_.push_back(static_cast<int>(i));
        }
    }
}

void ConsignmentDialog::Open(GameState* state, WindowManager* wm, ConsignmentSystem* cs) {
    consignment_ = cs;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Consignment.bin.txt",
        "Auction House", 80, 40, 700, 520);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    LoadCategories("assets/interface/Windows/ConsignmentCategory.bin.txt");

    char gold_buf[64];
    snprintf(gold_buf, sizeof(gold_buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(gold_buf, 10, 4, 0xffffcc00);

    auto* search_lbl = window_->AddWidget<Label>("Search:", 10, 28, 0xffcccccc);
    (void)search_lbl;
    search_input_ = window_->AddWidget<InputField>(70, 28, 140, 20);
    search_input_->SetPlaceholder("item name...");

    auto* cat1_lbl = window_->AddWidget<Label>("Category:", 220, 28, 0xffcccccc);
    (void)cat1_lbl;
    cat1_combo_ = window_->AddWidget<ComboBox>(290, 28, 140, 20);
    for (const auto& c : cat1_list_) {
        cat1_combo_->AddItem(c.name);
    }
    cat1_combo_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::ValueChanged) {
            int idx = e.int_value;
            if (idx >= 0 && idx < (int)cat1_list_.size()) {
                RefreshCat2Combo(cat1_list_[idx].id);
            }
        }
    });
    if (!cat1_list_.empty()) RefreshCat2Combo(cat1_list_[0].id);

    auto* search_btn = window_->AddWidget<Button>("Search", 440, 28, 70, 20);
    search_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    search_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) RefreshBrowse(state);
    });

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 520, 28, 70, 20);
    refresh_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            RefreshBrowse(state);
            RefreshMyListings(state);
            RefreshMyBids(state);
            RefreshStall(state);
        }
    });

    tabs_ = window_->AddWidget<TabPanel>(10, 52, 680, 370);
    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            browse_grid_->SetVisible(e.int_value == 0);
            my_grid_->SetVisible(e.int_value == 1);
            bid_grid_->SetVisible(e.int_value == 2);
            if (stall_list_) stall_list_->SetVisible(e.int_value == 3);
        }
    });

    browse_grid_ = window_->AddWidget<Grid>(6, 4, 165, 30, 14, 80);
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
    tabs_->AddTab("Browse", browse_grid_);

    my_grid_ = window_->AddWidget<Grid>(6, 4, 165, 30, 14, 80);
    my_grid_->SetPadding(3);
    tabs_->AddTab("My Listings", my_grid_);

    bid_grid_ = window_->AddWidget<Grid>(6, 4, 165, 30, 14, 80);
    bid_grid_->SetPadding(3);
    tabs_->AddTab("My Bids", bid_grid_);

    stall_list_ = new ListBox(0, 0, 660, 340);
    tabs_->AddTab("Stall", stall_list_);

    auto* buyout_btn = window_->AddWidget<Button>("Buyout", 10, 440, 80, 24);
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

    auto* bid_btn = window_->AddWidget<Button>("Bid", 100, 440, 70, 24);
    bid_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click || tabs_->GetActive() != 0) return;
        int idx = selected_row_ >= 0 ? selected_row_ * 4 : -1;
        if (idx >= 0 && idx < (int)cached_listings_.size())
            DoBid(state, cached_listings_[idx]);
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 200, 440, 80, 24);
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

    auto* list_btn = window_->AddWidget<Button>("List Item", 520, 440, 90, 24);
    list_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    list_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (tabs_->GetActive() == 3 && net_cb_.enabled && net_cb_.on_stall_open) {
            net_cb_.on_stall_open("My Stall");
            return;
        }
        DoListItem(state);
    });

    auto* stall_add_btn = window_->AddWidget<Button>("Stall Add", 420, 440, 80, 24);
    stall_add_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click || tabs_->GetActive() != 3) return;
        if (net_cb_.enabled && net_cb_.on_stall_add && !state->inventory.empty())
            net_cb_.on_stall_add(static_cast<uint8_t>(state->inventory[0].slot), 50);
    });

    status_label_ = window_->AddWidget<Label>("Select a listing or list an item", 10, 476, 0xffaaaaaa);
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

    int cat1 = 0, cat2 = 0;
    if (cat1_combo_) {
        int c1_idx = cat1_combo_->GetSelected();
        if (c1_idx >= 0 && c1_idx < (int)cat1_list_.size())
            cat1 = cat1_list_[c1_idx].id;
    }
    if (cat2_combo_) {
        int c2_idx = cat2_combo_->GetSelected();
        if (c2_idx >= 0 && c2_idx < (int)cat2_visible_indices_.size()) {
            int real_idx = cat2_visible_indices_[c2_idx];
            if (real_idx >= 0 && real_idx < (int)cat2_list_.size())
                cat2 = cat2_list_[real_idx].cat2_id;
        }
    }

    if (net_cb_.enabled && net_cb_.on_search) {
        std::string q = search_input_ ? search_input_->GetText() : "";
        net_cb_.on_search(q);
        cached_listings_ = state->consignment_browse;
    } else if (consignment_) {
        std::string q = search_input_ ? search_input_->GetText() : "";
        cached_listings_ = consignment_->SearchListings(q);
        if (cat1 > 0 && !cached_listings_.empty()) {
            std::string cat1_name;
            for (const auto& c : cat1_list_) {
                if (c.id == cat1) { cat1_name = c.name; break; }
            }
            if (!cat1_name.empty()) {
                auto it = std::remove_if(cached_listings_.begin(), cached_listings_.end(),
                    [&cat1_name](const AuctionListing& l) {
                        return l.item_name.find(cat1_name) == std::string::npos &&
                               l.item_name.find(cat1_name.substr(0, 4)) == std::string::npos;
                    });
                cached_listings_.erase(it, cached_listings_.end());
            }
        }
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
