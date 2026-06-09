#include "UIRenderer.hpp"
#include "FontManager.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <vector>
#include <cstring>
#include <algorithm>
#include <set>

#include <stb_image.h>
#include <engine/gx_render/VFS.h>
#include <engine/gx_render/Shader.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

static bgfx::VertexLayout getLayout() {
    static bgfx::VertexLayout layout;
    static bool init = false;
    if (!init) {
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        init = true;
    }
    return layout;
}

void UIRenderer::BeginFrame() {
    FlushBatch();
    batch_verts_.clear();
    batch_indices_.clear();
    current_batch_tex_ = BGFX_INVALID_HANDLE;
    atlas_bind_count_ = 0;
    scissor_stack_.clear();

    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
    bgfx::setViewMode(view_id_, bgfx::ViewMode::Sequential);
    float identity[16]; std::memset(identity, 0, sizeof(identity));
    identity[0] = identity[5] = identity[10] = identity[15] = 1.0f;
    bgfx::setViewTransform(view_id_, identity, identity);
    bgfx::touch(view_id_);

    FlushBatch();
}

void UIRenderer::FlushBatch() {
    if (batch_verts_.empty() || batch_indices_.empty()) return;

    auto prog = bgfx::isValid(ui_prog_) ? ui_prog_ : prog_;
    if (!bgfx::isValid(prog)) {
        batch_verts_.clear();
        batch_indices_.clear();
        return;
    }

    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    if (bgfx::allocTransientBuffers(&tvb, getLayout(),
        (uint16_t)batch_verts_.size(), &tib, (uint32_t)batch_indices_.size())) {
        memcpy(tvb.data, batch_verts_.data(), batch_verts_.size() * sizeof(UIVertex));
        memcpy(tib.data, batch_indices_.data(), batch_indices_.size() * sizeof(uint16_t));

        bgfx::setTexture(0, s_tex_, current_batch_tex_);
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);

        uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA;
        if (!scissor_stack_.empty()) {
            const auto& s = scissor_stack_.back();
            uint16_t px = (uint16_t)(s.x / logicalWidth * width);
            uint16_t py = (uint16_t)(s.y / logicalHeight * height);
            uint16_t pw = (uint16_t)(s.w / logicalWidth * width);
            uint16_t ph = (uint16_t)(s.h / logicalHeight * height);
            bgfx::setScissor(px, py, pw, ph);
        }
        bgfx::setState(state);
        bgfx::submit(view_id_, prog);
        atlas_bind_count_++;
    }

    batch_verts_.clear();
    batch_indices_.clear();
    current_batch_tex_ = BGFX_INVALID_HANDLE;
}

void UIRenderer::DrawGlyph(float x, float y, uint32_t color, int char_index) {
    FontAtlas& atlas = font_atlases_[current_font_size_];
    if (!atlas.ready) return;
    if (char_index < 0 || char_index >= (int)atlas.glyphs.size()) return;
    auto& g = atlas.glyphs[char_index];
    if (g.w < 0.5f || g.h < 0.5f) return;

    float sx0 = ((x + g.xoff) / logicalWidth) * 2.0f - 1.0f;
    float sy0 = 1.0f - ((y + g.yoff) / logicalHeight) * 2.0f;
    float sx1 = ((x + g.xoff + g.w) / logicalWidth) * 2.0f - 1.0f;
    float sy1 = 1.0f - ((y + g.yoff + g.h) / logicalHeight) * 2.0f;

    if (current_batch_tex_.idx != atlas.tex.idx) FlushBatch();
    current_batch_tex_ = atlas.tex;

    uint16_t base = (uint16_t)batch_verts_.size();
    batch_verts_.push_back({sx0, sy1, 0, color, g.u0, g.v1});
    batch_verts_.push_back({sx1, sy1, 0, color, g.u1, g.v1});
    batch_verts_.push_back({sx0, sy0, 0, color, g.u0, g.v0});
    batch_verts_.push_back({sx1, sy0, 0, color, g.u1, g.v0});
    batch_indices_.push_back(base);   batch_indices_.push_back(base+1); batch_indices_.push_back(base+2);
    batch_indices_.push_back(base+1); batch_indices_.push_back(base+3); batch_indices_.push_back(base+2);

    if (batch_verts_.size() >= BATCH_SIZE) FlushBatch();
}

void UIRenderer::DrawImageUV(float x, float y, float w, float h, bgfx::TextureHandle tex, float u1, float v1, float u2, float v2, UIColor tint) {
    bgfx::TextureHandle final_tex = tex;
    if (!bgfx::isValid(final_tex)) final_tex = white_tex_;
    if (!bgfx::isValid(final_tex)) return;

    if (current_batch_tex_.idx != final_tex.idx) FlushBatch();
    current_batch_tex_ = final_tex;

    uint32_t col = (tint.a << 24) | (tint.b << 16) | (tint.g << 8) | tint.r;
    float x2 = (x / logicalWidth) * 2.0f - 1.0f;
    float y2 = 1.0f - (y / logicalHeight) * 2.0f;
    float x3 = ((x + w) / logicalWidth) * 2.0f - 1.0f;
    float y3 = 1.0f - ((y + h) / logicalHeight) * 2.0f;

    uint16_t base = (uint16_t)batch_verts_.size();
    batch_verts_.push_back({x2, y3, 0, col, u1, v2});
    batch_verts_.push_back({x3, y3, 0, col, u2, v2});
    batch_verts_.push_back({x2, y2, 0, col, u1, v1});
    batch_verts_.push_back({x3, y2, 0, col, u2, v1});
    batch_indices_.push_back(base);   batch_indices_.push_back(base+1); batch_indices_.push_back(base+2);
    batch_indices_.push_back(base+1); batch_indices_.push_back(base+3); batch_indices_.push_back(base+2);

    if (batch_verts_.size() >= BATCH_SIZE) FlushBatch();
}

void UIRenderer::DrawImage(float x, float y, float w, float h, bgfx::TextureHandle tex, UIColor tint) {
    DrawImageUV(x, y, w, h, tex, 0, 0, 1, 1, tint);
}

void UIRenderer::DrawNinePatch(float x, float y, float w, float h, const TextureInfo& tex, float l, float t, float r, float b, UIColor tint) {
    if (!bgfx::isValid(tex.handle)) return;
    float iw = 1.0f / tex.width, ih = 1.0f / tex.height;
    DrawImageUV(x, y, l, t, tex.handle, 0, 0, l*iw, t*ih, tint);
    DrawImageUV(x+w-r, y, r, t, tex.handle, 1.0f-r*iw, 0, 1.0f, t*ih, tint);
    DrawImageUV(x, y+h-b, l, b, tex.handle, 0, 1.0f-b*ih, l*iw, 1.0f, tint);
    DrawImageUV(x+w-r, y+h-b, r, b, tex.handle, 1.0f-r*iw, 1.0f-b*ih, 1.0f, 1.0f, tint);
    DrawImageUV(x+l, y, w-l-r, t, tex.handle, l*iw, 0, 1.0f-r*iw, t*ih, tint);
    DrawImageUV(x+l, y+h-b, w-l-r, b, tex.handle, l*iw, 1.0f-b*ih, 1.0f-r*iw, 1.0f, tint);
    DrawImageUV(x, y+t, l, h-t-b, tex.handle, 0, t*ih, l*iw, 1.0f-b*ih, tint);
    DrawImageUV(x+w-r, y+t, r, h-t-b, tex.handle, 1.0f-r*iw, t*ih, 1.0f, 1.0f-b*ih, tint);
    DrawImageUV(x+l, y+t, w-l-r, h-t-b, tex.handle, l*iw, t*ih, 1.0f-r*iw, 1.0f-b*ih, tint);
}

void UIRenderer::DrawRect(float x, float y, float w, float h, UIColor color) { DrawImageUV(x, y, w, h, white_tex_, 0, 0, 1, 1, color); }
void UIRenderer::DrawBorder(float x, float y, float w, float h, UIColor c, float t) { DrawRect(x,y,w,t,c); DrawRect(x,y+h-t,w,t,c); DrawRect(x,y,t,h,c); DrawRect(x+w-t,y,t,h,c); }
void UIRenderer::DrawBar(float x, float y, float w, float h, float pct, UIColor fg, UIColor bg) { DrawRect(x,y,w,h,bg); if(pct>0) DrawRect(x,y,w*std::min(1.0f,pct),h,fg); }

void UIRenderer::DrawWindow(float x, float y, float w, float h, const char* title, UIColor titleColor) {
    DrawRect(x, y, w, h, {20, 20, 30, 200}); DrawRect(x, y, w, 24, titleColor);
    DrawBorder(x, y, w, h, {100, 100, 150, 150}); DrawText(x + 6, y + 4, 0xffffffff, title);
}

void UIRenderer::DrawButton(float x, float y, float w, float h, const char* text, bool hover) {
    UIColor c = hover ? UIColor{100, 100, 180, 255} : UIColor{60, 60, 100, 255};
    DrawRect(x, y, w, h, c); DrawBorder(x, y, w, h, {150, 150, 255, 255});
    float tw = MeasureText(text); DrawText(x + (w - tw) * 0.5f, y + (h - current_font_size_) * 0.5f, 0xffffffff, text);
}

int UIRenderer::DecodeUTF8(const char*& s) {
    if (!s || !*s) return -1;
    unsigned char c = (unsigned char)*s;
    int codepoint;
    int bytes;
    if (c < 0x80) { codepoint = c; bytes = 1; }
    else if ((c & 0xE0) == 0xC0) { codepoint = c & 0x1F; bytes = 2; }
    else if ((c & 0xF0) == 0xE0) { codepoint = c & 0x0F; bytes = 3; }
    else if ((c & 0xF8) == 0xF0) { codepoint = c & 0x07; bytes = 4; }
    else { codepoint = c; bytes = 1; }
    for (int i = 1; i < bytes; i++) {
        if (!s[i]) { codepoint = c; bytes = 1; break; }
        codepoint = (codepoint << 6) | (s[i] & 0x3F);
    }
    s += bytes;
    return codepoint;
}

bool UIRenderer::LoadCjkFont(const std::string& font_path, float size) {
    std::ifstream f(font_path, std::ios::binary);
    if (!f) {
        spdlog::warn("UIRenderer: CJK font not found: {}", font_path);
        return false;
    }
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, data.data(), 0)) return false;

    int aw = 1024, ah = 1024;
    std::vector<unsigned char> atlas_data(aw * ah, 0);

    int cjk_start = 0x4E00;
    int cjk_count = 0x9FFF - 0x4E00 + 1;
    std::vector<stbtt_bakedchar> chardata(cjk_count + 224);

    FontAtlas& base = font_atlases_[size];
    int total_start = std::min(base.range_first, cjk_start);
    int total_end = std::max(base.range_first + base.range_count - 1, cjk_start + cjk_count - 1);
    int total_count = total_end - total_start + 1;

    int baked = stbtt_BakeFontBitmap(data.data(), 0, size,
        atlas_data.data(), aw, ah,
        total_start, total_count, chardata.data());
    if (!baked) {
        aw = 2048; ah = 2048;
        atlas_data.resize(aw * ah, 0);
        atlas_pending_.clear();
        baked = stbtt_BakeFontBitmap(data.data(), 0, size,
            atlas_data.data(), aw, ah,
            total_start, total_count, chardata.data());
        if (!baked) {
            spdlog::warn("UIRenderer: CJK font atlas too large for {}", font_path);
            return false;
        }
    }

    std::vector<unsigned char> rgba(aw * ah * 4);
    for (int i = 0; i < aw * ah; i++) {
        rgba[i*4+0] = rgba[i*4+1] = rgba[i*4+2] = 255;
        rgba[i*4+3] = atlas_data[i];
    }

    if (bgfx::isValid(base.tex)) bgfx::destroy(base.tex);
    base.tex = bgfx::createTexture2D((uint16_t)aw, (uint16_t)ah, false, 1,
        bgfx::TextureFormat::RGBA8, 0, bgfx::copy(rgba.data(), (uint32_t)rgba.size()));

    base.glyphs.resize(total_count);
    for (int i = 0; i < total_count; i++) {
        auto& g = base.glyphs[i];
        g.u0 = (float)chardata[i].x0 / aw;
        g.v0 = (float)chardata[i].y0 / ah;
        g.u1 = (float)chardata[i].x1 / aw;
        g.v1 = (float)chardata[i].y1 / ah;
        g.xoff = chardata[i].xoff;
        g.yoff = chardata[i].yoff + size;
        g.xadvance = chardata[i].xadvance;
        g.w = (float)(chardata[i].x1 - chardata[i].x0);
        g.h = (float)(chardata[i].y1 - chardata[i].y0);
    }
    base.range_first = total_start;
    base.range_count = total_count;
    base.atlas_w = aw;
    base.atlas_h = ah;

    CjkFontConfig cfg;
    cfg.path = font_path;
    cfg.codepoint_start = cjk_start;
    cfg.codepoint_end = 0x9FFF;
    cjk_fonts_.push_back(cfg);

    spdlog::info("UIRenderer: CJK font loaded from {} with {} glyphs", font_path, total_count);
    return true;
}

void UIRenderer::SetLanguage(const std::string& lang) {
    active_language_ = lang;
    font_atlases_.clear();

    font_atlas_w_ = 1024;
    font_atlas_h_ = 1024;

    if (lang == "ko" || lang == "kr") {
        std::string fontPaths[] = {
            VFS::Resolve("assets/fonts/NotoSansKR-Regular.otf"),
            VFS::Resolve("assets/fonts/NanumGothic.ttf"),
            VFS::Resolve("assets/fonts/gulim.ttf"),
        };
        for (auto& fp : fontPaths) {
            if (LoadCjkFont(fp, current_font_size_)) break;
        }
        if (!font_atlases_[current_font_size_].ready) {
            GetOrCreateFontAtlas(current_font_size_);
            LoadCjkFont(fontPaths[0], current_font_size_);
        }
    } else if (lang == "zh" || lang == "zh-cn" || lang == "zh-tw") {
        std::string fontPaths[] = {
            VFS::Resolve("assets/fonts/NotoSansSC-Regular.otf"),
            VFS::Resolve("assets/fonts/NotoSansTC-Regular.otf"),
            VFS::Resolve("assets/fonts/msyh.ttf"),
        };
        for (auto& fp : fontPaths) {
            if (LoadCjkFont(fp, current_font_size_)) break;
        }
        if (!font_atlases_[current_font_size_].ready) {
            GetOrCreateFontAtlas(current_font_size_);
            LoadCjkFont(fontPaths[0], current_font_size_);
        }
    } else if (lang == "ja") {
        std::string fontPaths[] = {
            VFS::Resolve("assets/fonts/NotoSansJP-Regular.otf"),
            VFS::Resolve("assets/fonts/msgothic.ttc"),
        };
        for (auto& fp : fontPaths) {
            if (LoadCjkFont(fp, current_font_size_)) break;
        }
        if (!font_atlases_[current_font_size_].ready) {
            GetOrCreateFontAtlas(current_font_size_);
            LoadCjkFont(fontPaths[0], current_font_size_);
        }
    } else {
        GetOrCreateFontAtlas(current_font_size_);
    }

    spdlog::info("UIRenderer: language set to '{}'", lang);
}

std::string UIRenderer::GetLanguage() const {
    return active_language_;
}

void UIRenderer::SetFontFallback(const std::string& primary, const std::string& fallback) {
    font_fallbacks_.clear();
    if (!primary.empty()) font_fallbacks_.push_back(primary);
    if (!fallback.empty()) font_fallbacks_.push_back(fallback);
    spdlog::info("UIRenderer: font fallback: '{}' -> '{}'", primary, fallback);
}

void UIRenderer::DrawText(float x, float y, uint32_t color, const char* fmt, ...) {
    {
        Language fb_lang = FontManager::Instance().GetLanguage();
        const char* lang_str = FontManager::LanguageToString(fb_lang);
        if (active_language_ != lang_str) {
            SetLanguage(lang_str);
        }
    }
    FontAtlas& atlas = font_atlases_[current_font_size_];
    if (!atlas.ready) return;
    char buf[1024]; va_list args; va_start(args, fmt); vsnprintf(buf, 1024, fmt, args); va_end(args);
    float cur_x = x; const char* p = buf;
    int first = atlas.range_first;
    int count = atlas.range_count;
    while (*p) {
        int cp = DecodeUTF8(p);
        if (cp < 0) continue;
        int idx = cp - first;

        if (idx < 0 || idx >= count) {
            bool loaded = false;
            for (auto& cjk : cjk_fonts_) {
                if (cp >= cjk.codepoint_start && cp <= cjk.codepoint_end) {
                    loaded = true;
                    break;
                }
            }
            if (!loaded && cjk_fonts_.empty()) {
                std::string cjkFallback = VFS::Resolve("assets/fonts/NotoSansSC-Regular.otf");
                if (!cjkFallback.empty()) {
                    LoadCjkFont(cjkFallback, current_font_size_);
                    FontAtlas& updated = font_atlases_[current_font_size_];
                    idx = cp - updated.range_first;
                    if (idx >= 0 && idx < updated.range_count) {
                        DrawGlyph(cur_x, y, color, idx);
                        cur_x += updated.glyphs[idx].xadvance;
                        continue;
                    }
                }
            }
            continue;
        }
        DrawGlyph(cur_x, y, color, idx);
        cur_x += atlas.glyphs[idx].xadvance;
    }
}

void UIRenderer::DrawTextV(float x, float y, uint32_t color, const char* fmt, va_list args) {
    char buf[1024]; vsnprintf(buf, 1024, fmt, args); DrawText(x, y, color, "%s", buf);
}

void UIRenderer::DrawTextCentered(float y, uint32_t color, const char* fmt, ...) {
    char buf[1024]; va_list args; va_start(args, fmt); vsnprintf(buf, 1024, fmt, args); va_end(args);
    float x = (logicalWidth - MeasureText(buf)) * 0.5f; DrawText(x, y, color, "%s", buf);
}

float UIRenderer::MeasureText(const char* text) {
    FontAtlas& atlas = font_atlases_[current_font_size_];
    if (!atlas.ready) return strlen(text) * 8.0f;
    float w = 0; const char* p = text;
    int first = atlas.range_first;
    int count = atlas.range_count;
    while (*p) {
        int cp = DecodeUTF8(p);
        if (cp < 0) continue;
        int idx = cp - first;
        if (idx >= 0 && idx < count)
            w += atlas.glyphs[idx].xadvance;
    }
    return w;
}

void UIRenderer::SetFontSize(float size) {
    if (size <= 0) return;
    current_font_size_ = size;
    GetOrCreateFontAtlas(size);
}

void UIRenderer::LoadGlyphsForText(const std::string& text) {
    FontAtlas& atlas = GetOrCreateFontAtlas(current_font_size_);
    if (!atlas.ready) return;

    int min_cp = 255, max_cp = 32;
    const char* p = text.c_str();
    while (*p) {
        int cp = DecodeUTF8(p);
        if (cp < 0) continue;
        if (cp < atlas.range_first || cp >= atlas.range_first + atlas.range_count) {
            min_cp = std::min(min_cp, cp);
            max_cp = std::max(max_cp, cp);
        }
    }

    if (max_cp < min_cp) return;

    int new_first = std::min(atlas.range_first, min_cp);
    int new_last = std::max(atlas.range_first + atlas.range_count - 1, max_cp);
    int new_count = new_last - new_first + 1;

    if (new_count <= atlas.range_count && new_first >= atlas.range_first) return;

    std::string fontPath = VFS::Resolve("assets/interface/Windows/2002_EYA.ttf");
    std::ifstream f(fontPath, std::ios::binary);
    if (!f) return;
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, data.data(), 0)) return;

    int aw = atlas.atlas_w, ah = atlas.atlas_h;
    std::vector<unsigned char> atlas_data(aw * ah, 0);
    std::vector<stbtt_bakedchar> chardata(new_count);
    int baked = stbtt_BakeFontBitmap(data.data(), 0, current_font_size_,
        atlas_data.data(), aw, ah, new_first, new_count, chardata.data());
    if (!baked) {
        aw *= 2; ah *= 2;
        atlas_data.resize(aw * ah, 0);
        baked = stbtt_BakeFontBitmap(data.data(), 0, current_font_size_,
            atlas_data.data(), aw, ah, new_first, new_count, chardata.data());
        if (!baked) {
            spdlog::warn("UIRenderer: font atlas too small for range {}..{}", new_first, new_last);
            return;
        }
    }

    std::vector<unsigned char> rgba(aw * ah * 4);
    for (int i = 0; i < aw * ah; i++) {
        rgba[i*4+0] = rgba[i*4+1] = rgba[i*4+2] = 255;
        rgba[i*4+3] = atlas_data[i];
    }

    if (bgfx::isValid(atlas.tex)) bgfx::destroy(atlas.tex);
    atlas.tex = bgfx::createTexture2D((uint16_t)aw, (uint16_t)ah, false, 1,
        bgfx::TextureFormat::RGBA8, 0, bgfx::copy(rgba.data(), (uint32_t)rgba.size()));

    atlas.glyphs.resize(new_count);
    for (int i = 0; i < new_count; i++) {
        auto& g = atlas.glyphs[i];
        g.u0 = (float)chardata[i].x0 / aw;
        g.v0 = (float)chardata[i].y0 / ah;
        g.u1 = (float)chardata[i].x1 / aw;
        g.v1 = (float)chardata[i].y1 / ah;
        g.xoff = chardata[i].xoff;
        g.yoff = chardata[i].yoff + current_font_size_;
        g.xadvance = chardata[i].xadvance;
        g.w = (float)(chardata[i].x1 - chardata[i].x0);
        g.h = (float)(chardata[i].y1 - chardata[i].y0);
    }
    atlas.range_first = new_first;
    atlas.range_count = new_count;
    atlas.atlas_w = aw;
    atlas.atlas_h = ah;
    atlas.ready = true;
}

void UIRenderer::PushScissor(float x, float y, float w, float h) {
    FlushBatch();
    scissor_stack_.push_back({x, y, w, h});
}

void UIRenderer::PopScissor() {
    if (scissor_stack_.empty()) return;
    FlushBatch();
    scissor_stack_.pop_back();
}

bool UIRenderer::IsClipped(float x, float y, float w, float h) const {
    if (scissor_stack_.empty()) return false;
    const auto& s = scissor_stack_.back();
    return (x + w <= s.x || x >= s.x + s.w || y + h <= s.y || y >= s.y + s.h);
}

AtlasRegion UIRenderer::PackInAtlas(int w, int h) {
    AtlasRegion reg = {0,0,0,0, 0,0,0,0};

    if (atlas_cursor_x_ + w > ATLAS_SIZE) {
        atlas_cursor_x_ = 0;
        atlas_cursor_y_ += atlas_row_h_ + 1;
        atlas_row_h_ = 0;
    }
    if (atlas_cursor_y_ + h > ATLAS_SIZE) {
        spdlog::warn("UIRenderer: atlas full, can't pack {}x{}", w, h);
        return reg;
    }

    reg.x = atlas_cursor_x_;
    reg.y = atlas_cursor_y_;
    reg.w = w;
    reg.h = h;
    reg.u0 = (float)reg.x / ATLAS_SIZE;
    reg.v0 = (float)reg.y / ATLAS_SIZE;
    reg.u1 = (float)(reg.x + w) / ATLAS_SIZE;
    reg.v1 = (float)(reg.y + h) / ATLAS_SIZE;

    atlas_cursor_x_ += w + 1;
    atlas_row_h_ = std::max(atlas_row_h_, h);
    return reg;
}

void UIRenderer::UploadAtlas() {
    if (atlas_data_.empty()) return;

    if (bgfx::isValid(atlas_tex_)) {
        bgfx::destroy(atlas_tex_);
    }

    atlas_tex_ = bgfx::createTexture2D(
        ATLAS_SIZE, ATLAS_SIZE, false, 1,
        bgfx::TextureFormat::RGBA8,
        BGFX_SAMPLER_NONE,
        bgfx::copy(atlas_data_.data(), (uint32_t)(atlas_data_.size() * sizeof(uint32_t))));

    atlas_valid_ = bgfx::isValid(atlas_tex_);
    spdlog::info("UIRenderer: texture atlas uploaded ({}x{})", ATLAS_SIZE, ATLAS_SIZE);
}

void UIRenderer::BuildTextureAtlas() {
    atlas_data_.assign(ATLAS_SIZE * ATLAS_SIZE, 0);
    atlas_cursor_x_ = 0;
    atlas_cursor_y_ = 0;
    atlas_row_h_ = 0;
    atlas_regions_.clear();

    for (auto& [name, path] : atlas_pending_) {
        int w, h, n;
        unsigned char* d = stbi_load(path.c_str(), &w, &h, &n, 4);
        if (!d) continue;

        if (w > ATLAS_SIZE || h > ATLAS_SIZE) {
            auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w * h * 4));
            TextureInfo info; info.handle = tex; info.width = w; info.height = h;
            textures_[name] = info;
            stbi_image_free(d);
            continue;
        }

        AtlasRegion reg = PackInAtlas(w, h);
        if (reg.w == 0 && reg.h == 0) {
            auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w * h * 4));
            TextureInfo info; info.handle = tex; info.width = w; info.height = h;
            textures_[name] = info;
            stbi_image_free(d);
            continue;
        }

        uint32_t* src = (uint32_t*)d;
        for (int py = 0; py < h; py++) {
            std::memcpy(&atlas_data_[(reg.y + py) * ATLAS_SIZE + reg.x],
                       &src[py * w], w * sizeof(uint32_t));
        }

        atlas_regions_[name] = reg;

        TextureInfo info;
        info.handle = atlas_tex_;
        info.width = w;
        info.height = h;
        textures_[name] = info;

        stbi_image_free(d);
    }

    atlas_pending_.clear();
    UploadAtlas();

    for (auto& [name, reg] : atlas_regions_) {
        textures_[name].handle = atlas_tex_;
    }
}

void UIRenderer::Init() {
    ui_prog_ = ShaderUtils::LoadProgram("shaders/vs_ui.bin", "shaders/fs_ui.bin");
    if (!bgfx::isValid(ui_prog_)) {
        spdlog::error("UIRenderer: vs_ui+fs_ui failed, fallback to vs_default+fs_ui");
        ui_prog_ = ShaderUtils::LoadProgram("shaders/vs_default.bin", "shaders/fs_ui.bin");
    }
    if (!bgfx::isValid(ui_prog_)) {
        spdlog::error("UIRenderer: all UI shader fallbacks failed — UI will not render");
    } else {
        spdlog::info("UIRenderer: Program UI valid (handle={})", ui_prog_.idx);
    }
    s_tex_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff; white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));
    CreateFont();

    atlas_data_.resize(ATLAS_SIZE * ATLAS_SIZE, 0);

    spdlog::info("UIRenderer: initialized with texture atlas ({}x{})", ATLAS_SIZE, ATLAS_SIZE);
}

FontAtlas& UIRenderer::GetOrCreateFontAtlas(float size) {
    auto it = font_atlases_.find(size);
    if (it != font_atlases_.end()) return it->second;

    FontAtlas& atlas = font_atlases_[size];
    atlas.atlas_w = font_atlas_w_;
    atlas.atlas_h = font_atlas_h_;
    atlas.range_first = 32;
    atlas.range_count = 224;

    std::string fontPath = VFS::Resolve("assets/interface/Windows/2002_EYA.ttf");
    std::ifstream f(fontPath, std::ios::binary);
    if (!f) return atlas;
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, data.data(), 0)) return atlas;

    std::vector<unsigned char> atlas_data(atlas.atlas_w * atlas.atlas_h, 0);
    std::vector<stbtt_bakedchar> chardata(atlas.range_count);
    int baked = stbtt_BakeFontBitmap(data.data(), 0, size,
        atlas_data.data(), atlas.atlas_w, atlas.atlas_h,
        atlas.range_first, atlas.range_count, chardata.data());
    if (!baked) {
        spdlog::warn("UIRenderer: font atlas too small for size {}px, need larger atlas", size);
        atlas.atlas_w *= 2;
        atlas.atlas_h *= 2;
        atlas_data.resize(atlas.atlas_w * atlas.atlas_h, 0);
        baked = stbtt_BakeFontBitmap(data.data(), 0, size,
            atlas_data.data(), atlas.atlas_w, atlas.atlas_h,
            atlas.range_first, atlas.range_count, chardata.data());
        if (!baked) return atlas;
    }

    std::vector<unsigned char> rgba(atlas.atlas_w * atlas.atlas_h * 4);
    for (int i = 0; i < atlas.atlas_w * atlas.atlas_h; i++) {
        rgba[i*4+0] = rgba[i*4+1] = rgba[i*4+2] = 255;
        rgba[i*4+3] = atlas_data[i];
    }

    atlas.tex = bgfx::createTexture2D((uint16_t)atlas.atlas_w, (uint16_t)atlas.atlas_h, false, 1,
        bgfx::TextureFormat::RGBA8, 0, bgfx::copy(rgba.data(), (uint32_t)rgba.size()));

    atlas.glyphs.resize(atlas.range_count);
    for (int i = 0; i < atlas.range_count; i++) {
        auto& g = atlas.glyphs[i];
        g.u0 = (float)chardata[i].x0 / atlas.atlas_w;
        g.v0 = (float)chardata[i].y0 / atlas.atlas_h;
        g.u1 = (float)chardata[i].x1 / atlas.atlas_w;
        g.v1 = (float)chardata[i].y1 / atlas.atlas_h;
        g.xoff = chardata[i].xoff;
        g.yoff = chardata[i].yoff + size;
        g.xadvance = chardata[i].xadvance;
        g.w = (float)(chardata[i].x1 - chardata[i].x0);
        g.h = (float)(chardata[i].y1 - chardata[i].y0);
    }
    atlas.ready = true;
    return atlas;
}

void UIRenderer::CreateFont() {
    font_atlas_w_ = 1024;
    font_atlas_h_ = 1024;
    font_atlases_.clear();

    GetOrCreateFontAtlas(18.0f);
    current_font_size_ = 18.0f;

    SetLanguage("en");

    BuildTextureAtlas();

    spdlog::info("UIRenderer: font created, texture atlas cached");
}

TextureInfo UIRenderer::LoadTexture(const std::string& name, const std::string& path) {
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;

    auto at = atlas_regions_.find(name);
    if (at != atlas_regions_.end()) {
        TextureInfo info;
        info.handle = atlas_tex_;
        info.width = at->second.w;
        info.height = at->second.h;
        textures_[name] = info;
        return info;
    }

    std::string search[] = {
        path,
        VFS::Resolve("assets/textures/ui/" + path),
        VFS::Resolve("assets/textures/" + path),
        VFS::Resolve("assets/textures/unpacked/map/" + path)
    };

    for (auto& p : search) {
        if (p.empty()) continue;
        int w, h, n;
        unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
        if (d) {
            stbi_image_free(d);

            if (w <= 256 && h <= 256 && w > 0 && h > 0) {
                atlas_pending_.push_back({name, p});
                return TextureInfo{};
            }

            d = stbi_load(p.c_str(), &w, &h, &n, 4);
            if (d) {
                TextureInfo info;
                info.handle = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                    bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w*h*4));
                info.width = w; info.height = h;
                stbi_image_free(d);
                textures_[name] = info;
                return info;
            }
        }
    }
    return {};
}

void UIRenderer::Render() {
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
}

void UIRenderer::Shutdown() {
    FlushBatch();
    static bool done = false; if (done) return; done = true;
    for (auto& [n, t] : textures_) if (bgfx::isValid(t.handle) && t.handle.idx != atlas_tex_.idx) bgfx::destroy(t.handle);
    textures_.clear();
    if (bgfx::isValid(atlas_tex_)) bgfx::destroy(atlas_tex_);
    for (auto& [size, atlas] : font_atlases_) {
        if (bgfx::isValid(atlas.tex)) bgfx::destroy(atlas.tex);
    }
    font_atlases_.clear();
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_); if (bgfx::isValid(s_tex_)) bgfx::destroy(s_tex_);
    if (bgfx::isValid(ui_prog_)) bgfx::destroy(ui_prog_);
    atlas_data_.clear();
    atlas_regions_.clear();
    atlas_pending_.clear();
    batch_verts_.clear();
    batch_indices_.clear();
    scissor_stack_.clear();
}
