#pragma once
#include <ui/Screen.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/SceneRenderer.hpp>
#include <bgfx/bgfx.h>

class LoginScreen : public Screen {
public:
    void Init(GameState* state, NetworkClient* network) override;
    void Enter() override;
    void Exit() override;
    void Update(float dt) override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    bool HandlePacket(uint16_t type, const std::vector<uint8_t>& payload) override;
    void SetSceneClearer(std::function<void(uint32_t)> f) { set_clear_color_ = f; }
    void SetSceneRenderer(SceneRenderer* sr) { scene_renderer_ = sr; }
private:
    void LoadTexture(bgfx::TextureHandle& cache, const std::string& name);
    void TexturesLoadOnce();
    WindowManager wm_;
    std::function<void(uint32_t)> set_clear_color_;
    SceneRenderer* scene_renderer_ = nullptr;
    bool sent_ = false;
    bool textures_loaded_ = false;
    bool bg_ok_ = false;
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_bar_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_btn_ = BGFX_INVALID_HANDLE;
};
