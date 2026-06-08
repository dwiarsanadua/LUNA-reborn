#include "EffectManager.hpp"
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <audio/AudioManager.hpp>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

extern AudioManager* g_audio;

EffectManager::EffectManager() {}

EffectManager::~EffectManager() {
    Clear();
}

void EffectManager::Init(int pool_size) {
    pool_size_ = pool_size;
    pool_.resize(pool_size_);
    for (auto& slot : pool_) {
        slot.in_use = false;
    }
}

Effect* EffectManager::AcquireEffect() {
    // First try pool
    for (auto& slot : pool_) {
        if (!slot.in_use) {
            slot.in_use = true;
            slot.effect = Effect{};
            slot.effect.pooled = true;
            active_count_++;
            return &slot.effect;
        }
    }
    // Fallback: dynamic allocation
    dynamic_effects_.emplace_back();
    active_count_++;
    return &dynamic_effects_.back();
}

void EffectManager::ReleaseEffect(uint32_t id) {
    // Pooled effects are released by RemoveInactive scanning
    (void)id;
}

uint32_t EffectManager::SpawnDamageNumber(float world_x, float world_y, float world_z, int damage, DamageType type) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::DamageNumber;
    e->world_pos = glm::vec3(world_x, world_y, world_z);
    e->damage = damage;
    e->dmg_type = type;
    e->lifetime = (type == DamageType::Crit) ? 2.5f : 1.8f;
    e->priority = (type == DamageType::Crit) ? 2 : 1;
    return next_id_++;
}

uint32_t EffectManager::SpawnCameraShake(float intensity, float duration) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::CameraShake;
    e->shake_intensity = intensity;
    e->lifetime = duration;
    e->priority = 3;
    return next_id_++;
}

uint32_t EffectManager::SpawnAnimation(uint32_t entity_id, int anim_id) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::Animation;
    e->entity_id = entity_id;
    e->anim_id = anim_id;
    e->lifetime = 0.5f;
    e->priority = 4;
    return next_id_++;
}

uint32_t EffectManager::SpawnSound(const std::string& sound_name, float world_x, float world_y, float world_z) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::Sound;
    e->sound_name = sound_name;
    e->world_pos = glm::vec3(world_x, world_y, world_z);
    e->lifetime = 0.1f;
    e->priority = 1;
    return next_id_++;
}

uint32_t EffectManager::SpawnBillboard(float world_x, float world_y, float world_z, uint32_t color, float size, float duration) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::Billboard;
    e->world_pos = glm::vec3(world_x, world_y, world_z);
    e->billboard_color = color;
    e->billboard_size = size;
    e->lifetime = duration;
    e->priority = 1;
    return next_id_++;
}

uint32_t EffectManager::SpawnLight(const glm::vec3& color, float intensity, float range, float duration) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::Light;
    e->light.color = color;
    e->light.intensity = intensity;
    e->light.range = range;
    e->lifetime = duration;
    e->priority = 2;
    return next_id_++;
}

uint32_t EffectManager::SpawnModelEffect(const std::string& model_path, glm::vec3 pos, float scale, float duration) {
    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::Model;
    e->world_pos = pos;
    e->model_name = model_path;
    e->model_scale = scale;
    e->lifetime = duration;
    e->priority = 2;
    return next_id_++;
}

uint32_t EffectManager::SpawnLegacyEffect(const std::string& eft_path, glm::vec3 pos) {
    EftDefinition def = EftParser::Parse(eft_path);
    if (def.units.empty()) return 0;

    Effect* e = AcquireEffect();
    if (!e) return 0;
    e->type = EffectType::LegacyEft;
    e->world_pos = pos;
    e->prev_pos = pos;
    e->eft_def = def;
    e->lifetime = 10.0f;
    e->current_unit_idx = 0;
    e->unit_timer = 0;
    e->priority = 1;

    // Pre-spawn initial particles for each unit
    for (auto& unit : def.units) {
        if (unit.type == EftUnit::Particle) {
            for (int i = 0; i < unit.emit_rate; i++) {
                SpawnParticle(*e, unit);
            }
        }
    }

    return next_id_++;
}

void EffectManager::SpawnParticle(Effect& e, const EftUnit& unit) {
    ParticleInstance p;
    p.pos = e.world_pos + unit.pos_offset;
    p.start_pos = p.pos;
    p.life = unit.life_time + ((float)rand() / RAND_MAX) * 0.2f;
    p.color = unit.color_start;
    p.color_end = unit.color_end;
    p.start_size = unit.scale_start.x;
    p.end_size = unit.scale_end.x;
    p.size = p.start_size;
    p.alpha = unit.alpha_start;
    p.alpha_end = unit.alpha_end;
    p.additive = unit.additive;
    p.emit_index = (int)e.particles.size();

    // Random velocity
    float speed = unit.speed_min + ((float)rand() / RAND_MAX) * (unit.speed_max - unit.speed_min);
    float theta = ((float)rand() / RAND_MAX) * 6.283185f;
    float phi = ((float)rand() / RAND_MAX) * 3.141592f;
    p.vel.x = sinf(phi) * cosf(theta) * speed;
    p.vel.y = cosf(phi) * speed + unit.gravity * 0.5f;
    p.vel.z = sinf(phi) * sinf(theta) * speed;

    e.particles.push_back(p);
}

void EffectManager::UpdateLegacyEftUnit(Effect& e, float dt) {
    if (e.eft_def.units.empty()) return;

    e.unit_timer += dt;

    // Process current unit
    if (e.current_unit_idx < (int)e.eft_def.units.size()) {
        auto& unit = e.eft_def.units[e.current_unit_idx];

        // Emit particles at emit_rate per second
        float emit_interval = 1.0f / std::max(1, unit.emit_rate);
        if (unit.type == EftUnit::Particle && e.unit_timer >= emit_interval) {
            e.unit_timer -= emit_interval;
            SpawnParticle(e, unit);
        }

        // Process all particles
        for (auto& p : e.particles) {
            p.age += dt;
            if (p.age >= p.life) { p.active = false; continue; }

            float t = p.age / p.life;
            p.pos += p.vel * dt;
            p.vel.y += unit.gravity * dt;
            p.size = p.start_size + (p.end_size - p.start_size) * t;
            p.alpha = 1.0f + (p.alpha_end - 1.0f) * t;

            // Color blend
            if (p.color != p.color_end) {
                uint8_t r1 = (p.color >> 16) & 0xFF;
                uint8_t g1 = (p.color >> 8) & 0xFF;
                uint8_t b1 = p.color & 0xFF;
                uint8_t r2 = (p.color_end >> 16) & 0xFF;
                uint8_t g2 = (p.color_end >> 8) & 0xFF;
                uint8_t b2 = p.color_end & 0xFF;
                uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
                uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);
                uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);
                p.color = (0xFF << 24) | (b << 16) | (g << 8) | r;
            }
        }
    }

    // Clean dead particles
    e.particles.erase(std::remove_if(e.particles.begin(), e.particles.end(),
        [](auto& p) { return !p.active; }), e.particles.end());

    // Check if effect is done (all particles dead and all units processed)
    bool all_done = e.particles.empty();
    if (all_done && e.current_unit_idx >= (int)e.eft_def.units.size()) {
        e.active = false;
    }
}

void EffectManager::Update(float dt) {
    shake_offset_x_ = 0;
    shake_offset_y_ = 0;
    accumulated_light_ = glm::vec3(0);
    accumulated_light_intensity_ = 0;

    auto process_effect = [&](Effect& e) {
        if (!e.active) return;
        e.age += dt;
        e.prev_pos = e.world_pos;

        switch (e.type) {
        case EffectType::CameraShake: {
            float intensity = e.shake_intensity * (1.0f - e.age / e.lifetime);
            float angle = (float)(rand() % 628) / 100.0f;
            shake_offset_x_ += cosf(angle) * intensity;
            shake_offset_y_ += sinf(angle) * intensity;
            break;
        }
        case EffectType::Sound: {
            if (e.age < 0.05f && g_audio) {
                g_audio->PlaySFXByCategory(AudioManager::SFX_Hit, e.sound_name.c_str());
            }
            break;
        }
        case EffectType::Billboard: {
            float t = e.age / e.lifetime;
            e.screen_y -= 30.0f * dt; // float upward
            break;
        }
        case EffectType::Light: {
            float t = e.age / e.lifetime;
            float fade = (t < 0.1f) ? (t / 0.1f) : ((t > 0.9f) ? (1.0f - t) / 0.1f : 1.0f);
            accumulated_light_ += e.light.color * e.light.intensity * fade;
            accumulated_light_intensity_ += e.light.intensity * fade;
            break;
        }
        case EffectType::Model: {
            // Model is rendered via existing render path
            break;
        }
        case EffectType::Animation: {
            // Animation triggers are handled externally
            break;
        }
        case EffectType::LegacyEft: {
            UpdateLegacyEftUnit(e, dt);
            break;
        }
        default: break;
        }

        if (e.age >= e.lifetime && e.type != EffectType::LegacyEft) {
            e.active = false;
        }
    };

    // Process pooled effects
    for (auto& slot : pool_) {
        if (slot.in_use) process_effect(slot.effect);
    }

    // Process dynamic effects
    for (auto& e : dynamic_effects_) {
        process_effect(e);
    }

    RemoveInactive();
}

void EffectManager::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    // Extract camera position from inverse view matrix
    glm::mat4 inv_view = glm::inverse(view);
    glm::vec3 cam_pos(inv_view[3]);

    auto render_effect = [&](Effect& e) {
        if (!e.active) return;

        glm::ivec4 viewport(0, 0, (int)ui.width, (int)ui.height);

        switch (e.type) {
        case EffectType::DamageNumber: {
            float life_ratio = e.age / e.lifetime;
            float alpha = std::max(0.0f, 1.0f - life_ratio) * 255.0f;
            uint32_t color = 0;

            switch (e.dmg_type) {
            case DamageType::Normal: color = (int(alpha) << 24) | 0xffffff; break;
            case DamageType::Crit:   color = (int(alpha) << 24) | 0xff4444; break;
            case DamageType::Miss:   color = (int(alpha) << 24) | 0x888888; break;
            case DamageType::Heal:   color = (int(alpha) << 24) | 0x44ff44; break;
            }

            // Float upward in world-space Y
            glm::vec3 float_pos = e.world_pos;
            float_pos.y += e.age * 2.0f;
            glm::vec3 screen_pos = glm::project(float_pos, view, proj, viewport);
            float sx = screen_pos.x;
            float sy = ui.height - screen_pos.y;

            // Scale font size based on distance
            float dist = glm::distance(e.world_pos, cam_pos);
            float scale = std::min(1.5f, 30.0f / std::max(dist, 1.0f));

            if (e.dmg_type == DamageType::Crit) {
                ui.DrawText(sx - 50 * scale, sy - 12 * scale, color, "CRITICAL!");
                ui.DrawText(sx - 10 * scale, sy + 6 * scale, color, "%d", e.damage);
            } else if (e.dmg_type == DamageType::Miss) {
                ui.DrawText(sx - 10 * scale, sy, color, "MISS");
            } else {
                ui.DrawText(sx - 10 * scale, sy, color, "%d", e.damage);
            }
            break;
        }
        case EffectType::Billboard: {
            float life_ratio = e.age / e.lifetime;
            float alpha = std::max(0.0f, 1.0f - life_ratio);
            uint8_t a = (uint8_t)(alpha * 255);
            UIColor c = {
                (uint8_t)((e.billboard_color >> 16) & 0xFF),
                (uint8_t)((e.billboard_color >> 8) & 0xFF),
                (uint8_t)(e.billboard_color & 0xFF),
                a
            };

            // Float upward in world-space Y
            glm::vec3 float_pos = e.world_pos;
            float_pos.y += e.age * 2.0f;
            glm::vec3 screen_pos = glm::project(float_pos, view, proj, viewport);
            float sx = screen_pos.x;
            float sy = ui.height - screen_pos.y;

            // Scale size based on distance
            float dist = glm::distance(e.world_pos, cam_pos);
            float size = e.billboard_size * (30.0f / std::max(dist, 1.0f));

            if (sx > -size && sx < ui.width + size && sy > -size && sy < ui.height + size) {
                ui.DrawRect(sx - size * 0.5f, sy - size * 0.5f, size, size, c);
            }
            break;
        }
        case EffectType::LegacyEft: {
            for (auto& p : e.particles) {
                if (!p.active) continue;
                float life_ratio = p.age / p.life;
                float alpha = p.alpha * std::max(0.0f, 1.0f - life_ratio);
                if (alpha < 0.01f) continue;
                uint8_t a = (uint8_t)(alpha * 255);
                UIColor c = {
                    (uint8_t)((p.color >> 16) & 0xFF),
                    (uint8_t)((p.color >> 8) & 0xFF),
                    (uint8_t)(p.color & 0xFF),
                    a
                };

                // Project particle world position to screen
                glm::vec3 screen_pos = glm::project(p.pos, view, proj, viewport);
                float sx = screen_pos.x;
                float sy = ui.height - screen_pos.y;

                // Scale particle size based on distance
                float dist = glm::distance(p.pos, cam_pos);
                float draw_size = p.size * 4.0f * (30.0f / std::max(dist, 1.0f));

                if (sx > -64 && sx < ui.width + 64 && sy > -64 && sy < ui.height + 64) {
                    ui.DrawRect(sx - draw_size * 0.5f, sy - draw_size * 0.5f, draw_size, draw_size, c);
                }
            }
            break;
        }
        default: break;
        }
    };

    // Render pooled
    for (auto& slot : pool_) {
        if (slot.in_use) render_effect(slot.effect);
    }

    // Render dynamic
    for (auto& e : dynamic_effects_) {
        render_effect(e);
    }
}

void EffectManager::Clear() {
    for (auto& slot : pool_) {
        slot.in_use = false;
    }
    pool_.clear();
    dynamic_effects_.clear();
    active_count_ = 0;
}

void EffectManager::PreloadEft(const std::string& eft_path) {
    EftParser::Parse(eft_path);
}

void EffectManager::RemoveInactive() {
    // Release pooled inactive effects
    for (auto& slot : pool_) {
        if (slot.in_use && !slot.effect.active) {
            slot.effect = Effect{};
            slot.in_use = false;
            active_count_--;
        }
    }

    // Remove inactive dynamic effects
    dynamic_effects_.erase(
        std::remove_if(dynamic_effects_.begin(), dynamic_effects_.end(),
            [](auto& e) { return !e.active; }),
        dynamic_effects_.end());
}
