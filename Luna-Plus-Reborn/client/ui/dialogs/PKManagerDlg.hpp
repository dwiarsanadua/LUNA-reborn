#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/UIRenderer.hpp>
#include <string>
#include <functional>
#include <cstdint>

enum class PKMode {
    Peaceful,
    Aggressive,
    Defense
};

enum class LootMode {
    None,
    Party,
    All
};

struct PKState {
    PKMode mode = PKMode::Peaceful;
    int pk_points = 0;
    int kills = 0;
    float pk_timer = 0; // seconds since last PK action
    bool is_flagged = false;
    bool is_penalized = false;
    LootMode loot_mode = LootMode::Party;
};

class PKManagerDlg {
public:
    PKManagerDlg();

    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return window_ != nullptr; }

    // State
    const PKState& GetState() const { return state_; }
    void SetMode(PKMode mode);
    void SetLootMode(LootMode mode);
    void AddPKPoint(int pts);
    void ResetPKPoints();

    // Integration
    bool CanAttack(bool is_player) const;
    bool IsPvPAllowed() const { return state_.mode != PKMode::Peaceful; }
    void OnKill();
    void Update(float dt);

    using ModeChangeCallback = std::function<void(PKMode)>;
    void SetModeChangeCallback(ModeChangeCallback cb) { mode_cb_ = cb; }

    void Render(UIRenderer& ui);

private:
    Window* window_ = nullptr;
    PKState state_;

    ModeChangeCallback mode_cb_;

    void ApplyPenalties();
};
