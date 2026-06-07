#define GLFW_EXPOSE_NATIVE_COCOA
#include "RenderDevice.h"
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
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
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

    int fb_width, fb_height;
    glfwGetFramebufferSize(window_, &fb_width, &fb_height);
    width_ = fb_width;
    height_ = fb_height;
    
    int win_w, win_h;
    glfwGetWindowSize(window_, &win_w, &win_h);
    logical_width_ = win_w;
    logical_height_ = win_h;

    bgfx::PlatformData pd{};
    pd.nwh = glfwGetCocoaWindow(window_);

    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Metal; 
    bgfx_init.resolution.width = (uint32_t)width_;
    bgfx_init.resolution.height = (uint32_t)height_;
    bgfx_init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI | BGFX_RESET_FLUSH_AFTER_RENDER;
    bgfx_init.platformData = pd;

    if (!bgfx::init(bgfx_init)) {
        spdlog::error("RenderDevice: failed to initialize bgfx");
        return false;
    }

    // Set debug text and clear color globally
    bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);
    
    // Clear View 0 (Main) to dark gray
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xFF333333, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)width_, (uint16_t)height_);

    spdlog::info("RenderDevice: initialized. FB Size: {}x{}", width_, height_);
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
    
    // Ensure viewport and clear are set every frame
    bgfx::setViewRect(0, 0, 0, (uint16_t)width_, (uint16_t)height_);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xFF333333, 1.0f, 0);
    
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(1, 1, 0x0f, "LUNA Plus Reborn - BGFX ACTIVE");
    bgfx::dbgTextPrintf(1, 2, 0x0f, "Resolution: %dx%d", width_, height_);
    
    bgfx::touch(0);
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
