#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <vector>
#include <cstdint>

class Texture {
public:
    Texture();
    ~Texture();

    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(const void* data, size_t size);
    bool CreateRenderTarget(int width, int height);

    bool GenerateMipmaps();
    void SetUseMipmaps(bool use) { use_mipmaps_ = use; }

    void Bind(uint8_t stage = 0) const;
    void Unbind() const;

    bgfx::TextureHandle GetHandle() const { return handle_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    uint8_t GetNumMips() const { return num_mips_; }
    bool IsValid() const { return bgfx::isValid(handle_); }

private:
    static void DownsampleLevel(const uint8_t* src, int src_w, int src_h,
                                 uint8_t* dst, int dst_w, int dst_h);

    bgfx::TextureHandle handle_ = BGFX_INVALID_HANDLE;
    int width_ = 0, height_ = 0;
    uint8_t num_mips_ = 1;
    bool use_mipmaps_ = true;
    std::string file_path_;
    std::vector<uint8_t> base_pixels_;
};
