#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

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
    void PlaySFXInst(const std::string& name, float x, float y, float z);

    // 3D Audio Spatial
    int Play3D(const std::string& name, float x, float y, float z);
    void SetSoundPosition(int handle, float x, float y, float z);
    void SetListenerPosition(float x, float y, float z);
    void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
    void StopSound(int handle);

    // Distance model
    void SetDistanceModel(int model);
    void SetDopplerFactor(float factor);

    void SetMasterVolume(float vol);
    void SetBGMVolume(float vol);
    void SetSFXVolume(float vol);

    void Update();

private:
    struct SoundInstance {
        int handle = -1;
        int sound_lib_id = -1;
        glm::vec3 position{0};
        glm::vec3 velocity{0};
        float max_distance = 100.0f;
        float reference_distance = 10.0f;
        float rolloff = 1.0f;
        bool is_3d = false;
        bool active = true;
    };

    CSoundLib* sound_lib_ = nullptr;
    int sfx_count_ = 0;
    int current_bgm_id_ = -1;
    float master_vol_ = 1.0f;
    float bgm_vol_ = 0.7f;
    float sfx_vol_ = 1.0f;
    std::string current_bgm_;

    // 3D Audio state
    glm::vec3 listener_pos_{0};
    glm::vec3 listener_vel_{0};
    glm::vec3 listener_forward_{0, 0, -1};
    glm::vec3 listener_up_{0, 1, 0};
    glm::vec3 prev_listener_pos_{0};
    int distance_model_ = 0; // 0 = linear
    float doppler_factor_ = 1.0f;
    int next_sound_handle_ = 1;
    std::unordered_map<int, SoundInstance> active_sounds_;

    int CalculateAttenuation(const SoundInstance& snd) const;
};
