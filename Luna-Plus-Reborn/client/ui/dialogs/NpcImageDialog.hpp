#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <string>
#include <cstdint>
#include <bgfx/bgfx.h>

class NpcImageDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm, uint32_t npc_id, const std::string& npc_name = "", bool is_monster = false);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    void SetNpcId(uint32_t npc_id, const std::string& npc_name = "", bool is_monster = false);

private:
    std::string GetTextureName() const;

    Window* window_ = nullptr;
    bgfx::TextureHandle portrait_tex_ = BGFX_INVALID_HANDLE;
    uint32_t current_npc_id_ = 0;
    std::string current_npc_name_;
    bool is_monster_ = false;
};
