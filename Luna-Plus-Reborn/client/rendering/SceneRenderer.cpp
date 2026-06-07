#include "SceneRenderer.hpp"
#include <ecs/components/Transform.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void SceneRenderer::Initialize() {
    spdlog::info("SceneRenderer: initialized");
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, clear_color_, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);

    // Create shadow map
    // Try to create shadow map - fallback to no shadows if unsupported
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
    
    // Light view-projection matrix
    glm::vec3 light_pos = -light_dir * 500.0f;
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 light_proj = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, 0.1f, 1000.0f);
    shadow_mvp_ = light_proj * light_view;
    
    // Set shadow view
    bgfx::setViewClear(shadow_view_id_, BGFX_CLEAR_DEPTH | BGFX_CLEAR_COLOR, 0xffffffff, 1.0f, 0);
    bgfx::setViewRect(shadow_view_id_, 0, 0, shadow_map_size_, shadow_map_size_);
    // For shadow pass, use identity for view and projection as they're baked into shadow_mvp_
    glm::mat4 identity = glm::mat4(1.0f);
    bgfx::setViewTransform(shadow_view_id_, &identity, &identity);
    bgfx::setViewFrameBuffer(shadow_view_id_, shadow_fbo_);
    bgfx::touch(shadow_view_id_);
}

void SceneRenderer::EndShadowPass() {
    if (!shadow_initialized_) return;
    // Reset frame buffer - no direct reset needed
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
