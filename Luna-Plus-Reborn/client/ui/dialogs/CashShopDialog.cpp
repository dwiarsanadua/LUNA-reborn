#include "CashShopDialog.hpp"
#include <cstdio>
#include <algorithm>

void CashShopDialog::SetNetworkCallbacks(std::function<void(uint32_t)> buy_fn,
                                         std::function<void()> refresh_fn) {
    buy_fn_ = std::move(buy_fn);
    refresh_fn_ = std::move(refresh_fn);
}

std::vector<const GameState::NetworkCashShopItem*> CashShopDialog::FilteredNetworkItems(
    const GameState* state) const {
    std::vector<const GameState::NetworkCashShopItem*> out;
    if (!state) return out;
    for (const auto& item : state->network_cashshop_items) {
        if (!category_filter_.empty() && item.category != category_filter_) continue;
        out.push_back(&item);
    }
    return out;
}

void CashShopDialog::ClampSelection(GameState* state) {
    int count = 0;
    if (online_mode_ && state) {
        count = static_cast<int>(FilteredNetworkItems(state).size());
    } else if (shop_) {
        count = static_cast<int>(shop_->GetItems(category_filter_).size());
    }
    if (count <= 0) {
        selected_index_ = 0;
        return;
    }
    if (selected_index_ >= count) selected_index_ = count - 1;
    if (selected_index_ < 0) selected_index_ = 0;
}

uint32_t CashShopDialog::SelectedItemId(GameState* state) const {
    if (online_mode_ && state) {
        auto items = FilteredNetworkItems(state);
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items.size()))
            return items[static_cast<size_t>(selected_index_)]->item_id;
        return 0;
    }
    if (shop_) {
        auto items = shop_->GetItems(category_filter_);
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items.size()))
            return items[static_cast<size_t>(selected_index_)].item_id;
    }
    return 0;
}

void CashShopDialog::Open(GameState* state, WindowManager* wm, CashShopSystem* shop) {
    (void)info_label_;
    shop_ = shop;
    selected_index_ = 0;
    category_filter_.clear();
    online_mode_ = !state->network_cashshop_items.empty() && !state->offline_mode;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ItemShopDlg.bin.txt",
        "Cash Shop", 120, 40, 520, 480);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    char buf[96];
    snprintf(buf, sizeof(buf), "Luna Points: %d  |  Gold: %d",
        state->luna_points, state->gold);
    luna_label_ = window_->AddWidget<Label>(buf, 10, 4, 0xff88ccff);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 500, 400);
    shop_content_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    bp_content_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    bp_bar_ = window_->AddWidget<ProgressBar>(14, 380, 300, 18);

    auto* prev_btn = window_->AddWidget<Button>("< Prev", 14, 430, 70, 24);
    prev_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    prev_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            selected_index_--;
            ClampSelection(state);
            RefreshShop(state);
        }
    });

    auto* next_btn = window_->AddWidget<Button>("Next >", 90, 430, 70, 24);
    next_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    next_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            selected_index_++;
            ClampSelection(state);
            RefreshShop(state);
        }
    });

    auto* buy_btn = window_->AddWidget<Button>("Buy Selected", 170, 430, 120, 24);
    buy_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            online_mode_ = !state->network_cashshop_items.empty() && !state->offline_mode;
            uint32_t item_id = SelectedItemId(state);
            if (buy_fn_ && item_id != 0) {
                buy_fn_(item_id);
            } else if (shop_) {
                int luna = state->luna_points > 0 ? state->luna_points : 500;
                if (shop_->PurchaseItem(luna, item_id != 0 ? item_id : 1)) {
                    state->luna_points = luna;
                    state->chat_messages.push_back("Purchased from Cash Shop!");
                    RefreshShop(state);
                } else {
                    state->chat_messages.push_back("Not enough Luna Points!");
                }
            }
        }
    });

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 300, 430, 80, 24);
    refresh_btn->SetColors({60,60,60,220}, {100,100,100,220}, {40,40,40,220});
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && refresh_fn_) refresh_fn_();
    });

    tabs_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            shop_content_->SetVisible(e.int_value == 0);
            bp_content_->SetVisible(e.int_value == 1);
            bp_bar_->SetVisible(e.int_value == 1);
            if (e.int_value == 0) RefreshShop(state);
            else RefreshBattlePass(state);
        }
    });
    bp_content_->SetVisible(false);
    bp_bar_->SetVisible(false);

    RefreshShop(state);
}

void CashShopDialog::RefreshShop(GameState* state) {
    if (!shop_content_) return;
    online_mode_ = state && !state->network_cashshop_items.empty() && !state->offline_mode;
    ClampSelection(state);

    if (luna_label_ && state) {
        char hdr[96];
        snprintf(hdr, sizeof(hdr), "Luna Points: %d  |  Gold: %d",
            state->luna_points, state->gold);
        luna_label_->SetText(hdr);
    }

    std::string text = online_mode_ ? "=== Server Cash Shop ===\n\n" : "=== Cash Shop Items ===\n\n";
    int idx = 0;
    if (online_mode_ && state) {
        for (const auto* item : FilteredNetworkItems(state)) {
            const char* cur = item->currency_type == 0 ? "gold" : "LP";
            const char* mark = (idx == selected_index_) ? ">>" : "  ";
            char buf[384];
            snprintf(buf, sizeof(buf),
                "%s [%d] %s — %u %s (%s)\n    %s x%d  bought %u/%u %s%s\n",
                mark, idx + 1, item->name.c_str(), item->price, cur, item->category.c_str(),
                item->description.c_str(), item->stack_count,
                item->purchased_count, item->max_purchase,
                item->can_afford ? "" : "[cannot afford] ",
                item->on_sale ? "[SALE]" : "");
            text += buf;
            idx++;
        }
    } else if (shop_) {
        for (const auto& item : shop_->GetItems(category_filter_)) {
            const char* mark = (idx == selected_index_) ? ">>" : "  ";
            char buf[384];
            snprintf(buf, sizeof(buf), "%s [%d] %s — %d LP\n    %s (x%d)\n",
                mark, idx + 1, item.name.c_str(), item.price_luna,
                item.description.c_str(), item.item_count);
            text += buf;
            idx++;
        }
    }
    if (idx == 0) text += "(No items in this category)\n";
    text += "\nUse Prev/Next to select, then Buy Selected.";
    shop_content_->SetText(text);
}

void CashShopDialog::UpdateFromState(GameState* state) {
    if (!state || !shop_content_) return;
    online_mode_ = !state->network_cashshop_items.empty() && !state->offline_mode;
    RefreshShop(state);
}

void CashShopDialog::RefreshBattlePass(GameState* state) {
    if (!bp_content_) return;
    char buf[1024];
    int lvl = 0;
    int xp = 0;
    int max_xp = 1000;
    bool active = false;
    const char* season = "Season 1";

    if (state && online_mode_) {
        lvl = state->network_battle_pass_level;
        xp = static_cast<int>(state->network_battle_pass_xp);
        max_xp = static_cast<int>(state->network_battle_pass_max_xp);
        active = state->network_battle_pass_active;
        season = state->network_season_name.empty() ? "Season 1" : state->network_season_name.c_str();
    } else if (shop_) {
        lvl = shop_->GetBattlePassLevel();
        xp = shop_->GetBattlePassXP();
        max_xp = shop_->GetBattlePassMaxXP();
        active = shop_->IsBattlePassActive();
        season = shop_->GetSeasonName().c_str();
    }

    snprintf(buf, sizeof(buf),
        "=== Battle Pass (%s) ===\n"
        "Status: %s\n"
        "Level: %d\n"
        "Progress: %d / %d XP\n\n"
        "=== Current Rewards ===\n"
        "Level %d: %s\n\n"
        "Kill monsters to earn BP XP!\n"
        "(10 XP per kill)",
        season,
        active ? "Active" : "Not active",
        lvl, xp, max_xp,
        lvl, shop_ && !shop_->GetBattlePassRewards(lvl).empty()
            ? shop_->GetBattlePassRewards(lvl)[0].c_str() : "Gold");
    bp_content_->SetText(buf);
    if (bp_bar_) bp_bar_->SetProgress(max_xp > 0 ? (float)xp / (float)max_xp : 0.0f);
}
