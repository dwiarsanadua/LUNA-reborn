#pragma once
#include <ui/Screen.hpp>
#include <rendering/SceneRenderer.hpp>
#include <bgfx/bgfx.h>
#include <functional>

class LoginScreen : public Screen {
public:
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Exit() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    bool HandleChar(unsigned int codepoint) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
    void SetSceneClearer(std::function<void(uint32_t)> f) { set_clear_color_ = f; }
    void SetSceneRenderer(SceneRenderer* sr) { scene_renderer_ = sr; }
private:
    struct F { char buf[64]={0}; int pos=0; bool act=false; bool mask=false; char lbl[16]={0}; };
    bool DoLogin();
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    F fields_[2] = {};
    bool sent_ = false;
    std::function<void(uint32_t)> set_clear_color_;
    SceneRenderer* scene_renderer_ = nullptr;
    GameState* state_ = nullptr;
    NetworkClient* network_ = nullptr;
};
