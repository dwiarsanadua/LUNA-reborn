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

class GraphicEngine {
public:
    void Init();
    void BeginFrame(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& light_dir = glm::vec3(0.5f, -0.8f, -0.3f));
    void Render(TerrainRenderer* terrain, PropRenderer* props, WorldRenderer* world,
                const glm::mat4& view, const glm::mat4& proj);
    void RenderCharacters(float time, const glm::mat4& view, const glm::mat4& proj);
    void RenderParticles(ParticleRenderer* particles, const std::vector<glm::vec3>& ppos,
                         const std::vector<uint32_t>& pcol, const std::vector<float>& psiz,
                         const glm::mat4& view, const glm::mat4& proj);
    void RenderUI(UIRenderer& ui);
    void Shutdown();

    SceneRenderer* GetScene() { return &scene_; }

private:
    SceneRenderer scene_;
};
