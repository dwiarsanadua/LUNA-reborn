#include "EffectManager.hpp"
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <audio/AudioManager.hpp>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstdio>

extern AudioManager* g_audio;

uint32_t EffectManager::SpawnDamageNumber(float world_x, float world_y, float world_z, int damage, DamageType type) {
    Effect e;
    e.type = EffectType::DamageNumber;
    e.world_pos = glm::vec3(world_x, world_y, world_z);
    e.damage = damage;
    e.dmg_type = type;
    e.lifetime = (type == DamageType::Crit) ? 2.5f : 1.8f;
    e.priority = (type == DamageType::Crit) ? 2 : 1;
    effects_.push_back(e);
    return next_id_++;
}

uint32_t EffectManager::SpawnCameraShake(float intensity, float duration) {
    Effect e;
    e.type = EffectType::CameraShake;
    e.shake_intensity = intensity;
    e.lifetime = duration;
    e.priority = 3;
    effects_.push_back(e);
    return next_id_++;
}

uint32_t EffectManager::SpawnAnimation(uint32_t entity_id, int anim_id) {
    Effect e;
    e.type = EffectType::Animation;
    e.entity_id = entity_id;
    e.anim_id = anim_id;
    e.lifetime = 0.5f;
    e.priority = 4;
    effects_.push_back(e);
    return next_id_++;
}

uint32_t EffectManager::SpawnSound(const std::string& sound_name, float world_x, float world_y, float world_z) {
    Effect e;
    e.type = EffectType::Sound;
    e.sound_name = sound_name;
    e.world_pos = glm::vec3(world_x, world_y, world_z);
    e.lifetime = 0.1f; // Instant
    e.priority = 1;
    effects_.push_back(e);
    return next_id_++;
}

uint32_t EffectManager::SpawnBillboard(float world_x, float world_y, float world_z, uint32_t color, float size, float duration) {
    Effect e;
    e.type = EffectType::Billboard;
    e.world_pos = glm::vec3(world_x, world_y, world_z);
    e.billboard_color = color;
    e.billboard_size = size;
    e.lifetime = duration;
    e.priority = 1;
    effects_.push_back(e);
    return next_id_++;
}

uint32_t EffectManager::SpawnLegacyEffect(const std::string& eft_path, glm::vec3 pos) {
    EftDefinition def = EftParser::Parse(eft_path);
    if (def.units.empty()) return 0;

    Effect e;
    e.type = EffectType::LegacyEft;
    e.world_pos = pos;
    e.eft_def = def;
    e.lifetime = 5.0f; // Max safety
    
    for (auto& unit : def.units) {
        if (unit.type == EftUnit::Particle) {
            for (int i = 0; i < unit.emit_rate; i++) {
                ParticleInstance p;
                p.pos = pos + unit.pos_offset;
                p.life = unit.life_time;
                p.color = unit.color_start;
                p.texture = unit.asset_name;
                e.particles.push_back(p);
            }
        }
    }
    
    effects_.push_back(e);
    return next_id_++;
}

void EffectManager::Update(float dt) {
    shake_offset_x_ = 0;
    shake_offset_y_ = 0;
    
    for (auto& e : effects_) {
        if (!e.active) continue;
        e.age += dt;
        
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
        case EffectType::LegacyEft: {
            for (auto& p : e.particles) {
                p.age += dt;
                if (p.age >= p.life) p.active = false;
                p.pos += p.vel * dt;
            }
            break;
        }
        default: break;
        }
        
        if (e.age >= e.lifetime) e.active = false;
    }
    
    RemoveInactive();
}

void EffectManager::Render(UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj) {
    (void)view; (void)proj;
    
    for (auto& e : effects_) {
        if (!e.active) continue;
        
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
            
            float sy = 240.0f - e.age * 50.0f;
            
            if (e.dmg_type == DamageType::Crit) {
                ui.DrawText(580, sy - 12, color, "CRITICAL!");
                ui.DrawText(598, sy + 6, color, "%d", e.damage);
            } else if (e.dmg_type == DamageType::Miss) {
                ui.DrawText(600, sy, color, "MISS");
            } else {
                ui.DrawText(600, sy, color, "%d", e.damage);
            }
            break;
        }
        case EffectType::Billboard: {
            // Simple 2D overlay at fixed screen position
            float life_ratio = e.age / e.lifetime;
            float alpha = std::max(0.0f, 1.0f - life_ratio);
            uint8_t a = (uint8_t)(alpha * 255);
            UIColor c = {
                (uint8_t)((e.billboard_color >> 16) & 0xFF),
                (uint8_t)((e.billboard_color >> 8) & 0xFF),
                (uint8_t)(e.billboard_color & 0xFF),
                a
            };
            // Project world to screen (simplified)
            float sx = (e.world_pos.x * 12.0f + 640.0f) - e.billboard_size * 0.5f;
            float sy = (e.world_pos.z * 12.0f + 360.0f) - e.billboard_size * 0.5f - e.age * 20.0f;
            if (sx > -e.billboard_size && sx < 1280 + e.billboard_size && sy > -e.billboard_size && sy < 720 + e.billboard_size) {
                ui.DrawRect(sx, sy, e.billboard_size, e.billboard_size, c);
            }
            break;
        }
        case EffectType::LegacyEft: {
            for (auto& p : e.particles) {
                if (!p.active) continue;
                float life_ratio = p.age / p.life;
                float alpha = std::max(0.0f, 1.0f - life_ratio);
                uint8_t a = (uint8_t)(alpha * 255);
                UIColor c = {
                    (uint8_t)((p.color >> 16) & 0xFF),
                    (uint8_t)((p.color >> 8) & 0xFF),
                    (uint8_t)(p.color & 0xFF),
                    a
                };
                
                // Simplified 3D to 2D projection
                float sx = (p.pos.x * 12.0f + 640.0f) - p.size * 0.5f;
                float sy = (p.pos.z * 12.0f + 360.0f) - p.size * 0.5f;
                
                if (sx > -64 && sx < 1280 + 64 && sy > -64 && sy < 720 + 64) {
                    ui.DrawRect(sx, sy, p.size * 4.0f, p.size * 4.0f, c);
                }
            }
            break;
        }
        default: break;
        }
    }
}

void EffectManager::Clear() {
    effects_.clear();
}

void EffectManager::RemoveInactive() {
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
            [](auto& e) { return !e.active; }),
        effects_.end());
}
