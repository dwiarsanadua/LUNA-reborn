#include "AnimationSfxSync.hpp"
#include "AudioManager.hpp"
#include <spdlog/spdlog.h>

void AnimationSfxSync::Init() {
    sfx_timings_.clear();
    footstep_sfx_.clear();
    weapon_swing_sfx_.clear();
    LoadDefaults();
    spdlog::info("AnimationSfxSync: initialized with {} entries", sfx_timings_.size());
}

void AnimationSfxSync::Register(const std::string& anim_name, int frame, const std::string& sfx_name,
                                 const std::string& category, float volume) {
    AnimSfxTiming t;
    t.anim_name = anim_name; t.frame = frame; t.sfx = sfx_name;
    t.category = category; t.volume = volume; t.last_played = -999;
    sfx_timings_.push_back(t);
}

void AnimationSfxSync::OnAnimationFrame(const std::string& anim_name, int current_frame, float x, float y, float z) {
    for (auto& t : sfx_timings_) {
        if (t.anim_name == anim_name && t.frame == current_frame) {
            if (current_time_ - t.last_played > 0.1f) {
                PlaySfx(t.sfx, t.category, x, y, z, t.volume);
                t.last_played = current_time_;
            }
        }
    }
}

void AnimationSfxSync::OnFootstep(const std::string& surface_type, float x, float y, float z) {
    auto it = footstep_sfx_.find(surface_type);
    if (it != footstep_sfx_.end()) PlaySfx(it->second, "footstep", x, y, z, 0.7f);
    else PlaySfx("Footstep_Default.wav", "footstep", x, y, z, 0.7f);
}

void AnimationSfxSync::OnWeaponSwing(const std::string& weapon_type, float x, float y, float z) {
    auto it = weapon_swing_sfx_.find(weapon_type);
    if (it != weapon_swing_sfx_.end()) PlaySfx(it->second, "weapon", x, y, z, 1.0f);
    else PlaySfx("Weapon_NoWeapon_Upper.wav", "weapon", x, y, z, 1.0f);
}

void AnimationSfxSync::PlaySfx(const std::string& name, const std::string& category, float x, float y, float z, float volume) {
    (void)category; (void)volume;
    extern AudioManager* g_audio;
    if (g_audio) g_audio->Play3D(name, x, y, z);
}

void AnimationSfxSync::LoadDefaults() {
    // Footstep SFX per surface (actual files in audio/Character/)
    footstep_sfx_["grass"] = "Char_Footstep_Run_01.wav";
    footstep_sfx_["stone"] = "Char_Footstep_Run_02.wav";
    footstep_sfx_["wood"] = "Char_Footstep_Run_03.wav";
    footstep_sfx_["water"] = "Char_Footstep_Walk_01.wav";
    footstep_sfx_["default"] = "Char_Footstep_Run_01.wav";
    
    // Weapon swing SFX (actual files in audio/Weapon/)
    weapon_swing_sfx_["sword"] = "Weapon_TwoHand_01.wav";
    weapon_swing_sfx_["axe"] = "Weapon_TwoHand_02.wav";
    weapon_swing_sfx_["staff"] = "Weapon_TwoHand_03.wav";
    weapon_swing_sfx_["bow"] = "Weapon_Bow_01.wav";
    weapon_swing_sfx_["dagger"] = "Weapon_Dagger_01.wav";
    weapon_swing_sfx_["default"] = "Weapon_NoWeapon_Upper.wav";
    
    // Attack animation SFX timings
    Register("attack", 3, "Weapon_NoWeapon_Upper.wav", "hit");
    Register("attack", 5, "Weapon_NoWeapon_Upper.wav", "hit");
    Register("skill_cast", 2, "Weapon_TwoHand_01.wav", "skill");
    Register("skill_hit", 1, "Weapon_NoWeapon_Upper.wav", "skill");
    Register("die", 2, "Weapon_NoWeapon_Upper.wav", "voice");
    Register("levelup", 0, "button_ok.wav", "ui");
}
