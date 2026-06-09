#define MINIAUDIO_IMPLEMENTATION
#include "SoundLib.h"
#include <miniaudio.h>
#include <spdlog/spdlog.h>
#include <algorithm>

constexpr int MAX_BGM_SLOTS = 4;
constexpr int MAX_SFX_SLOTS = 64;
constexpr float RAMP_DURATION_MS = 50.0f;

CSoundLib::CSoundLib() {
    bgm_slots_.resize(MAX_BGM_SLOTS);
    sfx_slots_.resize(MAX_SFX_SLOTS);
    volume_ramps_.reserve(MAX_SFX_SLOTS);
}

CSoundLib::~CSoundLib() {
    Shutdown();
}

bool CSoundLib::Init(int frequency, int channels) {
    if (initialized_) {
        spdlog::warn("SoundLib: already initialized");
        return true;
    }

    engine_ = new ma_engine;
    ma_engine_config config = ma_engine_config_init();
    config.sampleRate = (unsigned int)frequency;
    config.channels = (unsigned int)channels;

    ma_result result = ma_engine_init(&config, engine_);
    if (result != MA_SUCCESS) {
        spdlog::error("SoundLib: failed to initialize miniaudio engine (error: {})", (int)result);
        delete engine_;
        engine_ = nullptr;
        return false;
    }

    initialized_ = true;
    spdlog::info("SoundLib: initialized ({} Hz, {} channels)", frequency, channels);
    return true;
}

void CSoundLib::Shutdown() {
    if (!initialized_) return;
    volume_ramps_.clear();
    crossfade_active_ = false;

    for (auto& slot : bgm_slots_) {
        if (slot.sound) {
            ma_sound_stop(slot.sound);
            ma_sound_uninit(slot.sound);
            delete slot.sound;
            slot.sound = nullptr;
        }
        slot.in_use = false;
    }

    for (auto& slot : sfx_slots_) {
        if (slot.sound) {
            ma_sound_stop(slot.sound);
            ma_sound_uninit(slot.sound);
            delete slot.sound;
            slot.sound = nullptr;
        }
        slot.in_use = false;
    }

    if (engine_) {
        ma_engine_uninit(engine_);
        delete engine_;
        engine_ = nullptr;
    }

    initialized_ = false;
    spdlog::info("SoundLib: shutdown");
}

CSoundLib::Slot* CSoundLib::FindSlot(int id, bool bgm) {
    auto& slots = bgm ? bgm_slots_ : sfx_slots_;
    for (auto& slot : slots) {
        if (slot.in_use && slot.id == id) return &slot;
    }
    return nullptr;
}

int CSoundLib::OpenBGM(const char* path) {
    if (!engine_ || !path) return -1;

    for (auto& slot : bgm_slots_) {
        if (slot.in_use) continue;

        auto* sound = new ma_sound;
        ma_result result = ma_sound_init_from_file(engine_, path,
            MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound);
        if (result != MA_SUCCESS) {
            spdlog::error("SoundLib: failed to load BGM '{}' (error: {})", path, (int)result);
            delete sound;
            return -1;
        }

        slot.sound = sound;
        slot.id = next_bgm_id_++;
        slot.in_use = true;
        slot.target_volume = 1.0f;
        spdlog::debug("SoundLib: loaded BGM '{}' (id={})", path, slot.id);
        return slot.id;
    }

    spdlog::warn("SoundLib: no free BGM slot for '{}'", path);
    return -1;
}

void CSoundLib::PlayBGM(int id, bool loop) {
    auto* slot = FindSlot(id, true);
    if (!slot || !slot->sound) return;
    ma_sound_set_looping(slot->sound, loop);
    ma_sound_set_volume(slot->sound, slot->target_volume * master_volume_);
    ma_sound_start(slot->sound);
}

void CSoundLib::StopBGM(int id) {
    auto* slot = FindSlot(id, true);
    if (!slot || !slot->sound) return;
    ma_sound_stop(slot->sound);
}

void CSoundLib::SetBGMVolume(int id, float volume) {
    auto* slot = FindSlot(id, true);
    if (!slot) return;
    slot->target_volume = volume;
    if (slot->sound) {
        ma_sound_set_volume(slot->sound, volume * master_volume_);
    }
}

void CSoundLib::SetBGMPan(int id, float pan) {
    auto* slot = FindSlot(id, true);
    if (!slot || !slot->sound) return;
    ma_sound_set_pan(slot->sound, pan);
}

bool CSoundLib::IsBGMPlaying(int id) const {
    for (const auto& slot : bgm_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            return ma_sound_is_playing(slot.sound);
        }
    }
    return false;
}

void CSoundLib::CrossfadeBGM(int from_id, int to_id, float duration_ms) {
    auto* from_slot = FindSlot(from_id, true);
    auto* to_slot = FindSlot(to_id, true);
    if (!to_slot || !to_slot->sound) return;

    if (from_slot && from_slot->sound) {
        ma_sound_set_fade_in_pcm_frames(from_slot->sound, ma_sound_get_volume(from_slot->sound), 0.0f,
            static_cast<ma_uint64>(duration_ms * 44.1f));
    }

    ma_sound_set_fade_in_pcm_frames(to_slot->sound, 0.0f, to_slot->target_volume * master_volume_,
        static_cast<ma_uint64>(duration_ms * 44.1f));
    ma_sound_start(to_slot->sound);

    crossfade_active_ = true;
    crossfade_from_id_ = from_id;
    crossfade_to_id_ = to_id;
    crossfade_duration_ms_ = duration_ms;
    crossfade_elapsed_ms_ = 0.0f;
}

int CSoundLib::LoadSFX(const char* path) {
    if (!engine_ || !path) return -1;

    for (auto& slot : sfx_slots_) {
        if (slot.in_use) continue;

        auto* sound = new ma_sound;
        ma_result result = ma_sound_init_from_file(engine_, path, 0, nullptr, nullptr, sound);
        if (result != MA_SUCCESS) {
            spdlog::error("SoundLib: failed to load SFX '{}' (error: {})", path, (int)result);
            delete sound;
            return -1;
        }

        slot.sound = sound;
        slot.id = next_sfx_id_++;
        slot.in_use = true;
        slot.target_volume = 1.0f;
        spdlog::debug("SoundLib: loaded SFX '{}' (id={})", path, slot.id);
        return slot.id;
    }

    spdlog::warn("SoundLib: no free SFX slot for '{}'", path);
    return -1;
}

void CSoundLib::PlaySFX(int id, float volume, float pan, bool use_ramp) {
    auto* slot = FindSlot(id, false);
    if (!slot || !slot->sound) return;

    float target = volume * master_volume_;
    ma_sound_set_pan(slot->sound, pan);
    ma_sound_stop(slot->sound);
    ma_sound_seek_to_pcm_frame(slot->sound, 0);

    if (use_ramp) {
        ma_sound_set_volume(slot->sound, 0.0f);
        ma_sound_start(slot->sound);
        ma_sound_set_fade_in_pcm_frames(slot->sound, 0.0f, target,
            static_cast<ma_uint64>(RAMP_DURATION_MS * 44.1f));

        VolumeRamp ramp;
        ramp.sfx_id = id;
        ramp.current = 0.0f;
        ramp.target = target;
        ramp.duration_ms = RAMP_DURATION_MS;
        ramp.elapsed_ms = 0.0f;
        ramp.active = true;
        volume_ramps_.push_back(ramp);
    } else {
        ma_sound_set_volume(slot->sound, target);
        ma_sound_start(slot->sound);
    }
}

void CSoundLib::StopSFX(int id) {
    auto* slot = FindSlot(id, false);
    if (!slot || !slot->sound) return;
    ma_sound_stop(slot->sound);

    volume_ramps_.erase(
        std::remove_if(volume_ramps_.begin(), volume_ramps_.end(),
            [id](const VolumeRamp& r) { return r.sfx_id == id; }),
        volume_ramps_.end());
}

void CSoundLib::SetSFXVolume(int id, float volume) {
    auto* slot = FindSlot(id, false);
    if (!slot) return;
    slot->target_volume = volume;
    if (slot->sound) {
        ma_sound_set_volume(slot->sound, volume * master_volume_);
    }
}

void CSoundLib::SetSFXPan(int id, float pan) {
    auto* slot = FindSlot(id, false);
    if (!slot || !slot->sound) return;
    ma_sound_set_pan(slot->sound, pan);
}

bool CSoundLib::IsSFXPlaying(int id) const {
    for (const auto& slot : sfx_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            return ma_sound_is_playing(slot.sound);
        }
    }
    return false;
}

void CSoundLib::SetMasterVolume(float volume) {
    master_volume_ = std::clamp(volume, 0.0f, 1.0f);
    if (engine_) {
        ma_engine_set_volume(engine_, master_volume_);
    }
}

float CSoundLib::GetMasterVolume() const {
    return master_volume_;
}

void CSoundLib::SetListenerPosition(float pos_x, float pos_y, float pos_z,
                                     float forward_x, float forward_y, float forward_z,
                                     float up_x, float up_y, float up_z) {
    if (!engine_) return;
    ma_engine_listener_set_position(engine_, 0, pos_x, pos_y, pos_z);
    ma_engine_listener_set_direction(engine_, 0, forward_x, forward_y, forward_z);
    ma_engine_listener_set_world_up(engine_, 0, up_x, up_y, up_z);
}

void CSoundLib::SetSoundPosition(int id, float pos_x, float pos_y, float pos_z, bool is_bgm) {
    auto* slot = FindSlot(id, is_bgm);
    if (!slot || !slot->sound) return;
    ma_sound_set_spatialization_enabled(slot->sound, true);
    ma_sound_set_position(slot->sound, pos_x, pos_y, pos_z);
}

void CSoundLib::SetSoundDistanceModel(int id, AttenuationModel model, float rolloff, float min_dist, float max_dist, bool is_bgm) {
    auto* slot = FindSlot(id, is_bgm);
    if (!slot || !slot->sound) return;
    SetSoundAttenuation(slot->sound, model, rolloff, min_dist, max_dist);
}

void CSoundLib::SetSoundVelocity(int id, float vel_x, float vel_y, float vel_z, bool is_bgm) {
    auto* slot = FindSlot(id, is_bgm);
    if (!slot || !slot->sound) return;
    ma_sound_set_velocity(slot->sound, vel_x, vel_y, vel_z);
}

void CSoundLib::SetSoundAttenuation(ma_sound* s, AttenuationModel model, float rolloff, float min_dist, float max_dist) {
    if (!s) return;
    ma_sound_set_spatialization_enabled(s, true);
    switch (model) {
        case AttenuationModel::None:
            ma_sound_set_attenuation_model(s, ma_attenuation_model_none);
            break;
        case AttenuationModel::Inverse:
            ma_sound_set_attenuation_model(s, ma_attenuation_model_inverse);
            break;
        case AttenuationModel::Linear:
            ma_sound_set_attenuation_model(s, ma_attenuation_model_linear);
            break;
        case AttenuationModel::Exponential:
            ma_sound_set_attenuation_model(s, ma_attenuation_model_exponential);
            break;
    }
    ma_sound_set_rolloff(s, rolloff);
    ma_sound_set_min_distance(s, min_dist);
    ma_sound_set_max_distance(s, max_dist);
    ma_sound_set_min_gain(s, 0.0f);
    ma_sound_set_max_gain(s, 1.0f);
}

void CSoundLib::Update() {
    float dt_ms = 16.0f;

    for (auto it = volume_ramps_.begin(); it != volume_ramps_.end(); ) {
        if (!it->active) { it = volume_ramps_.erase(it); continue; }
        it->elapsed_ms += dt_ms;
        if (it->elapsed_ms >= it->duration_ms) {
            it->active = false;
            it = volume_ramps_.erase(it);
        } else {
            ++it;
        }
    }

    if (crossfade_active_) {
        crossfade_elapsed_ms_ += dt_ms;
        if (crossfade_elapsed_ms_ >= crossfade_duration_ms_) {
            if (crossfade_from_id_ >= 0) {
                StopBGM(crossfade_from_id_);
            }
            crossfade_active_ = false;
            crossfade_from_id_ = -1;
            crossfade_to_id_ = -1;
        }
    }

    for (auto& slot : sfx_slots_) {
        if (slot.in_use && slot.sound) {
            if (!ma_sound_is_playing(slot.sound)) {
                ma_sound_seek_to_pcm_frame(slot.sound, 0);
            }
        }
    }
}
