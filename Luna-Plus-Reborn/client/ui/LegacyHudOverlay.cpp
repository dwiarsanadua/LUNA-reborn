#include "LegacyHudOverlay.hpp"
#include <ui/widgets/Label.hpp>
#include <ui/widgets/GaugeBar.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/InputField.hpp>
#include <cstdio>

void LegacyHudOverlay::Init(float screen_w, float screen_h) {
    char_gage_ = hud_wm_.LoadFromScript("assets/interface/Windows/CharGage.bin.txt");
    if (char_gage_) {
        char_gage_->SetRect(0, 0, char_gage_->GetW(), char_gage_->GetH());
        char_gage_->SetMovable(true);
        char_gage_->SetClosable(false);
    }

    quick_slot_ = hud_wm_.LoadFromScript("assets/interface/Windows/QuickSlot.bin.txt");
    if (quick_slot_) {
        float qw = quick_slot_->GetW() > 0 ? quick_slot_->GetW() : 450;
        float qh = quick_slot_->GetH() > 0 ? quick_slot_->GetH() : 50;
        quick_slot_->SetRect((screen_w - qw) * 0.5f, screen_h - qh - 8, qw, qh);
        quick_slot_->SetMovable(false);
        quick_slot_->SetClosable(false);
    }

    chat_window_ = hud_wm_.LoadFromScript("assets/interface/Windows/ChatWindow.bin.txt");
    if (chat_window_) {
        float cw = chat_window_->GetW() > 0 ? chat_window_->GetW() : 388;
        float ch = chat_window_->GetH() > 0 ? chat_window_->GetH() : 159;
        chat_window_->SetRect(0, screen_h - ch - 60, cw, ch);
        chat_window_->SetMovable(true);
        chat_window_->SetClosable(false);
        chat_window_->SetVisible(chat_visible_);
    }

    active_ = char_gage_ != nullptr || chat_window_ != nullptr;
}

void LegacyHudOverlay::Shutdown() {
    char_gage_ = nullptr;
    quick_slot_ = nullptr;
    chat_window_ = nullptr;
    active_ = false;
    quickslot_wired_ = false;
    last_chat_count_ = 0;
}

void LegacyHudOverlay::WireQuickSlot() {
    if (!quick_slot_ || quickslot_wired_ || !hotbar_handler_) return;
    for (const auto& w : quick_slot_->GetWidgets()) {
        if (auto* grid = dynamic_cast<Grid*>(w.get())) {
            grid->OnSlotEvent([this](int /*row*/, int col, const UIEvent& e) {
                if (e.type == UIEvent::Click && hotbar_handler_)
                    hotbar_handler_(col);
            });
        }
    }
    quickslot_wired_ = true;
}

void LegacyHudOverlay::SyncChatList(GameState* state) {
    if (!chat_window_ || !state) return;
    if (state->chat_messages.size() == last_chat_count_) return;
    if (auto* list = chat_window_->FindWidget<ListBox>("CHAT_LIST")) {
        list->Clear();
        for (const auto& line : state->chat_messages)
            list->AddItem(line);
    }
    last_chat_count_ = state->chat_messages.size();
}

void LegacyHudOverlay::SyncChatInput(GameState* state) {
    if (!chat_window_ || !state) return;
    if (auto* edit = chat_window_->FindWidget<InputField>("CHAT_EDITBOX")) {
        if (state->chat_open) {
            edit->SetFocused(true);
            if (edit->GetText() != state->chat_input)
                edit->SetText(state->chat_input);
        } else {
            edit->SetFocused(false);
        }
    }
}

void LegacyHudOverlay::ToggleChat() {
    chat_visible_ = !chat_visible_;
    if (chat_window_) chat_window_->SetVisible(chat_visible_);
}

void LegacyHudOverlay::Update(GameState* state, float screen_w, float screen_h) {
    if (!active_ || !state) return;

    if (quick_slot_) {
        float qw = quick_slot_->GetW() > 0 ? quick_slot_->GetW() : 450;
        float qh = quick_slot_->GetH() > 0 ? quick_slot_->GetH() : 50;
        quick_slot_->SetRect((screen_w - qw) * 0.5f, screen_h - qh - 8, qw, qh);
    }
    if (chat_window_ && chat_visible_) {
        float cw = chat_window_->GetW() > 0 ? chat_window_->GetW() : 388;
        float ch = chat_window_->GetH() > 0 ? chat_window_->GetH() : 159;
        chat_window_->SetRect(0, screen_h - ch - 60, cw, ch);
    }

    if (char_gage_) {
        if (auto* name = char_gage_->FindWidget<Label>("CG_GUAGENAME"))
            name->SetText(state->name);
        if (auto* lvl = char_gage_->FindWidget<Label>("CG_GUAGELEVEL")) {
            char buf[16]; snprintf(buf, sizeof(buf), "%d", state->level);
            lvl->SetText(buf);
        }

        float hp_pct = state->max_hp > 0 ? (float)state->hp / state->max_hp : 0;
        float mp_pct = state->max_mp > 0 ? (float)state->mp / state->max_mp : 0;
        int exp_next = state->level * 100 + 500;
        float exp_pct = exp_next > 0 ? (float)state->exp / exp_next : 0;

        if (auto* hp = char_gage_->FindWidget<GaugeBar>("CG_GUAGELIFE")) {
            hp->SetProgress(hp_pct);
            // Old HP bar shifts red -> yellow as health drops
            hp->SetFillColor(hp_pct > 0.5f ? UIColor{220, 50, 50, 255}
                            : hp_pct > 0.25f ? UIColor{230, 170, 40, 255}
                                             : UIColor{255, 60, 60, 255});
        }
        if (auto* mp = char_gage_->FindWidget<GaugeBar>("CG_GUAGEMANA")) {
            mp->SetProgress(mp_pct);
            mp->SetFillColor({60, 110, 240, 255});
        }
        if (auto* xp = char_gage_->FindWidget<GaugeBar>("CG_GUAGEEXPPOINT")) {
            xp->SetProgress(exp_pct);
            xp->SetFillColor({250, 210, 60, 255});
        }

        char hp_txt[32], mp_txt[32];
        snprintf(hp_txt, sizeof(hp_txt), "%d / %d", state->hp, state->max_hp);
        snprintf(mp_txt, sizeof(mp_txt), "%d / %d", state->mp, state->max_mp);
        if (auto* t = char_gage_->FindWidget<Label>("CG_GUAGELIFETEXT")) t->SetText(hp_txt);
        if (auto* t = char_gage_->FindWidget<Label>("CG_GUAGEMANATEXT")) t->SetText(mp_txt);
    }

    WireQuickSlot();
    SyncChatList(state);
    SyncChatInput(state);
    hud_wm_.Update(0, 0, 0, false, false);
}

void LegacyHudOverlay::Render(UIRenderer& ui) {
    if (!active_) return;
    hud_wm_.Render(ui);
}
