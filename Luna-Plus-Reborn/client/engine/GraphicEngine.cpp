#include "GraphicEngine.hpp"
#include <rendering/SceneRenderer.hpp>
#include <spdlog/spdlog.h>

void GraphicEngine::Init(uint16_t fb_width, uint16_t fb_height) {
    scene_.Initialize();

    // Initialize PostProcessor (FBO creation may fail on some backends)
    if (!post_fx_.Init(fb_width, fb_height)) {
        spdlog::warn("GraphicEngine: PostProcessor init failed, disabling post-fx");
        post_fx_.SetEnabled(false);
    }
}

void GraphicEngine::BeginFrame(const glm::mat4& view, const glm::mat4& proj, uint16_t width, uint16_t height) {
    // Set view order every frame — must run before any view clears color
    const bgfx::ViewId order[] = {
        static_cast<bgfx::ViewId>(ViewId::Sky),
        static_cast<bgfx::ViewId>(ViewId::Shadow),
        static_cast<bgfx::ViewId>(ViewId::Terrain),
        static_cast<bgfx::ViewId>(ViewId::Props),
        static_cast<bgfx::ViewId>(ViewId::Character),
        static_cast<bgfx::ViewId>(ViewId::Particle),
        static_cast<bgfx::ViewId>(ViewId::UI),
        static_cast<bgfx::ViewId>(ViewId::Debug),
        static_cast<bgfx::ViewId>(ViewId::Scene),
        static_cast<bgfx::ViewId>(ViewId::PostFX),
        static_cast<bgfx::ViewId>(ViewId::PostFX2),
        static_cast<bgfx::ViewId>(ViewId::PostFX3),
        static_cast<bgfx::ViewId>(ViewId::PostFX4),
    };
    bgfx::setViewOrder(0, sizeof(order) / sizeof(order[0]), order);

    // Explicitly set viewport for EVERY view to ensure Retina coverage
    for (bgfx::ViewId i = 0; i < (bgfx::ViewId)ViewId::Count; ++i) {
        bgfx::setViewRect(i, 0, 0, width, height);
    }
    // Shadow view uses fixed size
    bgfx::setViewRect(static_cast<bgfx::ViewId>(ViewId::Shadow), 0, 0, 1024, 1024);

    scene_.Render(view, proj);
}

void GraphicEngine::Render(TerrainRenderer* terrain, PropRenderer* props, WorldRenderer* world,
                            const glm::mat4& view, const glm::mat4& proj,
                            const glm::vec3& light_dir) {
    // Shadow pass: render depth from light's perspective
    scene_.BeginShadowPass(light_dir);
    if (terrain) terrain->RenderShadow(scene_.GetShadowViewId(), scene_.GetShadowMVP());
    if (props) props->RenderShadow(scene_.GetShadowViewId(), scene_.GetShadowMVP());
    scene_.EndShadowPass();

    // Redirect scene views to FBO if post-processing is enabled
    bgfx::FrameBufferHandle scene_fbo = BGFX_INVALID_HANDLE;
    if (post_fx_.IsEnabled() && bgfx::isValid(post_fx_.GetSceneFBO())) {
        scene_fbo = post_fx_.GetSceneFBO();
        scene_.SetSceneViewsFBO(scene_fbo);
    }

    // Main render — terrain uses RenderQueue
    render_queue_.Clear();
    if (terrain) {
        terrain->SetRenderQueue(&render_queue_);
        terrain->Render(view, proj, scene_.GetShadowMap(), scene_.GetShadowMVP());
        terrain->SetRenderQueue(nullptr);
    }

    // Props uses separate RenderQueue internally via its own queue member
    if (props) props->Render(view, proj);

    if (world) world->Render(view, proj);

    // Post-processing: read scene FBO → bloom → backbuffer
    if (bgfx::isValid(scene_fbo)) {
        post_fx_.Render(post_fx_.GetSceneTexture(),
                        static_cast<bgfx::ViewId>(ViewId::PostFX));
    }
}

void GraphicEngine::RenderCharacters(float time, const glm::mat4& view, const glm::mat4& proj) {
    if (g_char_renderer) g_char_renderer->Render(view, proj, time);
}

void GraphicEngine::RenderParticles(ParticleRenderer* particles,
                                     const std::vector<glm::vec3>& ppos,
                                     const std::vector<uint32_t>& pcol,
                                     const std::vector<float>& psiz,
                                     const glm::mat4& view, const glm::mat4& proj) {
    if (particles) particles->Render(view, proj, ppos, pcol, psiz);
}

void GraphicEngine::RenderUI(UIRenderer& ui) {
    ui.Render();
}

void GraphicEngine::Shutdown() {
    post_fx_.Shutdown();
    scene_.Shutdown();
}
