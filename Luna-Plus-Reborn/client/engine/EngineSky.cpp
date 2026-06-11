#include "EngineSky.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <engine/gx_render/RenderDevice.h>
#include <engine/gx_render/VFS.h>
#include <engine/gx_render/Shader.h>

void EngineSky::SetSampler(bgfx::UniformHandle sampler, bgfx::TextureHandle white_tex) {
    (void)sampler;
    white_tex_ = white_tex;
}

void EngineSky::Init() {
    spdlog::info("EngineSky: initialized (time={:.2f}, speed={:.3f})", time_of_day_, time_speed_);

    program_ = ShaderUtils::LoadProgram("shaders/vs_default.bin", "shaders/fs_sky.bin");
    if (!bgfx::isValid(program_)) {
        spdlog::warn("EngineSky: fs_sky.bin not found, fallback to fs_default.bin");
        program_ = ShaderUtils::LoadProgram("shaders/vs_default.bin", "shaders/fs_default.bin");
    }

    s_texSky_ = bgfx::createUniform("s_texSky", bgfx::UniformType::Sampler);

    // Load sky texture — try multiple possible paths
    sky_tex_ = LoadSkyTexture(VFS::Find("assets/textures/unpacked/map/01_human_sky.png"));
    if (!bgfx::isValid(sky_tex_)) { sky_tex_ = LoadSkyTexture(VFS::Find("assets/textures/unpacked/map/60_sky.png")); }
    if (!bgfx::isValid(sky_tex_)) { sky_tex_ = LoadSkyTexture(VFS::Find("assets_converted/mod_objs/01_human_sky.png")); }
    if (!bgfx::isValid(sky_tex_)) { sky_tex_ = LoadSkyTexture(VFS::Find("assets_converted/mod_objs/60_sky.png")); }
    if (!bgfx::isValid(sky_tex_)) { sky_tex_ = LoadSkyTexture(VFS::Find("assets/textures/01_human_sky.png")); }
    if (!bgfx::isValid(sky_tex_)) { sky_tex_ = LoadSkyTexture(VFS::Find("assets/textures/60_sky.png")); }

    BuildDomeMesh();
}

bgfx::TextureHandle EngineSky::LoadSkyTexture(const std::string& name) {
    int w, h, n;
    unsigned char* d = stbi_load(name.c_str(), &w, &h, &n, 4);
    if (!d) { spdlog::warn("EngineSky: cannot load texture {}", name); return BGFX_INVALID_HANDLE; }
    auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
        bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE,
        bgfx::copy(d, w * h * 4));
    stbi_image_free(d);
    spdlog::info("EngineSky: loaded texture {} ({}x{})", name, w, h);
    return tex;
}

void EngineSky::BuildDomeMesh() {
    struct DomeVertex { float x, y, z; uint32_t color; float u, v; };
    std::vector<DomeVertex> verts;
    std::vector<uint16_t> idx;

    int rings = 24;
    int sectors = 32;
    float radius = 500.0f;

    for (int r = 0; r <= rings; r++) {
        float theta = (float)r / rings * 3.14159f * 0.5f;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);
        for (int s = 0; s <= sectors; s++) {
            float phi = (float)s / sectors * 2.0f * 3.14159f;
            float x = radius * sinTheta * cosf(phi);
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinf(phi);
            // Vertex color gradient: top is sky color, bottom is fog color
            float t = (float)r / rings;
            glm::vec3 topCol = GetSkyTopColor();
            glm::vec3 botCol = GetFogColor();
            glm::vec3 col = topCol + (botCol - topCol) * t;
            uint32_t color = ((uint32_t)(col.r * 255) << 24) |
                             ((uint32_t)(col.g * 255) << 16) |
                             ((uint32_t)(col.b * 255) << 8) | 0xff;
            float u = (float)s / sectors;
            float v = 1.0f - t;
            verts.push_back({x, y, z, color, u, v});
        }
    }

    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < sectors; s++) {
            int a = r * (sectors + 1) + s;
            int b = a + 1;
            int c = (r + 1) * (sectors + 1) + s;
            int d = c + 1;
            idx.push_back(a); idx.push_back(c); idx.push_back(b);
            idx.push_back(b); idx.push_back(c); idx.push_back(d);
        }
    }

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    vb_ = bgfx::createVertexBuffer(
        bgfx::makeRef(verts.data(), static_cast<uint32_t>(verts.size() * sizeof(DomeVertex))), layout);
    ib_ = bgfx::createIndexBuffer(
        bgfx::makeRef(idx.data(), static_cast<uint32_t>(idx.size() * sizeof(uint16_t))));
}

void EngineSky::Update(float dt) {
    time_of_day_ += dt * time_speed_;
    if (time_of_day_ >= 1.0f) time_of_day_ -= 1.0f;
    cloud_offset_ += dt * 0.02f;
}

glm::vec3 EngineSky::LerpColor(const glm::vec3& a, const glm::vec3& b, float t) const {
    return a + (b - a) * std::max(0.0f, std::min(1.0f, t));
}

glm::vec3 EngineSky::GetSkyTopColor() const {
    float t = time_of_day_;
    if (t < 0.25f) return LerpColor({0.02f, 0.02f, 0.15f}, {0.3f, 0.2f, 0.6f}, t / 0.25f);
    if (t < 0.5f) return LerpColor({0.3f, 0.2f, 0.6f}, {0.4f, 0.6f, 0.9f}, (t - 0.25f) / 0.25f);
    if (t < 0.75f) return LerpColor({0.4f, 0.6f, 0.9f}, {0.6f, 0.3f, 0.2f}, (t - 0.5f) / 0.25f);
    return LerpColor({0.6f, 0.3f, 0.2f}, {0.02f, 0.02f, 0.15f}, (t - 0.75f) / 0.25f);
}

glm::vec3 EngineSky::GetSunColor() const {
    float t = time_of_day_;
    if (t < 0.25f) return LerpColor({0.1f, 0.1f, 0.3f}, {1.0f, 0.6f, 0.2f}, t / 0.25f);
    if (t < 0.5f) return LerpColor({1.0f, 0.6f, 0.2f}, {1.0f, 0.95f, 0.8f}, (t - 0.25f) / 0.25f);
    if (t < 0.75f) return LerpColor({1.0f, 0.95f, 0.8f}, {1.0f, 0.5f, 0.2f}, (t - 0.5f) / 0.25f);
    return LerpColor({1.0f, 0.5f, 0.2f}, {0.1f, 0.1f, 0.3f}, (t - 0.75f) / 0.25f);
}

glm::vec3 EngineSky::GetAmbientColor() const {
    float t = time_of_day_;
    if (t < 0.25f || t > 0.75f) return {0.05f, 0.05f, 0.1f};
    return {0.3f, 0.3f, 0.35f};
}

glm::vec3 EngineSky::GetFogColor() const {
    float t = time_of_day_;
    if (t < 0.25f) return LerpColor({0.02f, 0.02f, 0.05f}, {0.5f, 0.4f, 0.3f}, t / 0.25f);
    if (t < 0.5f) return LerpColor({0.5f, 0.4f, 0.3f}, {0.7f, 0.75f, 0.8f}, (t - 0.25f) / 0.25f);
    if (t < 0.75f) return LerpColor({0.7f, 0.75f, 0.8f}, {0.5f, 0.3f, 0.2f}, (t - 0.5f) / 0.25f);
    return LerpColor({0.5f, 0.3f, 0.2f}, {0.02f, 0.02f, 0.05f}, (t - 0.75f) / 0.25f);
}

float EngineSky::GetSunIntensity() const {
    float t = time_of_day_;
    if (t < 0.2f || t > 0.8f) return 0.1f;
    if (t < 0.3f) return (t - 0.2f) / 0.1f;
    if (t > 0.7f) return (0.8f - t) / 0.1f;
    return 1.0f;
}

glm::vec3 EngineSky::GetLightDirection() const {
    float angle = (time_of_day_ - 0.25f) * 2.0f * 3.14159f;
    float altitude = sinf(angle);
    float azimuth = cosf(angle);
    return glm::normalize(glm::vec3(azimuth, -altitude, 0.3f));
}

void EngineSky::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    (void)ui;
    if (!bgfx::isValid(program_) || !bgfx::isValid(vb_)) return;
    if (!bgfx::isValid(s_texSky_)) return;

    bgfx::ViewId sky_view = static_cast<bgfx::ViewId>(ViewId::Sky);

    // View order is set in GraphicEngine::BeginFrame

    glm::mat4 invView = glm::inverse(view);
    glm::vec3 cam_pos(invView[3]);

    // Clear with sky fog color
    glm::vec3 fog = GetFogColor();
    uint32_t clear_col = ((uint32_t)(fog.r * 255) << 24) |
                         ((uint32_t)(fog.g * 255) << 16) |
                         ((uint32_t)(fog.b * 255) << 8) | 0xff;

    // Sky is the base layer: clear color AND depth
    bgfx::setViewClear(sky_view, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clear_col, 1.0f, 0);
    // Viewport is already set globally in GraphicEngine::BeginFrame
    bgfx::setViewTransform(sky_view, glm::value_ptr(view), glm::value_ptr(proj));

    // Translate dome to camera position
    float mtx[16] = {
        1,0,0,0, 0,1,0,0, 0,0,1,0,
        cam_pos.x, cam_pos.y, cam_pos.z, 1
    };
    bgfx::setTransform(mtx);

    // Use sky texture if available, otherwise use white so vertex colors show
    bgfx::TextureHandle tex = bgfx::isValid(sky_tex_) ? sky_tex_ : white_tex_;
    bgfx::setTexture(0, s_texSky_, bgfx::isValid(tex) ? tex : bgfx::TextureHandle{bgfx::kInvalidHandle});
    bgfx::setVertexBuffer(0, vb_);
    bgfx::setIndexBuffer(ib_);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CW | BGFX_STATE_DEPTH_TEST_LEQUAL);
    bgfx::submit(sky_view, program_);
}

void EngineSky::Shutdown() {
    if (bgfx::isValid(vb_)) bgfx::destroy(vb_);
    if (bgfx::isValid(ib_)) bgfx::destroy(ib_);
    if (bgfx::isValid(sky_tex_)) bgfx::destroy(sky_tex_);
    if (bgfx::isValid(cloud_tex_)) bgfx::destroy(cloud_tex_);
    if (bgfx::isValid(program_)) bgfx::destroy(program_);
    if (bgfx::isValid(s_texSky_)) bgfx::destroy(s_texSky_);
}
