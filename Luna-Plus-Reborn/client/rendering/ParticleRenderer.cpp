#include "ParticleRenderer.hpp"
#include <fstream>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const bgfx::Memory* loadShader(const char* path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return nullptr;
    size_t size = file.tellg(); file.seekg(0);
    auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
    file.read(reinterpret_cast<char*>(mem->data), size);
    return mem;
}

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
    auto vs = loadShader("shaders/vs_default.bin");
    auto fs = loadShader("shaders/fs_default.bin");
    if (vs && fs) prog_ = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    s_tex_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff;
    white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, sizeof(white)));
}

void ParticleRenderer::Render(const glm::mat4& view, const glm::mat4& proj,
                               const std::vector<glm::vec3>& positions,
                               const std::vector<uint32_t>& colors,
                               const std::vector<float>& sizes) {
    if (!bgfx::isValid(prog_) || positions.empty()) return;
    size_t count = positions.size();
    if (count > 512) count = 512;

    // Billboard particles: each is a camera-facing quad with size
    // For simplicity, just render as screen-aligned colored dots
    std::vector<PartVertex> verts;
    verts.reserve(count * 4);
    std::vector<uint16_t> idx;
    idx.reserve(count * 6);

    for (size_t i = 0; i < count; i++) {
        uint16_t base = static_cast<uint16_t>(verts.size());
        float s = sizes[i] * 0.5f;
        uint32_t c = colors[i];
        verts.push_back({positions[i].x - s, positions[i].y - s, positions[i].z, c, 0, 0});
        verts.push_back({positions[i].x + s, positions[i].y - s, positions[i].z, c, 1, 0});
        verts.push_back({positions[i].x - s, positions[i].y + s, positions[i].z, c, 0, 1});
        verts.push_back({positions[i].x + s, positions[i].y + s, positions[i].z, c, 1, 1});
        idx.push_back(base); idx.push_back(base+1); idx.push_back(base+2);
        idx.push_back(base+1); idx.push_back(base+3); idx.push_back(base+2);
    }

    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, 1280, 720);

    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    if (bgfx::allocTransientBuffers(&tvb, getLayout(), (uint16_t)verts.size(), &tib, (uint32_t)idx.size())) {
        std::memcpy(tvb.data, verts.data(), verts.size() * sizeof(PartVertex));
        std::memcpy(tib.data, idx.data(), idx.size() * sizeof(uint16_t));
        bgfx::setTexture(0, s_tex_, white_tex_);
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(view_id_, prog_);
    }
}

void ParticleRenderer::Shutdown() {
    if (bgfx::isValid(prog_)) bgfx::destroy(prog_);
    if (bgfx::isValid(s_tex_)) bgfx::destroy(s_tex_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_);
}
