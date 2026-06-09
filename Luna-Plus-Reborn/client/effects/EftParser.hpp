#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>

enum class EftTargetType : uint8_t {
    Self = 0,
    Target = 1,
    Position = 2,
    Forward = 3,
    RandomInRange = 4,
};

struct EftAttribute {
    int32_t damage = 0;
    float damage_mult = 1.0f;
    float stun_chance = 0.0f;
    float stun_duration = 0.0f;
    float poison_damage = 0.0f;
    float poison_duration = 0.0f;
    float slow_amount = 0.0f;
    float slow_duration = 0.0f;
    float knockback_distance = 0.0f;
    int32_t heal_amount = 0;
    float heal_mult = 0.0f;
    uint32_t buff_id = 0;
    float buff_duration = 0.0f;
};

struct EftUnit {
    enum Type : uint8_t {
        Particle = 0,
        Animation = 1,
        Billboard = 2,
        Light = 3,
        Sound = 4,
        CameraShake = 5,
        Model = 6,
        Decal = 7,
    };
    Type type = Particle;
    std::string asset_name;
    float start_time = 0.0f;
    float life_time = 1.0f;
    float fade_in = 0.0f;
    float fade_out = 0.0f;
    bool repeat = false;
    int repeat_count = 1;
    float repeat_interval = 0.0f;
    bool follow_target = false;
    bool follow_bone = false;
    std::string bone_name;

    // Transform
    glm::vec3 pos_offset{0};
    glm::vec3 rot_offset{0};
    glm::vec3 scale_start{1}, scale_end{1};
    glm::vec3 direction{0, 0, 1};

    // Color/Alpha
    uint32_t color_start = 0xffffffff, color_end = 0xffffffff;
    float alpha_start = 1.0f, alpha_end = 1.0f;

    // Particle specific
    int emit_rate = 10;
    int emit_count = 0;
    float speed_min = 1.0f, speed_max = 5.0f;
    float gravity = -9.8f;
    bool additive = false;
    bool billboard = false;

    // Sound specific
    float volume = 1.0f;
    float pitch = 1.0f;
    float range = 30.0f;
    bool loop_sound = false;

    // Camera shake specific
    float shake_intensity = 1.0f;
    float shake_frequency = 10.0f;

    // Light specific
    float light_range = 10.0f;
    float light_intensity = 1.0f;

    // Attributes (damage/buff effects)
    EftAttribute attr;
    EftTargetType target_type = EftTargetType::Target;

    // Chaining
    uint32_t next_effect_id = 0;
    float next_delay = 0.0f;
};

struct EftDefinition {
    std::string name;
    float total_duration = 0.0f;
    float fade_in = 0.0f;
    float fade_out = 0.0f;
    bool loop = false;
    int loop_count = 1;
    EftTargetType default_target = EftTargetType::Target;
    std::vector<EftUnit> units;
};

class EftParser {
public:
    static EftDefinition Parse(const std::string& path);
    static std::vector<std::string> GetParsedEffectNames();

private:
    static std::unordered_map<std::string, EftDefinition> cache_;
    static std::string Trim(const std::string& s);
    static EftUnit::Type ParseUnitType(const std::string& token);
    static uint32_t ParseColor(const std::string& str);
    static bool ParseBool(const std::string& str);
    static float ParseFloat(const std::string& str, float def = 0.0f);
    static int ParseInt(const std::string& str, int def = 0);
};
