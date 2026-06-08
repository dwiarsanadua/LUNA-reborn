#include "UIRenderer.hpp"
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

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

struct UIVertex {
    float x, y, z;
    uint32_t color;
    float u, v;
};

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

static const bgfx::Memory* loadShader(const char* path) {
    std::string searchPaths[] = { "build/bin/" + std::string(path), std::string(path), VFS::Resolve("assets/" + std::string(path)) };
    for (const auto& p : searchPaths) {
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (file) {
            size_t size = file.tellg();
            file.seekg(0);
            auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
            file.read(reinterpret_cast<char*>(mem->data), size);
            spdlog::info("Shader: loaded {} ({} bytes)", p, size);
            return mem;
        }
    }
    return nullptr;
}

void UIRenderer::BeginFrame() {
    atlas_bind_count_ = 0;
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
    bgfx::setViewMode(view_id_, bgfx::ViewMode::Sequential);
    float identity[16]; std::memset(identity, 0, sizeof(identity));
    identity[0] = identity[5] = identity[10] = identity[15] = 1.0f;
    bgfx::setViewTransform(view_id_, identity, identity);
    bgfx::touch(view_id_);
}

void UIRenderer::DrawGlyph(float x, float y, uint32_t color, int char_index) {
    if (char_index < 0 || char_index >= 96) return;
    auto& g = font_glyphs_[char_index];
    if (g.w < 0.5f || g.h < 0.5f) return;
    bgfx::ProgramHandle prog = bgfx::isValid(ui_prog_) ? ui_prog_ : prog_;
    if (!bgfx::isValid(prog)) return;

    float sx0 = ( (x + g.xoff) / logicalWidth) * 2.0f - 1.0f;
    float sy0 = 1.0f - ( (y + g.yoff) / logicalHeight) * 2.0f;
    float sx1 = ( (x + g.xoff + g.w) / logicalWidth) * 2.0f - 1.0f;
    float sy1 = 1.0f - ( (y + g.yoff + g.h) / logicalHeight) * 2.0f;

    UIVertex verts[4] = { {sx0, sy1, 0, color, g.u0, g.v1}, {sx1, sy1, 0, color, g.u1, g.v1}, {sx0, sy0, 0, color, g.u0, g.v0}, {sx1, sy0, 0, color, g.u1, g.v0} };
    uint16_t idx[6] = {0,1,2, 1,3,2};
    bgfx::TransientVertexBuffer tvb; bgfx::TransientIndexBuffer tib;
    if (bgfx::allocTransientBuffers(&tvb, getLayout(), 4, &tib, 6)) {
        std::memcpy(tvb.data, verts, sizeof(verts)); std::memcpy(tib.data, idx, sizeof(idx));
        bgfx::setTexture(0, s_tex_, font_tex_); bgfx::setVertexBuffer(0, &tvb); bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA); bgfx::submit(view_id_, prog);
        atlas_bind_count_++;
    }
}

void UIRenderer::DrawImageUV(float x, float y, float w, float h, bgfx::TextureHandle tex, float u1, float v1, float u2, float v2, UIColor tint) {
    bgfx::TextureHandle final_tex = tex;
    if (!bgfx::isValid(final_tex)) final_tex = white_tex_;
    if (!bgfx::isValid(final_tex)) return;

    bgfx::ProgramHandle prog = bgfx::isValid(ui_prog_) ? ui_prog_ : prog_;
    if (!bgfx::isValid(prog)) return;
    uint32_t col = (tint.a << 24) | (tint.b << 16) | (tint.g << 8) | tint.r;
    float x2 = (x / logicalWidth) * 2.0f - 1.0f; float y2 = 1.0f - (y / logicalHeight) * 2.0f;
    float x3 = ((x + w) / logicalWidth) * 2.0f - 1.0f; float y3 = 1.0f - ((y + h) / logicalHeight) * 2.0f;
    UIVertex verts[4] = { {x2, y3, 0, col, u1, v2}, {x3, y3, 0, col, u2, v2}, {x2, y2, 0, col, u1, v1}, {x3, y2, 0, col, u2, v1} };
    uint16_t idx[6] = {0,1,2, 1,3,2};
    bgfx::TransientVertexBuffer tvb; bgfx::TransientIndexBuffer tib;
    if (bgfx::allocTransientBuffers(&tvb, getLayout(), 4, &tib, 6)) {
        std::memcpy(tvb.data, verts, sizeof(verts)); std::memcpy(tib.data, idx, sizeof(idx));
        bgfx::setTexture(0, s_tex_, final_tex); bgfx::setVertexBuffer(0, &tvb); bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA); bgfx::submit(view_id_, prog);
        atlas_bind_count_++;
    }
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
    float tw = MeasureText(text); DrawText(x + (w - tw) * 0.5f, y + (h - 18) * 0.5f, 0xffffffff, text);
}

void UIRenderer::DrawText(float x, float y, uint32_t color, const char* fmt, ...) {
    if (!font_ready_) return;
    char buf[1024]; va_list args; va_start(args, fmt); vsnprintf(buf, 1024, fmt, args); va_end(args);
    float cur_x = x; for (int i = 0; buf[i]; i++) {
        char c = buf[i]; if (c < 32 || c > 126) continue;
        DrawGlyph(cur_x, y, color, c - 32); cur_x += font_glyphs_[c - 32].xadvance;
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
    if (!font_ready_) return strlen(text) * 8.0f;
    float w = 0; for (int i = 0; text[i]; i++) { char c = text[i]; if (c >= 32 && c <= 126) w += font_glyphs_[c - 32].xadvance; }
    return w;
}

AtlasRegion UIRenderer::PackInAtlas(int w, int h) {
    AtlasRegion reg = {0,0,0,0, 0,0,0,0};

    // Simple row-packing: if doesn't fit current row, advance to next
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

    // Process pending textures
    for (auto& [name, path] : atlas_pending_) {
        int w, h, n;
        unsigned char* d = stbi_load(path.c_str(), &w, &h, &n, 4);
        if (!d) continue;

        if (w > ATLAS_SIZE || h > ATLAS_SIZE) {
            // Too large for atlas, create individual texture
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

        // Copy pixels into atlas
        uint32_t* src = (uint32_t*)d;
        for (int py = 0; py < h; py++) {
            std::memcpy(&atlas_data_[(reg.y + py) * ATLAS_SIZE + reg.x],
                       &src[py * w], w * sizeof(uint32_t));
        }

        atlas_regions_[name] = reg;

        TextureInfo info;
        info.handle = atlas_tex_; // Will be valid after upload
        info.width = w;
        info.height = h;
        textures_[name] = info;

        stbi_image_free(d);
    }

    atlas_pending_.clear();
    UploadAtlas();

    // Update texture handles to point to atlas
    for (auto& [name, reg] : atlas_regions_) {
        textures_[name].handle = atlas_tex_;
    }
}

void UIRenderer::Init() {
    auto vs = loadShader("shaders/vs_ui.bin"); auto fs = loadShader("shaders/fs_ui.bin");
    if (vs && fs) {
        bgfx::ShaderHandle vs_h = bgfx::createShader(vs);
        bgfx::ShaderHandle fs_h = bgfx::createShader(fs);
        if (!bgfx::isValid(vs_h)) spdlog::error("UIRenderer: vs_ui.bin GAGAL di Metal macOS!");
        if (!bgfx::isValid(fs_h)) spdlog::error("UIRenderer: fs_ui.bin GAGAL di Metal macOS!");
        ui_prog_ = bgfx::createProgram(vs_h, fs_h, true);
        if (!bgfx::isValid(ui_prog_))
            spdlog::error("UIRenderer: Program UI GAGAL di Metal macOS! Shader tidak kompatibel.");
        else
            spdlog::info("UIRenderer: Program UI valid (handle={})", ui_prog_.idx);
    }
    s_tex_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff; white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));
    CreateFont();

    // Initialize atlas data
    atlas_data_.resize(ATLAS_SIZE * ATLAS_SIZE, 0);

    spdlog::info("UIRenderer: initialized with texture atlas ({}x{})", ATLAS_SIZE, ATLAS_SIZE);
}

void UIRenderer::CreateFont() {
    std::string fontPath = VFS::Resolve("assets/interface/Windows/2002_EYA.ttf"); std::ifstream f(fontPath, std::ios::binary);
    if (!f) return; std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    stbtt_fontinfo info; if (!stbtt_InitFont(&info, data.data(), 0)) return;
    std::vector<unsigned char> atlas(512 * 128, 0); stbtt_bakedchar chardata[96];
    stbtt_BakeFontBitmap(data.data(), 0, font_size_, atlas.data(), 512, 128, 32, 96, chardata);
    std::vector<unsigned char> rgba(512 * 128 * 4); for (int i = 0; i < 512 * 128; i++) { rgba[i*4+0]=rgba[i*4+1]=rgba[i*4+2]=255; rgba[i*4+3]=atlas[i]; }
    font_tex_ = bgfx::createTexture2D(512, 128, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(rgba.data(), (uint32_t)rgba.size()));
    for (int i = 0; i < 96; i++) {
        font_glyphs_[i].u0 = (float)chardata[i].x0 / 512; font_glyphs_[i].v0 = (float)chardata[i].y0 / 128;
        font_glyphs_[i].u1 = (float)chardata[i].x1 / 512; font_glyphs_[i].v1 = (float)chardata[i].y1 / 128;
        font_glyphs_[i].xoff = chardata[i].xoff; font_glyphs_[i].yoff = chardata[i].yoff + font_size_;
        font_glyphs_[i].xadvance = chardata[i].xadvance; font_glyphs_[i].w = (float)(chardata[i].x1 - chardata[i].x0); font_glyphs_[i].h = (float)(chardata[i].y1 - chardata[i].y0);
    }
    font_ready_ = true;

    // Build the texture atlas on font load
    BuildTextureAtlas();

    spdlog::info("UIRenderer: font created, texture atlas cached");
}

TextureInfo UIRenderer::LoadTexture(const std::string& name, const std::string& path) {
    // Check cache first
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;

    // Check if already in atlas
    auto at = atlas_regions_.find(name);
    if (at != atlas_regions_.end()) {
        TextureInfo info;
        info.handle = atlas_tex_;
        info.width = at->second.w;
        info.height = at->second.h;
        textures_[name] = info;
        return info;
    }

    // Queue for atlas packing
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
                // Small texture, queue for atlas
                atlas_pending_.push_back({name, p});
                return TextureInfo{}; // Will be available after BuildTextureAtlas
            }

            // Large texture, load individually
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
    static bool done = false; if (done) return; done = true;
    for (auto& [n, t] : textures_) if (bgfx::isValid(t.handle) && t.handle.idx != atlas_tex_.idx) bgfx::destroy(t.handle);
    textures_.clear();
    if (bgfx::isValid(atlas_tex_)) bgfx::destroy(atlas_tex_);
    if (bgfx::isValid(font_tex_)) bgfx::destroy(font_tex_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_); if (bgfx::isValid(s_tex_)) bgfx::destroy(s_tex_);
    if (bgfx::isValid(ui_prog_)) bgfx::destroy(ui_prog_);
    atlas_data_.clear();
    atlas_regions_.clear();
    atlas_pending_.clear();
}
