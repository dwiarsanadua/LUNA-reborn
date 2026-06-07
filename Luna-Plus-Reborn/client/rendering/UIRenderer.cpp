#include "UIRenderer.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <vector>
#include <cstring>
#include <algorithm>

#include <stb_image.h>

// stb_truetype for font rasterization
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
    std::string searchPaths[] = { "build/bin/" + std::string(path), std::string(path), "assets/" + std::string(path) };
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
    }
}

void UIRenderer::DrawImageUV(float x, float y, float w, float h, bgfx::TextureHandle tex, float u1, float v1, float u2, float v2, UIColor tint) {
    if (!bgfx::isValid(tex) && !bgfx::isValid(white_tex_)) return;
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
        bgfx::setTexture(0, s_tex_, tex); bgfx::setVertexBuffer(0, &tvb); bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA); bgfx::submit(view_id_, prog);
    }
}

void UIRenderer::DrawImage(float x, float y, float w, float h, bgfx::TextureHandle tex, UIColor tint) {
    DrawImageUV(x, y, w, h, tex, 0, 0, 1, 1, tint);
}

void UIRenderer::DrawNinePatch(float x, float y, float w, float h, const TextureInfo& tex, float l, float t, float r, float b, UIColor tint) {
    if (!bgfx::isValid(tex.handle)) return;
    float iw = 1.0f / tex.width, ih = 1.0f / tex.height;
    // Corners
    DrawImageUV(x, y, l, t, tex.handle, 0, 0, l*iw, t*ih, tint); // TL
    DrawImageUV(x+w-r, y, r, t, tex.handle, 1.0f-r*iw, 0, 1.0f, t*ih, tint); // TR
    DrawImageUV(x, y+h-b, l, b, tex.handle, 0, 1.0f-b*ih, l*iw, 1.0f, tint); // BL
    DrawImageUV(x+w-r, y+h-b, r, b, tex.handle, 1.0f-r*iw, 1.0f-b*ih, 1.0f, 1.0f, tint); // BR
    // Edges
    DrawImageUV(x+l, y, w-l-r, t, tex.handle, l*iw, 0, 1.0f-r*iw, t*ih, tint); // Top
    DrawImageUV(x+l, y+h-b, w-l-r, b, tex.handle, l*iw, 1.0f-b*ih, 1.0f-r*iw, 1.0f, tint); // Bottom
    DrawImageUV(x, y+t, l, h-t-b, tex.handle, 0, t*ih, l*iw, 1.0f-b*ih, tint); // Left
    DrawImageUV(x+w-r, y+t, r, h-t-b, tex.handle, 1.0f-r*iw, t*ih, 1.0f, 1.0f-b*ih, tint); // Right
    // Center
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

void UIRenderer::Init() {
    auto vs = loadShader("shaders/vs_ui.bin"); auto fs = loadShader("shaders/fs_ui.bin");
    if (vs && fs) ui_prog_ = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    s_tex_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff; white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));
    CreateFont(); spdlog::info("UIRenderer: initialized");
}

void UIRenderer::CreateFont() {
    std::string fontPath = "assets/interface/Windows/2002_EYA.ttf"; std::ifstream f(fontPath, std::ios::binary);
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
}

TextureInfo UIRenderer::LoadTexture(const std::string& name, const std::string& path) {
    if (textures_.count(name)) return textures_[name];
    // Try with original path, then with extensions swapped
    std::string variants[] = { path, "", "", "" };
    // Generate .tif variant if .png was requested
    if (path.size() > 4) {
        std::string base = path.substr(0, path.find_last_of('.'));
        variants[1] = base + ".tif";
        variants[2] = "assets/textures/" + base + ".tif";
        variants[3] = "assets/textures/" + path;
    }
    std::string search[] = { path, variants[1], variants[2], variants[3],
                             "assets/textures/ui/" + path,
                             "assets/textures/" + path,
                             "assets_converted/mod_objs/" + path };
    for (auto& p : search) {
        if (p.empty()) continue;
        int w, h, n; unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
        if (d) {
            TextureInfo info; info.handle = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(d, w*h*4));
            info.width = w; info.height = h; stbi_image_free(d); textures_[name] = info; return info;
        }
    }
    return {};
}

void UIRenderer::Render() { bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height); }
void UIRenderer::Shutdown() {
    static bool done = false; if (done) return; done = true;
    for (auto& [n, t] : textures_) if (bgfx::isValid(t.handle)) bgfx::destroy(t.handle);
    textures_.clear(); if (bgfx::isValid(font_tex_)) bgfx::destroy(font_tex_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_); if (bgfx::isValid(s_tex_)) bgfx::destroy(s_tex_);
    if (bgfx::isValid(ui_prog_)) bgfx::destroy(ui_prog_);
}
