#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <cstdint>

class Sprite3D {
public:
    Sprite3D();
    ~Sprite3D();

    bool Init();
    void Render(const glm::vec3& position, const glm::vec2& size,
                bgfx::TextureHandle texture, uint32_t color = 0xFFFFFFFF);
    void Destroy();
};
