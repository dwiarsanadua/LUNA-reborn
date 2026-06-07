#pragma once
#include <string>
#include <unordered_map>

class CSoundLib;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool Initialize();
    void Shutdown();

    enum Category {
        BGM_Map, BGM_Combat,
        SFX_Skill, SFX_Hit, SFX_UI, SFX_Ambient, SFX_NPC, SFX_Monster,
        SFX_Weapon, SFX_Character, SFX_Footstep
    };

    // Auto-scan audio directory and play ambient sounds
    void ScanAudioDirectory();
    int GetTotalSFXLoaded() const { return sfx_count_; }

    void PlayBGM(const std::string& map_id);
    void StopBGM();
    void PlaySFX(const std::string& name);
    void PlaySFXByCategory(Category cat, const std::string& name);
    void Play3D(const std::string& name, float x, float y, float z);

    void SetMasterVolume(float vol);
    void SetBGMVolume(float vol);
    void SetSFXVolume(float vol);

    void Update();

private:
    CSoundLib* sound_lib_ = nullptr;
    int sfx_count_ = 0;
    int current_bgm_id_ = -1;
    float master_vol_ = 1.0f;
    float bgm_vol_ = 0.7f;
    float sfx_vol_ = 1.0f;
    std::string current_bgm_;
};
