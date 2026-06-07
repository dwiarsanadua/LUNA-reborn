#include "TerrainRenderer.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>

#include <stb_image.h>

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

TerrainRenderer::TerrainRenderer() = default;
TerrainRenderer::~TerrainRenderer() { Shutdown(); }

float TerrainRenderer::Noise(float x, float z) const {
    int ix = static_cast<int>(std::floor(x));
    int iz = static_cast<int>(std::floor(z));
    float fx = x - ix, fz = z - iz;
    auto hash = [](int x, int z) -> float {
        int n = x * 157 + z * 113;
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    };
    float sx = fx * fx * (3 - 2 * fx);
    float sz = fz * fz * (3 - 2 * fz);
    return hash(ix, iz) * (1 - sx) * (1 - sz)
         + hash(ix + 1, iz) * sx * (1 - sz)
         + hash(ix, iz + 1) * (1 - sx) * sz
         + hash(ix + 1, iz + 1) * sx * sz;
}

float TerrainRenderer::FractalNoise(float x, float z, int octaves) const {
    float val = 0, amp = 1, freq = 1, max = 0;
    for (int i = 0; i < octaves; i++) { val += Noise(x * freq, z * freq) * amp; max += amp; amp *= 0.5f; freq *= 2.0f; }
    return val / max;
}

float TerrainRenderer::GetHeight(float x, float z) const {
    if (use_hgt_ && hgt_width_ > 0 && hgt_height_ > 0) {
        float hx = (x / hgt_scale_) + hgt_width_ * 0.5f;
        float hz = (z / hgt_scale_) + hgt_height_ * 0.5f;
        int ix = static_cast<int>(hx), iz = static_cast<int>(hz);
        if (ix >= 0 && ix < hgt_width_ - 1 && iz >= 0 && iz < hgt_height_ - 1) {
            float fx = hx - ix, fz = hz - iz;
            float v00 = hgt_data_[iz * hgt_width_ + ix];
            float v10 = hgt_data_[iz * hgt_width_ + ix + 1];
            float v01 = hgt_data_[(iz + 1) * hgt_width_ + ix];
            float v11 = hgt_data_[(iz + 1) * hgt_width_ + ix + 1];
            return (v00 + (v10 - v00) * fx + ((v01 - v00) + (v11 - v01 - v10 + v00) * fx) * fz) * hgt_scale_;
        }
        return hgt_min_ * hgt_scale_;
    }
    return FractalNoise(x * 0.05f, z * 0.05f) * height_scale_;
}

static uint32_t HeightColor(float h, float hmin, float hmax) {
    float t = (h - hmin) / std::max(0.01f, hmax - hmin);
    if (t < 0.15f) { float p = t / 0.15f; return 0xff000000 | (20u<<16) | (uint8_t(60+p*40)<<8) | uint8_t(100+p*40); }
    else if (t < 0.30f) { float p = (t-0.15f)/0.15f; return 0xff000000 | (30u<<16) | (uint8_t(100+p*60)<<8) | uint8_t(140+p*40); }
    else if (t < 0.45f) { float p = (t-0.30f)/0.15f; return 0xff000000 | (uint8_t(180+p*40)<<16) | (uint8_t(160+p*30)<<8) | uint8_t(80+p*20); }
    else if (t < 0.70f) { float p = (t-0.45f)/0.25f; return 0xff000000 | (uint8_t(60+p*40)<<16) | (uint8_t(140+p*60)<<8) | uint8_t(30+p*20); }
    else { float p = (t-0.70f)/0.30f; uint8_t c = uint8_t(100+p*100); return 0xff000000 | (c<<16) | (c<<8) | c; }
}

bgfx::TextureHandle TerrainRenderer::LoadTileTexture(const std::string& name, int index) {
    (void)index;
    std::string paths[] = {
        "assets/textures/" + name,
        "assets_converted/mod_objs/" + name,
    };
    for (auto& p : paths) {
        int w, h, n;
        unsigned char* d = stbi_load(p.c_str(), &w, &h, &n, 4);
        if (d) {
            auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_NONE,
                bgfx::copy(d, w * h * 4));
            stbi_image_free(d);
            spdlog::info("Terrain: loaded texture {} ({}x{})", p, w, h);
            return tex;
        }
    }
    return BGFX_INVALID_HANDLE;
}

bool TerrainRenderer::LoadFromHGT(const std::string& hgt_path, float world_scale) {
    std::ifstream f(hgt_path);
    if (!f) { spdlog::error("Terrain: cannot open {}", hgt_path); return false; }
    int w, h; f >> w >> h;
    if (w <= 0 || h > 4096) return false;
    hgt_data_.resize(w * h); hgt_min_ = 1e9f; hgt_max_ = -1e9f;
    for (int i = 0; i < w * h; i++) { f >> hgt_data_[i]; hgt_min_ = std::min(hgt_min_, hgt_data_[i]); hgt_max_ = std::max(hgt_max_, hgt_data_[i]); }
    hgt_width_ = w; hgt_height_ = h; hgt_scale_ = world_scale; use_hgt_ = true;
    vertices_per_side_high_ = std::min(w, 256);
    vertices_per_side_med_ = vertices_per_side_high_ / 2;
    vertices_per_side_low_ = vertices_per_side_high_ / 4;
    size_ = static_cast<int>(w * world_scale);
    height_scale_ = (hgt_max_ - hgt_min_) * world_scale;
    
    // Try to load a tile texture as base
    tile_tex_ = LoadTileTexture("01_farm_ground_lv1.png", 0);
    if (!bgfx::isValid(tile_tex_)) {
        tile_tex_ = LoadTileTexture("19_ground_e0_01.png", 0);
    }
    use_tile_texture_ = bgfx::isValid(tile_tex_);
    
    BuildMesh();
    spdlog::info("Terrain: loaded {} ({}x{}, {} verts, range [{:.1f},{:.1f}]){}",
                 hgt_path, w, h, verts_.size(), hgt_min_, hgt_max_,
                 use_tile_texture_ ? " +texture" : "");
    return true;
}

glm::vec3 TerrainRenderer::CalculateNormal(int ix, int iz) const {
    return glm::vec3(0, 1, 0);
}

void TerrainRenderer::BuildMeshForLOD(int vps, std::vector<Vertex>& verts, std::vector<uint16_t>& idx) {
    verts.clear(); idx.clear();
    float step_x = use_hgt_ ? static_cast<float>(hgt_width_) / vps : static_cast<float>(size_) / vps;
    float step_z = use_hgt_ ? static_cast<float>(hgt_height_) / vps : static_cast<float>(size_) / vps;
    float half = size_ / 2.0f;
    
    for (int iz = 0; iz <= vps; iz++) {
        for (int ix = 0; ix <= vps; ix++) {
            float wx, wz, h;
            if (use_hgt_) {
                int hx = std::min(static_cast<int>(ix * step_x), hgt_width_ - 1);
                int hz = std::min(static_cast<int>(iz * step_z), hgt_height_ - 1);
                wx = (ix * step_x - hgt_width_ * 0.5f) * hgt_scale_;
                wz = (iz * step_z - hgt_height_ * 0.5f) * hgt_scale_;
                h = hgt_data_[hz * hgt_width_ + hx] * hgt_scale_;
            } else {
                wx = -half + ix * step_x;
                wz = -half + iz * step_z;
                h = GetHeight(wx, wz);
            }
            float u = static_cast<float>(ix) / vps;
            float v = static_cast<float>(iz) / vps;
            
            float hL = GetHeight(wx - step_x, wz);
            float hR = GetHeight(wx + step_x, wz);
            float hD = GetHeight(wx, wz - step_z);
            float hU = GetHeight(wx, wz + step_z);
            glm::vec3 n = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            
            float hmin_world = use_hgt_ ? hgt_min_ * hgt_scale_ : -height_scale_;
            float hmax_world = use_hgt_ ? hgt_max_ * hgt_scale_ : height_scale_;
            verts.push_back({wx, h, wz, n.x, n.y, n.z,
                HeightColor(h, hmin_world, hmax_world), u, v});
        }
    }
    
    for (int iz = 0; iz < vps; iz++) {
        for (int ix = 0; ix < vps; ix++) {
            int a = iz * (vps + 1) + ix;
            int b = a + 1, c = (iz + 1) * (vps + 1) + ix, d = c + 1;
            idx.push_back(a); idx.push_back(c); idx.push_back(b);
            idx.push_back(b); idx.push_back(c); idx.push_back(d);
        }
    }
}

void TerrainRenderer::BuildLODs() {
    std::vector<Vertex> verts;
    std::vector<uint16_t> idx;
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    
    BuildMeshForLOD(vertices_per_side_high_, verts, idx);
    if (!verts.empty()) {
        vb_high_ = bgfx::createVertexBuffer(bgfx::copy(verts.data(), static_cast<uint32_t>(verts.size() * sizeof(Vertex))), layout);
        ib_high_ = bgfx::createIndexBuffer(bgfx::copy(idx.data(), static_cast<uint32_t>(idx.size() * sizeof(uint16_t))));
    }
    
    BuildMeshForLOD(vertices_per_side_med_, verts, idx);
    if (!verts.empty()) {
        vb_med_ = bgfx::createVertexBuffer(bgfx::copy(verts.data(), static_cast<uint32_t>(verts.size() * sizeof(Vertex))), layout);
        ib_med_ = bgfx::createIndexBuffer(bgfx::copy(idx.data(), static_cast<uint32_t>(idx.size() * sizeof(uint16_t))));
    }
    
    BuildMeshForLOD(vertices_per_side_low_, verts, idx);
    if (!verts.empty()) {
        vb_low_ = bgfx::createVertexBuffer(bgfx::copy(verts.data(), static_cast<uint32_t>(verts.size() * sizeof(Vertex))), layout);
        ib_low_ = bgfx::createIndexBuffer(bgfx::copy(idx.data(), static_cast<uint32_t>(idx.size() * sizeof(uint16_t))));
    }
    
    lods_built_ = true;
}

void TerrainRenderer::SelectLOD(float cam_dist) {
    int new_lod = (cam_dist < 200.0f) ? 0 : (cam_dist < 500.0f ? 1 : 2);
    if (new_lod != current_lod_) {
        current_lod_ = new_lod;
        if (current_lod_ == 0) { vb_ = vb_high_; ib_ = ib_high_; }
        else if (current_lod_ == 1) { vb_ = vb_med_; ib_ = ib_med_; }
        else { vb_ = vb_low_; ib_ = ib_low_; }
    }
}

void TerrainRenderer::BuildMesh() {
    BuildLODs();
    BuildMeshForLOD(vertices_per_side_high_, verts_, idx_);
    vb_ = vb_high_; ib_ = ib_high_;
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj, const EnvData& env) {
    glm::mat4 invView = glm::inverse(view);
    glm::vec3 camPos(invView[3]);
    float camDist = glm::length(camPos);
    SelectLOD(camDist);
    
    bgfx::ProgramHandle prog = bgfx::isValid(terrain_program_) ? terrain_program_ : program_;
    if (!bgfx::isValid(prog) || !bgfx::isValid(vb_)) return;
    
    bgfx::setViewTransform(view_id_, &view, &proj);
    bgfx::setViewClear(view_id_, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    bgfx::setViewRect(view_id_, 0, 0, (uint16_t)width, (uint16_t)height);
    
    bgfx::setUniform(u_light_dir_, glm::value_ptr(env.light_dir));
    bgfx::setUniform(u_fog_data_,  glm::value_ptr(env.fog_data));
    bgfx::setUniform(u_fog_color_, glm::value_ptr(env.fog_color));

    bgfx::setTexture(0, s_tex_color_, bgfx::isValid(tile_tex_) ? tile_tex_ : white_tex_);
    bgfx::setTexture(1, s_tex_grass_, bgfx::isValid(grass_tex_) ? grass_tex_ : white_tex_);
    bgfx::setTexture(2, s_tex_rock_,  bgfx::isValid(rock_tex_) ? rock_tex_ : white_tex_);
    bgfx::setTexture(3, s_tex_dirt_,  bgfx::isValid(dirt_tex_) ? dirt_tex_ : white_tex_);

    bgfx::setVertexBuffer(0, vb_);
    bgfx::setIndexBuffer(ib_);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
    bgfx::submit(view_id_, prog);
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj, bgfx::TextureHandle shadow_map, const glm::mat4& shadow_mvp) {
    Render(view, proj);
}

void TerrainRenderer::RenderShadow(const glm::mat4& light_mvp) {
    if (!bgfx::isValid(shadow_program_) || !bgfx::isValid(vb_)) return;
    float identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    bgfx::setTransform(identity);
    bgfx::setUniform(u_shadow_mvp_, glm::value_ptr(light_mvp));
    bgfx::setVertexBuffer(0, vb_);
    bgfx::setIndexBuffer(ib_);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
    bgfx::submit(static_cast<bgfx::ViewId>(ViewId::Clear), shadow_program_);
}

void TerrainRenderer::Init(int size, float height_scale) {
    size_ = size; height_scale_ = height_scale;
    auto vs = loadShader("shaders/vs_terrain.bin");
    auto fs = loadShader("shaders/fs_terrain.bin");
    if (vs && fs) terrain_program_ = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    
    auto vs_s = loadShader("shaders/vs_default.bin");
    auto fs_s = loadShader("shaders/fs_unlit.bin");
    if (vs_s && fs_s) shadow_program_ = bgfx::createProgram(bgfx::createShader(vs_s), bgfx::createShader(fs_s), true);

    s_tex_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_tex_grass_ = bgfx::createUniform("s_texGrass", bgfx::UniformType::Sampler);
    s_tex_rock_  = bgfx::createUniform("s_texRock",  bgfx::UniformType::Sampler);
    s_tex_dirt_  = bgfx::createUniform("s_texDirt",  bgfx::UniformType::Sampler);
    u_shadow_map_ = bgfx::createUniform("u_shadowMap", bgfx::UniformType::Sampler);
    u_shadow_mvp_ = bgfx::createUniform("u_shadowMVP", bgfx::UniformType::Mat4);
    u_light_dir_ = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    u_fog_data_  = bgfx::createUniform("u_fogData",  bgfx::UniformType::Vec4);
    u_fog_color_ = bgfx::createUniform("u_fogColor", bgfx::UniformType::Vec4);
    
    uint32_t white = 0xffffffff;
    white_tex_ = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, sizeof(white)));
    grass_tex_ = LoadTileTexture("01_farm_ground_lv1.png", 0);
    rock_tex_  = LoadTileTexture("19_ground_e0_01.png", 1);
    // 15_ground_c0_01.png not found on disk; fall back to first texture
    dirt_tex_  = LoadTileTexture("01_farm_ground_lv1.png", 2);
    BuildMesh();
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj) {
    EnvData defaultEnv; Render(view, proj, defaultEnv);
}

void TerrainRenderer::Shutdown() {
    static bool is_shutdown = false;
    if (is_shutdown) return;
    is_shutdown = true;

    if (bgfx::isValid(vb_high_)) bgfx::destroy(vb_high_);
    if (bgfx::isValid(ib_high_)) bgfx::destroy(ib_high_);
    if (bgfx::isValid(vb_med_)) bgfx::destroy(vb_med_);
    if (bgfx::isValid(ib_med_)) bgfx::destroy(ib_med_);
    if (bgfx::isValid(vb_low_)) bgfx::destroy(vb_low_);
    if (bgfx::isValid(ib_low_)) bgfx::destroy(ib_low_);
    if (bgfx::isValid(tile_tex_)) bgfx::destroy(tile_tex_);
    if (bgfx::isValid(grass_tex_)) bgfx::destroy(grass_tex_);
    if (bgfx::isValid(rock_tex_)) bgfx::destroy(rock_tex_);
    if (bgfx::isValid(dirt_tex_)) bgfx::destroy(dirt_tex_);
    if (bgfx::isValid(white_tex_)) bgfx::destroy(white_tex_);
    if (bgfx::isValid(u_shadow_map_)) bgfx::destroy(u_shadow_map_);
    if (bgfx::isValid(u_shadow_mvp_)) bgfx::destroy(u_shadow_mvp_);
    if (bgfx::isValid(terrain_program_)) bgfx::destroy(terrain_program_);
    if (bgfx::isValid(shadow_program_)) bgfx::destroy(shadow_program_);
    if (bgfx::isValid(s_tex_color_)) bgfx::destroy(s_tex_color_);
    if (bgfx::isValid(u_light_dir_)) bgfx::destroy(u_light_dir_);
}
