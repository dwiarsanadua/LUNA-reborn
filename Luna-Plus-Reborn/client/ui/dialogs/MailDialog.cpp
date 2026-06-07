#include "MailDialog.hpp"
#include <cstdio>

void MailDialog::Open(GameState* state, WindowManager* wm) {
    window_ = wm->Open("Mail", 100, 50, 580, 460);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Mail System", 10, 4, 0xff88ccff);
    (void)title;

    // Compose area (top right)
    auto* compose_lbl = window_->AddWidget<Label>("Compose Mail:", 380, 28, 0xffffcc88);
    (void)compose_lbl;

    auto* to_lbl = window_->AddWidget<Label>("To:", 380, 48, 0xffcccccc);
    (void)to_lbl;
    to_input_ = window_->AddWidget<InputField>(410, 48, 150, 20);
    to_input_->SetPlaceholder("player name");

    auto* sub_lbl = window_->AddWidget<Label>("Subject:", 380, 72, 0xffcccccc);
    (void)sub_lbl;
    subject_input_ = window_->AddWidget<InputField>(440, 72, 120, 20);
    subject_input_->SetPlaceholder("subject");

    auto* send_btn = window_->AddWidget<Button>("Send", 480, 96, 70, 22);
    send_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    send_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && to_input_ && subject_input_) {
            std::string to = to_input_->GetText();
            std::string sub = subject_input_->GetText();
            if (!to.empty() && !sub.empty()) {
                MailMessage msg;
                msg.id = (uint32_t)(sent_.size() + inbox_.size() + 1);
                msg.from = state->name;
                msg.to = to;
                msg.subject = sub;
                msg.body = "Hello! Sending you this mail.";
                msg.sent_time = time(nullptr);
                sent_.push_back(msg);
                
                // Also add to recipient's inbox
                MailMessage inbox_msg = msg;
                inbox_msg.from = state->name;
                inbox_.insert(inbox_.begin(), inbox_msg);
                
                state->chat_messages.push_back("Mail sent to " + to + "!");
                RefreshSent();
                to_input_->SetText("");
                subject_input_->SetText("");
            }
        }
    });

    // Tab panel (left side)
    tabs_ = window_->AddWidget<TabPanel>(10, 28, 360, 340);

    // Inbox tab
    inbox_list_ = new ListBox(0, 0, 340, 310);
    tabs_->AddTab("Inbox", inbox_list_);

    // Sent tab
    sent_list_ = new ListBox(0, 0, 340, 310);
    tabs_->AddTab("Sent", sent_list_);

    // Detail label (right side below compose)
    detail_label_ = window_->AddWidget<Label>("Select a mail to read.", 380, 130, 0xffffffff);
    reward_label_ = window_->AddWidget<Label>("", 380, 250, 0xff88ff88);

    // Claim / Read button
    auto* read_btn = window_->AddWidget<Button>("Open / Claim", 380, 310, 120, 24);
    read_btn->SetColors({40,60,100,220}, {80,100,160,220}, {30,40,70,220});
    read_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        int tab = tabs_->GetActive();
        if (tab == 0) { // Inbox
            int sel = inbox_list_->GetSelected();
            if (sel >= 0 && sel < (int)inbox_.size()) {
                auto& msg = inbox_[sel];
                if (!msg.read) {
                    msg.read = true;
                    state->chat_messages.push_back("Mail opened: " + msg.subject);
                }
                if ((msg.has_item || msg.gold_attached > 0) && !msg.claimed) {
                    msg.claimed = true;
                    state->gold += msg.gold_attached;
                    char buf[128];
                    snprintf(buf, sizeof(buf), "Claimed %dg from mail!", msg.gold_attached);
                    state->chat_messages.push_back(buf);
                }
                ShowMailDetail(sel, true);
            }
        }
    });

    // Sample mail for demo
    MailMessage sys;
    sys.id = 1; sys.from = "System"; sys.to = state->name;
    sys.subject = "Welcome to Luna!";
    sys.body = "Welcome, adventurer! Here's your starting gift.";
    sys.has_item = true; sys.item_name = "Starter Pack"; sys.item_count = 1;
    sys.gold_attached = 500; sys.sent_time = time(nullptr); sys.read = false;
    inbox_.push_back(sys);

    MailMessage sys2;
    sys2.id = 2; sys2.from = "Guild Master"; sys2.to = state->name;
    sys2.subject = "Join a Guild!";
    sys2.body = "Visit the Guild Manager NPC to create or join a guild.";
    sys2.sent_time = time(nullptr); sys2.read = false;
    inbox_.push_back(sys2);

    RefreshInbox();
    RefreshSent();
}

void MailDialog::RefreshInbox() {
    if (!inbox_list_) return;
    inbox_list_->Clear();
    for (auto& msg : inbox_) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[%s] %s — %s",
            msg.read ? " " : "N",
            msg.from.c_str(), msg.subject.c_str());
        inbox_list_->AddItem(buf);
    }
}

void MailDialog::RefreshSent() {
    if (!sent_list_) return;
    sent_list_->Clear();
    for (auto& msg : sent_) {
        char buf[128];
        snprintf(buf, sizeof(buf), "To: %s — %s", msg.to.c_str(), msg.subject.c_str());
        sent_list_->AddItem(buf);
    }
}

void MailDialog::ShowMailDetail(int index, bool is_inbox) {
    auto& msgs = is_inbox ? inbox_ : sent_;
    if (index < 0 || index >= (int)msgs.size()) return;
    auto& msg = msgs[index];
    char buf[512];
    snprintf(buf, sizeof(buf),
        "From: %s\nSubject: %s\n\n%s\n\nAttachments: %s %s x%d | Gold: %dg\nStatus: %s",
        msg.from.c_str(), msg.subject.c_str(), msg.body.c_str(),
        msg.has_item ? msg.item_name.c_str() : "None",
        msg.has_item ? "x" : "", msg.has_item ? msg.item_count : 0,
        msg.gold_attached,
        msg.read ? "Read" : "Unread");
    if (detail_label_) detail_label_->SetText(buf);
    if (reward_label_) {
        if (msg.claimed) reward_label_->SetText("Rewards claimed.");
        else if (msg.has_item || msg.gold_attached > 0)
            reward_label_->SetText("Click 'Open / Claim' to collect rewards!");
        else
            reward_label_->SetText("");
    }
}

void MailDialog::UpdateFromState(GameState* state) {
    (void)state;
}
