#include "CashShopDialog.hpp"
#include <cstdio>

void CashShopDialog::Open(GameState* state, WindowManager* wm, CashShopSystem* shop) {
    (void)info_label_;
    shop_ = shop;
    window_ = wm->Open("Cash Shop", 120, 40, 520, 480);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    int luna = 500; // demo Luna Points
    char buf[64];
    snprintf(buf, sizeof(buf), "Luna Points: %d  |  Gold: %d", luna, state->gold);
    luna_label_ = window_->AddWidget<Label>(buf, 10, 4, 0xff88ccff);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 500, 400);

    // Tab 0: Shop
    shop_content_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 1: Battle Pass
    bp_content_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    bp_bar_ = window_->AddWidget<ProgressBar>(14, 380, 300, 18);

    // Action buttons
    auto* buy_btn = window_->AddWidget<Button>("Buy Selected", 14, 430, 120, 24);
    buy_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && shop_) {
            int luna = 500;
            if (shop_->PurchaseItem(luna, 1)) {
                state->chat_messages.push_back("Purchased from Cash Shop!");
                if (luna_label_) {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "Luna Points: %d  |  Gold: %d", luna, state->gold);
                    luna_label_->SetText(buf);
                }
            } else {
                state->chat_messages.push_back("Not enough Luna Points!");
            }
        }
    });

    auto* add_lp_btn = window_->AddWidget<Button>("+100 LP (Test)", 150, 430, 130, 24);
    add_lp_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    add_lp_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (luna_label_) {
                // Just update display
                char buf[64];
                snprintf(buf, sizeof(buf), "Luna Points: %d  |  Gold: %d", 600, 0);
                luna_label_->SetText(buf);
            }
        }
    });

    tabs_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            shop_content_->SetVisible(e.int_value == 0);
            bp_content_->SetVisible(e.int_value == 1);
            bp_bar_->SetVisible(e.int_value == 1);
            if (e.int_value == 0) RefreshShop(0);
            else RefreshBattlePass(state);
        }
    });
    bp_content_->SetVisible(false);
    bp_bar_->SetVisible(false);

    RefreshShop(0);
}

void CashShopDialog::RefreshShop(int) {
    if (!shop_ || !shop_content_) return;
    std::string text = "=== Cash Shop Items ===\n\n";
    auto items = shop_->GetItems();
    int idx = 0;
    for (auto& item : items) {
        char buf[256];
        snprintf(buf, sizeof(buf), "[%d] %s — %d LP\n    %s (x%d)\n",
            idx + 1, item.name.c_str(), item.price_luna,
            item.description.c_str(), item.item_count);
        text += buf;
        idx++;
    }
    text += "\nUse 'Buy Selected' to purchase item #1.\n(Full category browsing coming soon)";
    shop_content_->SetText(text);
}

void CashShopDialog::RefreshBattlePass(GameState* state) {
    if (!shop_ || !bp_content_) return;
    (void)state;
    char buf[1024];
    int lvl = shop_->GetBattlePassLevel();
    int xp = shop_->GetBattlePassXP();
    int max_xp = shop_->GetBattlePassMaxXP();
    bool active = shop_->IsBattlePassActive();
    
    snprintf(buf, sizeof(buf),
        "=== Battle Pass ===\n"
        "Status: %s\n"
        "Level: %d\n"
        "Progress: %d / %d XP\n\n"
        "=== Current Rewards ===\n"
        "Level %d: %s\n\n"
        "Kill monsters to earn BP XP!\n"
        "(10 XP per kill)",
        active ? "Active" : "Not active",
        lvl, xp, max_xp,
        lvl, shop_->GetBattlePassRewards(lvl).empty() ? "Gold" : shop_->GetBattlePassRewards(lvl)[0].c_str());
    bp_content_->SetText(buf);
    bp_bar_->SetProgress((float)xp / (float)max_xp);
}

void CashShopDialog::UpdateFromState(GameState* state) {
    (void)state;
}
