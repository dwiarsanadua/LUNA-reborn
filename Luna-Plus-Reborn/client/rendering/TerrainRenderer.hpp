#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <engine/gx_render/RenderDevice.h>

struct TerrainPatch {
    float min_x, min_z, max_x, max_z;
    float center_x, center_z;

    bgfx::VertexBufferHandle vb_high = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_high = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vb_med  = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_med  = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vb_low  = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_low  = BGFX_INVALID_HANDLE;

    int num_indices_high = 0;
    int num_indices_med  = 0;
    int num_indices_low  = 0;

    bool visible = true;
};

class TerrainRenderer {
public:
    TerrainRenderer();
    ~TerrainRenderer();

    void Init(int size = 50, float height_scale = 8.0f);
    bool LoadFromHGT(const std::string& hgt_path, float world_scale = 0.01f);
    void Render(const glm::mat4& view, const glm::mat4& proj);
    void Render(const glm::mat4& view, const glm::mat4& proj, const EnvData& env);
    void Render(const glm::mat4& view, const glm::mat4& proj, bgfx::TextureHandle shadow_map, const glm::mat4& shadow_mvp);
    void RenderShadow(const glm::mat4& light_mvp);
    void Shutdown();

    float GetHeight(float x, float z) const;
    int GetVisiblePatchCount() const { return visible_patches_; }
    int GetTotalPatchCount() const { return (int)patches_.size(); }

    float width = 1280.0f;
    float height = 720.0f;

private:
    void BuildPatches();
    void BuildPatchMesh(TerrainPatch& patch, int patch_x, int patch_z, int vps, bool high_detail);
    float Noise(float x, float z) const;
    float FractalNoise(float x, float z, int octaves = 4) const;
    bgfx::TextureHandle LoadTileTexture(const std::string& name, int index);
    void ExtractFrustumPlanes(const glm::mat4& vp, glm::vec4* planes) const;
    bool IsBoxVisible(const glm::vec4* planes, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) const;

    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle terrain_program_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle shadow_program_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_light_dir_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_fog_data_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_fog_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_shadow_map_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_shadow_mvp_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle tile_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle grass_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle rock_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle dirt_tex_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_grass_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_rock_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_dirt_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::Terrain);

    int size_ = 50;
    float height_scale_ = 8.0f;
    int patches_per_side_ = 4;
    int verts_per_patch_high_ = 32;
    int verts_per_patch_med_ = 16;
    int verts_per_patch_low_ = 8;

    std::vector<TerrainPatch> patches_;
    int visible_patches_ = 0;

    struct Vertex { float x, y, z; float nx, ny, nz; uint32_t color; float u, v; };
    static constexpr bgfx::VertexLayout& GetLayout();

    std::vector<float> hgt_data_;
    int hgt_width_ = 0, hgt_height_ = 0;
    float hgt_min_ = 0, hgt_max_ = 0;
    float hgt_scale_ = 1.0f;
    bool use_hgt_ = false;
    bool use_tile_texture_ = false;

    glm::vec3 CalculateNormal(int ix, int iz) const;
    std::vector<Vertex> verts_;
    std::vector<uint16_t> idx_;
};
