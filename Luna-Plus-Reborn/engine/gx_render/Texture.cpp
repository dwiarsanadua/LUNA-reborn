#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include <cmath>
#include <algorithm>

Texture::Texture() = default;

Texture::~Texture() {
    if (bgfx::isValid(handle_)) {
        bgfx::destroy(handle_);
    }
}

void Texture::DownsampleLevel(const uint8_t* src, int src_w, int src_h,
                               uint8_t* dst, int dst_w, int dst_h) {
    for (int y = 0; y < dst_h; ++y) {
        for (int x = 0; x < dst_w; ++x) {
            int sx = x * 2, sy = y * 2;
            int r = 0, g = 0, b = 0, a = 0, count = 0;
            for (int dy = 0; dy < 2 && sy + dy < src_h; ++dy) {
                for (int dx = 0; dx < 2 && sx + dx < src_w; ++dx) {
                    int idx = ((sy + dy) * src_w + (sx + dx)) * 4;
                    r += src[idx + 0];
                    g += src[idx + 1];
                    b += src[idx + 2];
                    a += src[idx + 3];
                    ++count;
                }
            }
            int di = (y * dst_w + x) * 4;
            dst[di + 0] = (uint8_t)(r / count);
            dst[di + 1] = (uint8_t)(g / count);
            dst[di + 2] = (uint8_t)(b / count);
            dst[di + 3] = (uint8_t)(a / count);
        }
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
    file_path_ = path;
    base_pixels_.assign(data, data + w * h * 4);

    if (use_mipmaps_ && w >= 4 && h >= 4) {
        num_mips_ = (uint8_t)(1 + (int)std::floor(std::log2(std::max(w, h))));

        std::vector<std::vector<uint8_t>> levels(num_mips_);
        levels[0] = base_pixels_;

        int mw = w, mh = h;
        for (uint8_t i = 1; i < num_mips_; ++i) {
            int pw = mw, ph = mh;
            mw = std::max(1, mw / 2);
            mh = std::max(1, mh / 2);
            levels[i].resize(mw * mh * 4);
            DownsampleLevel(levels[i - 1].data(), pw, ph, levels[i].data(), mw, mh);
        }

        size_t total = 0;
        for (auto& lv : levels) total += lv.size();
        std::vector<uint8_t> all_mips(total);
        size_t off = 0;
        for (auto& lv : levels) {
            std::memcpy(all_mips.data() + off, lv.data(), lv.size());
            off += lv.size();
        }

        handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, true, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_SAMPLER_MIP_POINT,
            bgfx::copy(all_mips.data(), (uint32_t)all_mips.size()));
    } else {
        num_mips_ = 1;
        const bgfx::Memory* mem = bgfx::copy(data, w * h * 4);
        handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
            bgfx::TextureFormat::RGBA8, 0, mem);
    }

    stbi_image_free(data);

    if (!bgfx::isValid(handle_)) {
        spdlog::error("Texture: failed to create GPU texture from '{}'", path);
        return false;
    }

    if (num_mips_ > 1) {
        bgfx::setName(handle_, path.c_str());
    }

    spdlog::debug("Texture: loaded '{}' ({}x{}, {} mips)", path, w, h, num_mips_);
    return true;
}

bool Texture::GenerateMipmaps() {
    if (!bgfx::isValid(handle_) || base_pixels_.empty()) {
        spdlog::warn("Texture::GenerateMipmaps: no base pixel data available");
        return false;
    }
    if (num_mips_ > 1) return true;

    int w = width_, h = height_;
    if (w < 4 || h < 4) return false;

    uint8_t num_mips = (uint8_t)(1 + (int)std::floor(std::log2(std::max(w, h))));

    std::vector<std::vector<uint8_t>> levels(num_mips);
    levels[0] = base_pixels_;

    int mw = w, mh = h;
    for (uint8_t i = 1; i < num_mips; ++i) {
        int pw = mw, ph = mh;
        mw = std::max(1, mw / 2);
        mh = std::max(1, mh / 2);
        levels[i].resize(mw * mh * 4);
        DownsampleLevel(levels[i - 1].data(), pw, ph, levels[i].data(), mw, mh);
    }

    size_t total = 0;
    for (auto& lv : levels) total += lv.size();
    std::vector<uint8_t> all_mips(total);
    size_t off = 0;
    for (auto& lv : levels) {
        std::memcpy(all_mips.data() + off, lv.data(), lv.size());
        off += lv.size();
    }

    bgfx::TextureHandle new_handle = bgfx::createTexture2D(
        (uint16_t)w, (uint16_t)h, true, 1,
        bgfx::TextureFormat::RGBA8,
        BGFX_SAMPLER_MIP_POINT,
        bgfx::copy(all_mips.data(), (uint32_t)all_mips.size()));

    if (!bgfx::isValid(new_handle)) {
        spdlog::error("Texture::GenerateMipmaps: failed to create mipmapped texture");
        return false;
    }

    bgfx::destroy(handle_);
    handle_ = new_handle;
    num_mips_ = num_mips;

    if (!file_path_.empty()) {
        bgfx::setName(handle_, file_path_.c_str());
    }

    spdlog::debug("Texture::GenerateMipmaps: generated {} levels ({}x{})", num_mips, w, h);
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
    base_pixels_.assign(img, img + w * h * 4);

    if (use_mipmaps_ && w >= 4 && h >= 4) {
        num_mips_ = (uint8_t)(1 + (int)std::floor(std::log2(std::max(w, h))));

        std::vector<std::vector<uint8_t>> levels(num_mips_);
        levels[0] = base_pixels_;

        int mw = w, mh = h;
        for (uint8_t i = 1; i < num_mips_; ++i) {
            int pw = mw, ph = mh;
            mw = std::max(1, mw / 2);
            mh = std::max(1, mh / 2);
            levels[i].resize(mw * mh * 4);
            DownsampleLevel(levels[i - 1].data(), pw, ph, levels[i].data(), mw, mh);
        }

        size_t total = 0;
        for (auto& lv : levels) total += lv.size();
        std::vector<uint8_t> all_mips(total);
        size_t o = 0;
        for (auto& lv : levels) {
            std::memcpy(all_mips.data() + o, lv.data(), lv.size());
            o += lv.size();
        }

        handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, true, 1,
            bgfx::TextureFormat::RGBA8,
            BGFX_SAMPLER_MIP_POINT,
            bgfx::copy(all_mips.data(), (uint32_t)all_mips.size()));
    } else {
        num_mips_ = 1;
        const bgfx::Memory* mem = bgfx::copy(img, w * h * 4);
        handle_ = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
            bgfx::TextureFormat::RGBA8, 0, mem);
    }

    stbi_image_free(img);
    return bgfx::isValid(handle_);
}

bool Texture::CreateRenderTarget(int width, int height) {
    width_ = width;
    height_ = height;
    num_mips_ = 1;
    base_pixels_.clear();

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
