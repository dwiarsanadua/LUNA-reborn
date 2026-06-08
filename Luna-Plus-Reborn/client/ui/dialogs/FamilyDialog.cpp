#include "FamilyDialog.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>

void FamilyDialog::SetNetworkCallbacks(std::function<void(const std::string&)> create_fn,
                                         std::function<void(uint32_t, const std::string&)> propose_fn,
                                         std::function<void()> accept_fn,
                                         std::function<void()> reject_fn,
                                         std::function<void()> divorce_fn,
                                         std::function<void()> leave_fn,
                                         std::function<void()> refresh_fn) {
    create_fn_ = std::move(create_fn);
    propose_fn_ = std::move(propose_fn);
    accept_fn_ = std::move(accept_fn);
    reject_fn_ = std::move(reject_fn);
    divorce_fn_ = std::move(divorce_fn);
    leave_fn_ = std::move(leave_fn);
    refresh_fn_ = std::move(refresh_fn);
}

const char* FamilyDialog::RelationLabel(uint8_t relation) {
    switch (relation) {
    case 2: return "Engaged";
    case 3: return "Married";
    case 4: return "Divorced";
    default: return "Single";
    }
}

void FamilyDialog::Open(GameState* state, WindowManager* wm, FamilySystem* family) {
    family_ = family;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Family.bin.txt",
        "Family & Couple", 180, 80, 440, 420);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Family & Couple System", 10, 4, 0xff88ccff);
    (void)title;

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 420, 260);
    tab1_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    tab2_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    tab3_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    window_->AddWidget<Label>("Family name:", 14, 300, 0xffcccccc);
    family_name_input_ = window_->AddWidget<InputField>(100, 298, 160, 22);
    family_name_input_->SetPlaceholder("MyFamily");
    family_name_input_->SetMaxLength(24);

    auto* create_btn = window_->AddWidget<Button>("Create", 270, 298, 70, 24);
    create_btn->SetColors({40,60,80,220}, {80,110,130,220}, {30,40,50,220});
    create_btn->OnEvent([this](const UIEvent& e) {
        if (e.type != UIEvent::Click || !family_name_input_) return;
        std::string name = family_name_input_->GetText();
        if (name.empty()) name = "MyFamily";
        if (create_fn_) create_fn_(name);
        else if (family_) family_->CreateFamily(0, name);
    });

    window_->AddWidget<Label>("Partner ID:", 14, 330, 0xffcccccc);
    partner_id_input_ = window_->AddWidget<InputField>(90, 328, 60, 22);
    partner_id_input_->SetPlaceholder("999");
    partner_id_input_->SetValidation(InputValidation::PositiveInteger);

    window_->AddWidget<Label>("Name:", 160, 330, 0xffcccccc);
    partner_name_input_ = window_->AddWidget<InputField>(200, 328, 100, 22);
    partner_name_input_->SetPlaceholder("Partner");

    auto* propose_btn = window_->AddWidget<Button>("Propose", 310, 328, 80, 24);
    propose_btn->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    propose_btn->OnEvent([this](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        uint32_t target_id = 999;
        if (partner_id_input_ && !partner_id_input_->GetText().empty())
            target_id = static_cast<uint32_t>(std::atoi(partner_id_input_->GetText().c_str()));
        std::string target_name = partner_name_input_ && !partner_name_input_->GetText().empty()
            ? partner_name_input_->GetText() : "Partner";
        if (propose_fn_) propose_fn_(target_id, target_name);
        else if (family_) family_->Propose(0, "Hero", target_id, target_name);
    });

    auto* accept_btn = window_->AddWidget<Button>("Accept", 14, 362, 70, 24);
    accept_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    accept_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (accept_fn_) accept_fn_();
            else if (family_) family_->AcceptProposal(0);
        }
    });

    auto* reject_btn = window_->AddWidget<Button>("Reject", 92, 362, 70, 24);
    reject_btn->SetColors({80,60,40,220}, {130,100,80,220}, {50,40,30,220});
    reject_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && reject_fn_) reject_fn_();
    });

    auto* divorce_btn = window_->AddWidget<Button>("Divorce", 170, 362, 70, 24);
    divorce_btn->SetColors({80,30,30,220}, {130,60,60,220}, {50,20,20,220});
    divorce_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (divorce_fn_) divorce_fn_();
            else if (family_) {
                family_->Divorce(state->selected_char);
                state->chat_messages.push_back("You are now divorced.");
            }
        }
    });

    auto* leave_btn = window_->AddWidget<Button>("Leave Family", 248, 362, 100, 24);
    leave_btn->SetColors({60,60,80,220}, {100,100,130,220}, {40,40,50,220});
    leave_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && leave_fn_) leave_fn_();
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
    if (!state || !tab1_label_) return;

    const bool use_network = !state->offline_mode && !state->network_family_members.empty();
    char buf[512];

    if (use_network) {
        const GameState::NetworkFamilyMember* me = nullptr;
        for (const auto& m : state->network_family_members) {
            if (m.character_id == static_cast<uint32_t>(state->selected_char)) {
                me = &m;
                break;
            }
        }
        if (me) {
            char date_buf[32] = "N/A";
            if (me->married_date > 0) {
                time_t t = static_cast<time_t>(me->married_date);
                struct tm tm_buf {};
#if defined(_WIN32)
                localtime_s(&tm_buf, &t);
#else
                localtime_r(&t, &tm_buf);
#endif
                strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", &tm_buf);
            }
            snprintf(buf, sizeof(buf),
                "Status: %s\n"
                "Partner: %s (ID: %u)\n"
                "Family: %s (ID: %u)\n"
                "Master ID: %u\n"
                "Married: %s\n"
                "You are %s",
                RelationLabel(me->relation),
                me->partner_name.empty() ? "None" : me->partner_name.c_str(),
                me->partner_id,
                state->network_family_name.empty() ? "None" : state->network_family_name.c_str(),
                state->network_family_id,
                state->network_family_master_id,
                me->relation == 3 ? date_buf : "No",
                me->is_master ? "family master" : "member");
            tab1_label_->SetText(buf);
        }

        if (tab2_label_) {
            std::string ml;
            for (const auto& m : state->network_family_members) {
                ml += m.name + " — " + RelationLabel(m.relation);
                if (m.is_master) ml += " [Master]";
                ml += "\n";
            }
            tab2_label_->SetText(ml.empty() ? "No family members" : ml);
        }

        if (tab3_label_) {
            if (state->network_can_accept_marriage) {
                snprintf(buf, sizeof(buf),
                    "Engaged to %s (ID: %u)\n\n"
                    "Click Accept to marry or Reject to cancel.",
                    state->network_engaged_partner_name.empty()
                        ? "partner" : state->network_engaged_partner_name.c_str(),
                    state->network_engaged_partner_id);
            } else {
                snprintf(buf, sizeof(buf),
                    "No pending proposals.\n\n"
                    "Actions available:\n"
                    "%s%s%s%s",
                    state->network_can_create_family ? "- Create a family\n" : "",
                    state->network_can_divorce ? "- Divorce (married)\n" : "",
                    state->network_can_leave_family ? "- Leave family\n" : "",
                    (!state->network_can_create_family && !state->network_can_divorce
                        && !state->network_can_leave_family)
                        ? "- Use Propose to start courtship" : "");
            }
            tab3_label_->SetText(buf);
        }
        if (refresh_fn_) refresh_fn_();
        return;
    }

    if (!family_) return;

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

    if (me && me->family_id > 0) {
        auto members = family_->GetFamilyMembers(me->family_id);
        std::string ml;
        for (auto& m : members)
            ml += m.name + " (" + family_->GetRelationName(m.relation) + ")\n";
        tab2_label_->SetText(ml.empty() ? "No family members found." : ml);
    } else {
        tab2_label_->SetText("Not in a family. Create or join one!");
    }

    snprintf(buf, sizeof(buf),
        "Pending proposals:\n%s",
        family_->IsEngaged(state->selected_char) ? "Engaged — accept or reject" : "None");
    tab3_label_->SetText(buf);
}

void FamilyDialog::UpdateFromState(GameState* state) {
    if (window_ && state->family_open) Refresh(state);
}
