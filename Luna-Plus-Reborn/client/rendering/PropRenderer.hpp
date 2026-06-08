#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <engine/gx_render/RenderDevice.h>

struct PropMesh {
    bgfx::VertexBufferHandle vb = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib = BGFX_INVALID_HANDLE;
    uint32_t num_indices = 0;
    bgfx::TextureHandle tex = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle normal_tex = BGFX_INVALID_HANDLE;
    std::string name;
};

struct PropInstance {
    glm::vec3 position{0};
    glm::vec3 rotation{0};
    float scale = 1.0f;
    uint32_t mesh_idx = 0;
};

class PropRenderer {
public:
    PropRenderer();
    ~PropRenderer();

    bool Init();
    int LoadObj(const std::string& path);
    int AddInstance(int mesh_idx, glm::vec3 pos, float scale = 1.0f, glm::vec3 rot = {0,0,0});
    void Render(const glm::mat4& view, const glm::mat4& proj);
    void Render(const glm::mat4& view, const glm::mat4& proj, const EnvData& env);
    void RenderShadow(bgfx::ViewId view_id, const glm::mat4& shadow_mvp);
    void Shutdown();
    void ClearProps();

    int GetDrawCallCount() const { return last_draw_calls_; }
    int GetInstanceCount() const { return (int)instances_.size(); }

    float width = 1280.0f;
    float height = 720.0f;

private:
    struct Vertex { float x, y, z; uint32_t color; float u, v; };
    bgfx::TextureHandle LoadTextureForMesh(const std::string& base_name);
    bgfx::TextureHandle LoadNormalMap(const std::string& base_name);

    std::vector<PropMesh> meshes_;
    std::vector<PropInstance> instances_;
    std::unordered_map<std::string, bgfx::TextureHandle> tex_cache_;
    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle shadow_program_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_light_dir_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_color_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle s_tex_normal_ = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_shadow_mvp_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle white_tex_ = BGFX_INVALID_HANDLE;
    bgfx::ViewId view_id_ = static_cast<bgfx::ViewId>(ViewId::Props);
    bgfx::VertexLayout layout_;
    int last_draw_calls_ = 0;
};
