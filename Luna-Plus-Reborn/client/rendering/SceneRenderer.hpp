#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <engine/gx_render/RenderDevice.h>

class SceneRenderer {
public:
    void Initialize();
    void Render(entt::registry& registry, const glm::mat4& view, const glm::mat4& proj);
    void Render(entt::registry& registry, const glm::mat4& view, const glm::mat4& proj, const EnvData& env);
    void Render(const glm::mat4& view, const glm::mat4& proj);
    void Shutdown();

    void SetClearColor(uint32_t color);
    void SetViewProjection(const glm::mat4& view, const glm::mat4& proj);

    // Shadow mapping
    void BeginShadowPass(const glm::vec3& light_dir);
    void EndShadowPass();
    bgfx::TextureHandle GetShadowMap() const { return shadow_map_; }
    const glm::mat4& GetShadowMVP() const { return shadow_mvp_; }
    bool HasShadow() const { return bgfx::isValid(shadow_map_); }

    float width = 1280.0f;
    float height = 720.0f;

private:
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::Debug);
    uint8_t shadow_view_id_ = 7;
    uint32_t clear_color_ = 0x443355FF;
    
    // Shadow map resources
    bgfx::FrameBufferHandle shadow_fbo_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle shadow_map_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle shadow_depth_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_shadow_map_ = BGFX_INVALID_HANDLE;
    glm::mat4 shadow_mvp_;
    int shadow_map_size_ = 1024;
    bool shadow_initialized_ = false;
};
