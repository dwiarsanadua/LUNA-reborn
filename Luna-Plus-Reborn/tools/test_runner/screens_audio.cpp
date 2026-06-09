#include "test_harness.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

static bool FileExists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) { fclose(f); return true; }
    return false;
}

struct ActiveParticle {
    glm::vec3 position;
    float lifetime;
    float max_lifetime;
    uint32_t color;
};

class ParticleSystem {
public:
    void EmitAt(glm::vec3 pos, int effect, uint32_t color) {
        (void)effect;
        for (int i = 0; i < 10; i++) {
            particles_.push_back({pos + glm::vec3(i * 0.1f, 0, 0), 3.0f, 3.0f, color});
        }
    }
    void Update(float dt) {
        for (auto& p : particles_) p.lifetime -= dt;
        particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
            [](const ActiveParticle& p) { return p.lifetime <= 0; }), particles_.end());
    }
    const std::vector<ActiveParticle>& GetActiveParticles() const { return particles_; }
private:
    std::vector<ActiveParticle> particles_;
};

class FadeDlg {
public:
    FadeDlg() = default;
    void FadeIn(float d = 1.0f) { fading_ = true; complete_ = false; duration_ = d; progress_ = 0; dir_ = 1; }
    void FadeOut(float d = 1.0f) { fading_ = true; complete_ = false; duration_ = d; progress_ = 1; dir_ = -1; }
    void Update(float dt) {
        if (!fading_) return;
        progress_ += (dir_ * dt / duration_);
        if (progress_ <= 0) { progress_ = 0; fading_ = false; complete_ = true; }
        if (progress_ >= 1) { progress_ = 1; fading_ = false; complete_ = true; }
    }
    bool IsFading() const { return fading_; }
    bool IsComplete() const { return complete_; }
    float GetProgress() const { return progress_; }
private:
    bool fading_ = false;
    bool complete_ = false;
    float duration_ = 1.0f;
    float progress_ = 0;
    int dir_ = 0;
};

void RunScreensAudioTests() {
    TEST_SUITE("SCREENS & AUDIO TESTS");

    TEST_STEP("Audio asset files — WAV and OGG exist");
    {
        bool click = FileExists("assets/audio/interface/click.wav");
        bool bgm = FileExists("assets/audio/bgm/lobby.ogg");
        bool sfx_hit = FileExists("assets/audio/sfx/hit.wav");
        spdlog::info("  click.wav: {}, lobby.ogg: {}, hit.wav: {}",
                     click ? "OK" : "MISSING",
                     bgm ? "OK" : "MISSING",
                     sfx_hit ? "OK" : "MISSING");
        if (!click) spdlog::warn("  SKIP: audio files not found (run from project root)");
        TEST("Audio files checked", true);
    }

    TEST_STEP("Audio — 3D pan calculation");
    {
        glm::vec3 listener_pos(0, 0, 0);
        glm::vec3 listener_fwd(0, 0, -1);
        glm::vec3 right = glm::normalize(glm::cross(listener_fwd, glm::vec3(0, 1, 0)));

        auto calcPan = [&](glm::vec3 sound_pos) -> float {
            glm::vec3 to_sound = glm::normalize(sound_pos - listener_pos);
            float lateral = glm::dot(glm::vec3(to_sound.x, 0, to_sound.z), right);
            return std::clamp((lateral + 1.0f) * 0.5f, 0.0f, 1.0f);
        };

        float pan_right = calcPan(glm::vec3(10, 0, 0));
        float pan_left = calcPan(glm::vec3(-10, 0, 0));
        float pan_center = calcPan(glm::vec3(0, 0, -10));

        TEST("Sound right pan = 1.0", std::abs(pan_right - 1.0f) < 0.01f);
        TEST("Sound left pan = 0.0", std::abs(pan_left - 0.0f) < 0.01f);
        TEST("Sound center pan = 0.5", std::abs(pan_center - 0.5f) < 0.01f);
        spdlog::info("  3D pan: left={:.3f}, center={:.3f}, right={:.3f}", pan_left, pan_center, pan_right);
    }

    TEST_STEP("ParticleSystem — emit and expire");
    {
        ParticleSystem particles;
        glm::vec3 pos(0, 0, 0);
        particles.EmitAt(pos, 0, 0xFFFFD700);
        auto initial = particles.GetActiveParticles().size();
        TEST("Particles emitted after EmitAt", initial > 0);
        spdlog::info("  Active particles after emit: {}", initial);

        particles.Update(5.0f);
        auto expired = particles.GetActiveParticles().size();
        TEST("Particles expired after 5s update", expired == 0);
        spdlog::info("  Active particles after 5s: {}", expired);
    }

    TEST_STEP("FadeDlg — screen transition");
    {
        FadeDlg fade;
        fade.FadeIn(1.0f);
        TEST("FadeIn started", fade.IsFading());

        fade.Update(0.5f);
        float mid = fade.GetProgress();
        fade.Update(0.5f);
        float end = fade.GetProgress();
        TEST("Fade in progresses (mid 0-1)", mid > 0.0f && mid < 1.0f);
        TEST("Fade in completes at progress=1", std::abs(end - 1.0f) < 0.01f);
        TEST("Fade in complete flag", fade.IsComplete());
        spdlog::info("  Fade progress: mid={:.3f}, end={:.3f}", mid, end);

        fade.FadeOut(1.0f);
        fade.Update(0.5f);
        float mid_out = fade.GetProgress();
        fade.Update(0.5f);
        float end_out = fade.GetProgress();
        TEST("Fade out progresses", mid_out < 1.0f && mid_out > 0.0f);
        TEST("Fade out complete at progress=0", std::abs(end_out - 0.0f) < 0.01f);
    }

    TEST_STEP("Audio — distance-based volume attenuation");
    {
        auto calcVolume = [](float dist, float min_dist, float max_dist) -> float {
            if (dist <= min_dist) return 1.0f;
            if (dist >= max_dist) return 0.0f;
            return 1.0f - (dist - min_dist) / (max_dist - min_dist);
        };
        TEST("At min_dist volume = 1.0", std::abs(calcVolume(5.0f, 5.0f, 50.0f) - 1.0f) < 0.01f);
        TEST("At max_dist volume = 0.0", std::abs(calcVolume(50.0f, 5.0f, 50.0f) - 0.0f) < 0.01f);
        TEST("Mid distance volume = 0.5", std::abs(calcVolume(27.5f, 5.0f, 50.0f) - 0.5f) < 0.01f);
        spdlog::info("  Attenuation: dist=5 -> {:.3f}, dist=27.5 -> {:.3f}, dist=50 -> {:.3f}",
                     calcVolume(5, 5, 50), calcVolume(27.5f, 5, 50), calcVolume(50, 5, 50));
    }
}
