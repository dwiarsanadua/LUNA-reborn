#pragma once
#include <rendering/UIRenderer.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

struct UISkinPart {
    std::string name;
    std::string texture_path;
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
    // Nine-patch slices
    float border_left = 0, border_right = 0, border_top = 0, border_bottom = 0;
    int tex_w = 0, tex_h = 0;
};

class UiSkinManager {
public:
    static void Init(const std::string& ui_texture_path = "assets/textures/ui/",
                     const std::string& interface_path = "assets/interface/");
    static void Shutdown();

    // Load a single UI texture by name
    static bgfx::TextureHandle LoadTexture(const std::string& name, const std::string& path = "");
    static bool IsLoaded(const std::string& name);

    // Get cached texture
    static bgfx::TextureHandle GetTexture(const std::string& name);
    static UISkinPart* GetSkinPart(const std::string& part_name);

    // Window skin definition
    struct WindowSkin {
        std::string name;
        // 9-patch textures for window chrome
        std::string tex_title_left, tex_title_center, tex_title_right;
        std::string tex_border_left, tex_border_right;
        std::string tex_border_top, tex_border_bottom;
        std::string tex_corner_tl, tex_corner_tr, tex_corner_bl, tex_corner_br;
        std::string tex_body;
        // Close/minimize button textures
        std::string tex_btn_close, tex_btn_close_hover;
        std::string tex_btn_min, tex_btn_min_hover;
        // Colors
        uint32_t title_color = 0x3C3C78E6;
        uint32_t body_color = 0x101020DC;
        uint32_t border_color = 0x64B4FFC8;
    };
    static WindowSkin* GetWindowSkin(const std::string& window_type);
    static void RegisterWindowSkin(const std::string& window_type, const WindowSkin& skin);

    // Button skin
    struct ButtonSkin {
        std::string tex_normal, tex_hover, tex_pressed, tex_disabled;
        uint32_t color_normal = 0x283250DC;
        uint32_t color_hover = 0x50648CDC;
        uint32_t color_pressed = 0x1E283CDC;
    };
    static ButtonSkin* GetButtonSkin(const std::string& btn_type = "default");

    // Bar/gauge skin
    struct GaugeSkin {
        std::string tex_fill, tex_bg, tex_border;
        uint32_t color_fill = 0xFF3C3CFF;
        uint32_t color_bg = 0x3C0000B4;
    };
    static GaugeSkin* GetGaugeSkin(const std::string& gauge_type = "hp");

    // Grid slot skin
    struct SlotSkin {
        std::string tex_normal, tex_hover, tex_highlight;
        uint32_t color_normal = 0x282832C8;
        uint32_t color_hover = 0x3C3C50C8;
    };
    static SlotSkin* GetSlotSkin(const std::string& slot_type = "inventory");

    // Scrollbar skin
    struct ScrollSkin {
        std::string tex_track, tex_thumb, tex_btn_up, tex_btn_down;
    };
    static ScrollSkin* GetScrollSkin();

    // Tab skin
    struct TabSkin {
        std::string tex_active, tex_inactive, tex_hover;
    };
    static TabSkin* GetTabSkin();

    // Draw a nine-patch window using skin
    static void DrawWindow(UIRenderer& ui, float x, float y, float w, float h,
                           const std::string& window_type, const char* title);
    static void DrawButton(UIRenderer& ui, float x, float y, float w, float h,
                           const std::string& btn_type, const char* text, bool hover = false, bool pressed = false);
    static void DrawGauge(UIRenderer& ui, float x, float y, float w, float h,
                          float pct, const std::string& gauge_type, const char* text = nullptr);
    static void DrawSlot(UIRenderer& ui, float x, float y, float size,
                         const std::string& slot_type, bool hover = false);

    // Load all window layout definitions from interface/Windows/*.txt
    static int LoadWindowLayouts(const std::string& windows_path);

    // Get all loaded texture names
    static std::vector<std::string> GetLoadedTextureNames();

private:
    static std::unordered_map<std::string, bgfx::TextureHandle> textures_;
    static std::unordered_map<std::string, UISkinPart> skin_parts_;
    static std::unordered_map<std::string, WindowSkin> window_skins_;
    static ButtonSkin default_button_skin_;
    static GaugeSkin hp_gauge_skin_, mp_gauge_skin_, exp_gauge_skin_;
    static SlotSkin inventory_slot_skin_;
    static ScrollSkin scroll_skin_;
    static TabSkin tab_skin_;
    static std::string texture_path_;
    static bool initialized_;
};
