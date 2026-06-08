#include "SceneRenderer.hpp"
#include <ecs/components/Transform.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <string>

void SceneRenderer::Initialize() {
    spdlog::info("SceneRenderer: initialized");
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, clear_color_, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);

    shadow_map_ = bgfx::createTexture2D(
        (uint16_t)shadow_map_size_, (uint16_t)shadow_map_size_,
        false, 1, bgfx::TextureFormat::D24S8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL);

    if (!bgfx::isValid(shadow_map_)) {
        shadow_map_ = bgfx::createTexture2D(
            (uint16_t)shadow_map_size_, (uint16_t)shadow_map_size_,
            false, 1, bgfx::TextureFormat::D16,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL);
    }

    if (!bgfx::isValid(shadow_map_)) {
        shadow_map_ = bgfx::createTexture2D(
            (uint16_t)shadow_map_size_, (uint16_t)shadow_map_size_,
            false, 1, bgfx::TextureFormat::D32F,
            BGFX_TEXTURE_RT);
    }

    if (bgfx::isValid(shadow_map_)) {
        shadow_fbo_ = bgfx::createFrameBuffer(shadow_map_size_, shadow_map_size_,
            bgfx::TextureFormat::D24S8);
        if (!bgfx::isValid(shadow_fbo_)) {
            shadow_fbo_ = bgfx::createFrameBuffer(shadow_map_size_, shadow_map_size_,
                bgfx::TextureFormat::D16);
        }
        if (!bgfx::isValid(shadow_fbo_)) {
            shadow_fbo_ = bgfx::createFrameBuffer(shadow_map_size_, shadow_map_size_,
                bgfx::TextureFormat::D32F);
        }
        shadow_initialized_ = bgfx::isValid(shadow_fbo_);
        spdlog::info("ShadowMap: created {}x{}", shadow_map_size_, shadow_map_size_);
    } else {
        spdlog::warn("ShadowMap: failed to create, falling back to no shadows");
        shadow_initialized_ = false;
    }
}

void SceneRenderer::Render(const glm::mat4& view, const glm::mat4& proj) {
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, clear_color_, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::touch(view_id_);
}

void SceneRenderer::Render(entt::registry& registry, const glm::mat4& view, const glm::mat4& proj) {
    EnvData env;
    Render(registry, view, proj, env);
}

void SceneRenderer::Render(entt::registry& registry, const glm::mat4& view, const glm::mat4& proj, const EnvData& env) {
    (void)env;
    (void)registry;
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, clear_color_, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::touch(view_id_);
}

void SceneRenderer::BeginShadowPass(const glm::vec3& light_dir) {
    if (!shadow_initialized_) return;

    glm::vec3 light_pos = -light_dir * 500.0f;
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 light_proj = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, 0.1f, 1000.0f);
    shadow_mvp_ = light_proj * light_view;

    bgfx::setViewClear(shadow_view_id_, BGFX_CLEAR_DEPTH | BGFX_CLEAR_COLOR, 0xffffffff, 1.0f, 0);
    bgfx::setViewRect(shadow_view_id_, 0, 0, shadow_map_size_, shadow_map_size_);
    glm::mat4 identity = glm::mat4(1.0f);
    bgfx::setViewTransform(shadow_view_id_, &identity, &identity);
    bgfx::setViewFrameBuffer(shadow_view_id_, shadow_fbo_);
    bgfx::touch(shadow_view_id_);
}

void SceneRenderer::EndShadowPass() {
    if (!shadow_initialized_) return;
}

void SceneRenderer::Shutdown() {
    if (bgfx::isValid(u_shadow_map_)) bgfx::destroy(u_shadow_map_);
    if (bgfx::isValid(shadow_map_)) bgfx::destroy(shadow_map_);
    if (bgfx::isValid(shadow_fbo_)) bgfx::destroy(shadow_fbo_);
    spdlog::info("SceneRenderer: shutdown");
}

void SceneRenderer::SetClearColor(uint32_t color) {
    clear_color_ = color;
    bgfx::setViewClear(view_id_, BGFX_CLEAR_DEPTH, color, 1.0f, 0);
}

void SceneRenderer::SetViewProjection(const glm::mat4& view, const glm::mat4& proj) {
    bgfx::setViewTransform(view_id_, &view, &proj);
}

void SceneRenderer::UpdateStats(float dt) {
    frame_count_++;
    elapsed_ += dt;
    if (elapsed_ >= 0.5f) {
        fps_ = frame_count_ / elapsed_;
        fps_smooth_ = fps_smooth_ * 0.9f + fps_ * 0.1f;
        frame_count_ = 0;
        elapsed_ = 0;
    }

    // Query bgfx stats
    const bgfx::Stats* stats = bgfx::getStats();
    if (stats) {
        last_draw_calls_ = stats->numDraw;
        last_primitives_ = 0;
        for (uint32_t i = 0; i < bgfx::Topology::Count; ++i) {
            last_primitives_ += stats->numPrims[i];
        }
        last_texture_memory_ = static_cast<uint32_t>(stats->textureMemoryUsed);
    }
}

void SceneRenderer::RenderDebugOverlay() {
    if (!show_debug_overlay_) return;

    // We use the debug view that's set up for overlay rendering.
    // The actual text rendering is done via UIRenderer, but here we
    // overlay on the existing view using bgfx's debug text system.
    const bgfx::Stats* stats = bgfx::getStats();

    bgfx::dbgTextClear();

    char buf[256];

    // FPS counter
    snprintf(buf, sizeof(buf), " FPS: %.1f", fps_smooth_);
    bgfx::dbgTextPrintf(0, 0, 0x0f, buf);

    // Draw call count
    snprintf(buf, sizeof(buf), " Draw Calls: %u", last_draw_calls_);
    bgfx::dbgTextPrintf(0, 1, 0x0e, buf);

    // Triangle count
    snprintf(buf, sizeof(buf), " Primitives: %u", last_primitives_);
    bgfx::dbgTextPrintf(0, 2, 0x0c, buf);

    // Memory usage
    float tex_mb = last_texture_memory_ / (1024.0f * 1024.0f);
    snprintf(buf, sizeof(buf), " Tex Memory: %.1f MB", tex_mb);
    bgfx::dbgTextPrintf(0, 3, 0x0f, buf);

    // Renderer info
    if (stats) {
        snprintf(buf, sizeof(buf), " Backend: %s", bgfx::getRendererName(bgfx::getRendererType()));
        bgfx::dbgTextPrintf(0, 5, 0x0f, buf);

        float cpu_ms = static_cast<float>(stats->cpuTimeFrame);
        float gpu_ms = static_cast<float>(stats->gpuTimeEnd - stats->gpuTimeBegin);
        snprintf(buf, sizeof(buf), " CPU: %.2f ms | GPU: %.2f ms", cpu_ms / 1000.0f, gpu_ms / 1000.0f);
        bgfx::dbgTextPrintf(0, 6, 0x0f, buf);
    }

    // View resolution
    snprintf(buf, sizeof(buf), " Resolution: %.0fx%.0f", width, height);
    bgfx::dbgTextPrintf(0, 11, 0x0f, buf);

    // Show/hide hint
    bgfx::dbgTextPrintf(0, 13, 0x44, " F11: Toggle Debug Overlay");
}
