#pragma once
#include <ui/Screen.hpp>
#include <bgfx/bgfx.h>

class LauncherScreen : public Screen {
public:
    void Enter() override;
    void Exit() override;
    void Render(UIRenderer& ui) override;
    bool HandleKey(int key, int scancode, int action, int mods) override;
    void Update(float dt) override;

private:
    bgfx::TextureHandle tex_bg_ = BGFX_INVALID_HANDLE;
    bool textures_loaded_ = false;
    float patch_progress_ = 0.0f;

    enum class State { Logo, Checking, Ready, Error };
    State state_ = State::Logo;
    float state_timer_ = 0;

    void TexturesLoadOnce();
};
