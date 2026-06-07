#pragma once
#include <ui/Screen.hpp>

class LoadingScreen : public Screen {
public:
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
private:
    float timer_ = 0;
    int tip_index_ = 0;
};
