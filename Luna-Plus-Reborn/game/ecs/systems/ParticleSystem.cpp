#include "ParticleSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>

static std::mt19937 particle_rng(std::random_device{}());
static std::uniform_real_distribution<float> p_dist(-1.0f, 1.0f);

void ParticleSystem::EmitBurst(glm::vec3 pos, uint32_t color, int count, float speed, float lifetime) {
    for (int i = 0; i < count; i++) {
        ActiveParticle p;
        p.position = pos;
        p.velocity = glm::vec3(p_dist(particle_rng), std::abs(p_dist(particle_rng)), p_dist(particle_rng)) * speed;
        p.color = color;
        p.size = 0.5f + std::abs(p_dist(particle_rng)) * 0.5f;
        p.life_remaining = lifetime * (0.5f + std::abs(p_dist(particle_rng)) * 0.5f);
        particles_.push_back(p);
    }
}

void ParticleSystem::Emit(entt::registry& reg, entt::entity entity, ParticleEffect effect) {
    if (!reg.valid(entity)) return;
    glm::vec3 pos(0);
    if (reg.all_of<Transform>(entity))
        pos = reg.get<Transform>(entity).position;
    uint32_t color = 0xFFFFFFFF;
    switch (effect) {
        case ParticleEffect::HitSpark:    color = 0xFFFFAA00; EmitBurst(pos, color, 8,  3.0f, 0.5f); break;
        case ParticleEffect::SkillCast:   color = 0xFF4488FF; EmitBurst(pos, color, 15, 5.0f, 1.0f); break;
        case ParticleEffect::LevelUp:     color = 0xFFFFD700; EmitBurst(pos, color, 20, 4.0f, 2.0f); break;
        case ParticleEffect::Death:       color = 0xFF888888; EmitBurst(pos, color, 12, 2.0f, 1.5f); break;
        case ParticleEffect::BuffGlow:    color = 0xFF44FF44; EmitBurst(pos, color, 6,  1.5f, 0.8f); break;
        default: break;
    }
}

void ParticleSystem::EmitAt(glm::vec3 position, ParticleEffect effect, uint32_t color) {
    switch (effect) {
        case ParticleEffect::HitSpark:    EmitBurst(position, color ? color : 0xFFFFAA00, 8,  3.0f, 0.5f); break;
        case ParticleEffect::SkillCast:   EmitBurst(position, color ? color : 0xFF4488FF, 15, 5.0f, 1.0f); break;
        case ParticleEffect::LevelUp:     EmitBurst(position, color ? color : 0xFFFFD700, 20, 4.0f, 2.0f); break;
        case ParticleEffect::Death:       EmitBurst(position, color ? color : 0xFF888888, 12, 2.0f, 1.5f); break;
        case ParticleEffect::BuffGlow:    EmitBurst(position, color ? color : 0xFF44FF44, 6,  1.5f, 0.8f); break;
        default: break;
    }
}

void ParticleSystem::Update(entt::registry& reg, float dt) {
    (void)reg;
    for (auto& p : particles_) {
        p.position += p.velocity * dt;
        p.velocity.y -= 9.8f * dt; // gravity
        p.life_remaining -= dt;
        float t = std::max(0.0f, p.life_remaining / 1.0f);
        p.size = std::max(0.0f, p.size - dt);
        p.color = (static_cast<uint32_t>(static_cast<uint8_t>(t * 255)) << 24) |
                  (p.color & 0x00FFFFFF);
    }
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const ActiveParticle& p) { return p.life_remaining <= 0 || p.size <= 0; }),
        particles_.end());
}
