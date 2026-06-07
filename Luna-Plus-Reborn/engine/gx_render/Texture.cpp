#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <cstring>

Texture::Texture() = default;

Texture::~Texture() {
    if (bgfx::isValid(handle_)) {
        bgfx::destroy(handle_);
    }
}

bool Texture::LoadFromFile(const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        spdlog::error("Texture: failed to load '{}': {}", path, stbi_failure_reason());
        return false;
    }

    width_ = w;
    height_ = h;

    const bgfx::Memory* mem = bgfx::copy(data, w * h * 4);
    stbi_image_free(data);

    handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
        bgfx::TextureFormat::RGBA8, 0, mem);

    if (!bgfx::isValid(handle_)) {
        spdlog::error("Texture: failed to create GPU texture from '{}'", path);
        return false;
    }

    spdlog::debug("Texture: loaded '{}' ({}x{})", path, w, h);
    return true;
}

bool Texture::LoadFromMemory(const void* data, size_t size) {
    int w, h, channels;
    unsigned char* img = stbi_load_from_memory(
        static_cast<const unsigned char*>(data), (int)size,
        &w, &h, &channels, 4);
    if (!img) {
        spdlog::error("Texture: failed to decode image from memory");
        return false;
    }

    width_ = w;
    height_ = h;

    const bgfx::Memory* mem = bgfx::copy(img, w * h * 4);
    stbi_image_free(img);

    handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
        bgfx::TextureFormat::RGBA8, 0, mem);
    return bgfx::isValid(handle_);
}

bool Texture::CreateRenderTarget(int width, int height) {
    width_ = width;
    height_ = height;

    handle_ = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1,
        bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    if (!bgfx::isValid(handle_)) {
        spdlog::error("Texture: failed to create render target ({}x{})", width, height);
        return false;
    }

    spdlog::debug("Texture: created render target ({}x{})", width, height);
    return true;
}

void Texture::Bind(uint8_t stage) const {
    if (bgfx::isValid(handle_)) {
        bgfx::setTexture(stage, BGFX_INVALID_HANDLE, handle_);
    }
}

void Texture::Unbind() const {}
