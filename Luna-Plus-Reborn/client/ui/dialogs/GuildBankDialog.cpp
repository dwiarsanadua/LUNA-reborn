#include "GuildBankDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/UiScriptParser.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void GuildBankDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GuildBankDlg.bin.txt",
            "Guild Bank", 280, 80, 460, 400);
    } else {
        window_ = new Window("Guild Bank", 280, 80, 460, 400);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 440, 280);

    // Bank items grid (7x6 = 42 slots)
    bank_grid_ = new Grid(7, 6, 52, 52, 8, 8);
    tabs_->AddTab("Bank Items", bank_grid_);

    // Log tab (placeholder)
    auto* log_label = new Label("Bank transaction log", 10, 10, 0xffcccccc);
    tabs_->AddTab("Log", log_label);

    // Gold display
    gold_label_ = window_->AddWidget<Label>("Guild Gold: 0", 10, 315, ColorPalette::TEXT_GOLD);
    gold_label_->SetRect(10, 315, 200, 20);

    // Deposit item section
    window_->AddWidget<Label>("Item Slot:", 10, 340, ColorPalette::BTN_NORMAL)->SetRect(10, 340, 60, 20);
    amount_input_ = window_->AddWidget<InputField>(72, 338, 50, 22);
    amount_input_->SetPlaceholder("slot");
    amount_input_->SetValidation(InputValidation::PositiveInteger);

    auto* dep_item_btn = window_->AddWidget<Button>("Deposit", 130, 338, 70, 22);
    dep_item_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    dep_item_btn->OnEvent([this](const UIEvent& e) {
        if (e.type != UIEvent::Click || !amount_input_) return;
        int slot = std::atoi(amount_input_->GetText().c_str());
        if (slot >= 0 && on_deposit_item_) {
            on_deposit_item_(0, slot, 1);
            amount_input_->SetText("");
        }
    });

    auto* wd_item_btn = window_->AddWidget<Button>("Withdraw", 210, 338, 80, 22);
    wd_item_btn->SetColors({40,40,80,220}, {80,80,130,220}, {30,30,50,220});
    wd_item_btn->OnEvent([this](const UIEvent& e) {
        if (e.type != UIEvent::Click || !amount_input_) return;
        int slot = std::atoi(amount_input_->GetText().c_str());
        if (slot >= 0 && on_withdraw_item_) {
            on_withdraw_item_(0, slot, 1);
            amount_input_->SetText("");
        }
    });

    // Gold deposit/withdraw section
    window_->AddWidget<Label>("Gold:", 10, 368, ColorPalette::BTN_NORMAL)->SetRect(10, 368, 40, 20);
    gold_input_ = window_->AddWidget<InputField>(50, 366, 80, 22);
    gold_input_->SetPlaceholder("amount");
    gold_input_->SetValidation(InputValidation::PositiveInteger);

    auto* dep_gold_btn = window_->AddWidget<Button>("Deposit Gold", 140, 366, 100, 22);
    dep_gold_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    dep_gold_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DepositGold();
    });

    auto* wd_gold_btn = window_->AddWidget<Button>("Withdraw Gold", 250, 366, 110, 22);
    wd_gold_btn->SetColors({40,40,80,220}, {80,80,130,220}, {30,30,50,220});
    wd_gold_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) WithdrawGold();
    });

    // Refresh button
    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 370, 315, 70, 22);
    refresh_btn->SetColors({60,60,80,220}, {100,100,140,220}, {40,40,60,220});
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_refresh_) on_refresh_();
    });

    items_.resize(42);
    if (state) UpdateFromState(state);
}

void GuildBankDialog::Close() {
    window_ = nullptr;
}

void GuildBankDialog::SetNetworkCallbacks(
    std::function<void(uint32_t, int, int)> deposit_item,
    std::function<void(uint32_t, int, int)> withdraw_item,
    std::function<void(uint32_t)> deposit_gold,
    std::function<void(uint32_t)> withdraw_gold,
    std::function<void()> refresh_bank) {
    on_deposit_item_ = deposit_item;
    on_withdraw_item_ = withdraw_item;
    on_deposit_gold_ = deposit_gold;
    on_withdraw_gold_ = withdraw_gold;
    on_refresh_ = refresh_bank;
}

void GuildBankDialog::DepositGold() {
    if (!gold_input_ || !on_deposit_gold_) return;
    uint32_t amount = static_cast<uint32_t>(std::atoi(gold_input_->GetText().c_str()));
    if (amount > 0) {
        on_deposit_gold_(amount);
        gold_input_->SetText("");
    }
}

void GuildBankDialog::WithdrawGold() {
    if (!gold_input_ || !on_withdraw_gold_) return;
    uint32_t amount = static_cast<uint32_t>(std::atoi(gold_input_->GetText().c_str()));
    if (amount > 0 && amount <= guild_gold_) {
        on_withdraw_gold_(amount);
        gold_input_->SetText("");
    }
}

void GuildBankDialog::UpdateBankItems(const std::vector<GuildBankItem>& items) {
    items_ = items;
    if (bank_grid_) RefreshDisplay();
}

void GuildBankDialog::UpdateBankGold(uint32_t gold) {
    guild_gold_ = gold;
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Guild Gold: %u", gold);
        gold_label_->SetText(buf);
    }
}

void GuildBankDialog::AddLogEntry(const GuildBankLog& entry) {
    spdlog::debug("GuildBank: {} {} x{} by {}",
                  entry.type == BankLogType::Deposit ? "deposit" : "withdraw",
                  entry.item_name, entry.amount, entry.player_name);
}

void GuildBankDialog::RefreshDisplay() {
    if (!bank_grid_) return;
    bank_grid_->ClearAll();

    for (size_t i = 0; i < items_.size() && i < 42; i++) {
        auto& item = items_[i];
        if (item.id > 0) {
            GridSlot gs;
            gs.empty = false;
            gs.count = item.count;
            char buf[48];
            if (item.enchant > 0)
                snprintf(buf, sizeof(buf), "+%d %s", item.enchant, item.name.c_str());
            else
                snprintf(buf, sizeof(buf), "%s", item.name.c_str());
            gs.text = buf;
            gs.userdata = static_cast<int>(item.id);
            int row = static_cast<int>(i / 7);
            int col = static_cast<int>(i % 7);
            bank_grid_->SetSlot(row, col, gs);
        }
    }
}

void GuildBankDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;

    // In a full implementation, sync from network state
    if (window_ && !window_->IsVisible()) {
        Close();
    }
}
