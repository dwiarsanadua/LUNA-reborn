#include "TargetCharacterDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

const char* TargetCharacterDialog::ClassName(int class_id) {
    static const char* names[] = {
        "Fighter", "Mage", "Archer", "Paladin", "Assassin",
        "Ranger", "Wizard", "Knight", "Bard", "Druid"
    };
    if (class_id >= 0 && class_id < 10) return names[class_id];
    return "Unknown";
}

void TargetCharacterDialog::Open(WindowManager* wm) {
    if (window_) return;
    CreateUI(wm);
}

void TargetCharacterDialog::CreateUI(WindowManager* wm) {
    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/TargetCharacterDlg.bin.txt",
            "Target Info", 0, 0, 260, 200, false);
    } else {
        window_ = new Window("Target Info", 0, 0, 260, 200);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(20);

    int y = 24;
    name_label_ = window_->AddWidget<Label>("", 12, y, ColorPalette::TEXT_TITLE);
    name_label_->SetRect(12, y, 236, 20);
    y += 22;

    level_label_ = window_->AddWidget<Label>("", 12, y, 0xffffff88);
    level_label_->SetRect(12, y, 120, 18);
    y += 20;

    class_label_ = window_->AddWidget<Label>("", 140, y - 18, ColorPalette::BTN_NORMAL);
    class_label_->SetRect(140, y - 18, 108, 18);

    hp_label_ = window_->AddWidget<Label>("HP: 0/0", 12, y, 0xffff8888);
    hp_label_->SetRect(12, y, 236, 18);
    y += 20;

    guild_label_ = window_->AddWidget<Label>("", 12, y, ColorPalette::TEXT_GOLD);
    guild_label_->SetRect(12, y, 236, 18);
    y += 18;

    title_label_ = window_->AddWidget<Label>("", 12, y, 0xff88ff88);
    title_label_->SetRect(12, y, 236, 18);
    y += 24;

    // Action buttons
    auto* whisper_btn = window_->AddWidget<Button>("Whisper", 12, y, 72, 22);
    whisper_btn->SetColors({50,50,80,220}, {80,80,130,220}, {30,30,50,220});
    whisper_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_whisper_ && has_target_)
            on_whisper_(target_.entity_id);
    });

    auto* party_btn = window_->AddWidget<Button>("Party", 92, y, 72, 22);
    party_btn->SetColors({40,70,40,220}, {70,120,70,220}, {30,50,30,220});
    party_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_party_invite_ && has_target_)
            on_party_invite_(target_.entity_id);
    });

    auto* friend_btn = window_->AddWidget<Button>("Add Friend", 172, y, 76, 22);
    friend_btn->SetColors({40,40,80,220}, {70,70,130,220}, {30,30,50,220});
    friend_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_add_friend_ && has_target_)
            on_add_friend_(target_.entity_id);
    });

    y += 26;
    auto* trade_btn = window_->AddWidget<Button>("Request Trade", 12, y, 236, 22);
    trade_btn->SetColors({80,60,40,220}, {130,100,70,220}, {50,40,30,220});
    trade_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_trade_request_ && has_target_)
            on_trade_request_(target_.entity_id);
    });

    UpdateDisplay();
}

void TargetCharacterDialog::Close() {
    window_ = nullptr;
    has_target_ = false;
}

void TargetCharacterDialog::SetTarget(const TargetInfo& info) {
    target_ = info;
    has_target_ = true;
    UpdateDisplay();
    spdlog::debug("TargetCharacterDialog: target set to {} (Lv.{})", info.name, info.level);
}

void TargetCharacterDialog::ClearTarget() {
    has_target_ = false;
    if (window_) {
        window_->SetVisible(false);
    }
}

void TargetCharacterDialog::SetNetworkCallbacks(
    std::function<void(uint32_t)> whisper,
    std::function<void(uint32_t)> party_invite,
    std::function<void(uint32_t)> add_friend,
    std::function<void(uint32_t)> trade_request) {
    on_whisper_ = whisper;
    on_party_invite_ = party_invite;
    on_add_friend_ = add_friend;
    on_trade_request_ = trade_request;
}

void TargetCharacterDialog::UpdateDisplay() {
    if (!window_) return;
    if (!has_target_) {
        window_->SetVisible(false);
        return;
    }
    window_->SetVisible(true);

    char buf[128];

    if (name_label_) {
        snprintf(buf, sizeof(buf), "%s%s", target_.name.c_str(),
                 target_.is_pk ? " [PK]" : "");
        name_label_->SetText(buf);
    }
    if (level_label_) {
        snprintf(buf, sizeof(buf), "Level %d", target_.level);
        level_label_->SetText(buf);
    }
    if (class_label_) {
        class_label_->SetText(ClassName(target_.class_id));
    }
    if (hp_label_) {
        int hp_pct = target_.max_hp > 0 ? (target_.hp * 100 / target_.max_hp) : 0;
        snprintf(buf, sizeof(buf), "HP: %d/%d (%d%%)", target_.hp, target_.max_hp, hp_pct);
        hp_label_->SetText(buf);
    }
    if (guild_label_) {
        if (!target_.guild_name.empty()) {
            snprintf(buf, sizeof(buf), "<%s>", target_.guild_name.c_str());
            guild_label_->SetText(buf);
        } else {
            guild_label_->SetText("");
        }
    }
    if (title_label_) {
        if (!target_.title.empty()) {
            snprintf(buf, sizeof(buf), "\"%s\"", target_.title.c_str());
            title_label_->SetText(buf);
        } else {
            title_label_->SetText("");
        }
    }
}

void TargetCharacterDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;

    if (window_ && !window_->IsVisible()) {
        has_target_ = false;
    }
}
