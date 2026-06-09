#include "DateMatchingDialog.hpp"
#include <cstdio>

void DateMatchingDialog::SetNetworkCallbacks(
    std::function<void()> search_fn,
    std::function<void()> save_profile_fn,
    std::function<void(uint32_t)> register_partner_fn,
    std::function<void(uint32_t)> recommend_partner_fn,
    std::function<void(const std::string&)> send_chat_fn,
    std::function<void()> enter_date_zone_fn,
    std::function<void()> enter_challenge_zone_fn) {
    on_search_ = std::move(search_fn);
    on_save_profile_ = std::move(save_profile_fn);
    on_register_partner_ = std::move(register_partner_fn);
    on_recommend_partner_ = std::move(recommend_partner_fn);
    on_send_chat_ = std::move(send_chat_fn);
    on_enter_date_zone_ = std::move(enter_date_zone_fn);
    on_enter_challenge_zone_ = std::move(enter_challenge_zone_fn);
}

void DateMatchingDialog::Open(GameState* state, WindowManager* wm) {
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/DateMatchingDlg.bin.txt",
        "Date Matching", 240, 120, 440, 360);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Date Matching System", 10, 4, 0xff88ffcc);
    (void)title;

    // Upper tabs: Partner List / My Info
    upper_tabs_ = window_->AddWidget<TabPanel>(10, 28, 310, 24);
    upper_tabs_->AddTab("Partner List", nullptr);
    upper_tabs_->AddTab("My Info", nullptr);

    // --- Partner List (upper tab 0) ---
    window_->AddWidget<Label>("Age:", 14, 60, 0xffcccccc);
    age_combo_ = window_->AddWidget<ComboBox>(44, 58, 80, 22);
    age_combo_->AddItem("All");
    age_combo_->AddItem("18-25");
    age_combo_->AddItem("26-35");
    age_combo_->AddItem("36+");

    window_->AddWidget<Label>("Region:", 130, 60, 0xffcccccc);
    region_combo_ = window_->AddWidget<ComboBox>(180, 58, 80, 22);
    region_combo_->AddItem("All");
    region_combo_->AddItem("North");
    region_combo_->AddItem("South");
    region_combo_->AddItem("East");
    region_combo_->AddItem("West");

    window_->AddWidget<Label>("Gender:", 14, 86, 0xffcccccc);
    gender_male_check_ = window_->AddWidget<CheckBox>("M", 68, 84);
    gender_female_check_ = window_->AddWidget<CheckBox>("F", 110, 84);

    auto* search_btn = window_->AddWidget<Button>("Search", 200, 82, 70, 24);
    search_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    search_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSearch();
    });

    search_list_ = window_->AddWidget<ListBox>(14, 110, 300, 100);
    search_list_->SetVisible(false);
    search_list_->SetAutoScroll(false);

    auto* prev_btn = window_->AddWidget<Button>("<", 100, 214, 30, 20);
    prev_btn->SetColors({60,60,80,220}, {100,100,130,220}, {40,40,50,220});
    prev_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && current_page_ > 0) {
            current_page_--;
            RefreshSearchList();
        }
    });

    page_label_ = window_->AddWidget<Label>("0", 136, 215, 0xffffffff);

    auto* next_btn = window_->AddWidget<Button>(">", 160, 214, 30, 20);
    next_btn->SetColors({60,60,80,220}, {100,100,130,220}, {40,40,50,220});
    next_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            current_page_++;
            RefreshSearchList();
        }
    });

    auto* start_chat_btn = window_->AddWidget<Button>("Start Chat", 220, 214, 90, 24);
    start_chat_btn->SetColors({40,80,100,220}, {80,130,160,220}, {30,50,70,220});
    start_chat_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && !search_results_.empty()) {
            chat_messages_.clear();
            chat_messages_.push_back("--- Chat started ---");
            RefreshChat();
        }
    });

    // Chat area
    window_->AddWidget<Label>("Chat:", 14, 242, 0xffcccccc);
    chat_list_ = window_->AddWidget<ListBox>(14, 258, 300, 80);
    chat_list_->SetVisible(false);
    chat_list_->SetAutoScroll(true);

    chat_input_ = window_->AddWidget<InputField>(14, 342, 220, 22);
    chat_input_->SetPlaceholder("Type a message...");
    chat_input_->SetVisible(false);

    auto* send_btn = window_->AddWidget<Button>("Send", 240, 342, 70, 22);
    send_btn->SetColors({40,60,80,220}, {80,110,130,220}, {30,40,50,220});
    send_btn->SetVisible(false);
    send_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSendChat();
    });

    // --- Lower tabs: My Info / Partner Info / Record ---
    lower_tabs_ = window_->AddWidget<TabPanel>(10, 60, 310, 20);
    lower_tabs_->AddTab("My Info", nullptr);
    lower_tabs_->AddTab("Partner Info", nullptr);
    lower_tabs_->AddTab("Record", nullptr);

    // My Info content (lower tab 0)
    my_nickname_label_ = window_->AddWidget<Label>("", 14, 86, 0xffffffff);
    my_gender_label_ = window_->AddWidget<Label>("", 14, 104, 0xffffffff);
    my_age_label_ = window_->AddWidget<Label>("", 14, 122, 0xffffffff);
    my_region_label_ = window_->AddWidget<Label>("", 14, 140, 0xffffffff);
    my_good_feeling_label_ = window_->AddWidget<Label>("", 14, 158, 0xffffffff);
    my_bad_feeling_label_ = window_->AddWidget<Label>("", 14, 176, 0xffffffff);

    window_->AddWidget<Label>("Introduction:", 14, 196, 0xffcccccc);
    my_introduction_input_ = window_->AddWidget<InputField>(14, 214, 220, 60);
    my_introduction_input_->SetPlaceholder("Write about yourself...");
    my_introduction_input_->SetMaxLength(512);

    save_profile_btn_ = window_->AddWidget<Button>("Save Profile", 180, 280, 100, 24);
    save_profile_btn_->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    save_profile_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSaveProfile();
    });

    // Partner Info content (lower tab 1)
    partner_nickname_label_ = window_->AddWidget<Label>("Partner: None", 14, 86, 0xffffffff);
    partner_gender_label_ = window_->AddWidget<Label>("", 14, 104, 0xffffffff);
    partner_age_label_ = window_->AddWidget<Label>("", 14, 122, 0xffffffff);
    partner_region_label_ = window_->AddWidget<Label>("", 14, 140, 0xffffffff);
    partner_good_feeling_label_ = window_->AddWidget<Label>("", 14, 158, 0xffffffff);
    partner_bad_feeling_label_ = window_->AddWidget<Label>("", 14, 176, 0xffffffff);
    partner_introduction_label_ = window_->AddWidget<Label>("", 14, 196, 0xffffffff);

    register_partner_btn_ = window_->AddWidget<Button>("Register", 180, 280, 80, 24);
    register_partner_btn_->SetColors({40,60,100,220}, {80,110,160,220}, {30,40,70,220});
    register_partner_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoRegisterPartner();
    });

    recommend_partner_btn_ = window_->AddWidget<Button>("Recommend", 264, 280, 80, 24);
    recommend_partner_btn_->SetColors({100,60,40,220}, {160,110,80,220}, {70,40,30,220});
    recommend_partner_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoRecommendPartner();
    });

    // Record content (lower tab 2)
    record_list_ = window_->AddWidget<ListBox>(14, 86, 300, 140);
    record_list_->SetAutoScroll(false);

    // Bottom action buttons
    auto* enter_date_btn = window_->AddWidget<Button>("Enter Date Zone", 14, 370, 130, 24);
    enter_date_btn->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    enter_date_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (on_enter_date_zone_) on_enter_date_zone_();
        }
    });

    auto* enter_challenge_btn = window_->AddWidget<Button>("Challenge Zone", 160, 370, 130, 24);
    enter_challenge_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    enter_challenge_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (on_enter_challenge_zone_) on_enter_challenge_zone_();
        }
    });

    // --- Tab switching visibility ---
    upper_tabs_->OnEvent([=](const UIEvent& e) {
        if (e.type != UIEvent::TabSelected) return;
        bool show_partner = (e.int_value == 0);
        bool show_info = (e.int_value == 1);

        // Partner list widgets
        age_combo_->SetVisible(show_partner);
        region_combo_->SetVisible(show_partner);
        gender_male_check_->SetVisible(show_partner);
        gender_female_check_->SetVisible(show_partner);
        search_btn->SetVisible(show_partner);
        search_list_->SetVisible(show_partner);
        prev_btn->SetVisible(show_partner);
        page_label_->SetVisible(show_partner);
        next_btn->SetVisible(show_partner);
        start_chat_btn->SetVisible(show_partner);
        chat_list_->SetVisible(show_partner);
        chat_input_->SetVisible(show_partner);
        send_btn->SetVisible(show_partner);

        // Lower tab area
        lower_tabs_->SetVisible(show_info);
        int li = lower_tabs_->GetActive();
        my_nickname_label_->SetVisible(show_info && li == 0);
        my_gender_label_->SetVisible(show_info && li == 0);
        my_age_label_->SetVisible(show_info && li == 0);
        my_region_label_->SetVisible(show_info && li == 0);
        my_good_feeling_label_->SetVisible(show_info && li == 0);
        my_bad_feeling_label_->SetVisible(show_info && li == 0);
        my_introduction_input_->SetVisible(show_info && li == 0);
        save_profile_btn_->SetVisible(show_info && li == 0);

        partner_nickname_label_->SetVisible(show_info && li == 1);
        partner_gender_label_->SetVisible(show_info && li == 1);
        partner_age_label_->SetVisible(show_info && li == 1);
        partner_region_label_->SetVisible(show_info && li == 1);
        partner_good_feeling_label_->SetVisible(show_info && li == 1);
        partner_bad_feeling_label_->SetVisible(show_info && li == 1);
        partner_introduction_label_->SetVisible(show_info && li == 1);
        register_partner_btn_->SetVisible(show_info && li == 1);
        recommend_partner_btn_->SetVisible(show_info && li == 1);

        record_list_->SetVisible(show_info && li == 2);
    });

    lower_tabs_->OnEvent([=](const UIEvent& e) {
        if (e.type != UIEvent::TabSelected) return;
        int li = e.int_value;
        bool show_info = upper_tabs_->GetActive() == 1;

        my_nickname_label_->SetVisible(show_info && li == 0);
        my_gender_label_->SetVisible(show_info && li == 0);
        my_age_label_->SetVisible(show_info && li == 0);
        my_region_label_->SetVisible(show_info && li == 0);
        my_good_feeling_label_->SetVisible(show_info && li == 0);
        my_bad_feeling_label_->SetVisible(show_info && li == 0);
        my_introduction_input_->SetVisible(show_info && li == 0);
        save_profile_btn_->SetVisible(show_info && li == 0);

        partner_nickname_label_->SetVisible(show_info && li == 1);
        partner_gender_label_->SetVisible(show_info && li == 1);
        partner_age_label_->SetVisible(show_info && li == 1);
        partner_region_label_->SetVisible(show_info && li == 1);
        partner_good_feeling_label_->SetVisible(show_info && li == 1);
        partner_bad_feeling_label_->SetVisible(show_info && li == 1);
        partner_introduction_label_->SetVisible(show_info && li == 1);
        register_partner_btn_->SetVisible(show_info && li == 1);
        recommend_partner_btn_->SetVisible(show_info && li == 1);

        record_list_->SetVisible(show_info && li == 2);
    });

    // Initial state: Partner List visible, My Info hidden
    lower_tabs_->SetVisible(false);
    my_nickname_label_->SetVisible(false);
    my_gender_label_->SetVisible(false);
    my_age_label_->SetVisible(false);
    my_region_label_->SetVisible(false);
    my_good_feeling_label_->SetVisible(false);
    my_bad_feeling_label_->SetVisible(false);
    my_introduction_input_->SetVisible(false);
    save_profile_btn_->SetVisible(false);
    partner_nickname_label_->SetVisible(false);
    partner_gender_label_->SetVisible(false);
    partner_age_label_->SetVisible(false);
    partner_region_label_->SetVisible(false);
    partner_good_feeling_label_->SetVisible(false);
    partner_bad_feeling_label_->SetVisible(false);
    partner_introduction_label_->SetVisible(false);
    register_partner_btn_->SetVisible(false);
    recommend_partner_btn_->SetVisible(false);
    record_list_->SetVisible(false);

    Refresh(state);
}

void DateMatchingDialog::Refresh(GameState* state) {
    if (!state) return;
    RefreshSearchList();
    RefreshChat();
    RefreshMyInfo();
    RefreshPartnerInfo();
    RefreshRecords();
}

void DateMatchingDialog::RefreshSearchList() {
    if (!search_list_) return;
    search_list_->Clear();
    int start = current_page_ * 10;
    int end = std::min(start + 10, (int)search_results_.size());
    for (int i = start; i < end; i++) {
        auto& p = search_results_[i];
        char buf[256];
        snprintf(buf, sizeof(buf), "%s | %s | Age:%d | %s",
            p.nickname.c_str(),
            p.gender == 0 ? "M" : "F",
            p.age,
            p.region.c_str());
        search_list_->AddItem(buf);
    }
    char page_buf[32];
    snprintf(page_buf, sizeof(page_buf), "%d", current_page_ + 1);
    page_label_->SetText(page_buf);
}

void DateMatchingDialog::RefreshChat() {
    if (!chat_list_) return;
    chat_list_->Clear();
    for (auto& msg : chat_messages_) {
        chat_list_->AddItem(msg);
    }
}

void DateMatchingDialog::RefreshMyInfo() {
    if (!my_nickname_label_) return;
    char buf[256];
    snprintf(buf, sizeof(buf), "Nickname: %s", my_profile_.nickname.empty() ? "---" : my_profile_.nickname.c_str());
    my_nickname_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Gender: %s", my_profile_.gender == 0 ? "Male" : "Female");
    my_gender_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Age: %d", my_profile_.age);
    my_age_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Region: %s", my_profile_.region.empty() ? "---" : my_profile_.region.c_str());
    my_region_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Good Feeling: %d", my_profile_.good_feeling);
    my_good_feeling_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Bad Feeling: %d", my_profile_.bad_feeling);
    my_bad_feeling_label_->SetText(buf);
    if (my_introduction_input_) {
        my_introduction_input_->SetText(my_profile_.introduction);
    }
}

void DateMatchingDialog::RefreshPartnerInfo() {
    if (!partner_nickname_label_) return;
    char buf[256];
    if (partner_profile_.character_id == 0) {
        partner_nickname_label_->SetText("Partner: None selected");
        partner_gender_label_->SetText("");
        partner_age_label_->SetText("");
        partner_region_label_->SetText("");
        partner_good_feeling_label_->SetText("");
        partner_bad_feeling_label_->SetText("");
        partner_introduction_label_->SetText("");
        return;
    }
    snprintf(buf, sizeof(buf), "Partner: %s", partner_profile_.nickname.c_str());
    partner_nickname_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Gender: %s", partner_profile_.gender == 0 ? "Male" : "Female");
    partner_gender_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Age: %d", partner_profile_.age);
    partner_age_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Region: %s", partner_profile_.region.c_str());
    partner_region_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Good Feeling: %d", partner_profile_.good_feeling);
    partner_good_feeling_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Bad Feeling: %d", partner_profile_.bad_feeling);
    partner_bad_feeling_label_->SetText(buf);
    snprintf(buf, sizeof(buf), "Introduction: %s",
        partner_profile_.introduction.empty() ? "None" : partner_profile_.introduction.c_str());
    partner_introduction_label_->SetText(buf);
}

void DateMatchingDialog::RefreshRecords() {
    if (!record_list_) return;
    record_list_->Clear();
    if (date_records_.empty()) {
        record_list_->AddItem("No date records yet.");
        return;
    }
    for (auto& r : date_records_) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s | %s | %u pts",
            r.partner_name.c_str(), r.last_date.c_str(), r.points);
        record_list_->AddItem(buf);
    }
}

void DateMatchingDialog::DoSearch() {
    if (on_search_) {
        on_search_();
        return;
    }
    search_results_.clear();
    DateMatchingProfile p1;
    p1.character_id = 1;
    p1.nickname = "Alice";
    p1.gender = 1;
    p1.age = 22;
    p1.region = "North";
    p1.introduction = "Love adventures!";
    search_results_.push_back(p1);

    DateMatchingProfile p2;
    p2.character_id = 2;
    p2.nickname = "Bob";
    p2.gender = 0;
    p2.age = 28;
    p2.region = "South";
    p2.introduction = "Music lover.";
    search_results_.push_back(p2);

    current_page_ = 0;
    RefreshSearchList();
}

void DateMatchingDialog::DoSendChat() {
    if (!chat_input_) return;
    std::string msg = chat_input_->GetText();
    if (msg.empty()) return;
    chat_messages_.push_back("You: " + msg);
    if (on_send_chat_) {
        on_send_chat_(msg);
    } else {
        chat_messages_.push_back("Partner: " + msg);
    }
    chat_input_->SetText("");
    RefreshChat();
}

void DateMatchingDialog::DoSaveProfile() {
    if (my_introduction_input_) {
        my_profile_.introduction = my_introduction_input_->GetText();
    }
    if (on_save_profile_) {
        on_save_profile_();
    }
}

void DateMatchingDialog::DoRegisterPartner() {
    if (partner_profile_.character_id == 0) return;
    if (on_register_partner_) {
        on_register_partner_(partner_profile_.character_id);
    }
}

void DateMatchingDialog::DoRecommendPartner() {
    if (partner_profile_.character_id == 0) return;
    if (on_recommend_partner_) {
        on_recommend_partner_(partner_profile_.character_id);
    }
}

void DateMatchingDialog::UpdateFromState(GameState* state) {
    if (window_) Refresh(state);
}
