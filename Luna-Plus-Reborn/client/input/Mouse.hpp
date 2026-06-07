#pragma once
#include <cstdint>
#include <string>
#include <vector>

class UIRenderer;

class Mouse {
public:
    enum CursorType {
        CURSOR_DEFAULT,
        CURSOR_HAND,
        CURSOR_IBEAM,
        CURSOR_CROSS,
        CURSOR_MOVE,
        CURSOR_SIZE_ALL,
        CURSOR_SIZE_NS,
        CURSOR_SIZE_EW,
        CURSOR_WAIT,
        CURSOR_ATTACK,
        CURSOR_TALK,
        CURSOR_PICKUP,
        CURSOR_COUNT
    };

    static void Init();
    static void Update();

    // Cursor
    static void SetCursor(CursorType type);
    static CursorType GetCursor() { return current_cursor_; }
    static void ShowCursor(bool show);
    static bool IsCursorVisible() { return cursor_visible_; }
    static void SetCursorTexture(const std::string& texture_path, int hot_x = 0, int hot_y = 0);

    // Click detection
    static bool IsDoubleClick(int button);
    static bool IsClickHeld(int button, float duration);

    // Drag detection
    static bool IsDragging(int button);
    static float GetDragDistance(int button);
    static float GetDragX(int button) { return drag_start_x_[button]; }
    static float GetDragY(int button) { return drag_start_y_[button]; }
    static float GetDragCurrentX(int button) { return drag_current_x_[button]; }
    static float GetDragCurrentY(int button) { return drag_current_y_[button]; }

    // Custom cursor rendering
    static void RenderCursor(UIRenderer& ui);
    static void SetCustomCursor(bool enabled) { use_custom_cursor_ = enabled; }

private:
    static CursorType current_cursor_;
    static bool cursor_visible_;
    static bool use_custom_cursor_;

    // Drag state
    static bool dragging_[8];
    static float drag_start_x_[8], drag_start_y_[8];
    static float drag_current_x_[8], drag_current_y_[8];
    static float drag_threshold_;

    // Custom cursor texture
    static std::string cursor_texture_path_;
    static int cursor_hot_x_, cursor_hot_y_;

    // Cursor texture loading
    static uint32_t cursor_textures_[CURSOR_COUNT];
    static bool cursor_textures_loaded_;
};
