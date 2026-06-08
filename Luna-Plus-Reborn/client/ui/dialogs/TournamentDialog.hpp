#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/InputField.hpp>
#include <gameobjects/TournamentSystem.hpp>
#include <functional>

class TournamentDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, TournamentSystem* ts);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(std::function<void(uint32_t)> register_fn,
                             std::function<void(uint32_t)> unregister_fn,
                             std::function<void(uint32_t)> claim_fn,
                             std::function<void()> refresh_fn);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* info_label_ = nullptr;
    Label* bracket_label_ = nullptr;
    InputField* tournament_id_input_ = nullptr;
    TournamentSystem* ts_ = nullptr;
    std::function<void(uint32_t)> register_fn_;
    std::function<void(uint32_t)> unregister_fn_;
    std::function<void(uint32_t)> claim_fn_;
    std::function<void()> refresh_fn_;
    void Refresh(GameState* state = nullptr);
    uint32_t SelectedTournamentId(GameState* state) const;
    static const char* StateLabel(uint8_t state);
};
