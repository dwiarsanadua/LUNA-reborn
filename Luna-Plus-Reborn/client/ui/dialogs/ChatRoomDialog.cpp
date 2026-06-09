#include "ChatRoomDialog.hpp"
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void ChatRoomDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ChatRoomMainDlg.bin.txt",
        "Chat Rooms", 80, 60, 580, 440);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    channels_info_.clear();
    channels_info_.push_back({"General", {}, {}, 0xffffffff, false, 0, nullptr});
    channels_info_.push_back({"Party", {}, {}, 0x8888ffff, false, 0, nullptr});
    channels_info_.push_back({"Guild", {}, {}, 0x44ff44ff, false, 0, nullptr});
    channels_info_.push_back({"Whisper", {}, {}, 0xffff88ff, false, 0, nullptr});
    channels_info_.push_back({"System", {}, {}, 0xffff8844, false, 0, nullptr});

    channels_ = window_->AddWidget<TabPanel>(10, 28, 560, 310);

    for (int i = 0; i < (int)channels_info_.size(); i++) {
        auto* msg_list = new ListBox(0, 0, 420, 260);
        msg_list->SetAutoScroll(true);
        msg_list->AddItem(("--- " + channels_info_[i].name + " Channel ---").c_str());
        channels_info_[i].msg_list = msg_list;
        channels_->AddTab(channels_info_[i].name, msg_list);
    }

    user_list_ = window_->AddWidget<ListBox>(440, 50, 130, 248);

    auto* user_label = window_->AddWidget<Label>("Users", 440, 28);
    user_label->SetRect(440, 28, 130, 20);
    user_label->SetColor(0xffcccccc);

    chat_input_ = window_->AddWidget<InputField>(10, 340, 400, 22);
    chat_input_->SetPlaceholder("Type a message...");

    whisper_target_ = window_->AddWidget<InputField>(10, 365, 120, 18);
    whisper_target_->SetPlaceholder("Whisper to...");

    auto* send_btn = window_->AddWidget<Button>("Send", 420, 340, 70, 22);
    send_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    send_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) SendMessage(state);
    });

    auto* create_btn = window_->AddWidget<Button>("+ Create", 340, 370, 90, 22);
    create_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_create_room_) {
            uint32_t room_id = static_cast<uint32_t>(channels_info_.size() + 1);
            on_create_room_(room_id);
        }
    });

    auto* leave_btn = window_->AddWidget<Button>("Leave", 440, 370, 70, 22);
    leave_btn->SetColors({80,40,40,220}, {130,60,60,220}, {50,30,30,220});
    leave_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            auto& ch = channels_info_[active_channel_];
            if (ch.is_room && on_leave_room_) {
                on_leave_room_(ch.room_id);
            }
        }
    });

    auto* close_btn = window_->AddWidget<Button>("X", 540, 4, 28, 20);
    close_btn->SetColors({120,40,40,220}, {180,60,60,220}, {80,30,30,220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });

    status_label_ = window_->AddWidget<Label>("Connected", 10, 395);
    status_label_->SetRect(10, 395, 400, 18);

    active_channel_ = 0;
    state->chatroom_open = true;
    RefreshMessages();
    RefreshUserList();
}

void ChatRoomDialog::Close() {
    window_ = nullptr;
}

void ChatRoomDialog::SetNetworkCallbacks(
    std::function<void(uint32_t, const std::string&)> send_chat,
    std::function<void(uint32_t)> create_room,
    std::function<void(uint32_t, uint32_t)> join_room,
    std::function<void(uint32_t)> leave_room) {
    on_send_chat_ = send_chat;
    on_create_room_ = create_room;
    on_join_room_ = join_room;
    on_leave_room_ = leave_room;
}

void ChatRoomDialog::SendMessage(GameState* state) {
    if (!chat_input_ || chat_input_->GetText().empty()) return;
    std::string msg = chat_input_->GetText();
    auto& ch = channels_info_[active_channel_];

    if (active_channel_ == 3 && whisper_target_ && !whisper_target_->GetText().empty()) {
        std::string target = whisper_target_->GetText();
        ch.messages.push_back("[To " + target + "] " + state->name + ": " + msg);
        if (on_send_chat_) {
            on_send_chat_(0, "/w " + target + " " + msg);
        }
    } else if (ch.is_room && on_send_chat_) {
        ch.messages.push_back("[" + ch.name + "] " + state->name + ": " + msg);
        on_send_chat_(ch.room_id, msg);
    } else if (on_send_chat_) {
        ch.messages.push_back("[" + ch.name + "] " + state->name + ": " + msg);
        on_send_chat_(static_cast<uint32_t>(active_channel_), msg);
    }

    chat_input_->SetText("");
    RefreshMessages();
}

void ChatRoomDialog::RefreshMessages() {
    if (!channels_) return;
    int active = channels_->GetActive();
    if (active < 0 || active >= (int)channels_info_.size()) return;
    active_channel_ = active;

    auto* msg_list = channels_info_[active].msg_list;
    if (!msg_list) return;

    msg_list->Clear();
    auto& ch = channels_info_[active];
    int start = std::max(0, (int)ch.messages.size() - 100);
    for (int i = start; i < (int)ch.messages.size(); i++) {
        msg_list->AddItem(ch.messages[i].c_str());
    }
}

void ChatRoomDialog::RefreshUserList() {
    if (!user_list_ || !channels_) return;
    int active = channels_->GetActive();
    if (active < 0 || active >= (int)channels_info_.size()) return;

    user_list_->Clear();
    auto& ch = channels_info_[active];
    user_list_->AddItem(("--- Users (" + std::to_string(ch.users.size()) + ") ---").c_str());
    for (auto& u : ch.users) {
        user_list_->AddItem(u.c_str());
    }

    if (status_label_) {
        std::string status = ch.name + " channel";
        if (ch.is_room) status += " (Room #" + std::to_string(ch.room_id) + ")";
        status += " - " + std::to_string(ch.users.size()) + " users";
        status_label_->SetText(status);
    }

    if (whisper_target_) {
        whisper_target_->SetVisible(active == 3);
    }
}

void ChatRoomDialog::AddSystemMessage(const std::string& msg) {
    int sys_idx = 4;
    if (sys_idx < (int)channels_info_.size()) {
        channels_info_[sys_idx].messages.push_back("[System] " + msg);
        if (channels_ && channels_->GetActive() == sys_idx) RefreshMessages();
    }
}

void ChatRoomDialog::AddChannelMessage(int channel, const std::string& sender, const std::string& msg) {
    if (channel < 0 || channel >= (int)channels_info_.size()) return;
    channels_info_[channel].messages.push_back("[" + channels_info_[channel].name + "] " + sender + ": " + msg);
    if (channels_ && channels_->GetActive() == channel) RefreshMessages();
}

void ChatRoomDialog::AddRoomMessage(uint32_t room_id, const std::string& sender, const std::string& msg) {
    int idx = FindRoomChannel(room_id);
    if (idx < 0) return;
    channels_info_[idx].messages.push_back("[" + channels_info_[idx].name + "] " + sender + ": " + msg);
    if (channels_ && channels_->GetActive() == idx) RefreshMessages();
}

void ChatRoomDialog::UpdateUserList(int channel, const std::vector<std::string>& users) {
    if (channel < 0 || channel >= (int)channels_info_.size()) return;
    channels_info_[channel].users = users;
    if (channels_ && channels_->GetActive() == channel) RefreshUserList();
}

int ChatRoomDialog::FindRoomChannel(uint32_t room_id) const {
    for (int i = 0; i < (int)channels_info_.size(); i++) {
        if (channels_info_[i].is_room && channels_info_[i].room_id == room_id)
            return i;
    }
    return -1;
}

void ChatRoomDialog::ClearChannelMessages(int channel) {
    if (channel < 0 || channel >= (int)channels_info_.size()) return;
    channels_info_[channel].messages.clear();
    if (channels_ && channels_->GetActive() == channel) RefreshMessages();
}

void ChatRoomDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;

    // Detect tab switch
    if (channels_) {
        int active = channels_->GetActive();
        if (active != active_channel_ && active >= 0 && active < (int)channels_info_.size()) {
            active_channel_ = active;
            RefreshMessages();
            RefreshUserList();
        }
    }

    // Sync chat messages from GameState (incoming network messages)
    if (!state->chat_messages.empty()) {
        for (auto& msg : state->chat_messages) {
            bool found = false;
            for (auto& ch : channels_info_) {
                for (auto& em : ch.messages) {
                    if (em == msg) { found = true; break; }
                }
                if (found) break;
            }
            if (!found) {
                channels_info_[0].messages.push_back(msg);
                if (channels_ && channels_->GetActive() == 0) RefreshMessages();
            }
        }
    }

    // Cleanup — if window was externally closed
    if (window_ && !window_->IsVisible()) {
        state->chatroom_open = false;
    }
}
