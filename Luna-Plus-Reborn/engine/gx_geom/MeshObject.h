// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <bgfx/bgfx.h>
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct MeshPart;

class MeshObject {
public:
    MeshObject() = default;
    ~MeshObject() { Destroy(); }

    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    MeshObject(MeshObject&& other) noexcept
        : vb_(other.vb_), ib_(other.ib_),
          vertex_count_(other.vertex_count_),
          index_count_(other.index_count_) {
        other.vb_ = BGFX_INVALID_HANDLE;
        other.ib_ = BGFX_INVALID_HANDLE;
    }
    MeshObject& operator=(MeshObject&& other) noexcept {
        if (this != &other) {
            Destroy();
            vb_ = other.vb_; other.vb_ = BGFX_INVALID_HANDLE;
            ib_ = other.ib_; other.ib_ = BGFX_INVALID_HANDLE;
            vertex_count_ = other.vertex_count_;
            index_count_ = other.index_count_;
        }
        return *this;
    }

    bool UploadToGPU(const MeshPart& part);
    void Render(uint8_t stream = 0) const;
    void Destroy();

    bgfx::VertexBufferHandle GetVertexBuffer() const { return vb_; }
    bgfx::IndexBufferHandle GetIndexBuffer() const { return ib_; }
    size_t GetIndexCount() const { return index_count_; }
    size_t GetVertexCount() const { return vertex_count_; }
    bool IsValid() const { return bgfx::isValid(vb_); }

private:
    bgfx::VertexBufferHandle vb_ = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ib_ = BGFX_INVALID_HANDLE;
    size_t vertex_count_ = 0;
    size_t index_count_ = 0;
};
