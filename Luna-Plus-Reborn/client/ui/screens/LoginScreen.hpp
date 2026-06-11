#pragma once
#include <ui/Screen.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/SceneRenderer.hpp>
#include <bgfx/bgfx.h>
#include <functional>
#include <glm/glm.hpp>

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
    
    WindowManager* GetWindowManager() { return &wm_; }
    
private:
    struct TextField { 
        char buf[64] = {0}; 
        int pos = 0; 
        bool active = false; 
        bool masked = false; 
        char label[16] = {0};
        glm::vec4 rect = {0,0,0,0};  // x, y, w, h
    };
    
    bool DoLogin();
    
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    WindowManager wm_;
    class Window* login_win_ = nullptr;
    TextField fields_[2] = {};
    bool sent_ = false;
    float anim_time_ = 0.0f;
    std::function<void(uint32_t)> set_clear_color_;
    SceneRenderer* scene_renderer_ = nullptr;
    GameState* state_ = nullptr;
    NetworkClient* network_ = nullptr;
};
