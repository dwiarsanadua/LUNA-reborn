#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include "EftParser.hpp"

enum class EffectType {
    DamageNumber,
    CameraShake,
    Animation,
    Sound,
    Billboard,
    LegacyEft,
    Light,
    Model
};

enum class DamageType {
    Normal,
    Crit,
    Miss,
    Heal
};

struct ParticleInstance {
    glm::vec3 pos{0}, vel{0};
    glm::vec3 start_pos{0};
    uint32_t color = 0xffffffff;
    uint32_t color_end = 0x00000000;
    float size = 1.0f;
    float start_size = 1.0f;
    float end_size = 0.0f;
    float age = 0, life = 1.0f;
    float rot = 0;
    float alpha = 1.0f;
    float alpha_end = 0.0f;
    std::string texture;
    bool active = true;
    bool additive = false;
    int emit_index = 0;
};

struct EffectLight {
    glm::vec3 color{1, 1, 1};
    float intensity = 1.0f;
    float range = 10.0f;
};

struct Effect {
    EffectType type;
    float lifetime = 2.0f;
    float age = 0;
    bool active = true;
    int priority = 0; // higher = more important
    bool pooled = false; // is from object pool
    
    // Position
    glm::vec3 world_pos{0};
    glm::vec3 prev_pos{0};
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
    int current_unit_idx = 0;
    float unit_timer = 0;
    
    // Sound data
    std::string sound_name;
    int sound_handle = -1;
    
    // Billboard data
    uint32_t billboard_color = 0xffffffff;
    float billboard_size = 16;
    std::string billboard_texture;
    bool billboard_faces_camera = true;
    
    // Light data
    EffectLight light;
    
    // Model data
    std::string model_name;
    uint32_t model_instance_id = 0;
    float model_scale = 1.0f;
    
    // Callback on complete
    std::function<void()> on_complete;
};

struct EffectPoolSlot {
    Effect effect;
    bool in_use = false;
};

class EffectManager {
public:
    EffectManager();
    ~EffectManager();
    
    void Init(int pool_size = 256);
    void Update(float dt);
    void Render(class UIRenderer& ui, const glm::mat4& view, const glm::mat4& proj);
    
    // Spawn effects
    uint32_t SpawnDamageNumber(float world_x, float world_y, float world_z, int damage, DamageType type = DamageType::Normal);
    uint32_t SpawnCameraShake(float intensity = 5.0f, float duration = 0.3f);
    uint32_t SpawnAnimation(uint32_t entity_id, int anim_id);
    uint32_t SpawnSound(const std::string& sound_name, float world_x = 0, float world_y = 0, float world_z = 0);
    uint32_t SpawnBillboard(float world_x, float world_y, float world_z, uint32_t color, float size = 16, float duration = 2.0f);
    uint32_t SpawnLight(const glm::vec3& color, float intensity, float range, float duration);
    uint32_t SpawnModelEffect(const std::string& model_path, glm::vec3 pos, float scale = 1.0f, float duration = 2.0f);
    uint32_t SpawnLegacyEffect(const std::string& eft_path, glm::vec3 pos);
    
    // Query
    float GetShakeOffsetX() const { return shake_offset_x_; }
    float GetShakeOffsetY() const { return shake_offset_y_; }
    int GetActiveCount() const { return active_count_; }
    
    void Clear();
    void PreloadEft(const std::string& eft_path);
    
private:
    // Object pool
    std::vector<EffectPoolSlot> pool_;
    int pool_size_ = 256;
    int active_count_ = 0;
    
    // Dynamic effects (non-pooled, e.g. for overflow)
    std::vector<Effect> dynamic_effects_;
    
    uint32_t next_id_ = 1;
    float shake_offset_x_ = 0;
    float shake_offset_y_ = 0;
    
    // Light accumulators
    glm::vec3 accumulated_light_{0};
    float accumulated_light_intensity_ = 0;
    
    Effect* AcquireEffect();
    void ReleaseEffect(uint32_t id);
    void RemoveInactive();
    
    void ProcessUnit(Effect& e, const EftUnit& unit, float dt);
    void SpawnParticle(Effect& e, const EftUnit& unit);
    void UpdateLegacyEftUnit(Effect& e, float dt);
};
