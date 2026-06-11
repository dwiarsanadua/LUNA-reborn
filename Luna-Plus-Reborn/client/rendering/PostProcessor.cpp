#include "PostProcessor.hpp"
#include <engine/gx_render/Shader.h>
#include <spdlog/spdlog.h>
#include <cstring>

struct PostVertex {
    float x, y, z;
    float u, v;
};

static bgfx::VertexLayout PostVertexLayout() {
    static bgfx::VertexLayout layout;
    static bool init = false;
    if (!init) {
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        init = true;
    }
    return layout;
}

bool PostProcessor::CreateRenderTargets(uint16_t w, uint16_t h) {
    auto fmt = bgfx::TextureFormat::RGBA16F;
    if (!bgfx::isTextureValid(0, false, 1, fmt, BGFX_TEXTURE_RT))
        fmt = bgfx::TextureFormat::RGBA8;

    uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT;

    scene_tex_ = bgfx::createTexture2D(w, h, false, 1, fmt, flags);
    if (!bgfx::isValid(scene_tex_)) return false;
    scene_fbo_ = bgfx::createFrameBuffer(1, &scene_tex_, true);
    if (!bgfx::isValid(scene_fbo_)) return false;

    for (int i = 0; i < 2; i++) {
        bloom_tex_[i] = bgfx::createTexture2D(w, h, false, 1, fmt, flags);
        if (!bgfx::isValid(bloom_tex_[i])) return false;
        bloom_fbo_[i] = bgfx::createFrameBuffer(1, &bloom_tex_[i], true);
        if (!bgfx::isValid(bloom_fbo_[i])) return false;
    }

    output_tex_ = bgfx::createTexture2D(w, h, false, 1, fmt, flags);
    if (!bgfx::isValid(output_tex_)) return false;
    auto output_fbo = bgfx::createFrameBuffer(1, &output_tex_, true);
    if (!bgfx::isValid(output_fbo)) return false;
    bgfx::destroy(output_fbo);

    return true;
}

void PostProcessor::RenderBloom(bgfx::ViewId view, bgfx::TextureHandle source) {
    bgfx::setViewFrameBuffer(view, bloom_fbo_[0]);
    bgfx::setViewRect(view, 0, 0, width_, height_);
    bgfx::setViewClear(view, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setTexture(0, s_tex_color_, source);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    bgfx::setVertexBuffer(0, fullscreen_vb_);
    bgfx::setIndexBuffer(fullscreen_ib_);
    bgfx::submit(view, bloom_prog_);
}

void PostProcessor::RenderBlur(bgfx::ViewId view, bgfx::TextureHandle source, bool horizontal) {
    int slot = horizontal ? 0 : 1;
    bgfx::setViewFrameBuffer(view, bloom_fbo_[slot]);
    bgfx::setViewRect(view, 0, 0, width_, height_);
    bgfx::setViewClear(view, BGFX_CLEAR_NONE, 0, 1.0f, 0);

    float blur_data[4] = {
        horizontal ? (1.0f / width_) : 0.0f,
        horizontal ? 0.0f : (1.0f / height_),
        0, 0
    };
    bgfx::setUniform(u_blur_data_, blur_data);
    bgfx::setTexture(0, s_tex_color_, source);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    bgfx::setVertexBuffer(0, fullscreen_vb_);
    bgfx::setIndexBuffer(fullscreen_ib_);
    bgfx::submit(view, blur_prog_);
}

void PostProcessor::RenderFinal(bgfx::ViewId view, bgfx::TextureHandle scene, bgfx::TextureHandle bloom) {
    bgfx::setViewFrameBuffer(view, BGFX_INVALID_HANDLE);
    bgfx::setViewRect(view, 0, 0, width_, height_);
    bgfx::setViewClear(view, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setTexture(0, s_tex_color_, scene);
    bgfx::setTexture(1, s_tex_bloom_, bloom);
    float intensity[4] = { bloom_intensity_, 0, 0, 0 };
    bgfx::setUniform(u_bloom_intensity_, intensity);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    bgfx::setVertexBuffer(0, fullscreen_vb_);
    bgfx::setIndexBuffer(fullscreen_ib_);
    bgfx::submit(view, final_prog_);
}

bool PostProcessor::Init(uint16_t width, uint16_t height) {
    width_ = width;
    height_ = height;

    bloom_prog_ = ShaderUtils::LoadProgram("shaders/vs_post.bin", "shaders/fs_post_bloom.bin");
    blur_prog_ = ShaderUtils::LoadProgram("shaders/vs_post.bin", "shaders/fs_post_blur.bin");
    final_prog_ = ShaderUtils::LoadProgram("shaders/vs_post.bin", "shaders/fs_post_final.bin");

    if (!bgfx::isValid(bloom_prog_) || !bgfx::isValid(blur_prog_) || !bgfx::isValid(final_prog_)) {
        spdlog::error("PostProcessor: failed to load shader programs");
        return false;
    }

    // Fullscreen quad (clip space)
    PostVertex verts[] = {
        {-1.0f,  1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
    };
    uint16_t idx[] = {0, 1, 2, 1, 3, 2};

    fullscreen_vb_ = bgfx::createVertexBuffer(
        bgfx::copy(verts, sizeof(verts)), PostVertexLayout());
    fullscreen_ib_ = bgfx::createIndexBuffer(
        bgfx::copy(idx, sizeof(idx)));

    s_tex_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_tex_bloom_ = bgfx::createUniform("s_texBloom", bgfx::UniformType::Sampler);
    u_bloom_threshold_ = bgfx::createUniform("u_bloomThreshold", bgfx::UniformType::Vec4);
    u_bloom_intensity_ = bgfx::createUniform("u_bloomIntensity", bgfx::UniformType::Vec4);
    u_blur_data_ = bgfx::createUniform("u_blurData", bgfx::UniformType::Vec4);

    if (!CreateRenderTargets(width, height)) {
        spdlog::error("PostProcessor: failed to create render targets");
        return false;
    }

    spdlog::info("PostProcessor: initialized ({}x{})", width, height);
    return true;
}

void PostProcessor::Render(bgfx::TextureHandle scene_color, bgfx::ViewId start_view) {
    if (!enabled_ || !bgfx::isValid(scene_color)) return;

    // Bright-pass extraction → bloom_tex_[0]
    RenderBloom(start_view, scene_color);

    // Blur H: bloom_tex_[0] → bloom_tex_[1]
    RenderBlur(bgfx::ViewId(uint8_t(start_view) + 1), bloom_tex_[0], true);

    // Blur V: bloom_tex_[1] → bloom_tex_[0]
    RenderBlur(bgfx::ViewId(uint8_t(start_view) + 2), bloom_tex_[1], false);

    // Final composite: scene + bloom → output (backbuffer)
    bloom_intensity_ = 0.0f; // Disable bloom for Luna Old feel
    RenderFinal(bgfx::ViewId(uint8_t(start_view) + 3), scene_color, bloom_tex_[0]);
}

void PostProcessor::Shutdown() {
    if (bgfx::isValid(fullscreen_vb_)) bgfx::destroy(fullscreen_vb_);
    if (bgfx::isValid(fullscreen_ib_)) bgfx::destroy(fullscreen_ib_);
    if (bgfx::isValid(bloom_prog_)) bgfx::destroy(bloom_prog_);
    if (bgfx::isValid(blur_prog_)) bgfx::destroy(blur_prog_);
    if (bgfx::isValid(final_prog_)) bgfx::destroy(final_prog_);
    if (bgfx::isValid(scene_tex_)) bgfx::destroy(scene_tex_);
    if (bgfx::isValid(scene_fbo_)) bgfx::destroy(scene_fbo_);
    for (int i = 0; i < 2; i++) {
        if (bgfx::isValid(bloom_tex_[i])) bgfx::destroy(bloom_tex_[i]);
        if (bgfx::isValid(bloom_fbo_[i])) bgfx::destroy(bloom_fbo_[i]);
    }
    if (bgfx::isValid(output_tex_)) bgfx::destroy(output_tex_);
    if (bgfx::isValid(s_tex_color_)) bgfx::destroy(s_tex_color_);
    if (bgfx::isValid(s_tex_bloom_)) bgfx::destroy(s_tex_bloom_);
    if (bgfx::isValid(u_bloom_threshold_)) bgfx::destroy(u_bloom_threshold_);
    if (bgfx::isValid(u_bloom_intensity_)) bgfx::destroy(u_bloom_intensity_);
    if (bgfx::isValid(u_blur_data_)) bgfx::destroy(u_blur_data_);
}
