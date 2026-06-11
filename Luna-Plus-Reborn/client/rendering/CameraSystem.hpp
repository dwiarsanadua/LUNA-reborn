#pragma once
#include <glm/glm.hpp>

struct Camera {
    glm::vec3 position{30.0f, 40.0f, 30.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float distance = 50.0f;
    float yaw = -45.0f;
    float pitch = -30.0f;
    float fov = 45.0f;
    float near_plane = 0.1f;
    float far_plane = 500.0f;
    float min_distance = 18.0f;
    float max_distance = 160.0f;
    float zoom_speed = 6.0f;
    int view_width = 1280;
    int view_height = 720;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    void Update(float dt);
    void Zoom(float delta);
    void SetShake(float x, float y) { shake_x_ = x; shake_y_ = y; }
private:
    float shake_x_ = 0;
    float shake_y_ = 0;
};
