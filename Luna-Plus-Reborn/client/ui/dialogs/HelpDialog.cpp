#include "HelpDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <cstdio>
#include <ctime>

void HelpDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/HelpDlg.bin.txt",
            "Help & Guide", 120, 60, 520, 440);
    } else {
        window_ = new Window("Help & Guide", 120, 60, 520, 440);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 500, 380);

    PopulateCommandsTab();
    PopulateSystemTab();
    PopulateTopicsTab();
    PopulateAboutTab();

    if (version_str_.empty()) {
        version_str_ = "LUNA Plus Reborn v1.1.0";
        build_date_ = __DATE__ " " __TIME__;
    }
}

void HelpDialog::Close() {
    window_ = nullptr;
}

void HelpDialog::SetTopics(const std::vector<HelpTopic>& topics) {
    topics_ = topics;
}

void HelpDialog::SetVersionInfo(const std::string& version, const std::string& build_date) {
    version_str_ = version;
    build_date_ = build_date;
}

void HelpDialog::PopulateCommandsTab() {
    auto* cmd_list = new ListBox(0, 0, 480, 360);
    cmd_list->SetAutoScroll(false);
    cmd_list->AddItem("=== Chat Commands ===");
    cmd_list->AddItem("/w <name> <msg>    — Whisper to player");
    cmd_list->AddItem("/party <msg>       — Party chat");
    cmd_list->AddItem("/guild <msg>       — Guild chat");
    cmd_list->AddItem("/shout <msg>       — Shout (area)");
    cmd_list->AddItem("");
    cmd_list->AddItem("=== Social Commands ===");
    cmd_list->AddItem("/friend add <name> — Add friend");
    cmd_list->AddItem("/friend remove     — Remove friend");
    cmd_list->AddItem("/party invite      — Invite to party");
    cmd_list->AddItem("/trade <name>      — Request trade");
    cmd_list->AddItem("/stall open        — Open street stall");
    cmd_list->AddItem("/stall close       — Close street stall");
    cmd_list->AddItem("");
    cmd_list->AddItem("=== Guild Commands ===");
    cmd_list->AddItem("/guild create <n>  — Create guild");
    cmd_list->AddItem("/guild invite <n>  — Invite to guild");
    cmd_list->AddItem("/guild leave       — Leave guild");
    cmd_list->AddItem("");
    cmd_list->AddItem("=== Navigation Commands ===");
    cmd_list->AddItem("/tp <map_id>       — Teleport to map (GM)");
    cmd_list->AddItem("/save              — Save position");
    cmd_list->AddItem("/recall            — Return to save point");
    cmd_list->AddItem("");
    cmd_list->AddItem("=== Hotkeys ===");
    cmd_list->AddItem("I           — Inventory");
    cmd_list->AddItem("K           — Skills");
    cmd_list->AddItem("C           — Character Info");
    cmd_list->AddItem("Q           — Quest Journal");
    cmd_list->AddItem("P           — Party");
    cmd_list->AddItem("G           — Guild");
    cmd_list->AddItem("M           — Minimap / World Map");
    cmd_list->AddItem("T           — Pet");
    cmd_list->AddItem("R           — Mount");
    cmd_list->AddItem("F           — Friends");
    cmd_list->AddItem("H           — Help (this dialog)");
    cmd_list->AddItem("Enter       — Chat / Confirm");
    cmd_list->AddItem("Escape      — Close / Cancel");
    cmd_list->AddItem("Tab         — Switch field");
    tabs_->AddTab("Commands", cmd_list);
}

void HelpDialog::PopulateSystemTab() {
    auto* sys_list = new ListBox(0, 0, 480, 360);
    sys_list->SetAutoScroll(false);

    sys_list->AddItem("=== System Information ===");
    char buf[256];
    snprintf(buf, sizeof(buf), "Version: %s", version_str_.c_str());
    sys_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "Build: %s", build_date_.c_str());
    sys_list->AddItem(buf);
    sys_list->AddItem("");
    sys_list->AddItem("=== Game Mechanics ===");
    sys_list->AddItem("Experience is shared among party members");
    sys_list->AddItem("with a +20% party bonus.");
    sys_list->AddItem("");
    sys_list->AddItem("Equipment can be upgraded via the");
    sys_list->AddItem("Upgrade NPC. Success rates decrease");
    sys_list->AddItem("with each enchant level.");
    sys_list->AddItem("");
    sys_list->AddItem("Monsters respawn after 30-60 seconds");
    sys_list->AddItem("Boss monsters respawn after 5 minutes");
    sys_list->AddItem("");
    sys_list->AddItem("=== Tips ===");
    sys_list->AddItem("• Use potions wisely during combat");
    sys_list->AddItem("• Party up for better XP gain");
    sys_list->AddItem("• Upgrade weapons before armor");
    sys_list->AddItem("• Complete quests for bonus rewards");
    sys_list->AddItem("• Visit the bank to store items");
    sys_list->AddItem("• Use the auction house to trade");
    sys_list->AddItem("• Pets can auto-loot for you");
    sys_list->AddItem("• Mounts increase movement speed");
    tabs_->AddTab("System", sys_list);
}

void HelpDialog::PopulateTopicsTab() {
    auto* topic_list = new ListBox(0, 0, 480, 360);
    topic_list->SetAutoScroll(false);

    if (topics_.empty()) {
        topic_list->AddItem("=== Quick Start Guide ===");
        topic_list->AddItem("");
        topic_list->AddItem("Welcome to LUNA Plus Reborn!");
        topic_list->AddItem("");
        topic_list->AddItem("1. Complete tutorial quests");
        topic_list->AddItem("2. Kill monsters to gain XP");
        topic_list->AddItem("3. Level up to learn new skills");
        topic_list->AddItem("4. Equip better gear");
        topic_list->AddItem("5. Join a guild for group play");
        topic_list->AddItem("6. Explore all maps");
        topic_list->AddItem("");
        topic_list->AddItem("=== Combat Guide ===");
        topic_list->AddItem("Click a monster to target it.");
        topic_list->AddItem("Press hotbar keys to use skills.");
        topic_list->AddItem("Use potions to restore HP/MP.");
        topic_list->AddItem("Watch for enemy attack patterns.");
        topic_list->AddItem("");
        topic_list->AddItem("=== Economy Guide ===");
        topic_list->AddItem("Sell loot to NPC shops.");
        topic_list->AddItem("Use the Auction House for rare items.");
        topic_list->AddItem("Open a street stall to sell to players.");
        topic_list->AddItem("Complete daily quests for gold.");
    } else {
        for (auto& t : topics_) {
            topic_list->AddItem(("--- " + t.title + " ---").c_str());
            topic_list->AddItem(t.content.c_str());
            topic_list->AddItem("");
        }
    }
    tabs_->AddTab("Guide", topic_list);
}

void HelpDialog::PopulateAboutTab() {
    auto* about_list = new ListBox(0, 0, 480, 360);
    about_list->SetAutoScroll(false);

    about_list->AddItem("=== About LUNA Plus Reborn ===");
    about_list->AddItem("");
    about_list->AddItem("A modern cross-platform revival of the");
    about_list->AddItem("classic MMORPG LUNA Online.");
    about_list->AddItem("");
    about_list->AddItem("Based on the original game by Eyasoft / GG.");
    about_list->AddItem("Rebuilt with modern C++ and bgfx rendering.");
    about_list->AddItem("");
    about_list->AddItem("=== Credits ===");
    about_list->AddItem("Development: LUNA Plus Team");
    about_list->AddItem("Engine: bgfx, entt, FlatBuffers");
    about_list->AddItem("Audio: OpenAL Soft");
    about_list->AddItem("Physics: Bullet Physics");
    about_list->AddItem("UI: Custom immediate-mode UI");
    about_list->AddItem("");
    char buf[128];
    snprintf(buf, sizeof(buf), "Version: %s", version_str_.c_str());
    about_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "Build: %s", build_date_.c_str());
    about_list->AddItem(buf);
    about_list->AddItem("");

    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    if (tm) {
        snprintf(buf, sizeof(buf), "Session: %04d-%02d-%02d %02d:%02d",
                 tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                 tm->tm_hour, tm->tm_min);
        about_list->AddItem(buf);
    }

    tabs_->AddTab("About", about_list);
}

void HelpDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    if (window_ && !window_->IsVisible()) Close();
}
