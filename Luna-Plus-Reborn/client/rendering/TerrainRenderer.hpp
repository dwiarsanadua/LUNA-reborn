#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <engine/gx_render/RenderDevice.h>

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

    float width = 1280.0f;
    float height = 720.0f;

private:
    void BuildMesh();
    float Noise(float x, float z) const;
    float FractalNoise(float x, float z, int octaves = 4) const;
    bgfx::TextureHandle LoadTileTexture(const std::string& name, int index);

    bgfx::VertexBufferHandle vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_ = BGFX_INVALID_HANDLE;
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
    int vertices_per_side_ = 64;
    int vertices_per_side_high_ = 128;
    int vertices_per_side_med_ = 64;
    int vertices_per_side_low_ = 32;
    int current_lod_ = 1; // 0=high, 1=med, 2=low
    
    // LOD buffers
    bgfx::VertexBufferHandle vb_high_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_high_ = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vb_med_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_med_ = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vb_low_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_low_ = BGFX_INVALID_HANDLE;
    bool lods_built_ = false;
    
    struct Vertex { float x, y, z; float nx, ny, nz; uint32_t color; float u, v; };
    
    void BuildLODs();
    void BuildMeshForLOD(int vps, std::vector<Vertex>& verts, std::vector<uint16_t>& idx);
    void SelectLOD(float cam_dist);

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
