#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <gameobjects/Pet.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <functional>

class PetDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, Pet* pet);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(std::function<void()> summon_fn,
                             std::function<void()> dismiss_fn,
                             std::function<void()> feed_fn,
                             std::function<void()> evolve_fn,
                             std::function<void()> refresh_fn);

private:
    Window* window_ = nullptr;
    Label* info_label_ = nullptr;
    Label* status_label_ = nullptr;
    ProgressBar* hunger_bar_ = nullptr;
    ProgressBar* exp_bar_ = nullptr;
    Pet* pet_ = nullptr;
    std::function<void()> summon_fn_;
    std::function<void()> dismiss_fn_;
    std::function<void()> feed_fn_;
    std::function<void()> evolve_fn_;
    std::function<void()> refresh_fn_;
    Button* feed_btn_ = nullptr;
    Button* evolve_btn_ = nullptr;
    void Refresh(GameState* state = nullptr);
};
