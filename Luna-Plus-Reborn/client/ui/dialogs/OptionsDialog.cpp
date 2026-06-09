#include "OptionsDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/ListBox.hpp>
#include <config/ConfigManager.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void OptionsDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Option.bin.txt");

    if (!window_) {
        spdlog::warn("OptionsDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("OPTIONS", 200, 80, 480, 420);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    tabs_ = window_->AddWidget<TabPanel>(10, 24, 460, 330);

    BuildGameplayTab();
    BuildChatFilterTab();
    BuildGraphicsTab();
    BuildAudioTab();
    BuildActionButtons();
    ReadSettings();
}

void OptionsDialog::BuildGameplayTab() {
    auto* list = new ListBox(0, 0, 440, 310);
    const char* items[] = {
        "Block Trade Requests",
        "Block Party Invites",
        "Block Friend Requests",
        "Block Duel Requests",
        "Show Battle Gauge",
        "Show Guild Name",
        "Show Party Name",
        "Show Other Names",
        "Show Party Damage",
        "Show Game Tips",
        "Hide Other Players",
        "Hide Other Pets",
        "Hide Stall Titles",
        "Auto Quest Notice",
        "",
        "[1-9] Toggle options  [R] Reset tab",
    };
    for (auto* s : items) list->AddItem(s);
    tabs_->AddTab("Gameplay", list);
}

void OptionsDialog::BuildChatFilterTab() {
    auto* list = new ListBox(0, 0, 440, 310);
    const char* items[] = {
        "Block Whispers",
        "Block General Chat",
        "Hide Speech Bubbles",
        "Block Party Chat",
        "Block Family Chat",
        "Block Shout/World Chat",
        "Block Guild Chat",
        "Block Alliance Chat",
        "Block System Messages",
        "Block EXP/Acquire Msgs",
        "Block Item Messages",
        "",
        "[1-9] Toggle filters",
    };
    for (auto* s : items) list->AddItem(s);
    tabs_->AddTab("Chat Filter", list);
}

void OptionsDialog::BuildGraphicsTab() {
    auto* list = new ListBox(0, 0, 440, 310);
    char buf[128];

    snprintf(buf, sizeof(buf), "View Distance: %d", sight_distance_);
    list->AddItem(buf);
    list->AddItem("[PageUp/Dn] Adjust view distance");
    list->AddItem("");

    bool hs = ConfigManager::GetBool("video.hero_shadow", true);
    snprintf(buf, sizeof(buf), "Hero Shadow: %s", hs ? "ON" : "OFF");
    list->AddItem(buf);

    bool ms = ConfigManager::GetBool("video.monster_shadow", true);
    snprintf(buf, sizeof(buf), "Monster Shadow: %s", ms ? "ON" : "OFF");
    list->AddItem(buf);

    bool os = ConfigManager::GetBool("video.others_shadow", true);
    snprintf(buf, sizeof(buf), "Others Shadow: %s", os ? "ON" : "OFF");
    list->AddItem(buf);
    list->AddItem("[1/2/3] Toggle Hero/Monster/Others Shadow");
    list->AddItem("");

    bool ac = ConfigManager::GetBool("video.auto_graphics", false);
    snprintf(buf, sizeof(buf), "Auto Graphics Control: %s", ac ? "ON" : "OFF");
    list->AddItem(buf);
    list->AddItem("[4] Toggle Auto Graphics");
    list->AddItem("");

    snprintf(buf, sizeof(buf), "Graphic Quality: %s", graphic_quality_basic_ ? "Basic" : "Lower");
    list->AddItem(buf);
    list->AddItem("[5] Toggle Graphic Quality");

    tabs_->AddTab("Graphics", list);
}

void OptionsDialog::BuildAudioTab() {
    auto* list = new ListBox(0, 0, 440, 310);
    char buf[128];

    snprintf(buf, sizeof(buf), "BGM: %s  Volume: %d%%",
             (bgm_volume_ > 0) ? "ON" : "OFF", bgm_volume_);
    list->AddItem(buf);
    list->AddItem("[Q] BGM-  [W] BGM+  [Z] Toggle BGM");
    list->AddItem("");

    snprintf(buf, sizeof(buf), "SFX: %s  Volume: %d%%",
             (sfx_volume_ > 0) ? "ON" : "OFF", sfx_volume_);
    list->AddItem(buf);
    list->AddItem("[A] SFX-  [S] SFX+  [X] Toggle SFX");

    tabs_->AddTab("Audio", list);
}

void OptionsDialog::BuildActionButtons() {
    auto* ok = window_->AddWidget<Button>("OK", 140, 365, 80, 28);
    ok->SetColors({50,90,130,220}, {80,120,160,220}, {30,60,100,220});
    ok->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnOK();
    });

    auto* cancel = window_->AddWidget<Button>("Cancel", 230, 365, 80, 28);
    cancel->SetColors({50,90,130,220}, {80,120,160,220}, {30,60,100,220});
    cancel->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnCancel();
    });

    auto* reset = window_->AddWidget<Button>("Reset", 320, 365, 80, 28);
    reset->SetColors({50,90,130,220}, {80,120,160,220}, {30,60,100,220});
    reset->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnReset();
    });
}

void OptionsDialog::ReadSettings() {
    sight_distance_ = ConfigManager::GetInt("video.view_distance", 100);
    bgm_volume_ = static_cast<int>(ConfigManager::GetFloat("audio.bgm_volume", 0.7f) * 100);
    sfx_volume_ = static_cast<int>(ConfigManager::GetFloat("audio.sfx_volume", 0.8f) * 100);
}

void OptionsDialog::ApplySettings() {
    ConfigManager::SetInt("video.view_distance", sight_distance_);
    ConfigManager::SetFloat("audio.bgm_volume", bgm_volume_ / 100.0f);
    ConfigManager::SetFloat("audio.sfx_volume", sfx_volume_ / 100.0f);
    ConfigManager::Save();
    spdlog::info("OptionsDialog: settings applied and saved");
}

void OptionsDialog::OnOK() {
    ApplySettings();
    Close();
}

void OptionsDialog::OnCancel() {
    Close();
}

void OptionsDialog::OnReset() {
    sight_distance_ = 100;
    bgm_volume_ = 80;
    sfx_volume_ = 80;
    graphic_quality_basic_ = true;
    ApplySettings();
    Close();
    spdlog::info("OptionsDialog: reset to defaults");
}

void OptionsDialog::Close() {
    if (window_) {
        window_->Close();
        window_ = nullptr;
    }
}
