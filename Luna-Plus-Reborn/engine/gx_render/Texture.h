#pragma once
#include <bgfx/bgfx.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(const void* data, size_t size);
    bool CreateRenderTarget(int width, int height);

    void Bind(uint8_t stage = 0) const;
    void Unbind() const;

    bgfx::TextureHandle GetHandle() const { return handle_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    bool IsValid() const { return bgfx::isValid(handle_); }

private:
    bgfx::TextureHandle handle_ = BGFX_INVALID_HANDLE;
    int width_ = 0, height_ = 0;
};
