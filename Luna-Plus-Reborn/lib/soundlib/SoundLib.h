#pragma once
// CSoundLib — miniaudio-backed sound library
// Port of Old Luna [Lib]SoundLib (Miles Sound System) to miniaudio.
// BGM uses streaming slots; SFX use preloaded in-memory slots with
// 50ms volume ramps to emulate Miles' click-free playback.

#include <vector>

struct ma_engine;
struct ma_sound;

enum class AttenuationModel {
    None,
    Inverse,
    Linear,
    Exponential,
};

class CSoundLib {
public:
    CSoundLib();
    ~CSoundLib();

    bool Init(int frequency = 44100, int channels = 2);
    void Shutdown();
    void Update();

    // BGM (streamed)
    int OpenBGM(const char* path);
    void PlayBGM(int id, bool loop = true);
    void StopBGM(int id);
    void SetBGMVolume(int id, float volume);
    void SetBGMPan(int id, float pan);
    bool IsBGMPlaying(int id) const;
    void CrossfadeBGM(int from_id, int to_id, float duration_ms);

    // SFX (preloaded)
    int LoadSFX(const char* path);
    void PlaySFX(int id, float volume = 1.0f, float pan = 0.0f, bool use_ramp = true);
    void StopSFX(int id);
    void SetSFXVolume(int id, float volume);
    void SetSFXPan(int id, float pan);
    bool IsSFXPlaying(int id) const;

    void SetMasterVolume(float volume);
    float GetMasterVolume() const;

    // 3D positional audio
    void SetListenerPosition(float pos_x, float pos_y, float pos_z,
                             float forward_x, float forward_y, float forward_z,
                             float up_x = 0.0f, float up_y = 1.0f, float up_z = 0.0f);
    void SetSoundPosition(int id, float pos_x, float pos_y, float pos_z, bool is_bgm = false);
    void SetSoundDistanceModel(int id, AttenuationModel model, float rolloff,
                               float min_dist, float max_dist, bool is_bgm = false);
    void SetSoundVelocity(int id, float vel_x, float vel_y, float vel_z, bool is_bgm = false);

private:
    struct Slot {
        ma_sound* sound = nullptr;
        int id = -1;
        bool in_use = false;
        float target_volume = 1.0f;
    };

    struct VolumeRamp {
        int sfx_id = -1;
        float current = 0.0f;
        float target = 1.0f;
        float duration_ms = 50.0f;
        float elapsed_ms = 0.0f;
        bool active = false;
    };

    Slot* FindSlot(int id, bool bgm);
    static void SetSoundAttenuation(ma_sound* s, AttenuationModel model,
                                    float rolloff, float min_dist, float max_dist);

    ma_engine* engine_ = nullptr;
    bool initialized_ = false;
    float master_volume_ = 1.0f;

    std::vector<Slot> bgm_slots_;
    std::vector<Slot> sfx_slots_;
    std::vector<VolumeRamp> volume_ramps_;
    int next_bgm_id_ = 1;
    int next_sfx_id_ = 1000;

    bool crossfade_active_ = false;
    int crossfade_from_id_ = -1;
    int crossfade_to_id_ = -1;
    float crossfade_duration_ms_ = 0.0f;
    float crossfade_elapsed_ms_ = 0.0f;
};
