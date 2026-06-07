#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class UIRenderer;

class MouseCursor {
public:
    enum Type {
        DEFAULT, HAND, IBEAM, CROSS, MOVE,
        SIZE_ALL, SIZE_NS, SIZE_EW, WAIT,
        ATTACK, TALK, PICKUP, TYPE_COUNT
    };

    static void Init();
    static void Set(Type type);
    static Type Get() { return current_; }
    static void Show(bool s) { visible_ = s; }
    static bool IsVisible() { return visible_; }

    // Load cursor textures from game assets
    static void LoadCursorTextures(const std::string& ui_path);
    static void Render(UIRenderer& ui, float x, float y);

    static constexpr float DEFAULT_SIZE = 24.0f;

private:
    struct CursorDef {
        std::string texture_name;
        int hot_x = 0, hot_y = 0;
        float size = DEFAULT_SIZE;
    };

    static Type current_;
    static bool visible_;
    static bool custom_rendered_;
    static CursorDef defs_[TYPE_COUNT];
    static uint32_t textures_[TYPE_COUNT];
    static bool textures_loaded_;
};
