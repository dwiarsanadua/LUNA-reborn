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

    // Helper to load a texture from a list of VFS paths, returns BGFX_INVALID_HANDLE if all fail
    bgfx::TextureHandle LoadTex(const char** paths, int count);

    // Launcher sprite textures (Launcher_01_XX.png)
    bgfx::TextureHandle tex_bg_       = BGFX_INVALID_HANDLE; // C_launcher.png  946x539
    bgfx::TextureHandle tex_panel_    = BGFX_INVALID_HANDLE; // _01.png  595x480   main panel frame
    bgfx::TextureHandle tex_btn_n_    = BGFX_INVALID_HANDLE; // _07.png   90x24   button normal
    bgfx::TextureHandle tex_btn_h_    = BGFX_INVALID_HANDLE; // _08.png   90x24   button hover
    bgfx::TextureHandle tex_btn_p_    = BGFX_INVALID_HANDLE; // _09.png   90x24   button pressed
    bgfx::TextureHandle tex_field_n_  = BGFX_INVALID_HANDLE; // _11.png  112x51   field normal
    bgfx::TextureHandle tex_field_a_  = BGFX_INVALID_HANDLE; // _12.png  112x51   field active
    bgfx::TextureHandle tex_div_      = BGFX_INVALID_HANDLE; // _05.png  472x9    divider/separator
    bgfx::TextureHandle tex_close_n_  = BGFX_INVALID_HANDLE; // _02.png   17x17   close btn normal
    bgfx::TextureHandle tex_close_h_  = BGFX_INVALID_HANDLE; // _03.png   17x17   close btn hover
    // Original Luna Plus converted assets
    bgfx::TextureHandle tex_logo_     = BGFX_INVALID_HANDLE; // loginbar_logo.png  87x62   Luna logo
    bgfx::TextureHandle tex_bar_      = BGFX_INVALID_HANDLE; // login_bar00.png  1024x128  bottom bar strip

    F fields_[2] = {};
    bool btn_login_hover_   = false;
    bool btn_offline_hover_ = false;
    bool sent_        = false;
    float anim_time_  = 0.0f;
    std::function<void(uint32_t)> set_clear_color_;
    SceneRenderer* scene_renderer_ = nullptr;
    GameState* state_    = nullptr;
    NetworkClient* network_ = nullptr;
};
