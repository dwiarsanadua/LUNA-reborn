#include "OptionsDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <spdlog/spdlog.h>
#include <config/ConfigManager.hpp>
#include <cstdio>

void OptionsDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Option.bin.txt");

    if (!window_) {
        spdlog::warn("OptionsDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("OPTIONS", 200, 80, 400, 350);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 24, 380, 290);

    // === Video Tab ===
    auto* video_list = new ListBox(0, 0, 360, 270);
    char buf[256];
    snprintf(buf, sizeof(buf), "Resolution: %dx%d", ConfigManager::GetResolutionWidth(), ConfigManager::GetResolutionHeight());
    video_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "Fullscreen: %s", ConfigManager::GetFullscreen() ? "ON" : "OFF");
    video_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "VSync: %s", ConfigManager::GetVSync() ? "ON" : "OFF");
    video_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "FPS Limit: %d", ConfigManager::GetFPSLimit());
    video_list->AddItem(buf);
    video_list->AddItem("");
    video_list->AddItem("[1] Toggle Fullscreen  [2] Toggle VSync");
    tabs->AddTab("Video", video_list);

    // === Audio Tab ===
    auto* audio_list = new ListBox(0, 0, 360, 270);
    snprintf(buf, sizeof(buf), "Master Volume: %.0f%%", ConfigManager::GetMasterVolume() * 100);
    audio_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "BGM Volume: %.0f%%", ConfigManager::GetBGMVolume() * 100);
    audio_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "SFX Volume: %.0f%%", ConfigManager::GetSFXVolume() * 100);
    audio_list->AddItem(buf);
    audio_list->AddItem("");
    audio_list->AddItem("[Q] Master-  [W] Master+  [A] BGM-  [S] BGM+");
    audio_list->AddItem("[Z] SFX-  [X] SFX+");
    tabs->AddTab("Audio", audio_list);

    // === Gameplay Tab ===
    auto* gameplay_list = new ListBox(0, 0, 360, 270);
    snprintf(buf, sizeof(buf), "Show Damage: %s", ConfigManager::ShowDamageNumbers() ? "YES" : "NO");
    gameplay_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "Camera Speed: %.1f", ConfigManager::GetCameraSpeed());
    gameplay_list->AddItem(buf);
    snprintf(buf, sizeof(buf), "Language: %s", ConfigManager::GetLanguage().c_str());
    gameplay_list->AddItem(buf);
    gameplay_list->AddItem("");
    gameplay_list->AddItem("[D] Toggle Damage Numbers");
    tabs->AddTab("Gameplay", gameplay_list);
}
