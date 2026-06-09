// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct AnimKeyframe {
    float time_seconds = 0.0f;
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
};

struct AnimBoneTrack {
    std::string bone_name;
    std::vector<AnimKeyframe> keyframes;
};

struct AnimClip {
    std::string name;
    float duration_sec = 1.0f;
    float fps = 30.0f;
    bool loop = false;
    std::vector<AnimBoneTrack> tracks;
    std::vector<int> bone_indices;

    glm::mat4 SampleAtTime(float time, int bone_index) const;
    void SampleAllAtTime(float time, glm::mat4* out_matrices, size_t count) const;
};

class AnimationSystem {
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;

    bool LoadFromJson(const std::string& path, AnimClip& out_clip);
    bool LoadFromAnm(const std::string& path, AnimClip& out_clip);

    void Update(float delta_time);
    void Play(AnimClip* clip, bool loop = true, float blend_time = 0.2f);
    void BlendTo(AnimClip* clip, float blend_time);
    void Stop();
    void SetSpeed(float speed) { speed_multiplier_ = speed; }
    float GetSpeed() const { return speed_multiplier_; }

    bool IsPlaying() const { return is_playing_; }
    float GetCurrentTime() const { return current_time_; }
    const AnimClip* GetCurrentClip() const { return current_clip_; }

    void GetBlendedPose(const std::vector<glm::mat4>& bind_pose,
                        glm::mat4* out_pose, size_t count);

private:
    void InterpolatePose(const AnimClip& clip, float time,
                         glm::mat4* out_pose, size_t count);

    AnimClip* current_clip_ = nullptr;
    AnimClip* next_clip_ = nullptr;
    float current_time_ = 0.0f;
    float blend_timer_ = 0.0f;
    float blend_duration_ = 0.0f;
    float speed_multiplier_ = 1.0f;
    bool is_playing_ = false;
    bool looping_ = true;
};
