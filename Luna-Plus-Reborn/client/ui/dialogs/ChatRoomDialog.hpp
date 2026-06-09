#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <string>
#include <vector>

struct ChatChannelInfo {
    std::string name;
    std::vector<std::string> messages;
    std::vector<std::string> users;
    uint32_t color = 0xffffffff;
    bool is_room = false;
    uint32_t room_id = 0;
    ListBox* msg_list = nullptr;
};

class ChatRoomDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);

    void SetNetworkCallbacks(
        std::function<void(uint32_t, const std::string&)> send_chat,
        std::function<void(uint32_t)> create_room,
        std::function<void(uint32_t, uint32_t)> join_room,
        std::function<void(uint32_t)> leave_room);

    void AddSystemMessage(const std::string& msg);
    void AddChannelMessage(int channel, const std::string& sender, const std::string& msg);
    void AddRoomMessage(uint32_t room_id, const std::string& sender, const std::string& msg);
    void UpdateUserList(int channel, const std::vector<std::string>& users);

private:
    Window* window_ = nullptr;
    TabPanel* channels_ = nullptr;
    InputField* chat_input_ = nullptr;
    Label* status_label_ = nullptr;
    ListBox* user_list_ = nullptr;
    InputField* whisper_target_ = nullptr;

    std::vector<ChatChannelInfo> channels_info_;
    int active_channel_ = 0;

    std::function<void(uint32_t, const std::string&)> on_send_chat_;
    std::function<void(uint32_t)> on_create_room_;
    std::function<void(uint32_t, uint32_t)> on_join_room_;
    std::function<void(uint32_t)> on_leave_room_;

    void SendMessage(GameState* state);
    void RefreshMessages();
    void RefreshUserList();
    int FindRoomChannel(uint32_t room_id) const;
    void ClearChannelMessages(int channel);
};
