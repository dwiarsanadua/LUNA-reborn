#include "StallOptionDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void StallOptionDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/StallOptionDlg.bin.txt",
            "Stall Options", 250, 120, 320, 320);
    } else {
        window_ = new Window("Stall Options", 250, 120, 320, 320);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    int y = 28;

    // Stall Title
    window_->AddWidget<Label>("Stall Title:", 14, y, ColorPalette::BTN_NORMAL)->SetRect(14, y, 100, 20);
    y += 20;
    title_input_ = window_->AddWidget<InputField>(14, y, 240, 22);
    title_input_->SetPlaceholder("Enter stall name...");
    if (state && state->my_stall.open) title_input_->SetText(state->my_stall.title);
    y += 28;

    auto* set_title_btn = window_->AddWidget<Button>("Set Title", 14, y, 100, 24);
    set_title_btn->SetColors({50,50,80,220}, {80,80,130,220}, {30,30,50,220});
    set_title_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSetTitle();
    });
    y += 30;

    // Greeting Message
    window_->AddWidget<Label>("Greeting:", 14, y, ColorPalette::BTN_NORMAL)->SetRect(14, y, 100, 20);
    y += 20;
    greeting_input_ = window_->AddWidget<InputField>(14, y, 240, 22);
    greeting_input_->SetPlaceholder("Welcome to my stall!");
    y += 28;

    auto* set_greet_btn = window_->AddWidget<Button>("Set Greeting", 14, y, 110, 24);
    set_greet_btn->SetColors({50,50,80,220}, {80,80,130,220}, {30,30,50,220});
    set_greet_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSetGreeting();
    });
    y += 30;

    // Tax Rate
    window_->AddWidget<Label>("Tax Rate (%):", 14, y, ColorPalette::BTN_NORMAL)->SetRect(14, y, 100, 20);
    y += 20;
    tax_input_ = window_->AddWidget<InputField>(14, y, 80, 22);
    tax_input_->SetPlaceholder("0");
    tax_input_->SetValidation(InputValidation::PositiveInteger);

    auto* set_tax_btn = window_->AddWidget<Button>("Set Tax", 100, y, 80, 22);
    set_tax_btn->SetColors({50,50,80,220}, {80,80,130,220}, {30,30,50,220});
    set_tax_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSetTax();
    });
    y += 30;

    // Decoration presets
    window_->AddWidget<Label>("Decoration:", 14, y, ColorPalette::BTN_NORMAL)->SetRect(14, y, 100, 20);
    y += 20;

    const char* deco_names[] = {"Basic", "Fancy", "Premium", "Event"};
    for (int i = 0; i < 4; i++) {
        float bx = 14 + (i % 2) * 130;
        float by = y + (i / 2) * 28;
        auto* deco_btn = window_->AddWidget<Button>(deco_names[i], bx, by, 120, 24);
        deco_btn->SetColors({60,50,70,220}, {100,80,120,220}, {40,30,50,220});
        deco_btn->OnEvent([this, i](const UIEvent& e) {
            if (e.type == UIEvent::Click) DoSetDecoration(i);
        });
    }
    y += 62;

    // Open / Close buttons
    auto* open_btn = window_->AddWidget<Button>("Open Stall", 14, y, 130, 26);
    open_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    open_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoOpenStall();
    });

    auto* close_btn = window_->AddWidget<Button>("Close Stall", 162, y, 130, 26);
    close_btn->SetColors({80,40,40,220}, {130,60,60,220}, {50,30,30,220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoCloseStall();
    });

    status_label_ = window_->AddWidget<Label>("", 14, y + 30, ColorPalette::TEXT_GOLD);
    status_label_->SetRect(14, y + 30, 280, 18);
    RefreshStatus(state);
}

void StallOptionDialog::Close() {
    window_ = nullptr;
}

void StallOptionDialog::DoOpenStall() {
    if (callbacks_.open_stall) {
        callbacks_.open_stall();
        if (status_label_) status_label_->SetText("Stall opened!");
    }
}

void StallOptionDialog::DoCloseStall() {
    if (callbacks_.close_stall) {
        callbacks_.close_stall();
        if (status_label_) status_label_->SetText("Stall closed.");
    }
}

void StallOptionDialog::DoSetTitle() {
    if (!title_input_) return;
    std::string title = title_input_->GetText();
    if (title.empty()) return;
    if (callbacks_.set_title) {
        callbacks_.set_title(title);
        if (status_label_) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Title set to: %s", title.c_str());
            status_label_->SetText(buf);
        }
    }
}

void StallOptionDialog::DoSetGreeting() {
    if (!greeting_input_) return;
    std::string msg = greeting_input_->GetText();
    if (msg.empty()) return;
    if (callbacks_.set_greeting) {
        callbacks_.set_greeting(msg);
        if (status_label_) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Greeting set");
            status_label_->SetText(buf);
        }
    }
}

void StallOptionDialog::DoSetTax() {
    if (!tax_input_) return;
    uint32_t rate = static_cast<uint32_t>(std::atoi(tax_input_->GetText().c_str()));
    if (rate > 100) rate = 100;
    if (callbacks_.set_tax_rate) {
        callbacks_.set_tax_rate(rate);
        if (status_label_) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Tax rate: %u%%", rate);
            status_label_->SetText(buf);
        }
    }
}

void StallOptionDialog::DoSetDecoration(int idx) {
    if (callbacks_.set_decoration) {
        callbacks_.set_decoration(static_cast<uint8_t>(idx));
        if (status_label_) {
            const char* names[] = {"Basic", "Fancy", "Premium", "Event"};
            char buf[64];
            snprintf(buf, sizeof(buf), "Decoration: %s", names[idx % 4]);
            status_label_->SetText(buf);
        }
    }
}

void StallOptionDialog::RefreshStatus(GameState* state) {
    if (!status_label_ || !state) return;
    if (state->my_stall.open) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Stall OPEN — %zu items", state->my_stall.items.size());
        status_label_->SetText(buf);
    } else {
        status_label_->SetText("Stall CLOSED");
    }
}

void StallOptionDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    RefreshStatus(state);
    if (window_ && !window_->IsVisible()) Close();
}
