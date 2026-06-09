#include "PartyMatchingDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void PartyMatchingDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/PartyMatchingDlg.bin.txt",
        "Party Matching", 200, 100, 400, 350);
    window_->SetClosable(true);
    window_->SetMovable(true);

    info_label_ = window_->AddWidget<Label>("Party Recruitment Board", 20, 10, ColorPalette::TEXT_GOLD);

    tabs_ = window_->AddWidget<TabPanel>(10, 35, 380, 260);

    list_grid_ = new Grid(1, 8, 370, 28, 0, 0);
    tabs_->AddTab("Recruiting", list_grid_);

    window_->AddWidget<Button>("Create Party", 20, 310, 120, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoCreate();
    });

    window_->AddWidget<Button>("Refresh", 160, 310, 100, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) RebuildListGrid();
    });

    window_->AddWidget<Button>("Close", 300, 310, 80, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void PartyMatchingDialog::SetListings(const std::vector<PartyListing>& listings) {
    listings_ = listings;
    RebuildListGrid();
}

void PartyMatchingDialog::SetApplyCallback(std::function<void(uint32_t)> on_apply) {
    on_apply_ = on_apply;
}

void PartyMatchingDialog::SetCreateCallback(std::function<void(const std::string&, uint16_t, uint16_t)> on_create) {
    on_create_ = on_create;
}

void PartyMatchingDialog::RebuildListGrid() {
    if (!list_grid_) return;
    list_grid_->ClearAll();

    for (size_t i = 0; i < listings_.size(); i++) {
        const auto& p = listings_[i];
        char label[128];
        snprintf(label, sizeof(label), "%s - %s [Lv%d-%d] %d/%d",
                 p.title.c_str(), p.leader_name.c_str(),
                 p.min_level, p.max_level, p.member_count, p.max_members);
        GridSlot gs;
        gs.empty = false;
        gs.text = label;
        gs.userdata = (int)p.party_id;
        list_grid_->SetSlot((int)i, 0, gs);
    }

    list_grid_->OnSlotEvent([this](int row, int, const UIEvent& e) {
        if (e.type == UIEvent::Click) DoApply(row);
    });
}

void PartyMatchingDialog::DoApply(int row) {
    if (row >= 0 && row < (int)listings_.size() && on_apply_) {
        on_apply_(listings_[row].party_id);
    }
}

void PartyMatchingDialog::DoCreate() {
    if (on_create_) on_create_("Party", 1, 999);
}

void PartyMatchingDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void PartyMatchingDialog::Close() {
    window_ = nullptr;
    tabs_ = nullptr;
    list_grid_ = nullptr;
    info_label_ = nullptr;
    listings_.clear();
}
