#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

// Parser for .eft (Effect Format Luna) files.
// These are text-based effect definition files from the original game.
// Format: key=value pairs defining particle emitters, triggers, and visuals.

struct EftUnit {
    enum Type { Particle, Animation, Billboard, Light, Sound, CameraShake, Model };
    Type type = Particle;
    std::string asset_name;
    float start_time = 0;
    float life_time = 1.0f;
    bool repeat = false;
    
    // Transform
    glm::vec3 pos_offset{0};
    glm::vec3 rot_offset{0};
    glm::vec3 scale_start{1}, scale_end{1};
    
    // Color/Alpha
    uint32_t color_start = 0xffffffff, color_end = 0xffffffff;
    float alpha_start = 1.0f, alpha_end = 1.0f;
    
    // Particle specific
    int emit_rate = 10;
    float speed_min = 1.0f, speed_max = 5.0f;
    float gravity = -9.8f;
    bool additive = false;
};

struct EftDefinition {
    std::string name;
    float total_duration = 0;
    bool loop = false;
    std::vector<EftUnit> units;
    uint32_t next_effect_id = 0;
};

class EftParser {
public:
    static EftDefinition Parse(const std::string& path);
    static std::vector<std::string> GetParsedEffectNames();

private:
    static std::unordered_map<std::string, EftDefinition> cache_;
    static std::string Trim(const std::string& s);
};
