#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

struct MailMessage {
    uint32_t id;
    std::string from;
    std::string to;
    std::string subject;
    std::string body;
    bool has_item = false;
    std::string item_name;
    int item_count = 0;
    int gold_attached = 0;
    bool read = false;
    bool claimed = false;
    time_t sent_time;
};

class MailDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    ListBox* inbox_list_ = nullptr;
    ListBox* sent_list_ = nullptr;
    Label* detail_label_ = nullptr;
    Label* reward_label_ = nullptr;
    InputField* to_input_ = nullptr;
    InputField* subject_input_ = nullptr;
    InputField* body_input_ = nullptr;
    std::vector<MailMessage> inbox_;
    std::vector<MailMessage> sent_;
    int selected_ = -1;
    void RefreshInbox();
    void RefreshSent();
    void ShowMailDetail(int index, bool is_inbox);
};
