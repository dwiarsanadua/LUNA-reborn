#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <engine/gx_render/RenderDevice.h>

struct UIColor { uint8_t r, g, b, a; };

// Per-glyph info for font atlas
struct FontGlyph {
    float u0, v0, u1, v1; // UV coords in atlas
    float xoff, yoff;     // pixel offset from cursor
    float xadvance;        // advance to next char
    float w, h;            // glyph pixel size
};

struct TextureInfo {
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
    int width = 0;
    int height = 0;
};

class UIRenderer {
public:
    void Init();
    void BeginFrame();
    
    // Textured UI
    TextureInfo LoadTexture(const std::string& name, const std::string& path);
    void DrawImage(float x, float y, float w, float h, bgfx::TextureHandle tex, UIColor tint = {255,255,255,255});
    void DrawImageUV(float x, float y, float w, float h, bgfx::TextureHandle tex, float u1, float v1, float u2, float v2, UIColor tint = {255,255,255,255});
    
    // Nine-patch rendering (for resizable windows)
    void DrawNinePatch(float x, float y, float w, float h, const TextureInfo& tex,
                       float border_l, float border_r, float border_t, float border_b, UIColor tint = {255,255,255,255});

    // Primitive shapes
    void DrawRect(float x, float y, float w, float h, UIColor color);
    void DrawBar(float x, float y, float w, float h, float pct, UIColor fg, UIColor bg);
    void DrawBorder(float x, float y, float w, float h, UIColor c, float t = 1.0f);
    
    // Windows
    void DrawWindow(float x, float y, float w, float h, const char* title, UIColor titleColor);
    void DrawButton(float x, float y, float w, float h, const char* text, bool hover = false);
    
    // Text — rendered as textured quads from font atlas
    void DrawText(float x, float y, uint32_t color, const char* fmt, ...);
    void DrawTextV(float x, float y, uint32_t color, const char* fmt, va_list args);
    void DrawTextCentered(float y, uint32_t color, const char* fmt, ...);
    float MeasureText(const char* text);
    
    void Render();
    void Shutdown();

    bgfx::UniformHandle GetSampler() const { return s_tex_; }
    bgfx::TextureHandle GetWhiteTexture() const { return white_tex_; }

    float width = 1280.0f;
    float height = 720.0f;
    float logicalWidth = 1280.0f;
    float logicalHeight = 720.0f;

private:
    void CreateFont();
    void DrawGlyph(float x, float y, uint32_t color, int char_index);
    
    bgfx::ProgramHandle prog_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle ui_prog_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::UI);
    std::unordered_map<std::string, TextureInfo> textures_;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    TextureInfo login_bg_;
    
    // Font atlas
    bgfx::TextureHandle font_tex_ = BGFX_INVALID_HANDLE;
    FontGlyph font_glyphs_[96]; // ASCII 32-127
    int font_atlas_w_ = 512;
    int font_atlas_h_ = 128;
    float font_size_ = 18.0f;
    bool font_ready_ = false;
};

extern UIRenderer* g_ui;
