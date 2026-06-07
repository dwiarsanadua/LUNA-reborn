#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 GetMatrix() const {
        auto mat = glm::translate(glm::mat4{1.0f}, position);
        mat *= glm::mat4_cast(rotation);
        mat = glm::scale(mat, scale);
        return mat;
    }
};
