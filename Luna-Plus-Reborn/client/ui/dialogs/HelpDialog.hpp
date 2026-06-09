#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/Label.hpp>
#include <string>
#include <vector>

struct HelpTopic {
    std::string title;
    std::string content;
};

class HelpDialog {
public:
    Window* GetWindow() { return window_; }

    void Open(WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);

    void SetTopics(const std::vector<HelpTopic>& topics);
    void SetVersionInfo(const std::string& version, const std::string& build_date);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;

    std::vector<HelpTopic> topics_;
    std::string version_str_;
    std::string build_date_;

    void PopulateCommandsTab();
    void PopulateSystemTab();
    void PopulateTopicsTab();
    void PopulateAboutTab();
};
