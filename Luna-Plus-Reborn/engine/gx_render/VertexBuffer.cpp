#include "VertexBuffer.h"
#include <spdlog/spdlog.h>
#include <cstring>

VertexBuffer::VertexBuffer() = default;

VertexBuffer::~VertexBuffer() {
    Destroy();
}

bool VertexBuffer::Create(const void* vertices, size_t vertex_count,
                           size_t vertex_stride, const bgfx::VertexLayout& layout) {
    layout_ = layout;
    vertex_count_ = vertex_count;
    vertex_stride_ = vertex_stride;

    const bgfx::Memory* mem = bgfx::copy(vertices, (uint32_t)(vertex_count * vertex_stride));
    handle_ = bgfx::createVertexBuffer(mem, layout_);

    if (!bgfx::isValid(handle_)) {
        spdlog::error("VertexBuffer: failed to create");
        return false;
    }

    return true;
}

bool VertexBuffer::Update(const void* vertices, size_t vertex_count, size_t offset) {
    if (!bgfx::isValid(handle_)) return false;
    // Regular vertex buffers don't support update — destroy and recreate
    bgfx::destroy(handle_);
    size_t size = vertex_count * vertex_stride_;
    const bgfx::Memory* mem = bgfx::copy((const uint8_t*)vertices + offset * vertex_stride_, (uint32_t)size);
    handle_ = bgfx::createVertexBuffer(mem, layout_);
    return bgfx::isValid(handle_);
}

void VertexBuffer::Bind(uint8_t stream) const {
    if (bgfx::isValid(handle_)) {
        bgfx::setVertexBuffer(stream, handle_);
    }
}

void VertexBuffer::Unbind() const {}

void VertexBuffer::Destroy() {
    if (bgfx::isValid(handle_)) {
        bgfx::destroy(handle_);
        handle_ = BGFX_INVALID_HANDLE;
    }
}

IndexBuffer::IndexBuffer() = default;

IndexBuffer::~IndexBuffer() {
    Destroy();
}

bool IndexBuffer::Create(const void* indices, size_t index_count, bool index32) {
    index_count_ = index_count;
    index32_ = index32;
    uint32_t index_size = index32 ? 4 : 2;
    const bgfx::Memory* mem = bgfx::copy(indices, (uint32_t)(index_count * index_size));
    handle_ = bgfx::createIndexBuffer(mem, index32 ? BGFX_BUFFER_INDEX32 : 0);

    if (!bgfx::isValid(handle_)) {
        spdlog::error("IndexBuffer: failed to create");
        return false;
    }
    return true;
}

bool IndexBuffer::Update(const void* indices, size_t index_count, size_t offset) {
    if (!bgfx::isValid(handle_)) return false;
    bgfx::destroy(handle_);
    uint32_t index_size = index32_ ? 4 : 2;
    const bgfx::Memory* mem = bgfx::copy((const uint8_t*)indices + offset * index_size, (uint32_t)(index_count * index_size));
    handle_ = bgfx::createIndexBuffer(mem, index32_ ? BGFX_BUFFER_INDEX32 : 0);
    return bgfx::isValid(handle_);
}

void IndexBuffer::Bind() const {
    if (bgfx::isValid(handle_)) {
        bgfx::setIndexBuffer(handle_);
    }
}

void IndexBuffer::Unbind() const {}

void IndexBuffer::Destroy() {
    if (bgfx::isValid(handle_)) {
        bgfx::destroy(handle_);
        handle_ = BGFX_INVALID_HANDLE;
    }
}
