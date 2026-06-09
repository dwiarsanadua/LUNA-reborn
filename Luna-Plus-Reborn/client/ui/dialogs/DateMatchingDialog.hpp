#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/ComboBox.hpp>
#include <ui/widgets/CheckBox.hpp>
#include <ui/widgets/ListBox.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

struct DateMatchingProfile {
    uint32_t character_id = 0;
    std::string nickname;
    uint8_t gender = 0;
    uint8_t age = 0;
    std::string region;
    std::string introduction;
    uint8_t good_feeling = 0;
    uint8_t bad_feeling = 0;
    uint32_t points = 0;
};

struct DateRecordEntry {
    std::string partner_name;
    std::string last_date;
    uint32_t points = 0;
};

class DateMatchingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(
        std::function<void()> search_fn,
        std::function<void()> save_profile_fn,
        std::function<void(uint32_t)> register_partner_fn,
        std::function<void(uint32_t)> recommend_partner_fn,
        std::function<void(const std::string&)> send_chat_fn,
        std::function<void()> enter_date_zone_fn,
        std::function<void()> enter_challenge_zone_fn);

    void SetMyProfile(const DateMatchingProfile& profile) { my_profile_ = profile; }
    void SetPartnerProfile(const DateMatchingProfile& profile) { partner_profile_ = profile; }
    void SetSearchResults(const std::vector<DateMatchingProfile>& results) { search_results_ = results; }
    void SetDateRecords(const std::vector<DateRecordEntry>& records) { date_records_ = records; }
    void AddChatMessage(const std::string& msg) { chat_messages_.push_back(msg); }

private:
    Window* window_ = nullptr;
    TabPanel* upper_tabs_ = nullptr;
    TabPanel* lower_tabs_ = nullptr;

    // Partner List (upper tab 0)
    ComboBox* age_combo_ = nullptr;
    ComboBox* region_combo_ = nullptr;
    CheckBox* gender_male_check_ = nullptr;
    CheckBox* gender_female_check_ = nullptr;
    ListBox* search_list_ = nullptr;
    Label* page_label_ = nullptr;

    // Chat area
    ListBox* chat_list_ = nullptr;
    InputField* chat_input_ = nullptr;

    // My Info (lower tab 0)
    Label* my_nickname_label_ = nullptr;
    Label* my_gender_label_ = nullptr;
    Label* my_age_label_ = nullptr;
    Label* my_region_label_ = nullptr;
    Label* my_good_feeling_label_ = nullptr;
    Label* my_bad_feeling_label_ = nullptr;
    InputField* my_introduction_input_ = nullptr;
    Button* save_profile_btn_ = nullptr;

    // Partner Info (lower tab 1)
    Label* partner_nickname_label_ = nullptr;
    Label* partner_gender_label_ = nullptr;
    Label* partner_age_label_ = nullptr;
    Label* partner_region_label_ = nullptr;
    Label* partner_good_feeling_label_ = nullptr;
    Label* partner_bad_feeling_label_ = nullptr;
    Label* partner_introduction_label_ = nullptr;
    Button* register_partner_btn_ = nullptr;
    Button* recommend_partner_btn_ = nullptr;

    // Record (lower tab 2)
    ListBox* record_list_ = nullptr;

    std::function<void()> on_search_;
    std::function<void()> on_save_profile_;
    std::function<void(uint32_t)> on_register_partner_;
    std::function<void(uint32_t)> on_recommend_partner_;
    std::function<void(const std::string&)> on_send_chat_;
    std::function<void()> on_enter_date_zone_;
    std::function<void()> on_enter_challenge_zone_;

    DateMatchingProfile my_profile_;
    DateMatchingProfile partner_profile_;
    std::vector<DateMatchingProfile> search_results_;
    std::vector<DateRecordEntry> date_records_;
    std::vector<std::string> chat_messages_;
    int current_page_ = 0;

    void Refresh(GameState* state);
    void RefreshSearchList();
    void RefreshChat();
    void RefreshMyInfo();
    void RefreshPartnerInfo();
    void RefreshRecords();
    void DoSearch();
    void DoSendChat();
    void DoSaveProfile();
    void DoRegisterPartner();
    void DoRecommendPartner();
};
