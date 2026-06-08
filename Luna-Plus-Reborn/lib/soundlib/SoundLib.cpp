#define MINIAUDIO_IMPLEMENTATION
#include "SoundLib.h"
#include <miniaudio.h>
#include <spdlog/spdlog.h>
#include <algorithm>

constexpr int MAX_BGM_SLOTS = 4;
constexpr int MAX_SFX_SLOTS = 64;

CSoundLib::CSoundLib() {
    bgm_slots_.resize(MAX_BGM_SLOTS);
    sfx_slots_.resize(MAX_SFX_SLOTS);
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
        spdlog::debug("SoundLib: loaded BGM '{}' (id={})", path, slot.id);
        return slot.id;
    }

    spdlog::warn("SoundLib: no free BGM slot for '{}'", path);
    return -1;
}

void CSoundLib::PlayBGM(int id, bool loop) {
    for (auto& slot : bgm_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_set_looping(slot.sound, loop);
            ma_sound_start(slot.sound);
            return;
        }
    }
}

void CSoundLib::StopBGM(int id) {
    for (auto& slot : bgm_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_stop(slot.sound);
            return;
        }
    }
}

void CSoundLib::SetBGMVolume(int id, float volume) {
    for (auto& slot : bgm_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_set_volume(slot.sound, volume * master_volume_);
            return;
        }
    }
}

bool CSoundLib::IsBGMPlaying(int id) const {
    for (const auto& slot : bgm_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            return ma_sound_is_playing(slot.sound);
        }
    }
    return false;
}

int CSoundLib::LoadSFX(const char* path) {
    if (!engine_ || !path) return -1;

    // Check if already loaded
    for (const auto& slot : sfx_slots_) {
        if (slot.in_use && slot.sound) {
            // Reuse existing sound (we don't track paths, so load fresh)
        }
    }

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
        spdlog::debug("SoundLib: loaded SFX '{}' (id={})", path, slot.id);
        return slot.id;
    }

    spdlog::warn("SoundLib: no free SFX slot for '{}'", path);
    return -1;
}

void CSoundLib::PlaySFX(int id, float volume, float pan) {
    for (auto& slot : sfx_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_set_volume(slot.sound, volume * master_volume_);
            ma_sound_set_pan(slot.sound, pan);
            ma_sound_stop(slot.sound);
            ma_sound_seek_to_pcm_frame(slot.sound, 0);
            ma_sound_start(slot.sound);
            return;
        }
    }
}

void CSoundLib::StopSFX(int id) {
    for (auto& slot : sfx_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_stop(slot.sound);
            return;
        }
    }
}

void CSoundLib::SetSFXVolume(int id, float volume) {
    for (auto& slot : sfx_slots_) {
        if (slot.in_use && slot.id == id && slot.sound) {
            ma_sound_set_volume(slot.sound, volume * master_volume_);
            return;
        }
    }
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

void CSoundLib::Update() {
    // Clean up finished non-looping SFX
    for (auto& slot : sfx_slots_) {
        if (slot.in_use && slot.sound) {
            if (!ma_sound_is_playing(slot.sound)) {
                ma_sound_seek_to_pcm_frame(slot.sound, 0);
            }
        }
    }
}
