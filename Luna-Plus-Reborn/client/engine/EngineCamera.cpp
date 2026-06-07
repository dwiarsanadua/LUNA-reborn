#include "EngineCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

EngineCamera::EngineCamera() {
    camera_.target = glm::vec3(0, 0, 0);
    camera_.distance = distance_;
    camera_.yaw = yaw_;
    camera_.pitch = pitch_;
}

void EngineCamera::Init(int width, int height) {
    camera_.view_width = width;
    camera_.view_height = height;
    camera_.target = target_;
    camera_.distance = distance_;
    camera_.yaw = yaw_;
    camera_.pitch = pitch_;
}

void EngineCamera::SetTarget(const glm::vec3& t) { target_ = t; }
void EngineCamera::SetDistance(float d) { distance_ = d; camera_.distance = d; }
void EngineCamera::SetYaw(float y) { yaw_ = y; camera_.yaw = y; }
void EngineCamera::SetPitch(float p) { pitch_ = p; camera_.pitch = p; }
void EngineCamera::SetShake(float x, float y) { camera_.SetShake(x, y); }
void EngineCamera::Zoom(float delta) { camera_.Zoom(delta); }

glm::mat4 EngineCamera::GetViewMatrix() const { return camera_.GetViewMatrix(); }
glm::mat4 EngineCamera::GetProjectionMatrix() const { return camera_.GetProjectionMatrix(); }

void EngineCamera::TransitionTo(const glm::vec3& target, float distance, float yaw, float pitch, float duration) {
    from_target_ = target_;
    to_target_ = target;
    from_dist_ = distance_;
    to_dist_ = distance;
    from_yaw_ = yaw_;
    to_yaw_ = yaw;
    from_pitch_ = pitch_;
    to_pitch_ = pitch;
    transition_time_ = 0;
    transition_dur_ = duration;
    transitioning_ = true;
}

void EngineCamera::Update(float dt) {
    camera_.target = target_;
    camera_.distance = distance_;
    camera_.yaw = yaw_;
    camera_.pitch = pitch_;
    
    if (transitioning_) {
        transition_time_ += dt;
        float t = std::min(1.0f, transition_time_ / transition_dur_);
        float s = t * t * (3.0f - 2.0f * t); // smoothstep
        
        camera_.target = from_target_ + (to_target_ - from_target_) * s;
        camera_.distance = from_dist_ + (to_dist_ - from_dist_) * s;
        camera_.yaw = from_yaw_ + (to_yaw_ - from_yaw_) * s;
        camera_.pitch = from_pitch_ + (to_pitch_ - from_pitch_) * s;
        
        if (t >= 1.0f) transitioning_ = false;
    }
    
    camera_.Update(dt);
}
