#include "StreetStallDialog.hpp"
#include <cstdio>

void StreetStallDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/StallSell.bin.txt",
        "Street Stall", 200, 80, 480, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    char buf[64];
    snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
    gold_label_ = window_->AddWidget<Label>(buf, 10, 4, 0xffffcc00);

    auto* title_lbl = window_->AddWidget<Label>("Title:", 10, 28, 0xffcccccc);
    (void)title_lbl;
    title_input_ = window_->AddWidget<InputField>(60, 28, 180, 20);
    title_input_->SetPlaceholder("My Cool Shop");

    auto* price_lbl = window_->AddWidget<Label>("Price:", 10, 52, 0xffcccccc);
    (void)price_lbl;
    price_input_ = window_->AddWidget<InputField>(60, 52, 100, 20);
    price_input_->SetPlaceholder("50");

    // My Stall section
    auto* my_label = window_->AddWidget<Label>("--- My Stall ---", 10, 76, 0xff88ff88);
    (void)my_label;
    my_stall_list_ = new ListBox(10, 94, 460, 100);
    my_stall_list_->AddItem("(Stall closed)");

    // Nearby stalls section
    auto* nearby_label = window_->AddWidget<Label>("--- Nearby Stalls ---", 10, 200, 0xff88ff88);
    (void)nearby_label;
    nearby_list_ = new ListBox(10, 218, 460, 100);
    nearby_list_->AddItem("(No nearby stalls)");
    nearby_list_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int sel = nearby_list_->GetSelected();
            if (sel < 0) return;
            const char* text = nearby_list_->GetItem(sel);
            if (!text) return;
            unsigned owner = 0; int slot = 0;
            if (sscanf(text, "slot %u", &slot) == 1 ||
                sscanf(text, "  slot %u", &slot) == 1) {
                selected_nearby_slot_ = (uint8_t)slot;
            }
        }
    });

    // Buttons row 1
    auto* open_btn = window_->AddWidget<Button>("Open Stall", 10, 326, 90, 24);
    open_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    open_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoOpenStall(state);
    });

    auto* add_btn = window_->AddWidget<Button>("Add Item", 110, 326, 80, 24);
    add_btn->SetColors({50,80,50,220}, {80,130,80,220}, {30,50,30,220});
    add_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoAddItem(state);
    });

    auto* buy_btn = window_->AddWidget<Button>("Buy Item", 200, 326, 80, 24);
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoBuyItem(state);
    });

    auto* close_btn = window_->AddWidget<Button>("Close Stall", 290, 326, 90, 24);
    close_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    close_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoCloseStall(state);
    });

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 390, 326, 70, 24);
    refresh_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoRefresh(state);
    });

    status_label_ = window_->AddWidget<Label>("Ready", 10, 358, 0xffaaaaaa);

    DoRefresh(state);
}

void StreetStallDialog::RefreshMyStall(GameState* state) {
    if (!my_stall_list_ || !state) return;
    my_stall_list_->Clear();
    if (state->my_stall.open) {
        my_stall_list_->AddItem("[OPEN] " + state->my_stall.title);
        for (const auto& it : state->my_stall.items) {
            char line[128];
            snprintf(line, sizeof(line), "  slot %u: %s x%u @ %ug",
                it.slot, it.item_name.c_str(), it.count, it.price);
            my_stall_list_->AddItem(line);
        }
    } else {
        my_stall_list_->AddItem("(Stall closed — click Open Stall)");
    }
}

void StreetStallDialog::RefreshNearby(GameState* state) {
    if (!nearby_list_ || !state) return;
    nearby_list_->Clear();
    bool any = false;
    for (const auto& stall : state->nearby_stalls) {
        if (!stall.open) continue;
        any = true;
        char header[128];
        snprintf(header, sizeof(header), "[%s] %s", stall.owner_name.c_str(), stall.title.c_str());
        nearby_list_->AddItem(header);
        for (const auto& it : stall.items) {
            char line[128];
            snprintf(line, sizeof(line), "  slot %u: %s x%u @ %ug (owner %u)",
                it.slot, it.item_name.c_str(), it.count, it.price, stall.owner_id);
            nearby_list_->AddItem(line);
            selected_nearby_owner_ = stall.owner_id;
            selected_nearby_slot_ = it.slot;
        }
    }
    if (!any)
        nearby_list_->AddItem("(No nearby stalls — /stall list to refresh)");
}

void StreetStallDialog::DoOpenStall(GameState* state) {
    if (!state) return;
    std::string title = title_input_ ? title_input_->GetText() : "";
    if (title.empty()) title = "My Stall";
    if (net_cb_.enabled && net_cb_.on_open) {
        net_cb_.on_open(title);
        if (status_label_) status_label_->SetText("Opening stall...");
    } else {
        state->my_stall.open = true;
        state->my_stall.title = title;
        if (status_label_) status_label_->SetText("Stall opened (offline)");
        RefreshMyStall(state);
    }
}

void StreetStallDialog::DoAddItem(GameState* state) {
    if (!state) return;
    if (state->inventory.empty()) {
        if (status_label_) status_label_->SetText("No items in inventory!");
        return;
    }
    int price_val = price_input_ ? std::atoi(price_input_->GetText().c_str()) : 0;
    if (price_val <= 0) price_val = 50;
    uint8_t slot = static_cast<uint8_t>(state->inventory[0].slot);
    if (net_cb_.enabled && net_cb_.on_add) {
        net_cb_.on_add(slot, static_cast<uint32_t>(price_val));
        if (status_label_) status_label_->SetText("Adding item to stall...");
    } else {
        if (status_label_) status_label_->SetText("Item added (offline)");
    }
}

void StreetStallDialog::DoBuyItem(GameState* state) {
    if (!state) return;
    if (selected_nearby_owner_ == 0 && selected_nearby_slot_ == 0) {
        if (status_label_) status_label_->SetText("Select an item from nearby stalls first!");
        return;
    }
    if (net_cb_.enabled && net_cb_.on_buy) {
        net_cb_.on_buy(selected_nearby_owner_, selected_nearby_slot_);
        if (status_label_) status_label_->SetText("Buying item...");
    } else {
        if (status_label_) status_label_->SetText("Buy not available (online required)");
    }
}

void StreetStallDialog::DoCloseStall(GameState* state) {
    if (!state) return;
    if (net_cb_.enabled && net_cb_.on_close) {
        net_cb_.on_close();
        if (status_label_) status_label_->SetText("Closing stall...");
    } else {
        state->my_stall.open = false;
        state->my_stall.items.clear();
        if (status_label_) status_label_->SetText("Stall closed (offline)");
        RefreshMyStall(state);
    }
}

void StreetStallDialog::DoRefresh(GameState* state) {
    if (!state) return;
    if (net_cb_.enabled && net_cb_.on_list)
        net_cb_.on_list();
    RefreshMyStall(state);
    RefreshNearby(state);
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
}

void StreetStallDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    RefreshMyStall(state);
    RefreshNearby(state);
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
}
