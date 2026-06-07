#include "FamilyDialog.hpp"
#include <cstdio>

void FamilyDialog::Open(GameState* state, WindowManager* wm, FamilySystem* family) {
    family_ = family;
    window_ = wm->Open("Family & Couple", 200, 100, 420, 360);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Family & Couple System", 10, 4, 0xff88ccff);
    (void)title;

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 400, 280);

    // Tab 0: My Status
    tab1_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 1: Family Members
    tab2_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 2: Proposals
    tab3_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Proposal actions
    auto* propose_btn = window_->AddWidget<Button>("Propose to Partner", 14, 310, 140, 24);
    propose_btn->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    propose_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (family_) {
                family_->Propose(state->selected_char, state->name, 999, "NPC_Sweetheart");
                state->chat_messages.push_back("Proposal sent to NPC_Sweetheart!");
                Refresh(state);
            }
        }
    });

    auto* accept_btn = window_->AddWidget<Button>("Accept Proposal", 164, 310, 130, 24);
    accept_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    accept_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (family_ && family_->IsEngaged(state->selected_char)) {
                family_->AcceptProposal(state->selected_char);
                state->chat_messages.push_back("You are now married! Congratulations!");
                Refresh(state);
            }
        }
    });

    auto* divorce_btn = window_->AddWidget<Button>("Divorce", 304, 310, 100, 24);
    divorce_btn->SetColors({80,30,30,220}, {130,60,60,220}, {50,20,20,220});
    divorce_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (family_ && family_->IsMarried(state->selected_char)) {
                family_->Divorce(state->selected_char);
                state->chat_messages.push_back("You are now divorced.");
                Refresh(state);
            }
        }
    });

    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            tab1_label_->SetVisible(e.int_value == 0);
            tab2_label_->SetVisible(e.int_value == 1);
            tab3_label_->SetVisible(e.int_value == 2);
        }
    });

    Refresh(state);
}

void FamilyDialog::Refresh(GameState* state) {
    if (!family_) return;

    char buf[512];

    // Tab 0: My Status
    auto* me = family_->GetMember(state->selected_char);
    if (me) {
        snprintf(buf, sizeof(buf),
            "Status: %s\n"
            "Partner: %s (ID: %u)\n"
            "Family: %s (ID: %u)\n"
            "Married: %s\n"
            "Children: %zu",
            family_->GetRelationName(me->relation).c_str(),
            me->partner_name.empty() ? "None" : me->partner_name.c_str(),
            me->partner_id,
            me->family_name.empty() ? "None" : me->family_name.c_str(),
            me->family_id,
            me->married_date > 0 ? "Yes" : "No",
            me->children_ids.size());
    } else {
        snprintf(buf, sizeof(buf), "No relationship data.\nUse 'Propose' to start a relationship!");
    }
    tab1_label_->SetText(buf);

    // Tab 1: Family Members
    if (me && me->family_id > 0) {
        auto members = family_->GetFamilyMembers(me->family_id);
        std::string ml;
        for (auto& m : members) {
            ml += m.name + " (" + family_->GetRelationName(m.relation) + ")\n";
        }
        if (ml.empty()) ml = "No family members found.";
        snprintf(buf, sizeof(buf), "%s", ml.c_str());
    } else {
        snprintf(buf, sizeof(buf), "Not in a family. Create or join one!");
    }
    tab2_label_->SetText(buf);

    // Tab 2: Proposals
    snprintf(buf, sizeof(buf),
        "Pending proposals to you:\n"
        "%s\n\n"
        "Use 'Accept Proposal' to accept,\n"
        "or 'Propose' to send one!",
        family_->IsEngaged(state->selected_char) ? "From: NPC_Sweetheart (ID: 999)" : "No pending proposals");
    tab3_label_->SetText(buf);
}

void FamilyDialog::UpdateFromState(GameState* state) {
    if (window_ && state->family_open) {
        Refresh(state);
    }
}
