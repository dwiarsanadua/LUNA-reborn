#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <engine/gx_render/RenderDevice.h>

struct PartVertex { float x, y, z; uint32_t color; float u, v; };

class ParticleRenderer {
public:
    static constexpr size_t MAX_PARTICLES = 512;

    void Init();
    void Render(const glm::mat4& view, const glm::mat4& proj,
                const std::vector<glm::vec3>& positions,
                const std::vector<uint32_t>& colors,
                const std::vector<float>& sizes);
    void Render(const glm::mat4& view, const glm::mat4& proj,
                const std::vector<glm::vec3>& positions,
                const std::vector<uint32_t>& colors,
                const std::vector<float>& sizes,
                bgfx::TextureHandle texture);
    void SetParticleTexture(const std::string& path);
    void Shutdown();

private:
    bgfx::ProgramHandle prog_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle particle_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::Particle);
};
