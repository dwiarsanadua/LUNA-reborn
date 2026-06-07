// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#include "MeshObject.h"
#include "Model.h"
#include <spdlog/spdlog.h>
#include <cstring>

struct SkinnedVertex {
    float x, y, z;
    float nx, ny, nz;
    uint32_t color;
    float u, v;
    uint8_t bone_indices[4] = {0,0,0,0};
    float bone_weights[4] = {0,0,0,0};
};

static bgfx::VertexLayout s_layout;
static bool s_layout_init = false;

static const bgfx::VertexLayout& GetMeshLayout() {
    if (!s_layout_init) {
        s_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Indices,  4, bgfx::AttribType::Uint8)
            .add(bgfx::Attrib::Weight,   4, bgfx::AttribType::Float)
            .end();
        s_layout_init = true;
    }
    return s_layout;
}

bool MeshObject::UploadToGPU(const MeshPart& part) {
    Destroy();

    vertex_count_ = part.positions.size();
    index_count_ = part.indices.size();

    if (vertex_count_ == 0 || index_count_ == 0) {
        spdlog::warn("MeshObject::UploadToGPU: empty mesh part '{}'", part.name);
        return false;
    }

    std::vector<SkinnedVertex> verts(vertex_count_);
    for (size_t i = 0; i < vertex_count_; ++i) {
        verts[i].x = part.positions[i].x;
        verts[i].y = part.positions[i].y;
        verts[i].z = part.positions[i].z;
        if (i < part.normals.size()) {
            verts[i].nx = part.normals[i].x;
            verts[i].ny = part.normals[i].y;
            verts[i].nz = part.normals[i].z;
        }
        if (i < part.colors.size()) {
            verts[i].color = part.colors[i];
        } else {
            verts[i].color = 0xffffffff;
        }
        if (i < part.uvs.size()) {
            verts[i].u = part.uvs[i].x;
            verts[i].v = part.uvs[i].y;
        }
        if (part.bone_count > 0 && i < part.bone_indices.size() / 4) {
            for (int k = 0; k < 4; ++k) {
                verts[i].bone_indices[k] = part.bone_indices[i * 4 + k];
                verts[i].bone_weights[k] = part.bone_weights[i * 4 + k];
            }
        }
    }

    const bgfx::Memory* vmem = bgfx::copy(verts.data(), (uint32_t)(verts.size() * sizeof(SkinnedVertex)));
    vb_ = bgfx::createVertexBuffer(vmem, GetMeshLayout());
    if (!bgfx::isValid(vb_)) {
        spdlog::error("MeshObject::UploadToGPU: failed to create vertex buffer");
        return false;
    }

    const bgfx::Memory* imem = bgfx::copy(part.indices.data(), (uint32_t)(part.indices.size() * sizeof(uint16_t)));
    ib_ = bgfx::createIndexBuffer(imem);
    if (!bgfx::isValid(ib_)) {
        spdlog::error("MeshObject::UploadToGPU: failed to create index buffer");
        bgfx::destroy(vb_);
        vb_ = BGFX_INVALID_HANDLE;
        return false;
    }

    spdlog::debug("MeshObject::UploadToGPU: '{}' ({} verts, {} indices)",
                  part.name, vertex_count_, index_count_);
    return true;
}

void MeshObject::Render(uint8_t stream) const {
    if (bgfx::isValid(vb_)) {
        bgfx::setVertexBuffer(stream, vb_);
    }
    if (bgfx::isValid(ib_)) {
        bgfx::setIndexBuffer(ib_);
    }
}

void MeshObject::Destroy() {
    if (bgfx::isValid(vb_)) {
        bgfx::destroy(vb_);
        vb_ = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(ib_)) {
        bgfx::destroy(ib_);
        ib_ = BGFX_INVALID_HANDLE;
    }
    vertex_count_ = 0;
    index_count_ = 0;
}
