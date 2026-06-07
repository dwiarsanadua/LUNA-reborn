#pragma once
#include <rendering/CameraSystem.hpp>
#include <glm/glm.hpp>
#include <functional>

class EngineCamera {
public:
    EngineCamera();
    void Init(int width, int height);
    void Update(float dt);
    
    void SetTarget(const glm::vec3& target);
    void SetDistance(float dist);
    void SetYaw(float yaw);
    void SetPitch(float pitch);
    void SetShake(float x, float y);
    void Zoom(float delta);
    
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    
    Camera* GetCamera() { return &camera_; }
    const glm::vec3& GetTarget() const { return target_; }
    float GetDistance() const { return distance_; }
    
    // Transitions
    void TransitionTo(const glm::vec3& target, float distance, float yaw, float pitch, float duration);

private:
    Camera camera_;
    glm::vec3 target_{0};
    float distance_ = 80.0f;
    float yaw_ = -45.0f;
    float pitch_ = -40.0f;
    
    // Transition state
    bool transitioning_ = false;
    float transition_time_ = 0;
    float transition_dur_ = 0;
    glm::vec3 from_target_{0}, to_target_{0};
    float from_dist_ = 80, to_dist_ = 80;
    float from_yaw_ = -45, to_yaw_ = -45;
    float from_pitch_ = -40, to_pitch_ = -40;
};
