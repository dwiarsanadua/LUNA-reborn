#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include "RenderDevice.h"
#include "VFS.h"
#include <GLFW/glfw3native.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>

bool RenderDevice::Init(const RenderDeviceConfig& config) {
    if (!glfwInit()) {
        spdlog::error("RenderDevice: failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    
    window_ = glfwCreateWindow(config.width, config.height, config.title,
                                config.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
                                nullptr);
    if (!window_) {
        spdlog::error("RenderDevice: failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    // Force window to front and give OS time to realize it
    glfwShowWindow(window_);
    glfwFocusWindow(window_);
    for(int i=0; i<10; i++) { glfwPollEvents(); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

    VFS::InitFromExecutable();

    int fb_width, fb_height;
    glfwGetFramebufferSize(window_, &fb_width, &fb_height);
    width_ = fb_width;
    height_ = fb_height;
    
    int win_w, win_h;
    glfwGetWindowSize(window_, &win_w, &win_h);
    logical_width_ = win_w;
    logical_height_ = win_h;

    bgfx::PlatformData pd{};
#if defined(_WIN32)
    pd.nwh = glfwGetWin32Window(window_);
#elif defined(__APPLE__)
    pd.nwh = glfwGetCocoaWindow(window_);
#else
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window_);
#endif

    bgfx::Init bgfx_init;
#if defined(_WIN32)
    bgfx_init.type = bgfx::RendererType::Direct3D11;
#elif defined(__APPLE__)
    bgfx_init.type = bgfx::RendererType::Metal;
#else
    bgfx_init.type = bgfx::RendererType::Count; // auto-detect (Vulkan/OpenGL)
#endif
    bgfx_init.resolution.width = (uint32_t)width_;
    bgfx_init.resolution.height = (uint32_t)height_;
    bgfx_init.resolution.reset = (config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE) | BGFX_RESET_FLUSH_AFTER_RENDER;
    bgfx_init.platformData = pd;

    if (!bgfx::init(bgfx_init)) {
        spdlog::error("RenderDevice: failed to initialize bgfx");
        return false;
    }

    // Force internal resolution update
    bgfx::reset((uint32_t)width_, (uint32_t)height_, bgfx_init.resolution.reset);

    // Set debug text and clear color globally - Disabled for clean look
    bgfx::setDebug(BGFX_DEBUG_NONE);
    
    // Clear View 0 (Main) to dark gray
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,     0xFF6688AA, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)width_, (uint16_t)height_);

    // Create DX9-style fixed-function uniforms
    u_ambient_ = bgfx::createUniform("u_ambient", bgfx::UniformType::Vec4);
    u_light_dir_ = bgfx::createUniform("u_light_dir", bgfx::UniformType::Vec4);
    u_light_diffuse_ = bgfx::createUniform("u_light_diffuse", bgfx::UniformType::Vec4);
    u_light_specular_ = bgfx::createUniform("u_light_specular", bgfx::UniformType::Vec4);
    u_fog_ = bgfx::createUniform("u_fog", bgfx::UniformType::Vec4);
    u_fog_end_ = bgfx::createUniform("u_fog_end", bgfx::UniformType::Vec4);

    // Set initial lighting values
    SetLightingUniforms();

    spdlog::info("RenderDevice: initialized. FB Size: {}x{}", width_, height_);
    spdlog::info("RenderDevice: bgfx renderer = {}", bgfx::getRendererName(bgfx::getRendererType()));
    return true;
}

void RenderDevice::Shutdown() {
    bgfx::shutdown();
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

void RenderDevice::BeginFrame() {
    glfwPollEvents();
    
    // View 6 (Debug): debug text overlay only — don't clear the screen underneath
    bgfx::ViewId debugView = static_cast<bgfx::ViewId>(ViewId::Debug);
    bgfx::setViewRect(debugView, 0, 0, (uint16_t)width_, (uint16_t)height_);
    bgfx::setViewClear(debugView, BGFX_CLEAR_NONE, 0, 1.0f, 0);
    
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(1, 1, 0x0f, "LUNA Plus Reborn - BGFX ACTIVE");
    bgfx::dbgTextPrintf(1, 2, 0x0f, "Resolution: %dx%d", width_, height_);
    
    SetLightingUniforms();

    bgfx::touch(debugView);
}

void RenderDevice::SetLightingUniforms() {
    float ambient[4]   = {0.2f, 0.2f, 0.3f, 1.0f};
    float light_dir[4] = {0.5f, -0.8f, 0.3f, 0.0f};
    float light_diff[4]= {0.8f, 0.8f, 0.8f, 1.0f};
    float light_spec[4]= {0.6f, 0.6f, 0.6f, 16.0f};
    float fog[4]       = {0.0f, 0.0f, 0.0f, 50.0f};
    float fog_end[4]   = {1.0f/150.0f, 0, 0, 0};
    if (bgfx::isValid(u_ambient_))  bgfx::setUniform(u_ambient_, ambient);
    if (bgfx::isValid(u_light_dir_)) bgfx::setUniform(u_light_dir_, light_dir);
    if (bgfx::isValid(u_light_diffuse_)) bgfx::setUniform(u_light_diffuse_, light_diff);
    if (bgfx::isValid(u_light_specular_)) bgfx::setUniform(u_light_specular_, light_spec);
    if (bgfx::isValid(u_fog_)) bgfx::setUniform(u_fog_, fog);
    if (bgfx::isValid(u_fog_end_)) bgfx::setUniform(u_fog_end_, fog_end);
}

void RenderDevice::EndFrame() {
    bgfx::frame();
}

void RenderDevice::SetClearColor(uint32_t color) {
    clear_color_ = color;
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, color, 1.0f, 0);
}

void RenderDevice::Clear(uint32_t buffers) {
    bgfx::setViewClear(0, buffers, clear_color_, 1.0f, 0);
}

void RenderDevice::SetViewport(int x, int y, int w, int h) {
    bgfx::setViewRect(0, (uint16_t)x, (uint16_t)y, (uint16_t)w, (uint16_t)h);
}

void RenderDevice::ResetViewport() {
    bgfx::setViewRect(0, 0, 0, (uint16_t)width_, (uint16_t)height_);
}

void RenderDevice::SetState(uint64_t state, uint32_t rgba) {
    bgfx::setState(state, rgba);
}

void RenderDevice::SetStencil(uint32_t stencil) {
    bgfx::setStencil(stencil);
}

void RenderDevice::SetViewMatrix(const float* view) {
    bgfx::setViewTransform(0, view, nullptr);
}

void RenderDevice::SetProjMatrix(const float* proj) {
    bgfx::setViewTransform(0, nullptr, proj);
}

void RenderDevice::SetTransform(const float* model) {
    bgfx::setTransform(model);
}

void RenderDevice::SetUniform(bgfx::UniformHandle handle, const void* value) {
    bgfx::setUniform(handle, value);
}

void RenderDevice::SetTexture(uint8_t stage, bgfx::UniformHandle sampler,
                               bgfx::TextureHandle texture, uint32_t flags) {
    bgfx::setTexture(stage, sampler, texture, flags);
}
