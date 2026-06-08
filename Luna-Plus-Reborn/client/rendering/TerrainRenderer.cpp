#include "TerrainRenderer.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
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

bgfx::VertexLayout& TerrainRenderer::GetLayout() {
    static bgfx::VertexLayout layout;
    static bool init = false;
    if (!init) {
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        init = true;
    }
    return layout;
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
        "assets/textures/unpacked/map/" + name,
        "assets/textures/unpacked/farm/" + name,
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
    if (w <= 0 || h > 65536 || w * h > 50000000) return false;
    hgt_data_.resize(w * h); hgt_min_ = 1e9f; hgt_max_ = -1e9f;
    for (int i = 0; i < w * h; i++) { f >> hgt_data_[i]; hgt_min_ = std::min(hgt_min_, hgt_data_[i]); hgt_max_ = std::max(hgt_max_, hgt_data_[i]); }
    hgt_width_ = w; hgt_height_ = h; hgt_scale_ = world_scale; use_hgt_ = true;
    size_ = static_cast<int>(w * world_scale);
    height_scale_ = (hgt_max_ - hgt_min_) * world_scale;

    tile_tex_ = LoadTileTexture("01_farm_ground_lv1.png", 0);
    if (!bgfx::isValid(tile_tex_)) {
        tile_tex_ = LoadTileTexture("19_ground_e0_01.png", 0);
    }
    use_tile_texture_ = bgfx::isValid(tile_tex_);

    BuildPatches();
    spdlog::info("Terrain: loaded {} ({}x{}, {} patches, range [{:.1f},{:.1f}]){}",
                 hgt_path, w, h, patches_.size(), hgt_min_, hgt_max_,
                 use_tile_texture_ ? " +texture" : "");
    return true;
}

glm::vec3 TerrainRenderer::CalculateNormal(int ix, int iz) const {
    return glm::vec3(0, 1, 0);
}

void TerrainRenderer::BuildPatchMesh(TerrainPatch& patch, int patch_x, int patch_z, int vps, bool high_detail) {
    std::vector<Vertex> verts;
    std::vector<uint16_t> idx;

    float world_min_x = -size_ / 2.0f + patch_x * (float)size_ / patches_per_side_;
    float world_min_z = -size_ / 2.0f + patch_z * (float)size_ / patches_per_side_;
    float patch_size = (float)size_ / patches_per_side_;

    float step = patch_size / vps;

    for (int iz = 0; iz <= vps; iz++) {
        for (int ix = 0; ix <= vps; ix++) {
            float wx = world_min_x + ix * step;
            float wz = world_min_z + iz * step;
            float h = GetHeight(wx, wz);

            float hL = GetHeight(wx - step, wz);
            float hR = GetHeight(wx + step, wz);
            float hD = GetHeight(wx, wz - step);
            float hU = GetHeight(wx, wz + step);
            glm::vec3 n = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));

            float u = (float)ix / vps;
            float v = (float)iz / vps;

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

    auto& layout = GetLayout();

    if (high_detail) {
        if (bgfx::isValid(patch.vb_high)) bgfx::destroy(patch.vb_high);
        if (bgfx::isValid(patch.ib_high)) bgfx::destroy(patch.ib_high);
        patch.vb_high = bgfx::createVertexBuffer(bgfx::copy(verts.data(), (uint32_t)(verts.size() * sizeof(Vertex))), layout);
        patch.ib_high = bgfx::createIndexBuffer(bgfx::copy(idx.data(), (uint32_t)(idx.size() * sizeof(uint16_t))));
        patch.num_indices_high = (int)idx.size();
    } else {
        if (bgfx::isValid(patch.vb_low)) bgfx::destroy(patch.vb_low);
        if (bgfx::isValid(patch.ib_low)) bgfx::destroy(patch.ib_low);
        patch.vb_low = bgfx::createVertexBuffer(bgfx::copy(verts.data(), (uint32_t)(verts.size() * sizeof(Vertex))), layout);
        patch.ib_low = bgfx::createIndexBuffer(bgfx::copy(idx.data(), (uint32_t)(idx.size() * sizeof(uint16_t))));
        patch.num_indices_low = (int)idx.size();
    }
}

void TerrainRenderer::BuildPatches() {
    patches_.clear();
    patches_.reserve(patches_per_side_ * patches_per_side_);

    for (int pz = 0; pz < patches_per_side_; pz++) {
        for (int px = 0; px < patches_per_side_; px++) {
            TerrainPatch patch;
            float world_min_x = -size_ / 2.0f + px * (float)size_ / patches_per_side_;
            float world_min_z = -size_ / 2.0f + pz * (float)size_ / patches_per_side_;
            float patch_size = (float)size_ / patches_per_side_;

            patch.min_x = world_min_x;
            patch.min_z = world_min_z;
            patch.max_x = world_min_x + patch_size;
            patch.max_z = world_min_z + patch_size;
            patch.center_x = (patch.min_x + patch.max_x) * 0.5f;
            patch.center_z = (patch.min_z + patch.max_z) * 0.5f;

            BuildPatchMesh(patch, px, pz, verts_per_patch_high_, true);
            BuildPatchMesh(patch, px, pz, verts_per_patch_med_, false);

            patches_.push_back(patch);
        }
    }

    spdlog::info("Terrain: built {} patches ({}x{} grid)", patches_.size(), patches_per_side_, patches_per_side_);
}

void TerrainRenderer::ExtractFrustumPlanes(const glm::mat4& vp, glm::vec4* planes) const {
    // Left
    planes[0] = glm::vec4(
        vp[0][3] + vp[0][0],
        vp[1][3] + vp[1][0],
        vp[2][3] + vp[2][0],
        vp[3][3] + vp[3][0]);
    // Right
    planes[1] = glm::vec4(
        vp[0][3] - vp[0][0],
        vp[1][3] - vp[1][0],
        vp[2][3] - vp[2][0],
        vp[3][3] - vp[3][0]);
    // Bottom
    planes[2] = glm::vec4(
        vp[0][3] + vp[0][1],
        vp[1][3] + vp[1][1],
        vp[2][3] + vp[2][1],
        vp[3][3] + vp[3][1]);
    // Top
    planes[3] = glm::vec4(
        vp[0][3] - vp[0][1],
        vp[1][3] - vp[1][1],
        vp[2][3] - vp[2][1],
        vp[3][3] - vp[3][1]);
    // Near
    planes[4] = glm::vec4(
        vp[0][3] + vp[0][2],
        vp[1][3] + vp[1][2],
        vp[2][3] + vp[2][2],
        vp[3][3] + vp[3][2]);
    // Far
    planes[5] = glm::vec4(
        vp[0][3] - vp[0][2],
        vp[1][3] - vp[1][2],
        vp[2][3] - vp[2][2],
        vp[3][3] - vp[3][2]);

    for (int i = 0; i < 6; i++) {
        float len = glm::length(glm::vec3(planes[i]));
        if (len > 0.0001f) planes[i] /= len;
    }
}

bool TerrainRenderer::IsBoxVisible(const glm::vec4* planes, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) const {
    for (int i = 0; i < 6; i++) {
        const auto& p = planes[i];
        // Check if box is entirely outside this plane
        float d = std::max(min_x * p.x, max_x * p.x)
                + std::max(min_y * p.y, max_y * p.y)
                + std::max(min_z * p.z, max_z * p.z)
                + p.w;
        if (d < 0) return false;
    }
    return true;
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj, const EnvData& env) {
    bgfx::ProgramHandle prog = bgfx::isValid(terrain_program_) ? terrain_program_ : program_;
    if (!bgfx::isValid(prog)) return;

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

    // Extract frustum planes from combined VP matrix
    glm::mat4 vp = proj * view;
    glm::vec4 frustum[6];
    ExtractFrustumPlanes(vp, frustum);

    // Camera position
    glm::mat4 invView = glm::inverse(view);
    glm::vec3 camPos(invView[3]);

    visible_patches_ = 0;

    for (auto& patch : patches_) {
        // Frustum cull
        float min_y = -50.0f, max_y = 50.0f;
        patch.visible = IsBoxVisible(frustum,
            patch.min_x, min_y, patch.min_z,
            patch.max_x, max_y, patch.max_z);
        if (!patch.visible) continue;

        // Distance-based LOD
        float dx = patch.center_x - camPos.x;
        float dz = patch.center_z - camPos.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        bgfx::VertexBufferHandle vb = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle ib = BGFX_INVALID_HANDLE;

        if (dist < 200.0f) {
            vb = patch.vb_high;
            ib = patch.ib_high;
        } else if (dist < 500.0f) {
            vb = patch.vb_med;
            ib = patch.ib_med;
        } else {
            vb = patch.vb_low;
            ib = patch.ib_low;
        }

        if (!bgfx::isValid(vb)) continue;

        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, vb);
        bgfx::setIndexBuffer(ib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(view_id_, prog);
        visible_patches_++;
    }
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj, bgfx::TextureHandle shadow_map, const glm::mat4& shadow_mvp) {
    (void)shadow_map;
    (void)shadow_mvp;
    EnvData env;
    Render(view, proj, env);
}

void TerrainRenderer::RenderShadow(const glm::mat4& light_mvp) {
    if (!bgfx::isValid(shadow_program_)) return;

    for (auto& patch : patches_) {
        if (!patch.visible) continue;
        bgfx::VertexBufferHandle vb = patch.vb_high;
        bgfx::IndexBufferHandle ib = patch.ib_high;
        if (!bgfx::isValid(vb)) continue;

        float identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        bgfx::setTransform(identity);
        bgfx::setUniform(u_shadow_mvp_, glm::value_ptr(light_mvp));
        bgfx::setVertexBuffer(0, vb);
        bgfx::setIndexBuffer(ib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(static_cast<bgfx::ViewId>(ViewId::Clear), shadow_program_);
    }
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
    dirt_tex_  = LoadTileTexture("01_farm_ground_lv1.png", 2);
    BuildPatches();
}

void TerrainRenderer::Render(const glm::mat4& view, const glm::mat4& proj) {
    EnvData defaultEnv; Render(view, proj, defaultEnv);
}

void TerrainRenderer::Shutdown() {
    static bool is_shutdown = false;
    if (is_shutdown) return;
    is_shutdown = true;

    for (auto& patch : patches_) {
        if (bgfx::isValid(patch.vb_high)) bgfx::destroy(patch.vb_high);
        if (bgfx::isValid(patch.ib_high)) bgfx::destroy(patch.ib_high);
        if (bgfx::isValid(patch.vb_med)) bgfx::destroy(patch.vb_med);
        if (bgfx::isValid(patch.ib_med)) bgfx::destroy(patch.ib_med);
        if (bgfx::isValid(patch.vb_low)) bgfx::destroy(patch.vb_low);
        if (bgfx::isValid(patch.ib_low)) bgfx::destroy(patch.ib_low);
    }
    patches_.clear();
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
