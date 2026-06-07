#include "Font.h"
#include <spdlog/spdlog.h>
#include <cstring>

Font::Font() {
    FT_Init_FreeType(&ft_lib_);
}

Font::~Font() {
    if (bgfx::isValid(vb_)) bgfx::destroy(vb_);
    if (bgfx::isValid(ib_)) bgfx::destroy(ib_);
    if (bgfx::isValid(atlas_)) bgfx::destroy(atlas_);
    if (face_) FT_Done_Face(face_);
    FT_Done_FreeType(ft_lib_);
}

bool Font::Load(const std::string& font_path, int size) {
    if (FT_New_Face(ft_lib_, font_path.c_str(), 0, &face_)) {
        spdlog::error("Font: failed to load '{}'", font_path);
        return false;
    }

    size_ = size;
    FT_Set_Pixel_Sizes(face_, 0, size);
    line_height_ = (int)(face_->size->metrics.height >> 6);

    atlas_data_.resize(atlas_width_ * atlas_height_, 0);
    EnsureAtlas();

    spdlog::info("Font: loaded '{}' (size={})", font_path, size);
    return true;
}

void Font::SetSize(int size) {
    if (face_ && size != size_) {
        size_ = size;
        FT_Set_Pixel_Sizes(face_, 0, size);
        line_height_ = (int)(face_->size->metrics.height >> 6);
        glyphs_.clear();
        atlas_cursor_x_ = 0;
        atlas_cursor_y_ = 0;
        atlas_row_height_ = 0;
        std::fill(atlas_data_.begin(), atlas_data_.end(), 0);
        EnsureAtlas();
    }
}

void Font::EnsureAtlas() {
    if (bgfx::isValid(atlas_)) bgfx::destroy(atlas_);

    const bgfx::Memory* mem = bgfx::copy(atlas_data_.data(),
        (uint32_t)(atlas_width_ * atlas_height_));
    atlas_ = bgfx::createTexture2D((uint16_t)atlas_width_, (uint16_t)atlas_height_,
        false, 1, bgfx::TextureFormat::R8, 0, mem);
}

void Font::LoadGlyphs(const std::string& text) {
    for (unsigned char c : text) {
        uint32_t glyph_idx = FT_Get_Char_Index(face_, c);
        if (glyphs_.count(glyph_idx) > 0) continue;

        if (FT_Load_Glyph(face_, glyph_idx, FT_LOAD_RENDER)) continue;

        FT_Bitmap& bitmap = face_->glyph->bitmap;

        if (atlas_cursor_x_ + bitmap.width > (uint32_t)atlas_width_) {
            atlas_cursor_x_ = 0;
            atlas_cursor_y_ += atlas_row_height_;
            atlas_row_height_ = 0;
        }

        if (atlas_cursor_y_ + (int)bitmap.rows > atlas_height_) {
            spdlog::warn("Font: atlas full, rebuilding...");
            return;
        }

        for (uint32_t y = 0; y < bitmap.rows; ++y) {
            for (uint32_t x = 0; x < bitmap.width; ++x) {
                int dest_y = atlas_cursor_y_ + y;
                int dest_x = atlas_cursor_x_ + x;
                atlas_data_[dest_y * atlas_width_ + dest_x] =
                    bitmap.buffer[y * bitmap.width + x];
            }
        }

        GlyphInfo info;
        info.x = (uint16_t)atlas_cursor_x_;
        info.y = (uint16_t)atlas_cursor_y_;
        info.width = (uint16_t)bitmap.width;
        info.height = (uint16_t)bitmap.rows;
        info.bearing_x = (int16_t)face_->glyph->bitmap_left;
        info.bearing_y = (int16_t)face_->glyph->bitmap_top;
        info.advance = (uint16_t)(face_->glyph->advance.x >> 6);
        glyphs_[glyph_idx] = info;

        atlas_cursor_x_ += bitmap.width;
        atlas_row_height_ = std::max(atlas_row_height_, (int)bitmap.rows);
    }

    EnsureAtlas();
}

int Font::DrawText(const std::string& text, int x, int y, uint32_t color) {
    LoadGlyphs(text);

    float fx = (float)x;
    float fy = (float)y;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    float a = ((color >> 24) & 0xFF) / 255.0f;

    for (unsigned char c : text) {
        uint32_t glyph_idx = FT_Get_Char_Index(face_, c);
        auto it = glyphs_.find(glyph_idx);
        if (it == glyphs_.end()) {
            fx += face_->glyph->advance.x >> 6;
            continue;
        }

        const GlyphInfo& glyph = it->second;
        if (glyph.width == 0 && glyph.height == 0) {
            fx += glyph.advance;
            continue;
        }

        float x0 = fx + (float)glyph.bearing_x;
        float y0 = fy - (float)glyph.bearing_y;
        float x1 = x0 + (float)glyph.width;
        float y1 = y0 + (float)glyph.height;

        float tx0 = (float)glyph.x / atlas_width_;
        float ty0 = (float)glyph.y / atlas_height_;
        float tx1 = (float)(glyph.x + glyph.width) / atlas_width_;
        float ty1 = (float)(glyph.y + glyph.height) / atlas_height_;

        size_t base = vertex_data_.size() / 8;

        float r2 = r, g2 = g, b2 = b, a2 = a;
        float verts[] = {
            x0, y0, tx0, ty0, r2, g2, b2, a2,
            x1, y0, tx1, ty0, r2, g2, b2, a2,
            x1, y1, tx1, ty1, r2, g2, b2, a2,
            x0, y1, tx0, ty1, r2, g2, b2, a2,
        };
        vertex_data_.insert(vertex_data_.end(), std::begin(verts), std::end(verts));

        uint16_t idx = static_cast<uint16_t>(base);
        uint16_t indices[] = {
            idx, uint16_t(idx + 1), uint16_t(idx + 2),
            idx, uint16_t(idx + 2), uint16_t(idx + 3),
        };
        index_data_.insert(index_data_.end(), std::begin(indices), std::end(indices));

        fx += (float)glyph.advance;
    }

    return (int)fx;
}

int Font::GetTextWidth(const std::string& text) {
    int width = 0;
    for (unsigned char c : text) {
        uint32_t glyph_idx = FT_Get_Char_Index(face_, c);
        if (FT_Load_Glyph(face_, glyph_idx, FT_LOAD_DEFAULT)) continue;
        width += (int)(face_->glyph->advance.x >> 6);
    }
    return width;
}

void Font::Flush() {
    if (vertex_data_.empty()) return;

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .end();

    if (bgfx::isValid(vb_)) bgfx::destroy(vb_);
    if (bgfx::isValid(ib_)) bgfx::destroy(ib_);

    vb_ = bgfx::createVertexBuffer(
        bgfx::copy(vertex_data_.data(), (uint32_t)(vertex_data_.size() * sizeof(float))),
        layout);
    ib_ = bgfx::createIndexBuffer(
        bgfx::copy(index_data_.data(), (uint32_t)(index_data_.size() * sizeof(uint16_t))));

    vertex_data_.clear();
    index_data_.clear();
}
