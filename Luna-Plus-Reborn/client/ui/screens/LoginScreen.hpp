#pragma once
#include <ui/Screen.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/SceneRenderer.hpp>
#include <bgfx/bgfx.h>
#include <string>

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

    struct InputField {
        char buffer[64] = {0};
        int cursor_pos = 0;
        bool active = false;
        bool masked = false;
        std::string label;
        float x = 0, y = 0, w = 200, h = 28;
    };

private:
    void LoadTexture(bgfx::TextureHandle& cache, const std::string& name);
    void TexturesLoadOnce();
    void DrawField(UIRenderer& ui, const InputField& field, bool focus);
    bool DoLogin();

    WindowManager wm_;
    std::function<void(uint32_t)> set_clear_color_;
    SceneRenderer* scene_renderer_ = nullptr;
    bool sent_ = false;
    bool textures_loaded_ = false;
    bool bg_ok_ = false;
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_bar_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tex_btn_ = BGFX_INVALID_HANDLE;
    InputField id_field_;
    InputField pw_field_;
    bool save_id_ = false;
    std::string error_message_;
    float error_timer_ = 0;
};
