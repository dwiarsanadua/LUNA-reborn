#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <functional>
#include <cstdint>

class FarmUpgradeDialog {
public:
    using UpgradeCallback = std::function<void(int farm_zone, int farm_id, int upgrade_kind)>;

    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm,
              int farm_grade, int farm_zone, int farm_id,
              UpgradeCallback on_upgrade = nullptr);
    void Close();
    void Refresh(GameState* state);

private:
    Window* window_ = nullptr;
    int farm_grade_ = 1;
    int farm_zone_ = 0;
    int farm_id_ = 0;
    UpgradeCallback on_upgrade_;

    static constexpr int kMaxGrade = 10;
    static int UpgradeGoldCost(int grade);
    static int UpgradeHonorCost(int grade);
};
