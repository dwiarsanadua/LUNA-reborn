#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

class PostProcessor {
public:
    bool Init(uint16_t width, uint16_t height);
    void Render(bgfx::TextureHandle scene_color, bgfx::ViewId start_view);
    void Shutdown();

    void SetBloomThreshold(float t) { bloom_threshold_ = t; }
    void SetBloomIntensity(float i) { bloom_intensity_ = i; }
    bool IsEnabled() const { return enabled_; }
    void SetEnabled(bool e) { enabled_ = e; }

    bgfx::TextureHandle GetOutput() const { return output_tex_; }
    bgfx::TextureHandle GetSceneTexture() const { return scene_tex_; }
    bgfx::FrameBufferHandle GetSceneFBO() const { return scene_fbo_; }

private:
    bool CreateRenderTargets(uint16_t w, uint16_t h);
    void RenderBloom(bgfx::ViewId view, bgfx::TextureHandle source);
    void RenderBlur(bgfx::ViewId view, bgfx::TextureHandle source, bool horizontal);
    void RenderFinal(bgfx::ViewId view, bgfx::TextureHandle scene, bgfx::TextureHandle bloom);

    bgfx::ProgramHandle bloom_prog_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle blur_prog_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle final_prog_ = BGFX_INVALID_HANDLE;

    bgfx::VertexBufferHandle fullscreen_vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle fullscreen_ib_ = BGFX_INVALID_HANDLE;

    bgfx::TextureHandle scene_tex_ = BGFX_INVALID_HANDLE;
    bgfx::FrameBufferHandle scene_fbo_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle bloom_tex_[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
    bgfx::FrameBufferHandle bloom_fbo_[2] = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
    bgfx::TextureHandle output_tex_ = BGFX_INVALID_HANDLE;

    bgfx::UniformHandle u_bloom_threshold_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_bloom_intensity_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_bloom_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_blur_data_ = BGFX_INVALID_HANDLE;

    float bloom_threshold_ = 1.0f;
    float bloom_intensity_ = 0.3f;
    bool enabled_ = true;
    uint16_t width_ = 1280;
    uint16_t height_ = 720;
};
