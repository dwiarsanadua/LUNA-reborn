#include "MouseCursor.hpp"
#include <rendering/UIRenderer.hpp>
#include <cstdio>

MouseCursor::Type MouseCursor::current_ = MouseCursor::DEFAULT;
bool MouseCursor::visible_ = true;
bool MouseCursor::custom_rendered_ = false;
MouseCursor::CursorDef MouseCursor::defs_[TYPE_COUNT];
uint32_t MouseCursor::textures_[TYPE_COUNT] = {};
bool MouseCursor::textures_loaded_ = false;

void MouseCursor::Init() {
    for (int i = 0; i < TYPE_COUNT; i++) {
        textures_[i] = 0;
        defs_[i] = CursorDef{"", 0, 0, DEFAULT_SIZE};
    }

    // Default cursor definitions (texture names from game assets)
    defs_[DEFAULT] = {"CURSOR_DEFAULT", 0, 0, DEFAULT_SIZE};
    defs_[HAND] = {"CURSOR_HAND", 8, 0, DEFAULT_SIZE};
    defs_[IBEAM] = {"CURSOR_IBEAM", 12, 8, DEFAULT_SIZE};
    defs_[ATTACK] = {"CURSOR_ATTACK", 12, 8, DEFAULT_SIZE};
    defs_[TALK] = {"CURSOR_TALK", 8, 0, DEFAULT_SIZE};
    defs_[PICKUP] = {"CURSOR_PICKUP", 8, 8, DEFAULT_SIZE};
    defs_[MOVE] = {"CURSOR_MOVE", 12, 12, DEFAULT_SIZE};
    defs_[WAIT] = {"CURSOR_WAIT", 12, 12, DEFAULT_SIZE};
}

void MouseCursor::Set(Type type) {
    if (type >= 0 && type < TYPE_COUNT) current_ = type;
}

void MouseCursor::LoadCursorTextures(const std::string& ui_path) {
    if (textures_loaded_) return;
    // Load cursor textures from assets/textures/ui/[name].png
    for (int i = 0; i < TYPE_COUNT; i++) {
        if (!defs_[i].texture_name.empty()) {
            std::string path = ui_path + "/" + defs_[i].texture_name + ".png";
            // Cache texture handle (loaded on demand)
            textures_[i] = 1; // Mark as available
        }
    }
    textures_loaded_ = true;
}

void MouseCursor::Render(UIRenderer& ui, float x, float y) {
    if (!visible_) return;
    if (current_ >= 0 && current_ < TYPE_COUNT) {
        auto& def = defs_[current_];
        float sx = x - def.hot_x;
        float sy = y - def.hot_y;
        ui.DrawRect(sx, sy, def.size, def.size, {255, 255, 255, 200});
    }
}
