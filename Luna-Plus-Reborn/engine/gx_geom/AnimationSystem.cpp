// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#include "AnimationSystem.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ── JSON loading ────────────────────────────────────────────────
bool AnimationSystem::LoadFromJson(const std::string& path, AnimClip& out_clip) {
    std::ifstream f(path);
    if (!f.is_open()) {
        spdlog::error("AnimationSystem::LoadFromJson: cannot open {}", path);
        return false;
    }

    try {
        json j;
        f >> j;

        out_clip.name = j.value("name", path);
        out_clip.duration_sec = j.value("duration", 1.0f);
        out_clip.fps = j.value("fps", 30.0f);
        out_clip.loop = j.value("loop", false);

        if (j.contains("tracks")) {
            for (auto& jtrack : j["tracks"]) {
                AnimBoneTrack track;
                track.bone_name = jtrack.value("bone", "");
                if (jtrack.contains("keyframes")) {
                    for (auto& jkf : jtrack["keyframes"]) {
                        AnimKeyframe kf;
                        kf.time_seconds = jkf.value("time", 0.0f);
                        if (jkf.contains("position")) {
                            kf.position.x = jkf["position"][0];
                            kf.position.y = jkf["position"][1];
                            kf.position.z = jkf["position"][2];
                        }
                        if (jkf.contains("rotation")) {
                            kf.rotation.x = jkf["rotation"][0];
                            kf.rotation.y = jkf["rotation"][1];
                            kf.rotation.z = jkf["rotation"][2];
                            kf.rotation.w = jkf["rotation"][3];
                        }
                        if (jkf.contains("scale")) {
                            kf.scale.x = jkf["scale"][0];
                            kf.scale.y = jkf["scale"][1];
                            kf.scale.z = jkf["scale"][2];
                        }
                        track.keyframes.push_back(kf);
                    }
                }
                out_clip.tracks.push_back(track);
            }
        }

        out_clip.bone_indices.resize(out_clip.tracks.size());
        for (size_t i = 0; i < out_clip.tracks.size(); ++i) {
            out_clip.bone_indices[i] = (int)i;
        }

        spdlog::info("AnimationSystem: loaded JSON anim '{}' ({} tracks, {:.2f}s)",
                     out_clip.name, out_clip.tracks.size(), out_clip.duration_sec);
        return true;
    } catch (std::exception& e) {
        spdlog::error("AnimationSystem::LoadFromJson: {} — {}", e.what(), path);
        return false;
    }
}

// ── Legacy .anm binary loading ──────────────────────────────────
bool AnimationSystem::LoadFromAnm(const std::string& path, AnimClip& out_clip) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        spdlog::error("AnimationSystem::LoadFromAnm: cannot open {}", path);
        return false;
    }

    f.seekg(0, std::ios::end);
    size_t fileSize = f.tellg();
    f.seekg(0);
    if (fileSize < 160) return false;

    std::vector<uint8_t> buf(fileSize);
    f.read(reinterpret_cast<char*>(buf.data()), fileSize);

    auto rdu32 = [&](size_t off) -> uint32_t {
        uint32_t v; std::memcpy(&v, buf.data() + off, 4); return v;
    };
    auto rdf32 = [&](size_t off) -> float {
        float v; std::memcpy(&v, buf.data() + off, 4); return v;
    };

    uint32_t lastFrame = rdu32(12);
    uint32_t frameSpeed = rdu32(16);
    uint32_t objNum = rdu32(20);
    char nameBuf[129] = {};
    std::memcpy(nameBuf, buf.data() + 32, 128);

    out_clip.name = nameBuf;
    out_clip.fps = frameSpeed > 0 ? (float)frameSpeed : 30.0f;
    out_clip.duration_sec = out_clip.fps > 0 ? (float)lastFrame / out_clip.fps : 1.0f;
    out_clip.loop = false;

    size_t off = 160;
    for (uint32_t i = 0; i < objNum; ++i) {
        if (off + 8 > fileSize) break;
        uint32_t objType = rdu32(off);
        uint32_t objSize = rdu32(off + 4);
        (void)objType; (void)objSize;
        off += 8;
        if (off + 152 > fileSize) break;

        uint32_t rotNum = rdu32(off + 4);
        uint32_t posNum = rdu32(off + 8);
        uint32_t scaleNum = rdu32(off + 12);

        char objName[129] = {};
        std::memcpy(objName, buf.data() + off + 20, 128);
        off += 152;

        if (rotNum == 0 && posNum == 0 && scaleNum == 0) continue;

        AnimBoneTrack track;
        track.bone_name = objName;

        size_t kfCount = std::max({rotNum, posNum, scaleNum});
        track.keyframes.resize(kfCount);

        for (uint32_t k = 0; k < rotNum && off + 24 <= fileSize; ++k) {
            float qx = rdf32(off + 8);
            float qy = rdf32(off + 12);
            float qz = rdf32(off + 16);
            float qw = rdf32(off + 20);
            track.keyframes[k].rotation = glm::quat(qw, qx, qy, qz);
            off += 24;
        }

        for (uint32_t k = 0; k < posNum && off + 20 <= fileSize; ++k) {
            float px = rdf32(off + 8);
            float py = rdf32(off + 12);
            float pz = rdf32(off + 16);
            track.keyframes[k].position = glm::vec3(px, py, pz);
            track.keyframes[k].time_seconds = (float)k / out_clip.fps;
            off += 20;
        }

        for (uint32_t k = 0; k < scaleNum && off + 36 <= fileSize; ++k) {
            float sx = rdf32(off + 8);
            float sy = rdf32(off + 12);
            float sz = rdf32(off + 16);
            track.keyframes[k].scale = glm::vec3(sx, sy, sz);
            off += 36;
        }

        // Skip mesh keys
        for (uint32_t k = 0; k < rdu32(off - 36 + 16) && off + 16 <= fileSize; ++k) {
            uint32_t vn = rdu32(off + 8);
            uint32_t tvn = rdu32(off + 12);
            off += 16 + vn * 12 + tvn * 8;
        }

        out_clip.tracks.push_back(track);
    }

    out_clip.bone_indices.resize(out_clip.tracks.size());
    for (size_t i = 0; i < out_clip.tracks.size(); ++i) {
        out_clip.bone_indices[i] = (int)i;
    }

    spdlog::info("AnimationSystem: loaded ANM '{}' ({} tracks, {:.2f}s)",
                 out_clip.name, out_clip.tracks.size(), out_clip.duration_sec);
    return !out_clip.tracks.empty();
}

// ── Keyframe sampling ───────────────────────────────────────────
static glm::mat4 SampleTrack(const AnimBoneTrack& track, float time) {
    if (track.keyframes.empty()) return glm::mat4(1.0f);

    // Find surrounding keyframes
    size_t k0 = 0, k1 = 0;
    for (size_t i = 0; i + 1 < track.keyframes.size(); ++i) {
        if (time >= track.keyframes[i].time_seconds &&
            time <= track.keyframes[i + 1].time_seconds) {
            k0 = i;
            k1 = i + 1;
            break;
        }
    }
    if (k0 == k1) {
        k1 = track.keyframes.size() - 1;
    }

    const auto& kf0 = track.keyframes[k0];
    const auto& kf1 = track.keyframes[k1];

    float t = 0.0f;
    float dt = kf1.time_seconds - kf0.time_seconds;
    if (dt > 0.0f) t = (time - kf0.time_seconds) / dt;
    t = glm::clamp(t, 0.0f, 1.0f);

    glm::vec3 pos = glm::mix(kf0.position, kf1.position, t);
    glm::quat rot = glm::slerp(kf0.rotation, kf1.rotation, t);
    glm::vec3 scl = glm::mix(kf0.scale, kf1.scale, t);

    glm::mat4 m(1.0f);
    m = glm::translate(m, pos);
    m *= glm::mat4_cast(rot);
    m = glm::scale(m, scl);
    return m;
}

glm::mat4 AnimClip::SampleAtTime(float time, int bone_index) const {
    if (bone_index < 0 || bone_index >= (int)tracks.size())
        return glm::mat4(1.0f);
    return SampleTrack(tracks[bone_index], time);
}

void AnimClip::SampleAllAtTime(float time, glm::mat4* out_matrices, size_t count) const {
    size_t n = std::min(count, tracks.size());
    for (size_t i = 0; i < n; ++i) {
        out_matrices[i] = SampleTrack(tracks[i], time);
    }
    for (size_t i = n; i < count; ++i) {
        out_matrices[i] = glm::mat4(1.0f);
    }
}

// ── Animation system runtime ────────────────────────────────────
void AnimationSystem::Play(AnimClip* clip, bool loop, float blend_time) {
    if (!clip) return;
    if (current_clip_ && current_clip_ != clip) {
        BlendTo(clip, blend_time);
        return;
    }
    current_clip_ = clip;
    current_time_ = 0.0f;
    looping_ = loop;
    is_playing_ = true;
    blend_timer_ = blend_duration_ = 0.0f;
    next_clip_ = nullptr;
}

void AnimationSystem::BlendTo(AnimClip* clip, float blend_time) {
    if (!clip) return;
    next_clip_ = clip;
    blend_timer_ = 0.0f;
    blend_duration_ = blend_time;
    looping_ = clip->loop;
}

void AnimationSystem::Stop() {
    is_playing_ = false;
    current_clip_ = nullptr;
    next_clip_ = nullptr;
    current_time_ = 0.0f;
    blend_timer_ = blend_duration_ = 0.0f;
}

void AnimationSystem::Update(float delta_time) {
    if (!is_playing_ || !current_clip_) return;

    current_time_ += delta_time;

    if (looping_) {
        if (current_clip_->duration_sec > 0.0f) {
            current_time_ = fmod(current_time_, current_clip_->duration_sec);
        }
    } else {
        if (current_time_ >= current_clip_->duration_sec) {
            current_time_ = current_clip_->duration_sec;
            is_playing_ = false;
        }
    }

    // Handle blending
    if (next_clip_ && blend_duration_ > 0.0f) {
        blend_timer_ += delta_time;
        if (blend_timer_ >= blend_duration_) {
            current_clip_ = next_clip_;
            current_time_ = 0.0f;
            next_clip_ = nullptr;
            blend_timer_ = blend_duration_ = 0.0f;
        }
    }
}

void AnimationSystem::InterpolatePose(const AnimClip& clip, float time,
                                      glm::mat4* out_pose, size_t count) {
    clip.SampleAllAtTime(time, out_pose, count);
}

void AnimationSystem::GetBlendedPose(const std::vector<glm::mat4>& bind_pose,
                                     glm::mat4* out_pose, size_t count) {
    if (!current_clip_) {
        for (size_t i = 0; i < count; ++i) {
            out_pose[i] = glm::mat4(1.0f);
        }
        return;
    }

    size_t n = std::min(count, current_clip_->tracks.size());
    std::vector<glm::mat4> pose0(count, glm::mat4(1.0f));
    InterpolatePose(*current_clip_, current_time_, pose0.data(), count);

    if (next_clip_ && blend_duration_ > 0.0f) {
        float blend_factor = glm::clamp(blend_timer_ / blend_duration_, 0.0f, 1.0f);
        std::vector<glm::mat4> pose1(count, glm::mat4(1.0f));
        InterpolatePose(*next_clip_, current_time_, pose1.data(), count);
        for (size_t i = 0; i < n; ++i) {
            // Decompose and blend position, rotation, scale
            glm::vec3 p0, p1, s0, s1, sk0, sk1;
            glm::quat r0, r1;
            glm::vec3 persp0, persp1;
            glm::decompose(pose0[i], s0, r0, p0, sk0, persp0);
            glm::decompose(pose1[i], s1, r1, p1, sk1, persp1);

            glm::vec3 pos = glm::mix(p0, p1, blend_factor);
            glm::quat rot = glm::slerp(r0, r1, blend_factor);
            glm::vec3 scl = glm::mix(s0, s1, blend_factor);

            glm::mat4 m(1.0f);
            m = glm::translate(m, pos);
            m *= glm::mat4_cast(rot);
            m = glm::scale(m, scl);
            out_pose[i] = m;
        }
    } else {
        for (size_t i = 0; i < count; ++i) {
            out_pose[i] = pose0[i];
        }
    }
}
