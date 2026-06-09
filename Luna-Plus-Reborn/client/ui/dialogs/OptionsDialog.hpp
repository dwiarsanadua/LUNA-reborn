#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/CheckBox.hpp>
#include <ui/widgets/Button.hpp>
#include <string>
#include <vector>

class OptionsDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close();
    void OnOK();
    void OnCancel();
    void OnReset();

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;

    // Settings state
    int sight_distance_ = 100;
    int bgm_volume_ = 80;
    int sfx_volume_ = 80;
    bool graphic_quality_basic_ = true;

    void BuildGameplayTab();
    void BuildChatFilterTab();
    void BuildGraphicsTab();
    void BuildAudioTab();
    void BuildActionButtons();
    void ApplySettings();
    void ReadSettings();
};
