#include "LauncherScreen.hpp"
#include <config/Paths.hpp>
#include <engine/gx_render/VFS.h>
#include <engine/gx_render/Shader.h>
#include <ui/ClientFlow.hpp>
#include <stb_image.h>
#include <spdlog/spdlog.h>

void LauncherScreen::Enter() {
    spdlog::info("Entering Launcher Screen");
    TexturesLoadOnce();
    state_ = State::Logo;
    state_timer_ = 0;
}

void LauncherScreen::Exit() {
    spdlog::info("Exiting Launcher Screen");
}

void LauncherScreen::TexturesLoadOnce() {
    if (textures_loaded_) return;

    std::string bg_paths[] = {
        VFS::Find("assets/textures/ui/Launcher/Launcher_01_01.png"),
        VFS::Find("assets/textures/Launcher_01_01.png"),
    };
    for (auto& p : bg_paths) {
        if (p.empty()) continue;
        int w, h, n;
        unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
        if (d) {
            tex_bg_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w*h*4));
            stbi_image_free(d);
            if (bgfx::isValid(tex_bg_)) break;
        }
    }
    textures_loaded_ = true;
}

void LauncherScreen::Render(UIRenderer& ui) {
    float lw = ui.logicalWidth;
    float lh = ui.logicalHeight;

    // Background — fullscreen
    if (bgfx::isValid(tex_bg_))
        ui.DrawImage(0, 0, lw, lh, tex_bg_);
    else
        ui.DrawRect(0, 0, lw, lh, UIColor{20, 20, 40, 255});

    // Logo text
    ui.DrawTextCentered(lh * 0.15f, 0xFFFFCC88, "LUNA Online Plus");
    ui.DrawTextCentered(lh * 0.15f + 28, 0xFF888888, "Reborn v1.1.0");

    // START button
    float bx = lw * 0.5f - 100;
    float by = lh * 0.48f;
    ui.DrawButton(bx, by, 200, 48, "  START", false);
    ui.DrawTextCentered(by + 14, 0xFFFFFFFF, "PRESS ENTER");

    // Patch status
    if (state_ == State::Checking) {
        ui.DrawTextCentered(lh * 0.65f, 0xFFAAAAAA, "Checking for updates...");
    } else if (state_ == State::Error) {
        ui.DrawTextCentered(lh * 0.65f, 0xFFFF4444, "Patch check failed, continuing...");
    }

    // Copyright
    ui.DrawTextCentered(lh * 0.93f, 0xFF666666, "LUNA Plus Reborn");
    ui.DrawTextCentered(lh * 0.96f, 0xFF444444, "Based on LUNA Online by Eyasoft / GG");
}

bool LauncherScreen::HandleKey(int key, int scancode, int action, int mods) {
    if (action != 1) return false;
    if (key == 257 || key == 32) { // Enter or Space
        ClientFlow::StartLogin();
        return true;
    }
    return false;
}

void LauncherScreen::Update(float dt) {
    state_timer_ += dt;
    if (state_ == State::Logo && state_timer_ > 0.5f) {
        state_ = State::Ready;
    }
}
