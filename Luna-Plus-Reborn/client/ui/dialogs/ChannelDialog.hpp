#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Grid.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

struct ChannelInfo {
    int id = 0;
    std::string name;
    int load_pct = 0;
    bool is_premium = false;
};

class ChannelDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetChannels(const std::vector<ChannelInfo>& channels);
    void SetChannelCallback(std::function<void(int)> on_select);

private:
    void RebuildGrid();

    Window* window_ = nullptr;
    Grid* grid_ = nullptr;
    Label* title_label_ = nullptr;
    std::vector<ChannelInfo> channels_;
    std::function<void(int)> on_select_;
};
