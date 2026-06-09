#pragma once
#include <ui/Screen.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <vector>
#include <string>

class LoadingScreen : public Screen {
public:
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
private:
    void LoadTips();
    void ShowRandomTip();

    float timer_ = 0;
    int tip_index_ = 0;
    ProgressBar* progress_bar_ = nullptr;
    std::vector<std::string> tips_;
    float tip_timer_ = 0.0f;
    int current_tip_ = 0;
};
