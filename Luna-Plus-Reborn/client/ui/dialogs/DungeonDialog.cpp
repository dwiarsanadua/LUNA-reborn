#include "DungeonDialog.hpp"
#include <cstdio>

void DungeonDialog::Open(GameState* state, WindowManager* wm, DungeonSystem* dungeon) {
    dungeon_ = dungeon;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/LimitDungeonDlg.bin.txt",
        "Dungeons", 150, 60, 500, 440);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    info_label_ = window_->AddWidget<Label>("", 10, 4, 0xffffcc88);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 480, 360);

    // Tab 0: Available Dungeons
    tab0_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 1: Active Instance
    tab1_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 2: Leaderboard
    tab2_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            tab0_label_->SetVisible(e.int_value == 0);
            tab1_label_->SetVisible(e.int_value == 1);
            tab2_label_->SetVisible(e.int_value == 2);
        }
    });
    tab1_label_->SetVisible(false);
    tab2_label_->SetVisible(false);

    // Action buttons
    auto* enter_btn = window_->AddWidget<Button>("Enter Dungeon", 14, 400, 130, 24);
    enter_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    enter_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && dungeon_) {
            auto dungeons = dungeon_->GetAvailableDungeons();
            if (!dungeons.empty()) {
                auto& d = dungeons[0];
                std::vector<uint32_t> members = {(uint32_t)state->selected_char};
                uint32_t inst_id = dungeon_->CreateInstance(d.map_id, state->selected_char, members);
                dungeon_->EnterInstance(inst_id, state->selected_char);
                state->in_dungeon = true;
                state->dungeon_map = d.map_id;
                char buf[128];
                snprintf(buf, sizeof(buf), "Entering %s! (Instance #%u)", d.name.c_str(), inst_id);
                state->chat_messages.push_back(buf);
                Refresh(state);
            }
        }
    });

    auto* leave_btn = window_->AddWidget<Button>("Leave Dungeon", 160, 400, 130, 24);
    leave_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    leave_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && dungeon_ && state->in_dungeon) {
            uint32_t inst_id = dungeon_->FindInstanceByMember(state->selected_char);
            if (inst_id > 0) {
                dungeon_->LeaveInstance(inst_id, state->selected_char);
                state->in_dungeon = false;
                state->dungeon_map = 0;
                state->chat_messages.push_back("Left the dungeon.");
                Refresh(state);
            }
        }
    });

    auto* complete_btn = window_->AddWidget<Button>("Complete (Test)", 310, 400, 130, 24);
    complete_btn->SetColors({40,40,100,220}, {80,80,160,220}, {30,30,70,220});
    complete_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && dungeon_) {
            uint32_t inst_id = dungeon_->FindInstanceByMember(state->selected_char);
            if (inst_id > 0 && state->in_dungeon) {
                dungeon_->CompleteInstance(inst_id);
                state->in_dungeon = false;
                state->dungeon_map = 0;
                state->chat_messages.push_back("Dungeon completed! Rewards: +500 XP, +200 Gold!");
                state->exp = std::min(state->exp + 500, state->exp_next);
                state->gold += 200;
                Refresh(state);
            }
        }
    });

    Refresh(state);
}

void DungeonDialog::Refresh(GameState* state) {
    if (!dungeon_) return;
    char buf[1024];

    // Tab 0: Available Dungeons
    auto defs = dungeon_->GetAvailableDungeons();
    if (!defs.empty()) {
        std::string dl;
        for (auto& d : defs) {
            dl += d.name + " (Lv." + std::to_string(d.min_level) + "-" +
                   std::to_string(d.max_level) + ") | Party: " +
                   std::to_string(d.party_size_min) + "-" + std::to_string(d.party_size_max) + "\n";
            dl += "  Time: " + std::to_string((int)d.time_limit / 60) + "min | " +
                  "XP: " + std::to_string(d.xp_reward) + " | Gold: " + std::to_string(d.gold_reward) + "\n\n";
        }
        snprintf(buf, sizeof(buf), "%s", dl.c_str());
    } else {
        snprintf(buf, sizeof(buf), "No dungeons available.\nCheck your level requirements.");
    }
    tab0_label_->SetText(buf);

    // Tab 1: Active Instance
    if (state->in_dungeon) {
        uint32_t inst_id = dungeon_->FindInstanceByMember(state->selected_char);
        auto* inst = dungeon_->GetInstance(inst_id);
        if (inst) {
            snprintf(buf, sizeof(buf),
                "Active Instance #%u\n"
                "Map: %s\n"
                "Time: %.0f/%.0f sec\n"
                "Party: %zu members\n"
                "Status: %s",
                inst->id, inst->name.c_str(), inst->elapsed, inst->time_limit,
                inst->member_ids.size(),
                inst->completed ? "Completed" : inst->failed ? "Failed" : "In Progress");
        } else {
            snprintf(buf, sizeof(buf), "Instance not found.");
        }
    } else {
        snprintf(buf, sizeof(buf), "Not in a dungeon.\n\nEnter a dungeon to see instance details.");
    }
    tab1_label_->SetText(buf);

    // Tab 2: Leaderboard
    auto lb = dungeon_->GetLeaderboard(0, 5);
    if (!lb.empty()) {
        std::string ll;
        for (auto& e : lb) {
            ll += e.party_name + " - " + std::to_string((int)e.clear_time) + "s\n";
        }
        snprintf(buf, sizeof(buf), "%s", ll.c_str());
    } else {
        snprintf(buf, sizeof(buf), "No leaderboard entries yet.\nComplete a dungeon to set a record!");
    }
    tab2_label_->SetText(buf);
}

void DungeonDialog::UpdateFromState(GameState* state) {
    if (window_ && state->dungeon_open) Refresh(state);
}
