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

void GraphicEngine::BeginFrame(const glm::mat4& view, const glm::mat4& proj) {
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
