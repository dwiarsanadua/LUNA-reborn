#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <vector>

struct PosColorVertex {
    float x, y, z;
    uint32_t color;
};

class WorldRenderer {
public:
    WorldRenderer();
    ~WorldRenderer();

    void Init();
    void Render(const glm::mat4& view, const glm::mat4& proj);
    void Shutdown();

    void SetPlayerPos(const glm::vec3& pos);

private:
    void CreateGroundGrid(int size);
    void CreatePlayerCube();
    void CreateWaterPlane(int size);

    bgfx::VertexBufferHandle ground_vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ground_ib_ = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle player_vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle player_ib_ = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle water_vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle water_ib_ = BGFX_INVALID_HANDLE;

    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle water_program_ = BGFX_INVALID_HANDLE;

    bgfx::UniformHandle u_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_time_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = 1;

    glm::vec3 player_pos_{0, 0, 0};
    int grid_size_ = 20;
};
