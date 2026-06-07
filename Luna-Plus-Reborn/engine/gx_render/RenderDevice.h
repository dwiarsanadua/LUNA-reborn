#pragma once
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>

struct EnvData {
    glm::vec4 light_dir{0.5f, -0.8f, 0.3f, 0.0f};
    glm::vec4 fog_data{50.0f, 200.0f, 0.8f, 0.0f};
    glm::vec4 fog_color{0.1f, 0.1f, 0.18f, 1.0f};
};

struct RenderDeviceConfig {
    int width = 800;
    int height = 600;
    bool vsync = true;
    bool fullscreen = false;
    const char* title = "LUNA Online Plus";
};

enum class ViewId : bgfx::ViewId {
    Clear     = 0,
    Terrain   = 1,
    Props     = 2,
    Character = 3,
    Particle  = 4,
    UI        = 5,
    Debug     = 6,
    Count     = 7
};

class RenderDevice {
public:
    bool Init(const RenderDeviceConfig& config);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void SetClearColor(uint32_t color);
    void Clear(uint32_t buffers = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);

    void SetViewport(int x, int y, int w, int h);
    void ResetViewport();

    GLFWwindow* GetWindow() const { return window_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    int GetLogicalWidth() const { return logical_width_; }
    int GetLogicalHeight() const { return logical_height_; }
    bool ShouldClose() const { return glfwWindowShouldClose(window_); }

    void SetState(uint64_t state, uint32_t rgba = 0);
    void SetStencil(uint32_t stencil);

    void SetViewMatrix(const float* view);
    void SetProjMatrix(const float* proj);
    void SetTransform(const float* model);

    void SetUniform(bgfx::UniformHandle handle, const void* value);
    void SetTexture(uint8_t stage, bgfx::UniformHandle sampler,
                    bgfx::TextureHandle texture, uint32_t flags = 0);

private:
    GLFWwindow* window_ = nullptr;
    int width_ = 800, height_ = 600;
    int logical_width_ = 800, logical_height_ = 600;
    bgfx::ViewId main_view_ = static_cast<bgfx::ViewId>(ViewId::Clear);
    uint32_t clear_color_ = 0x000000FF;
};
