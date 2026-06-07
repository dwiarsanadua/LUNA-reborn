#include "WorldRenderer.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

static const bgfx::Memory* loadShader(const char* path) {
    std::string searchPaths[] = {
        std::string("build/bin/") + path,
        std::string("bin/") + path,
        path,
        std::string("../") + path,
        std::string("assets/") + path
    };

    for (const auto& p : searchPaths) {
        std::ifstream file(p, std::ios::binary | std::ios::ate);
        if (file) {
            size_t size = file.tellg();
            file.seekg(0);
            auto* mem = bgfx::alloc(static_cast<uint32_t>(size));
            file.read(reinterpret_cast<char*>(mem->data), size);
            spdlog::info("Shader: loaded {} ({} bytes)", p, size);
            return mem;
        }
    }
    return nullptr;
}

WorldRenderer::WorldRenderer() = default;
WorldRenderer::~WorldRenderer() { Shutdown(); }

void WorldRenderer::Init() {
    auto vs = loadShader("shaders/vs_default.bin");
    auto fs = loadShader("shaders/fs_unlit.bin");
    if (vs && fs) {
        program_ = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    }

    auto vs_post = loadShader("shaders/vs_post.bin");
    auto fs_water = loadShader("shaders/fs_water.bin");
    if (vs_post && fs_water) {
        water_program_ = bgfx::createProgram(bgfx::createShader(vs_post), bgfx::createShader(fs_water), true);
        spdlog::info("WorldRenderer: water program initialized");
    }

    u_color_ = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    s_tex_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    u_time_ = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

    uint32_t white = 0xffffffff;
    white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0,
                                        bgfx::makeRef(&white, sizeof(white)));
    
    CreateGroundGrid(grid_size_);
    CreatePlayerCube();
    CreateWaterPlane(50);
    
    spdlog::info("WorldRenderer: initialized");
}

void WorldRenderer::CreateGroundGrid(int size) {
    struct Vertex { float x, y, z; uint32_t color; };
    std::vector<Vertex> verts;
    std::vector<uint16_t> idx;

    for (int z = -size; z <= size; z++) {
        uint32_t color = (z % 5 == 0) ? 0xff666666 : 0xff444444;
        verts.push_back({(float)-size, 0, (float)z, color});
        verts.push_back({(float)size, 0, (float)z, color});
        int base = static_cast<int>(verts.size()) - 2;
        idx.push_back(base); idx.push_back(base + 1);
    }
    for (int x = -size; x <= size; x++) {
        uint32_t color = (x % 5 == 0) ? 0xff666666 : 0xff444444;
        verts.push_back({(float)x, 0, (float)-size, color});
        verts.push_back({(float)x, 0, (float)size, color});
        int base = static_cast<int>(verts.size()) - 2;
        idx.push_back(base); idx.push_back(base + 1);
    }

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    ground_vb_ = bgfx::createVertexBuffer(
        bgfx::copy(verts.data(), static_cast<uint32_t>(verts.size() * sizeof(Vertex))),
        layout);
    ground_ib_ = bgfx::createIndexBuffer(
        bgfx::copy(idx.data(), static_cast<uint32_t>(idx.size() * sizeof(uint16_t))));
}

void WorldRenderer::CreatePlayerCube() {
    struct Vertex { float x, y, z; uint32_t color; };
    uint32_t green = 0xff00ff00;
    float s = 0.5f;

    Vertex verts[] = {
        { -s, -s,  s, green }, {  s, -s,  s, green }, {  s,  s,  s, green }, { -s,  s,  s, green },
        { -s, -s, -s, green }, {  s, -s, -s, green }, {  s,  s, -s, green }, { -s,  s, -s, green },
    };
    uint16_t idx[] = {
        0,1,2, 0,2,3,  1,5,6, 1,6,2,  5,4,7, 5,7,6,  4,0,3, 4,3,7,
        3,2,6, 3,6,7,  4,5,1, 4,1,0,
    };

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    player_vb_ = bgfx::createVertexBuffer(
        bgfx::copy(verts, sizeof(verts)), layout);
    player_ib_ = bgfx::createIndexBuffer(
        bgfx::copy(idx, sizeof(idx)));
}

void WorldRenderer::CreateWaterPlane(int size) {
    struct Vertex { float x, y, z; uint32_t color; float u, v; };
    float s = (float)size;
    Vertex verts[] = {
        { -s, -5.0f,  s, 0xbbffaa44, 0, 0 },
        {  s, -5.0f,  s, 0xbbffaa44, 1, 0 },
        { -s, -5.0f, -s, 0xbbffaa44, 0, 1 },
        {  s, -5.0f, -s, 0xbbffaa44, 1, 1 },
    };
    uint16_t idx[] = { 0, 1, 2, 1, 3, 2 };

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    water_vb_ = bgfx::createVertexBuffer(bgfx::copy(verts, sizeof(verts)), layout);
    water_ib_ = bgfx::createIndexBuffer(bgfx::copy(idx, sizeof(idx)));
}

void WorldRenderer::Render(const glm::mat4& view, const glm::mat4& proj) {
    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, 1280, 720);

    // Ground grid
    if (bgfx::isValid(program_) && bgfx::isValid(ground_vb_)) {
        bgfx::setTexture(0, s_tex_color_, white_tex_);
        bgfx::setVertexBuffer(0, ground_vb_);
        bgfx::setIndexBuffer(ground_ib_);
        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(view_id_, program_);
    }

    // Water plane
    if (bgfx::isValid(water_program_) && bgfx::isValid(water_vb_)) {
        float time = (float)bgfx::getStats()->cpuTimeFrame / 1000000.0f;
        float time_vec[4] = { time, 0, 0, 0 };
        bgfx::setUniform(u_time_, time_vec);
        bgfx::setTexture(0, s_tex_color_, white_tex_);
        bgfx::setVertexBuffer(0, water_vb_);
        bgfx::setIndexBuffer(water_ib_);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(view_id_, water_program_);
    }

    // Player cube
    if (bgfx::isValid(program_) && bgfx::isValid(player_vb_)) {
        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, player_pos_.x, player_pos_.y, player_pos_.z, 1};
        bgfx::setTransform(mtx);
        bgfx::setTexture(0, s_tex_color_, white_tex_);
        bgfx::setVertexBuffer(0, player_vb_);
        bgfx::setIndexBuffer(player_ib_);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(view_id_, program_);
    }
}

void WorldRenderer::Shutdown() {
    if (bgfx::isValid(ground_vb_)) bgfx::destroy(ground_vb_);
    if (bgfx::isValid(ground_ib_)) bgfx::destroy(ground_ib_);
    if (bgfx::isValid(player_vb_)) bgfx::destroy(player_vb_);
    if (bgfx::isValid(player_ib_)) bgfx::destroy(player_ib_);
    if (bgfx::isValid(water_vb_)) bgfx::destroy(water_vb_);
    if (bgfx::isValid(water_ib_)) bgfx::destroy(water_ib_);
    if (bgfx::isValid(s_tex_color_)) bgfx::destroy(s_tex_color_);
    if (bgfx::isValid(u_time_)) bgfx::destroy(u_time_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_);
    if (bgfx::isValid(program_)) bgfx::destroy(program_);
    if (bgfx::isValid(water_program_)) bgfx::destroy(water_program_);
    if (bgfx::isValid(u_color_)) bgfx::destroy(u_color_);
}

void WorldRenderer::SetPlayerPos(const glm::vec3& pos) {
    player_pos_ = pos;
}
