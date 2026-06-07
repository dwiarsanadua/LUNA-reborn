#pragma once
#include <ui/Window.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <cstdint>

class FishingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state);
    void Close() { window_ = nullptr; active_ = false; }
    void Update(float dt);
    void Render(UIRenderer& ui);
    
    bool IsActive() const { return active_; }
    bool IsCatching() const { return catching_; }
    float GetProgress() const { return progress_; }
    void StartCasting() { casting_ = true; cast_timer_ = 0; }
    
    void SetResultCallback(std::function<void(bool success, int fish_type)> cb) { result_cb_ = cb; }

private:
    Window* window_ = nullptr;
    Label* status_label_ = nullptr;
    Label* fish_label_ = nullptr;
    ProgressBar* progress_bar_ = nullptr;
    
    bool active_ = false;
    bool casting_ = false;
    bool catching_ = false;
    bool caught_ = false;
    
    float cast_timer_ = 0;
    float wait_timer_ = 0;
    float bite_timer_ = 0;
    float catch_window_ = 0;
    float progress_ = 0;
    float progress_dir_ = 1.0f;
    int fish_type_ = 0;
    
    std::function<void(bool, int)> result_cb_;
    
    static constexpr float MAX_WAIT = 8.0f;
    static constexpr float CATCH_WINDOW = 0.3f;
};
