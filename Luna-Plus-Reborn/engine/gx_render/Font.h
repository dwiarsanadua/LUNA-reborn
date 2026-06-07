#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include <ft2build.h>
#include FT_FREETYPE_H

struct GlyphInfo {
    uint16_t x, y;
    uint16_t width, height;
    int16_t bearing_x, bearing_y;
    uint16_t advance;
};

class Font {
public:
    Font();
    ~Font();

    bool Load(const std::string& font_path, int size = 14);
    void SetSize(int size);

    int DrawText(const std::string& text, int x, int y, uint32_t color = 0xFFFFFFFF);
    int GetTextWidth(const std::string& text);

    void Flush();

    int GetSize() const { return size_; }
    int GetLineHeight() const { return line_height_; }

private:
    void LoadGlyphs(const std::string& text);
    void EnsureAtlas();

    FT_Library ft_lib_;
    FT_Face face_;
    int size_ = 14;
    int line_height_ = 0;

    bgfx::TextureHandle atlas_ = BGFX_INVALID_HANDLE;
    int atlas_width_ = 512;
    int atlas_height_ = 512;
    int atlas_cursor_x_ = 0;
    int atlas_cursor_y_ = 0;
    int atlas_row_height_ = 0;

    std::unordered_map<uint32_t, GlyphInfo> glyphs_;
    std::vector<uint8_t> atlas_data_;

    bgfx::VertexBufferHandle vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_ = BGFX_INVALID_HANDLE;
    std::vector<float> vertex_data_;
    std::vector<uint16_t> index_data_;
};
