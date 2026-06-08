#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <engine/gx_render/RenderDevice.h>

struct UIColor { uint8_t r, g, b, a; };

struct FontGlyph {
    float u0, v0, u1, v1;
    float xoff, yoff;
    float xadvance;
    float w, h;
};

struct TextureInfo {
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
    int width = 0;
    int height = 0;
};

struct AtlasRegion {
    float u0, v0, u1, v1;
    int x, y, w, h;
};

class UIRenderer {
public:
    void Init();
    void BeginFrame();

    TextureInfo LoadTexture(const std::string& name, const std::string& path);
    void DrawImage(float x, float y, float w, float h, bgfx::TextureHandle tex, UIColor tint = {255,255,255,255});
    void DrawImageUV(float x, float y, float w, float h, bgfx::TextureHandle tex, float u1, float v1, float u2, float v2, UIColor tint = {255,255,255,255});

    void DrawNinePatch(float x, float y, float w, float h, const TextureInfo& tex,
                       float border_l, float border_r, float border_t, float border_b, UIColor tint = {255,255,255,255});

    void DrawRect(float x, float y, float w, float h, UIColor color);
    void DrawBar(float x, float y, float w, float h, float pct, UIColor fg, UIColor bg);
    void DrawBorder(float x, float y, float w, float h, UIColor c, float t = 1.0f);

    void DrawWindow(float x, float y, float w, float h, const char* title, UIColor titleColor);
    void DrawButton(float x, float y, float w, float h, const char* text, bool hover = false);

    void DrawText(float x, float y, uint32_t color, const char* fmt, ...);
    void DrawTextV(float x, float y, uint32_t color, const char* fmt, va_list args);
    void DrawTextCentered(float y, uint32_t color, const char* fmt, ...);
    float MeasureText(const char* text);

    void Render();
    void Shutdown();

    bgfx::UniformHandle GetSampler() const { return s_tex_; }
    bgfx::TextureHandle GetWhiteTexture() const { return white_tex_; }
    bgfx::TextureHandle GetAtlasTexture() const { return atlas_valid_ ? atlas_tex_ : white_tex_; }

    float width = 1280.0f;
    float height = 720.0f;
    float logicalWidth = 1280.0f;
    float logicalHeight = 720.0f;

private:
    void CreateFont();
    void DrawGlyph(float x, float y, uint32_t color, int char_index);
    void BuildTextureAtlas();
    AtlasRegion PackInAtlas(int w, int h);
    void UploadAtlas();

    bgfx::ProgramHandle prog_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle ui_prog_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::UI);
    std::unordered_map<std::string, TextureInfo> textures_;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    TextureInfo login_bg_;

    bgfx::TextureHandle font_tex_ = BGFX_INVALID_HANDLE;
    FontGlyph font_glyphs_[96];
    int font_atlas_w_ = 512;
    int font_atlas_h_ = 128;
    float font_size_ = 18.0f;
    bool font_ready_ = false;

    // Texture atlas for UI textures
    static constexpr int ATLAS_SIZE = 2048;
    bgfx::TextureHandle atlas_tex_ = BGFX_INVALID_HANDLE;
    std::vector<uint32_t> atlas_data_;
    int atlas_cursor_x_ = 0;
    int atlas_cursor_y_ = 0;
    int atlas_row_h_ = 0;
    bool atlas_valid_ = false;
    std::unordered_map<std::string, AtlasRegion> atlas_regions_;
    std::vector<std::pair<std::string, std::string>> atlas_pending_;
    int atlas_bind_count_ = 0;
};

extern UIRenderer* g_ui;
