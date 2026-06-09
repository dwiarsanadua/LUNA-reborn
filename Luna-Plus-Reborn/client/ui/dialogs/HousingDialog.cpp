#include "HousingDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <cstdio>
#include <algorithm>
#include <cstdlib>

void HousingDialog::SetNetworkCallbacks(
    std::function<void(uint8_t template_id)> buy_fn,
    std::function<void(uint32_t house_id)> enter_fn,
    std::function<void(uint32_t house_id, uint32_t item_id, float x, float y)> place_fn,
    std::function<void()> refresh_fn) {
    buy_fn_ = std::move(buy_fn);
    enter_fn_ = std::move(enter_fn);
    place_fn_ = std::move(place_fn);
    refresh_fn_ = std::move(refresh_fn);
}

void HousingDialog::SetWarehouseCallbacks(
    std::function<void(uint32_t, int, int)> deposit_item,
    std::function<void(uint32_t, int, int)> withdraw_item,
    std::function<void(uint32_t)> deposit_gold,
    std::function<void(uint32_t)> withdraw_gold) {
    wh_deposit_item_ = std::move(deposit_item);
    wh_withdraw_item_ = std::move(withdraw_item);
    wh_deposit_gold_ = std::move(deposit_gold);
    wh_withdraw_gold_ = std::move(withdraw_gold);
}

const GameState::NetworkHouseInfo* HousingDialog::SelectedHouse(const GameState* state) const {
    if (!state) return nullptr;
    for (const auto& h : state->network_houses) {
        if (h.house_id == selected_house_id_) return &h;
    }
    if (!state->network_houses.empty()) return &state->network_houses[0];
    return nullptr;
}

void HousingDialog::ClampTemplateSelection(GameState* state) {
    int count = online_mode_ && state
        ? static_cast<int>(state->network_house_templates.size()) : 3;
    if (count <= 0) {
        selected_template_ = 0;
        return;
    }
    if (selected_template_ >= count) selected_template_ = count - 1;
    if (selected_template_ < 0) selected_template_ = 0;
}

void HousingDialog::Open(GameState* state, WindowManager* wm) {
    selected_template_ = 0;
    selected_house_id_ = state->network_selected_house_id;
    online_mode_ = !state->offline_mode && !state->network_house_templates.empty();
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/HousingMainPointDlg.bin.txt",
        "Housing", 100, 60, 500, 420);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    char hdr[96];
    snprintf(hdr, sizeof(hdr), "Gold: %d", state->gold);
    info_label_ = window_->AddWidget<Label>(hdr, 10, 4, 0xff88ff88);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 480, 320);
    overview_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    furniture_grid_ = window_->AddWidget<Grid>(4, 4, 100, 28, 14, 80);
    furniture_grid_->SetPadding(3);
    auto* decor_lbl = window_->AddWidget<Label>(
        "Decoration Mode\n\nSelect furniture and place in your house.\nUse Place Furniture button.",
        14, 56, 0xffcccccc);

    tabs_->OnEvent([this, state, decor_lbl](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            overview_label_->SetVisible(e.int_value == 0);
            furniture_grid_->SetVisible(e.int_value == 1);
            decor_lbl->SetVisible(e.int_value == 2);
            Refresh(state);
        }
    });
    furniture_grid_->SetVisible(false);
    decor_lbl->SetVisible(false);

    auto* tmpl_prev = window_->AddWidget<Button>("< Tmpl", 10, 360, 70, 24);
    tmpl_prev->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            selected_template_--;
            ClampTemplateSelection(state);
            Refresh(state);
        }
    });
    auto* tmpl_next = window_->AddWidget<Button>("Tmpl >", 85, 360, 70, 24);
    tmpl_next->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            selected_template_++;
            ClampTemplateSelection(state);
            Refresh(state);
        }
    });

    auto* buy_btn = window_->AddWidget<Button>("Buy House", 165, 360, 100, 24);
    buy_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    buy_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            online_mode_ = !state->offline_mode && !state->network_house_templates.empty();
            if (buy_fn_ && online_mode_) {
                if (selected_template_ >= 0
                    && selected_template_ < static_cast<int>(state->network_house_templates.size())) {
                    buy_fn_(state->network_house_templates[static_cast<size_t>(selected_template_)].template_id);
                }
            } else if (state->gold >= 10000) {
                state->gold -= 10000;
                state->chat_messages.push_back("Congratulations! You bought a house (offline)!");
                Refresh(state);
            } else {
                state->chat_messages.push_back("Not enough gold! Need 10,000g.");
            }
        }
    });

    auto* enter_btn = window_->AddWidget<Button>("Enter", 275, 360, 70, 24);
    enter_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            online_mode_ = !state->offline_mode && !state->network_houses.empty();
            uint32_t hid = selected_house_id_;
            if (!hid && !state->network_houses.empty())
                hid = state->network_houses[0].house_id;
            if (enter_fn_ && online_mode_ && hid != 0) {
                enter_fn_(hid);
            } else if (info_label_) {
                info_label_->SetText("Entered your house (offline demo).");
            }
        }
    });

    auto* place_btn = window_->AddWidget<Button>("Place Table", 355, 360, 110, 24);
    place_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            online_mode_ = !state->offline_mode && !state->network_houses.empty();
            uint32_t hid = selected_house_id_;
            if (!hid && !state->network_houses.empty())
                hid = state->network_houses[0].house_id;
            if (place_fn_ && online_mode_ && hid != 0) {
                float x = static_cast<float>((state->network_houses.size() % 5) * 2.0f);
                float y = static_cast<float>((state->network_houses.size() % 3) * 2.0f);
                place_fn_(hid, place_item_id_, x, y);
            } else {
                state->chat_messages.push_back("Placed Wooden Table (offline demo).");
                Refresh(state);
            }
        }
    });

    // --- Warehouse Tab ---
    auto* wh_panel = new Panel(0, 0, 480, 340);

    wh_gold_label_ = wh_panel->AddWidget<Label>("Warehouse Gold: 0", 10, 4, ColorPalette::TEXT_GOLD);
    wh_gold_label_->SetRect(10, 4, 300, 20);

    warehouse_grid_ = wh_panel->AddWidget<Grid>(5, 6, 60, 50, 10, 28);
    warehouse_grid_->SetPadding(4);

    // Warehouse item deposit/withdraw
    auto* wh_lbl1 = wh_panel->AddWidget<Label>("Item Slot:", 10, 260, ColorPalette::BTN_NORMAL);
    wh_lbl1->SetRect(10, 260, 70, 20);
    wh_item_input_ = wh_panel->AddWidget<InputField>(80, 258, 50, 22);
    wh_item_input_->SetPlaceholder("0");
    wh_item_input_->SetValidation(InputValidation::PositiveInteger);

    auto* wh_dep_btn = wh_panel->AddWidget<Button>("Deposit", 140, 258, 70, 22);
    wh_dep_btn->SetColors({40,80,40,220}, {60,120,60,220}, {30,50,30,220});
    wh_dep_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoDepositItem();
    });

    auto* wh_wd_btn = wh_panel->AddWidget<Button>("Withdraw", 220, 258, 80, 22);
    wh_wd_btn->SetColors({80,40,40,220}, {120,60,60,220}, {50,30,30,220});
    wh_wd_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoWithdrawItem();
    });

    // Warehouse gold deposit/withdraw
    auto* wh_lbl2 = wh_panel->AddWidget<Label>("Gold:", 10, 286, ColorPalette::BTN_NORMAL);
    wh_lbl2->SetRect(10, 286, 70, 20);
    wh_gold_input_ = wh_panel->AddWidget<InputField>(60, 284, 80, 22);
    wh_gold_input_->SetPlaceholder("0");
    wh_gold_input_->SetValidation(InputValidation::PositiveInteger);

    auto* wh_dep_gold = wh_panel->AddWidget<Button>("Deposit Gold", 150, 284, 100, 22);
    wh_dep_gold->SetColors({40,80,40,220}, {60,120,60,220}, {30,50,30,220});
    wh_dep_gold->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoDepositGold();
    });

    auto* wh_wd_gold = wh_panel->AddWidget<Button>("Withdraw Gold", 260, 284, 110, 22);
    wh_wd_gold->SetColors({80,40,40,220}, {120,60,60,220}, {50,30,30,220});
    wh_wd_gold->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoWithdrawGold();
    });

    tabs_->AddTab("Warehouse", wh_panel);

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 10, 390, 80, 24);
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && refresh_fn_) refresh_fn_();
    });

    Refresh(state);
}

void HousingDialog::Refresh(GameState* state) {
    if (!state) return;
    online_mode_ = !state->offline_mode && !state->network_house_templates.empty();
    if (state->network_selected_house_id) selected_house_id_ = state->network_selected_house_id;
    ClampTemplateSelection(state);

    if (info_label_) {
        char hdr[96];
        snprintf(hdr, sizeof(hdr), "Gold: %d", state->gold);
        info_label_->SetText(hdr);
    }

    if (overview_label_) {
        std::string text = "=== Housing ===\n\n";
        if (online_mode_) {
            text += state->network_can_buy_house ? "Status: No house — pick a template below\n\n"
                                                 : "Status: You own a house\n\n";
            if (!state->network_houses.empty()) {
                text += "=== Your Houses ===\n";
                for (const auto& h : state->network_houses) {
                    char buf[256];
                    snprintf(buf, sizeof(buf),
                        "  #%u %s — map %u, furniture %u/%u\n",
                        h.house_id, h.name.c_str(), h.map_id,
                        h.furniture_count, h.max_furniture);
                    text += buf;
                }
                text += "\n";
            }
            text += "=== Available Templates ===\n";
            for (size_t i = 0; i < state->network_house_templates.size(); ++i) {
                const auto& t = state->network_house_templates[i];
                const char* mark = (static_cast<int>(i) == selected_template_) ? ">>" : "  ";
                char buf[256];
                snprintf(buf, sizeof(buf),
                    "%s [%zu] %s — %u gold (%u slots, map %u)\n",
                    mark, i + 1, t.name.c_str(), t.price, t.max_furniture, t.map_id);
                text += buf;
            }
        } else {
            text += "Offline demo mode.\nBuy a house for 10,000g or connect online for server shop.\n";
        }
        overview_label_->SetText(text);
    }

    if (furniture_grid_) {
        furniture_grid_->ClearAll();
        const GameState::NetworkHouseInfo* house = SelectedHouse(state);
        int slot = 0;
        if (house && !house->furniture.empty()) {
            for (const auto& f : house->furniture) {
                if (slot >= 16) break;
                GridSlot gs;
                gs.empty = false;
                gs.text = f.name;
                furniture_grid_->SetSlot(slot / 4, slot % 4, gs);
                slot++;
            }
        } else if (!online_mode_) {
            const char* sample[] = {"Wooden Table", "Comfy Chair", "Lamp", "Rug",
                "Bookshelf", "Bed", "Desk", "Cabinet"};
            for (int i = 0; i < 8; ++i) {
                GridSlot gs;
                gs.empty = false;
                gs.text = sample[i];
                furniture_grid_->SetSlot(i / 4, i % 4, gs);
            }
        }
    }
}

void HousingDialog::RefreshWarehouse(GameState* state) {
    if (!warehouse_grid_ || !state) return;

    warehouse_grid_->ClearAll();
    if (wh_gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Warehouse Gold: %u", state->storage_gold);
        wh_gold_label_->SetText(buf);
    }

    // Display sample warehouse items
    for (size_t i = 0; i < state->storage_items.size() && i < 30; i++) {
        auto& si = state->storage_items[i];
        if (si.id > 0) {
            GridSlot gs;
            gs.empty = false;
            gs.count = si.count;
            gs.text = si.name.empty() ? ("Item_" + std::to_string(si.id)) : si.name;
            gs.userdata = static_cast<int>(si.id);
            int row = static_cast<int>(i / 5);
            int col = static_cast<int>(i % 5);
            warehouse_grid_->SetSlot(row, col, gs);
        }
    }
}

void HousingDialog::DoDepositItem() {
    if (!wh_item_input_) return;
    int slot = std::atoi(wh_item_input_->GetText().c_str());
    if (slot >= 0 && wh_deposit_item_) {
        wh_deposit_item_(selected_house_id_, slot, 1);
        wh_item_input_->SetText("");
    }
}

void HousingDialog::DoWithdrawItem() {
    if (!wh_item_input_) return;
    int slot = std::atoi(wh_item_input_->GetText().c_str());
    if (slot >= 0 && wh_withdraw_item_) {
        wh_withdraw_item_(selected_house_id_, slot, 1);
        wh_item_input_->SetText("");
    }
}

void HousingDialog::DoDepositGold() {
    if (!wh_gold_input_) return;
    uint32_t amount = static_cast<uint32_t>(std::atoi(wh_gold_input_->GetText().c_str()));
    if (amount > 0 && wh_deposit_gold_) {
        wh_deposit_gold_(amount);
        wh_gold_input_->SetText("");
    }
}

void HousingDialog::DoWithdrawGold() {
    if (!wh_gold_input_) return;
    uint32_t amount = static_cast<uint32_t>(std::atoi(wh_gold_input_->GetText().c_str()));
    if (amount > 0 && wh_withdraw_gold_) {
        wh_withdraw_gold_(amount);
        wh_gold_input_->SetText("");
    }
}

void HousingDialog::UpdateFromState(GameState* state) {
    Refresh(state);
}
