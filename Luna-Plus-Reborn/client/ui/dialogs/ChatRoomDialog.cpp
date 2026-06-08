#include "ChatRoomDialog.hpp"
#include <cstdio>

void ChatRoomDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    (void)msg_display_;
    (void)active_channel_;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ChatRoomMainDlg.bin.txt",
        "Chat Rooms", 100, 80, 500, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    channel_names_ = {"General", "Party", "Guild", "Whisper", "System"};

    channels_ = window_->AddWidget<TabPanel>(10, 28, 480, 310);

    for (int i = 0; i < (int)channel_names_.size(); i++) {
        auto* msg_list = new ListBox(0, 0, 460, 270);
        msg_list->AddItem(("--- " + channel_names_[i] + " Channel ---").c_str());
        msg_list->AddItem("Welcome to " + channel_names_[i] + "!");
        channels_->AddTab(channel_names_[i], msg_list);
    }

    // Chat input
    chat_input_ = window_->AddWidget<InputField>(10, 340, 380, 22);
    chat_input_->SetPlaceholder("Type a message...");

    auto* send_btn = window_->AddWidget<Button>("Send", 400, 340, 70, 22);
    send_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    send_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) SendMessage(state);
    });

    auto* join_btn = window_->AddWidget<Button>("+ Create Room", 340, 370, 130, 22);
    join_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            channel_names_.push_back("Room #" + std::to_string(channel_names_.size() + 1));
            auto* msg_list = new ListBox(0, 0, 460, 270);
            msg_list->AddItem("--- New Room ---");
            channels_->AddTab(channel_names_.back(), msg_list);
        }
    });
}

void ChatRoomDialog::SendMessage(GameState* state) {
    if (!chat_input_ || chat_input_->GetText().empty()) return;
    std::string msg = chat_input_->GetText();
    std::string channel = channel_names_[channels_->GetActive()];
    state->chat_messages.push_back("[" + channel + "] " + state->name + ": " + msg);
    chat_input_->SetText("");
}

void ChatRoomDialog::UpdateFromState(GameState* state) {
    (void)state;
}
