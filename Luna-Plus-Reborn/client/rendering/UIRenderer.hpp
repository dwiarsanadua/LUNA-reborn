#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <cstdint>
#include <engine/gx_render/RenderDevice.h>

struct UIColor { uint8_t r, g, b, a; };

struct UIVertex {
    float x, y, z;
    uint32_t color;
    float u, v;
};

struct FontGlyph {
    float u0, v0, u1, v1;
    float xoff, yoff;
    float xadvance;
    float w, h;
};

struct FontAtlas {
    bgfx::TextureHandle tex = BGFX_INVALID_HANDLE;
    std::vector<FontGlyph> glyphs;
    int atlas_w = 512;
    int atlas_h = 128;
    int range_first = 32;
    int range_count = 224;
    bool ready = false;
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

struct ScissorRect {
    float x, y, w, h;
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

    void FlushBatch();
    void Render();
    void Shutdown();

    void SetFontSize(float size);
    void LoadGlyphsForText(const std::string& text);

    void PushScissor(float x, float y, float w, float h);
    void PopScissor();
    bool IsClipped(float x, float y, float w, float h) const;

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

    static int DecodeUTF8(const char*& s);

    FontAtlas& GetOrCreateFontAtlas(float size);
    bool LoadCjkFont(const std::string& font_path, float size);
    void SetLanguage(const std::string& lang);
    std::string GetLanguage() const;
    void SetFontFallback(const std::string& primary, const std::string& fallback);

    struct CjkFontConfig {
        std::string path;
        std::string language;
        int codepoint_start = 0x4E00;
        int codepoint_end   = 0x9FFF;
    };

    // Batch accumulator
    std::vector<UIVertex> batch_verts_;
    std::vector<uint16_t> batch_indices_;
    bgfx::TextureHandle current_batch_tex_ = BGFX_INVALID_HANDLE;
    static constexpr int BATCH_SIZE = 4096;

    bgfx::ProgramHandle prog_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle ui_prog_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::UI);
    std::unordered_map<std::string, TextureInfo> textures_;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    TextureInfo login_bg_;

    // Font system — per-size atlases
    std::unordered_map<float, FontAtlas> font_atlases_;
    float current_font_size_ = 18.0f;
    int font_atlas_w_ = 1024;
    int font_atlas_h_ = 1024;

    std::string active_language_ = "en";
    std::vector<CjkFontConfig> cjk_fonts_;
    std::vector<std::string> font_fallbacks_;

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

    // Scissor stack
    std::vector<ScissorRect> scissor_stack_;
};

extern UIRenderer* g_ui;
