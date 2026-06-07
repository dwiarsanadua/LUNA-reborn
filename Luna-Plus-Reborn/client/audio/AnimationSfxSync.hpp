#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

struct AnimSfxEntry {
    std::string anim_name;
    int frame = 0;          // Frame number to trigger SFX
    std::string sfx_name;
    std::string category;   // "hit", "footstep", "skill", "voice"
    float volume = 1.0f;
    float min_interval = 0; // Minimum time between repeats
};

class AnimationSfxSync {
public:
    void Init();
    
    // Register SFX for animation frames
    void Register(const std::string& anim_name, int frame, const std::string& sfx_name,
                  const std::string& category = "hit", float volume = 1.0f);
    
    // Trigger SFX for current animation frame
    void OnAnimationFrame(const std::string& anim_name, int current_frame, float x, float y, float z);
    
    // Footstep SFX based on surface type
    void OnFootstep(const std::string& surface_type, float x, float y, float z);
    
    // Weapon swing SFX
    void OnWeaponSwing(const std::string& weapon_type, float x, float y, float z);
    
    // Advance time (for interval tracking)
    void Update(float dt) { current_time_ += dt; }
    
    // Load default SFX mappings
    void LoadDefaults();
    
    int GetEntryCount() const { return (int)sfx_timings_.size(); }

private:
    struct AnimSfxTiming {
        std::string anim_name;
        int frame;
        std::string sfx;
        std::string category;
        float volume;
        float last_played; // timestamp to enforce min_interval
    };
    std::vector<AnimSfxTiming> sfx_timings_;
    std::unordered_map<std::string, std::string> footstep_sfx_;
    std::unordered_map<std::string, std::string> weapon_swing_sfx_;
    float current_time_ = 0;
    
    void PlaySfx(const std::string& name, const std::string& category, float x, float y, float z, float volume);
};
