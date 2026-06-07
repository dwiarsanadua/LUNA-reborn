#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include "EftParser.hpp"

enum class EffectType {
    DamageNumber,
    CameraShake,
    Animation,
    Sound,
    Billboard,
    LegacyEft
};

enum class DamageType {
    Normal,
    Crit,
    Miss,
    Heal
};

struct ParticleInstance {
    glm::vec3 pos{0}, vel{0};
    uint32_t color = 0xffffffff;
    float size = 1.0f;
    float age = 0, life = 1.0f;
    std::string texture;
    bool active = true;
};

struct Effect {
    EffectType type;
    float lifetime = 2.0f;
    float age = 0;
    bool active = true;
    int priority = 0; // higher = more important
    
    // Position
    glm::vec3 world_pos{0};
    float screen_x = 0, screen_y = 0;
    
    // Entity binding
    uint32_t entity_id = UINT32_MAX;
    
    // Damage number data
    int damage = 0;
    DamageType dmg_type = DamageType::Normal;
    
    // Camera shake data
    float shake_intensity = 0;
    float shake_decay = 2.0f;
    
    // Animation data
    int anim_id = 0;
    
    // Legacy EFT data (5.1A)
    EftDefinition eft_def;
    std::vector<ParticleInstance> particles;
    
    // Sound data
    std::string sound_name;
    
    // Billboard data
    uint32_t billboard_color = 0xffffffff;
    float billboard_size = 16;
    
    // Callback on complete
    std::function<void()> on_complete;
};

class EffectManager {
public:
    EffectManager() = default;
    
    void Update(float dt);
    void Render(class UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj);
    
    // Spawn effects
    uint32_t SpawnDamageNumber(float world_x, float world_y, float world_z, int damage, DamageType type = DamageType::Normal);
    uint32_t SpawnCameraShake(float intensity = 5.0f, float duration = 0.3f);
    uint32_t SpawnAnimation(uint32_t entity_id, int anim_id);
    uint32_t SpawnSound(const std::string& sound_name, float world_x = 0, float world_y = 0, float world_z = 0);
    uint32_t SpawnBillboard(float world_x, float world_y, float world_z, uint32_t color, float size = 16, float duration = 2.0f);
    uint32_t SpawnLegacyEffect(const std::string& eft_path, glm::vec3 pos);
    
    // Query
    float GetShakeOffsetX() const { return shake_offset_x_; }
    float GetShakeOffsetY() const { return shake_offset_y_; }
    
    void Clear();
    
private:
    std::vector<Effect> effects_;
    uint32_t next_id_ = 1;
    float shake_offset_x_ = 0;
    float shake_offset_y_ = 0;
    
    void RemoveInactive();
};
