#include "TradeDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void TradeDialog::Open(GameState* state, TradingSystem* trading, WindowManager* wm) {
    trading_ = trading;
    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Trade.bin.txt");
    if (!window_) {
        spdlog::warn("TradeDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("TRADE", 200, 100, 500, 400);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    if (!net_cb_.enabled && trading_) {
        session_id_ = trading_->RequestTrade(state->selected_char, state->name,
                                             state->selected_char + 1, "NPC_Trader");
        auto* session = trading_->GetSession(session_id_);
        if (session) {
            TradeItem ti;
            ti.id = 201; ti.name = "Silver Ore"; ti.count = 5; ti.enchant = 0; ti.inventory_slot = -1;
            session->items_b.push_back(ti);
            ti.id = 202; ti.name = "Gold Ore"; ti.count = 3; ti.enchant = 0; ti.inventory_slot = -1;
            session->items_b.push_back(ti);
            session->gold_b = 500;
        }
    }

    auto* my_label = window_->AddWidget<Label>("Your Items (click to add)", 10, 26, ColorPalette::TEXT_GOLD);
    (void)my_label;
    my_items_ = window_->AddWidget<Grid>(3, 4, 50, 35, 10, 46);
    my_items_->OnSlotEvent([this, state](int row, int col, const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        int idx = row * 4 + col;
        if (net_cb_.enabled && net_cb_.on_add_item) {
            if (idx >= 0 && idx < (int)state->inventory.size())
                net_cb_.on_add_item(static_cast<uint8_t>(state->inventory[idx].slot));
            return;
        }
        if (trading_) {
            if (idx >= 0 && idx < (int)state->inventory.size()) {
                auto& inv = state->inventory[idx];
                TradeItem ti;
                ti.id = inv.id; ti.name = inv.name; ti.count = 1;
                ti.enchant = inv.enchant; ti.inventory_slot = idx;
                trading_->AddItem(session_id_, state->selected_char, ti);
                Refresh(state);
            }
        }
    });
    my_gold_label_ = window_->AddWidget<Label>("Gold: 0 — click to add 100g", 10, 195, ColorPalette::TEXT_GOLD_BOLD);
    my_gold_label_->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (net_cb_.enabled && net_cb_.on_add_gold) {
            if (state->gold >= 100) net_cb_.on_add_gold(100);
            return;
        }
        if (trading_ && state->gold >= 100) {
            trading_->SetGold(session_id_, state->selected_char,
                              trading_->GetSession(session_id_)->gold_a + 100);
            state->gold -= 100;
            Refresh(state);
        }
    });

    auto* their_label = window_->AddWidget<Label>("Partner Items", 260, 26, ColorPalette::TEXT_GOLD);
    (void)their_label;
    their_items_ = window_->AddWidget<Grid>(3, 4, 50, 35, 260, 46);
    their_gold_label_ = window_->AddWidget<Label>("Gold: 0", 260, 195, ColorPalette::TEXT_GOLD_BOLD);

    auto* confirm_btn = window_->AddWidget<Button>("Confirm", 140, 260, 80, 24);
    confirm_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    confirm_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (net_cb_.enabled && net_cb_.on_confirm) {
            net_cb_.on_confirm();
            return;
        }
        if (trading_ && trading_->Confirm(session_id_, state->selected_char)) {
            if (status_label_) status_label_->SetText("Trade completed!");
            state->chat_messages.push_back("Trade completed successfully!");
            Refresh(state);
        } else if (status_label_) {
            status_label_->SetText("Waiting for other player...");
        }
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 240, 260, 80, 24);
    cancel_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    cancel_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        if (net_cb_.enabled && net_cb_.on_cancel) {
            net_cb_.on_cancel();
            if (status_label_) status_label_->SetText("Trade cancelled.");
            return;
        }
        if (trading_) {
            trading_->CancelTrade(session_id_, state->selected_char);
            if (status_label_) status_label_->SetText("Trade cancelled.");
            state->chat_messages.push_back("Trade cancelled.");
        }
    });

    status_label_ = window_->AddWidget<Label>("Add items from inventory to begin trading", 10, 300, ColorPalette::BTN_NORMAL);
    Refresh(state);
}

void TradeDialog::Refresh(GameState* state) {
    if (!window_ || !state) return;

    if (net_cb_.enabled || state->trade_active) {
        my_items_->ClearAll();
        for (size_t i = 0; i < state->trade_my_items.size() && i < 12; i++) {
            const auto& item = state->trade_my_items[i];
            int r = static_cast<int>(i / 4), c = static_cast<int>(i % 4);
            char buf[48];
            snprintf(buf, sizeof(buf), "%s x%u",
                item.name.empty() ? ("Item_" + std::to_string(item.item_id)).c_str() : item.name.c_str(),
                item.count);
            GridSlot gs; gs.empty = false; gs.text = buf;
            my_items_->SetSlot(r, c, gs);
        }
        if (my_gold_label_) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Gold: %u (click +100)", state->trade_my_gold);
            my_gold_label_->SetText(buf);
        }

        their_items_->ClearAll();
        for (size_t i = 0; i < state->trade_their_items.size() && i < 12; i++) {
            const auto& item = state->trade_their_items[i];
            int r = static_cast<int>(i / 4), c = static_cast<int>(i % 4);
            char buf[48];
            snprintf(buf, sizeof(buf), "%s x%u",
                item.name.empty() ? ("Item_" + std::to_string(item.item_id)).c_str() : item.name.c_str(),
                item.count);
            GridSlot gs; gs.empty = false; gs.text = buf;
            their_items_->SetSlot(r, c, gs);
        }
        if (their_gold_label_) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Gold: %u", state->trade_their_gold);
            their_gold_label_->SetText(buf);
        }

        if (status_label_) {
            if (state->trade_completed)
                status_label_->SetText("Trade completed!");
            else if (state->trade_my_confirmed)
                status_label_->SetText("You confirmed. Waiting for partner...");
            else if (state->trade_active)
                status_label_->SetText("Trading with " + state->trade_partner_name);
            else
                status_label_->SetText("Requesting trade...");
        }
        return;
    }

    if (!trading_) return;
    auto* session = trading_->GetSession(session_id_);
    if (!session) return;

    my_items_->ClearAll();
    for (int i = 0; i < (int)session->items_a.size() && i < 12; i++) {
        int r = i / 4, c = i % 4;
        char buf[48];
        snprintf(buf, sizeof(buf), "%s x%d", session->items_a[i].name.c_str(), session->items_a[i].count);
        GridSlot gs; gs.empty = false; gs.text = buf;
        my_items_->SetSlot(r, c, gs);
    }
    if (my_gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", session->gold_a);
        my_gold_label_->SetText(buf);
    }

    their_items_->ClearAll();
    for (int i = 0; i < (int)session->items_b.size() && i < 12; i++) {
        int r = i / 4, c = i % 4;
        char buf[48];
        snprintf(buf, sizeof(buf), "%s x%d", session->items_b[i].name.c_str(), session->items_b[i].count);
        GridSlot gs; gs.empty = false; gs.text = buf;
        their_items_->SetSlot(r, c, gs);
    }
    if (their_gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", session->gold_b);
        their_gold_label_->SetText(buf);
    }

    if (status_label_) {
        if (session->completed) status_label_->SetText("Trade completed! Both sides confirmed.");
        else if (session->confirmed_a) status_label_->SetText("You confirmed. Waiting for NPC...");
        else status_label_->SetText("Add items and confirm when ready.");
    }
}

void TradeDialog::UpdateFromState(GameState* state) {
    if (window_ && state && state->trade_open)
        Refresh(state);
}
