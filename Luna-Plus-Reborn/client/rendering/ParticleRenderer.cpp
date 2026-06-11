#include "ParticleRenderer.hpp"
#include <engine/gx_render/Shader.h>
#include <fstream>
#include <cstring>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

static bgfx::VertexLayout getLayout() {
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    return layout;
}

void ParticleRenderer::Init() {
    prog_ = ShaderUtils::LoadProgram("shaders/vs_default.bin", "shaders/fs_default.bin");
    s_tex_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff;
    white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(&white, sizeof(white)));
}

void ParticleRenderer::Render(const glm::mat4& view, const glm::mat4& proj,
                               const std::vector<glm::vec3>& positions,
                               const std::vector<uint32_t>& colors,
                               const std::vector<float>& sizes) {
    Render(view, proj, positions, colors, sizes, BGFX_INVALID_HANDLE);
}

void ParticleRenderer::Render(const glm::mat4& view, const glm::mat4& proj,
                               const std::vector<glm::vec3>& positions,
                               const std::vector<uint32_t>& colors,
                               const std::vector<float>& sizes,
                               bgfx::TextureHandle texture) {
    if (!bgfx::isValid(prog_) || positions.empty()) return;
    size_t count = std::min(positions.size(), MAX_PARTICLES);
    if (count < positions.size()) {
        spdlog::warn("ParticleRenderer: overflow, {} particles truncated to {}",
                     positions.size(), MAX_PARTICLES);
    }

    // Extract camera right and up vectors from inverse view matrix
    glm::mat4 inv_view = glm::inverse(view);
    glm::vec3 cam_right(inv_view[0][0], inv_view[1][0], inv_view[2][0]);
    glm::vec3 cam_up(inv_view[0][1], inv_view[1][1], inv_view[2][1]);

    std::vector<PartVertex> verts;
    verts.reserve(count * 4);
    std::vector<uint16_t> idx;
    idx.reserve(count * 6);

    for (size_t i = 0; i < count; i++) {
        uint16_t base = static_cast<uint16_t>(verts.size());
        float s = sizes[i] * 0.5f;
        uint32_t c = colors[i];
        glm::vec3 p = positions[i];

        verts.push_back({p.x + (-cam_right.x + cam_up.x) * s,
                         p.y + (-cam_right.y + cam_up.y) * s,
                         p.z + (-cam_right.z + cam_up.z) * s, c, 0, 0});
        verts.push_back({p.x + ( cam_right.x + cam_up.x) * s,
                         p.y + ( cam_right.y + cam_up.y) * s,
                         p.z + ( cam_right.z + cam_up.z) * s, c, 1, 0});
        verts.push_back({p.x + (-cam_right.x - cam_up.x) * s,
                         p.y + (-cam_right.y - cam_up.y) * s,
                         p.z + (-cam_right.z - cam_up.z) * s, c, 0, 1});
        verts.push_back({p.x + ( cam_right.x - cam_up.x) * s,
                         p.y + ( cam_right.y - cam_up.y) * s,
                         p.z + ( cam_right.z - cam_up.z) * s, c, 1, 1});
        idx.push_back(base); idx.push_back(base+1); idx.push_back(base+2);
        idx.push_back(base+1); idx.push_back(base+3); idx.push_back(base+2);
    }

    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, bgfx::BackbufferRatio::Equal);

    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    if (bgfx::allocTransientBuffers(&tvb, getLayout(), (uint16_t)verts.size(), &tib, (uint32_t)idx.size())) {
        std::memcpy(tvb.data, verts.data(), verts.size() * sizeof(PartVertex));
        std::memcpy(tib.data, idx.data(), idx.size() * sizeof(uint16_t));
        bgfx::TextureHandle tex = bgfx::isValid(texture) ? texture :
                                  (bgfx::isValid(particle_tex_) ? particle_tex_ : white_tex_);
        bgfx::setTexture(0, s_tex_, tex);
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(view_id_, prog_);
    }
}

void ParticleRenderer::SetParticleTexture(const std::string& path) {
    if (bgfx::isValid(particle_tex_)) {
        bgfx::destroy(particle_tex_);
        particle_tex_ = BGFX_INVALID_HANDLE;
    }
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        spdlog::warn("ParticleRenderer: failed to load texture {}: {}", path, stbi_failure_reason());
        return;
    }
    const bgfx::Memory* mem = bgfx::copy(data, w * h * 4);
    stbi_image_free(data);
    particle_tex_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                                           bgfx::TextureFormat::RGBA8, 0, mem);
    if (bgfx::isValid(particle_tex_)) {
        spdlog::info("ParticleRenderer: loaded texture {} ({}x{})", path, w, h);
    } else {
        spdlog::warn("ParticleRenderer: failed to create GPU texture from {}", path);
    }
}

void ParticleRenderer::Shutdown() {
    if (bgfx::isValid(prog_)) bgfx::destroy(prog_);
    if (bgfx::isValid(s_tex_)) bgfx::destroy(s_tex_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_);
    if (bgfx::isValid(particle_tex_)) bgfx::destroy(particle_tex_);
}
