#pragma once
#include <bgfx/bgfx.h>
#include <vector>
#include <cstdint>

class VertexBuffer {
public:
    VertexBuffer();
    ~VertexBuffer();

    bool Create(const void* vertices, size_t vertex_count, size_t vertex_stride,
                const bgfx::VertexLayout& layout);
    bool Update(const void* vertices, size_t vertex_count, size_t offset = 0);
    void Bind(uint8_t stream = 0) const;
    void Unbind() const;
    void Destroy();

    bgfx::VertexBufferHandle GetHandle() const { return handle_; }
    size_t GetVertexCount() const { return vertex_count_; }
    size_t GetVertexStride() const { return vertex_stride_; }
    bool IsValid() const { return bgfx::isValid(handle_); }

private:
    bgfx::VertexBufferHandle handle_ = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout layout_;
    size_t vertex_count_ = 0;
    size_t vertex_stride_ = 0;
};

class IndexBuffer {
public:
    IndexBuffer();
    ~IndexBuffer();

    bool Create(const void* indices, size_t index_count, bool index32 = false);
    bool Update(const void* indices, size_t index_count, size_t offset = 0);
    void Bind() const;
    void Unbind() const;
    void Destroy();

    bgfx::IndexBufferHandle GetHandle() const { return handle_; }
    size_t GetIndexCount() const { return index_count_; }
    bool IsValid() const { return bgfx::isValid(handle_); }

private:
    bgfx::IndexBufferHandle handle_ = BGFX_INVALID_HANDLE;
    size_t index_count_ = 0;
    bool index32_ = false;
};
