#include "NpcImageDialog.hpp"
#include <ui/skin/UiSkinManager.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/UIRenderer.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

static constexpr float PORTRAIT_W = 120.0f;
static constexpr float PORTRAIT_H = 160.0f;
static constexpr float WIN_W = 150.0f;
static constexpr float WIN_H = 210.0f;

NpcImageDialog::NpcImageDialog() {}

void NpcImageDialog::Open(WindowManager* wm, uint32_t npc_id, const std::string& npc_name, bool is_monster) {
    if (window_) return;
    current_npc_id_ = npc_id;
    current_npc_name_ = npc_name;
    is_monster_ = is_monster;
    portrait_tex_ = BGFX_INVALID_HANDLE;

    if (wm) {
        window_ = wm->Open("", 600, 200, WIN_W, WIN_H);
    } else {
        window_ = new Window("", 600, 200, WIN_W, WIN_H);
    }

    if (!npc_name.empty()) {
        window_->SetTitle(npc_name);
    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "NPC %u", npc_id);
        window_->SetTitle(buf);
    }

    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->SetCustomBackground([this](UIRenderer& ui, float x, float y, float w, float h) {
        if (!bgfx::isValid(portrait_tex_)) {
            std::string tex_name = GetTextureName();
            portrait_tex_ = UiSkinManager::LoadTexture(tex_name);
            if (!bgfx::isValid(portrait_tex_)) {
                spdlog::warn("NpcImageDialog: no portrait texture '{}'", tex_name);
            }
        }

        ui.DrawRect(x, y, w, h, {16, 16, 32, 220});
        ui.DrawBorder(x, y, w, h, {100, 180, 255, 200});

        if (bgfx::isValid(portrait_tex_)) {
            float px = x + (w - PORTRAIT_W) * 0.5f;
            float py = y + 28.0f + ((h - 28.0f) - PORTRAIT_H) * 0.5f;
            ui.DrawImage(px, py, PORTRAIT_W, PORTRAIT_H, portrait_tex_);
        }
    });
}

void NpcImageDialog::Close() {
    window_ = nullptr;
}

void NpcImageDialog::SetNpcId(uint32_t npc_id, const std::string& npc_name, bool is_monster) {
    current_npc_id_ = npc_id;
    current_npc_name_ = npc_name;
    is_monster_ = is_monster;
    portrait_tex_ = BGFX_INVALID_HANDLE;
}

std::string NpcImageDialog::GetTextureName() const {
    char buf[64];
    if (is_monster_) {
        snprintf(buf, sizeof(buf), "npcImage/mon%u_basic", current_npc_id_);
    } else {
        snprintf(buf, sizeof(buf), "npcImage/npc%u_01", current_npc_id_);
    }
    return std::string(buf);
}
