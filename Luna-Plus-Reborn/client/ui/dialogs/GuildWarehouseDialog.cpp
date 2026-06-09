#include "GuildWarehouseDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void GuildWarehouseDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/GuildWarehouse.bin.txt");

    if (!window_) {
        spdlog::warn("GuildWarehouseDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("GUILD_WAREHOUSE", 200, 100, 480, 400);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    float tab_y = 28;
    float tab_h = 310;
    float content_x = 10;
    float content_w = 460;

    tabs_ = window_->AddWidget<TabPanel>(content_x, tab_y, content_w, tab_h);

    for (int t = 0; t < TABS; t++) {
        char tab_name[32];
        snprintf(tab_name, sizeof(tab_name), "Tab %d", t + 1);

        auto* grid = new Grid(6, 6, 38, 38, 8, 8);
        grids_[t] = grid;
        tabs_->AddTab(tab_name, grid);

        items_[t].resize(SLOTS_PER_TAB);
        for (int i = 0; i < SLOTS_PER_TAB; i++)
            items_[t][i] = {0, "", 0, i};
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "Gold: %u", gold_);
    gold_label_ = window_->AddWidget<Label>(buf, 10, tab_y + tab_h + 6, ColorPalette::TEXT_GOLD);

    snprintf(buf, sizeof(buf), "Min Rank: %u", min_rank_);
    rank_label_ = window_->AddWidget<Label>(buf, 200, tab_y + tab_h + 6, ColorPalette::BTN_NORMAL);

    auto* deposit_btn = window_->AddWidget<Button>("Deposit", 10, tab_y + tab_h + 28, 70, 22);
    deposit_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GuildWarehouse: deposit item");
        }
    });

    auto* withdraw_btn = window_->AddWidget<Button>("Withdraw", 88, tab_y + tab_h + 28, 70, 22);
    withdraw_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GuildWarehouse: withdraw item");
        }
    });

    auto* rank_btn = window_->AddWidget<Button>("Set Rank", 166, tab_y + tab_h + 28, 70, 22);
    rank_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GuildWarehouse: set rank dialog");
        }
    });

    auto* close_btn = window_->AddWidget<Button>("Close", 244, tab_y + tab_h + 28, 70, 22);
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            Close();
        }
    });
}

void GuildWarehouseDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

void GuildWarehouseDialog::UpdateFromState(GameState* state) {
    if (!state) return;
    if (state->guild_id == 0) return;

    for (int t = 0; t < TABS; t++) {
        if (!grids_[t]) continue;
        grids_[t]->ClearAll();
        for (int i = 0; i < SLOTS_PER_TAB && i < (int)items_[t].size(); i++) {
            auto& item = items_[t][i];
            if (item.id > 0) {
                GridSlot gs;
                gs.empty = false;
                gs.count = item.count;
                gs.text = item.name;
                gs.userdata = (int)item.id;
                int row = i / 6, col = i % 6;
                grids_[t]->SetSlot(row, col, gs);
            }
        }
    }

    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %u", gold_);
        gold_label_->SetText(buf);
    }
}

void GuildWarehouseDialog::SetItems(int tab, const std::vector<GuildWareItem>& items) {
    if (tab < 0 || tab >= TABS) return;
    items_[tab] = items;
    items_[tab].resize(SLOTS_PER_TAB);
}

void GuildWarehouseDialog::SetGold(uint32_t gold) {
    gold_ = gold;
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %u", gold_);
        gold_label_->SetText(buf);
    }
}

void GuildWarehouseDialog::SetWarehouseRank(uint8_t rank) {
    min_rank_ = rank;
    if (rank_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Min Rank: %u", min_rank_);
        rank_label_->SetText(buf);
    }
}

void GuildWarehouseDialog::SetMinRank(uint8_t rank) {
    SetWarehouseRank(rank);
}
