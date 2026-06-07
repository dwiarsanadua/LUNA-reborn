#include "Sprite3D.h"
#include <spdlog/spdlog.h>

static bool s_layout_initialized = false;
static bgfx::VertexLayout s_sprite_layout;

Sprite3D::Sprite3D() {
    if (!s_layout_initialized) {
        s_sprite_layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        s_layout_initialized = true;
    }
}

Sprite3D::~Sprite3D() = default;

bool Sprite3D::Init() {
    return true;
}

void Sprite3D::Render(const glm::vec3& position, const glm::vec2& size,
                       bgfx::TextureHandle texture, uint32_t color) {
    float hw = size.x * 0.5f;
    float hh = size.y * 0.5f;

    struct Vertex { float x, y, z, u, v; uint32_t color; };
    Vertex verts[4] = {
        { position.x - hw, position.y - hh, position.z, 0, 0, color },
        { position.x + hw, position.y - hh, position.z, 1, 0, color },
        { position.x + hw, position.y + hh, position.z, 1, 1, color },
        { position.x - hw, position.y + hh, position.z, 0, 1, color },
    };
    uint16_t indices[6] = { 0, 1, 2, 0, 2, 3 };

    bgfx::setVertexBuffer(0, bgfx::createVertexBuffer(
        bgfx::makeRef(verts, sizeof(verts)), s_sprite_layout));
    bgfx::setIndexBuffer(bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))));
    bgfx::setTexture(0, BGFX_INVALID_HANDLE, texture);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, BGFX_INVALID_HANDLE);
}
