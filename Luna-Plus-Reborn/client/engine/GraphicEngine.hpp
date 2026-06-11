#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <rendering/TerrainRenderer.hpp>
#include <rendering/PropRenderer.hpp>
#include <rendering/WorldRenderer.hpp>
#include <rendering/ParticleRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <rendering/UIRenderer.hpp>
#include <rendering/SceneRenderer.hpp>
#include <rendering/PostProcessor.hpp>
#include <rendering/RenderQueue.hpp>

class GraphicEngine {
public:
    void Init(uint16_t fb_width, uint16_t fb_height);
    void BeginFrame(const glm::mat4& view, const glm::mat4& proj, uint16_t width, uint16_t height);
    void Render(TerrainRenderer* terrain, PropRenderer* props, WorldRenderer* world,

                const glm::mat4& view, const glm::mat4& proj,
                const glm::vec3& light_dir = glm::vec3(0.5f, -0.8f, -0.3f));
    void RenderCharacters(float time, const glm::mat4& view, const glm::mat4& proj);
    void RenderParticles(ParticleRenderer* particles, const std::vector<glm::vec3>& ppos,
                         const std::vector<uint32_t>& pcol, const std::vector<float>& psiz,
                         const glm::mat4& view, const glm::mat4& proj);
    void RenderUI(UIRenderer& ui);
    void Shutdown();

    SceneRenderer* GetScene() { return &scene_; }
    PostProcessor* GetPostFX() { return &post_fx_; }

private:
    SceneRenderer scene_;
    PostProcessor post_fx_;
    RenderQueue render_queue_;
};
