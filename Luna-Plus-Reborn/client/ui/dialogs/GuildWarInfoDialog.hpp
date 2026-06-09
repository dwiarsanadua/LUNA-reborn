#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/Label.hpp>
#include <string>

class GuildWarInfoDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetWarData(const std::vector<std::string>& enemies,
                    const std::vector<std::string>& allies,
                    int wins, int draws, int losses);

private:
    Window* window_ = nullptr;
    ListBox* enemy_list_ = nullptr;
    ListBox* ally_list_ = nullptr;
    Label* war_record_ = nullptr;
};
