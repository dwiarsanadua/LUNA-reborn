#include "GraphicEngine.hpp"
#include <rendering/SceneRenderer.hpp>

void GraphicEngine::Init() {
    scene_.Initialize();
}

void GraphicEngine::BeginFrame(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& light_dir) {
    // Shadow pass: render depth from light's perspective
    scene_.BeginShadowPass(light_dir);
    scene_.EndShadowPass();
    
    // Main scene view
    scene_.Render(view, proj);
}

void GraphicEngine::Render(TerrainRenderer* terrain, PropRenderer* props, WorldRenderer* world, const glm::mat4& view, const glm::mat4& proj) {
    if (terrain) terrain->Render(view, proj, scene_.GetShadowMap(), scene_.GetShadowMVP());
    if (props) props->Render(view, proj);
    if (world) world->Render(view, proj);
}

void GraphicEngine::RenderCharacters(float time, const glm::mat4& view, const glm::mat4& proj) {
    CharRenderer_Render(view, proj, time);
}

void GraphicEngine::RenderParticles(ParticleRenderer* particles,
                                     const std::vector<glm::vec3>& ppos,
                                     const std::vector<uint32_t>& pcol,
                                     const std::vector<float>& psiz,
                                     const glm::mat4& view, const glm::mat4& proj) {
    if (particles) particles->Render(view, proj, ppos, pcol, psiz);
}

void GraphicEngine::RenderUI(UIRenderer& ui) {
    (void)ui;
}

void GraphicEngine::Shutdown() {
    scene_.Shutdown();
}
