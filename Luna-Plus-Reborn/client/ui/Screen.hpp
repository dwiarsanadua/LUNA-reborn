#pragma once
#include <rendering/UIRenderer.hpp>
#include <glm/glm.hpp>
#include <memory>

struct GameState;
class NetworkClient;

class Screen {
public:
    virtual ~Screen() = default;
    virtual void Init(GameState* state, NetworkClient* network) { state_ = state; network_ = network; }
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float dt) = 0;
    virtual void Render(UIRenderer& ui) = 0;
    virtual void Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) { (void)view;(void)proj; Render(ui); }
    virtual bool HandleKey(int key, int scancode, int action, int mods) { (void)key;(void)scancode;(void)action;(void)mods; return false; }
    virtual bool HandleChar(unsigned int codepoint) { (void)codepoint; return false; }
    virtual bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) { (void)type;(void)payload; return false; }
protected:
    GameState* state_ = nullptr;
    NetworkClient* network_ = nullptr;
};
