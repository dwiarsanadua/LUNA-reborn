#pragma once
#include <entt/entt.hpp>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

enum class ParticleType : uint8_t {
    Point, Cone, Sphere, Billboard
};

enum class ParticleEffect : uint8_t {
    None, HitSpark, SkillCast, LevelUp, Death, BuffGlow, WeatherRain
};

struct ParticleEmitter {
    ParticleType type = ParticleType::Point;
    ParticleEffect effect = ParticleEffect::None;
    uint32_t max_particles = 50;
    float emission_rate = 10.0f;
    float lifetime = 1.0f;
    float speed = 5.0f;
    uint32_t color_start = 0xFFFFFFFF;
    uint32_t color_end = 0x00FFFFFF;
    float size_start = 1.0f, size_end = 0.1f;
    float timer = 0;
    bool active = false;
};

struct ActiveParticle {
    glm::vec3 position{0};
    glm::vec3 velocity{0};
    uint32_t color;
    float size;
    float life_remaining;
};

class ParticleSystem {
public:
    void Emit(entt::registry& reg, entt::entity entity, ParticleEffect effect);
    void EmitAt(glm::vec3 position, ParticleEffect effect, uint32_t color = 0xFFFFFFFF);
    void Update(entt::registry& reg, float dt);
    std::vector<ActiveParticle> GetActiveParticles() const { return particles_; }

private:
    std::vector<ActiveParticle> particles_;
    void EmitBurst(glm::vec3 pos, uint32_t color, int count, float speed, float lifetime);
};
