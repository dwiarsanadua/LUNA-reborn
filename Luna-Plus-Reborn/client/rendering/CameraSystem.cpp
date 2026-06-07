#define GLM_ENABLE_EXPERIMENTAL
#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

glm::mat4 Camera::GetViewMatrix() const {
    glm::vec3 shake_offset(shake_x_, shake_y_, 0);
    return glm::lookAt(position + shake_offset, target + shake_offset, up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    float aspect = static_cast<float>(view_width) / static_cast<float>(view_height);
    return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

void Camera::Update(float) {
    glm::vec3 offset(0.0f, 0.0f, distance);
    offset = glm::rotateY(offset, glm::radians(yaw));
    offset = glm::rotate(offset, glm::radians(pitch), glm::cross(glm::vec3{0,1,0}, offset));
    position = target + offset;
}

void Camera::Zoom(float delta) {
    distance = std::max(min_distance, std::min(max_distance, distance - delta * zoom_speed));
}
