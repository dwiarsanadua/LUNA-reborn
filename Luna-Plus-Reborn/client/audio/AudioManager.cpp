#include "AudioManager.hpp"
#include <lib/soundlib/SoundLib.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <filesystem>

static const std::unordered_map<int, std::string> s_default_bgm_map = {
    {1, "BGM_Title"}, {2, "BGM_AlkerPlains"}, {51, "BGM_AlkerPlains"},
    {52, "BGM_BlueMoon"}, {53, "BGM_AruaCity"}, {54, "BGM_SnowyMountain"},
    {61, "BGM_Desert"}, {62, "BGM_Forest"}, {71, "BGM_Dungeon"},
    {72, "BGM_Boss"}, {81, "BGM_Seaside"}, {82, "BGM_Island"},
    {91, "BGM_Castle"}, {99, "BGM_Event"},
};

AudioManager::AudioManager() { sound_lib_ = new CSoundLib(); }

AudioManager::~AudioManager() { 
    Shutdown();
}

bool AudioManager::Initialize() {
    if (!sound_lib_->Init()) {
        spdlog::error("AudioManager: failed to initialize sound system");
        return false;
    }
    sound_lib_->SetMasterVolume(master_vol_);
    ScanAudioDirectory();
    spdlog::info("AudioManager: initialized");
    return true;
}

void AudioManager::Shutdown() {
    static bool is_shutdown = false;
    if (is_shutdown) return;
    is_shutdown = true;

    if (sound_lib_) {
        sound_lib_->Shutdown();
        delete sound_lib_;
        sound_lib_ = nullptr;
    }
    active_sounds_.clear();
}

void AudioManager::PlayBGM(const std::string& map_id) {
    std::string path = ASSETS_PATH + std::string("audio/BGM/") + map_id + ".mp3";
    if (!std::filesystem::exists(path)) {
        spdlog::warn("BGM: file not found, skipping: {}", path);
        return;
    }
    if (current_bgm_id_ != -1) {
        sound_lib_->StopBGM(current_bgm_id_);
        current_bgm_id_ = -1;
    }
    int id = sound_lib_->OpenBGM(path.c_str());
    if (id >= 0) {
        sound_lib_->PlayBGM(id, true);
        current_bgm_id_ = id;
        current_bgm_ = map_id;
        spdlog::info("BGM: playing {} (id={})", map_id, id);
    } else {
        spdlog::warn("BGM: failed to open {}", path);
    }
}

void AudioManager::StopBGM() {
    if (current_bgm_id_ != -1) {
        sound_lib_->StopBGM(current_bgm_id_);
        current_bgm_id_ = -1;
    }
    current_bgm_.clear();
    crossfade_.active = false;
}

std::string AudioManager::GetBGMForMap(int map_id) const {
    auto it = s_default_bgm_map.find(map_id);
    if (it != s_default_bgm_map.end()) return it->second;
    return "BGM_Map" + std::to_string(map_id);
}

void AudioManager::SmoothBGMTransition(const std::string& new_bgm, float duration) {
    if (new_bgm == current_bgm_) return;
    if (!sound_lib_) return;

    std::string path = ASSETS_PATH + std::string("audio/BGM/") + new_bgm + ".mp3";
    if (!std::filesystem::exists(path)) {
        spdlog::warn("BGM: transition file not found: {}", path);
        return;
    }

    int new_id = sound_lib_->OpenBGM(path.c_str());
    if (new_id < 0) {
        spdlog::warn("BGM: failed to open {} for transition", path);
        return;
    }

    if (current_bgm_id_ != -1) {
        crossfade_.active = true;
        crossfade_.duration = duration;
        crossfade_.elapsed = 0.0f;
        crossfade_.prev_bgm_id = current_bgm_id_;
        crossfade_.new_bgm_id = new_id;
        crossfade_.prev_vol = 1.0f;
        crossfade_.new_vol = 0.0f;
        crossfade_.new_bgm_name = new_bgm;
        sound_lib_->PlayBGM(new_id, true);
        sound_lib_->SetBGMVolume(new_id, 0);
    } else {
        sound_lib_->PlayBGM(new_id, true);
        current_bgm_id_ = new_id;
        current_bgm_ = new_bgm;
        spdlog::info("BGM: playing {} (id={})", new_bgm, new_id);
    }
}

void AudioManager::PlaySFX(const std::string& name) {
    std::string path = ASSETS_PATH + std::string("audio/SFX/") + name;
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return;
    fclose(f);
    int id = sound_lib_->LoadSFX(path.c_str());
    if (id >= 0) {
        sound_lib_->PlaySFX(id);
    }
}

void AudioManager::PlaySFXByCategory(Category cat, const std::string& name) {
    std::string folder = "SFX";
    switch(cat) {
        case SFX_Skill: folder = "Weapon"; break;
        case SFX_Hit: folder = "Character"; break;
        case SFX_UI: folder = "Interface"; break;
        case SFX_Ambient: folder = "SFX"; break;
        case SFX_NPC: folder = "Character"; break;
        case SFX_Monster: folder = "Monster"; break;
        case SFX_Weapon: folder = "Weapon"; break;
        case SFX_Character: folder = "Character"; break;
        case SFX_Footstep: folder = "Character"; break;
        default: break;
    }
    std::string path = ASSETS_PATH + std::string("audio/") + folder + "/" + name;
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return;
    fclose(f);
    int id = sound_lib_->LoadSFX(path.c_str());
    if (id >= 0) {
        sound_lib_->PlaySFX(id);
    }
}

void AudioManager::PlaySFXInst(const std::string& name, float x, float y, float z) {
    // Immediate SFX at a world position (non-spatial)
    PlaySFX(name);
}

int AudioManager::Play3D(const std::string& name, float x, float y, float z) {
    std::string path = ASSETS_PATH + std::string("audio/SFX/") + name;
    // Check file exists before attempting load, to avoid log flood
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return -1;
    fclose(f);
    int lib_id = sound_lib_->LoadSFX(path.c_str());
    if (lib_id < 0) return -1;

    SoundInstance inst;
    inst.handle = next_sound_handle_++;
    inst.sound_lib_id = lib_id;
    inst.position = glm::vec3(x, y, z);
    inst.is_3d = true;
    inst.active = true;

    int vol = CalculateAttenuation(inst);
    sound_lib_->PlaySFX(lib_id, vol);

    active_sounds_[inst.handle] = inst;
    return inst.handle;
}

void AudioManager::SetSoundPosition(int handle, float x, float y, float z) {
    auto it = active_sounds_.find(handle);
    if (it == active_sounds_.end()) return;
    auto& snd = it->second;
    snd.velocity = glm::vec3(x, y, z) - snd.position;
    snd.position = glm::vec3(x, y, z);

    if (snd.is_3d && sound_lib_) {
        int vol = CalculateAttenuation(snd);
        sound_lib_->SetSFXVolume(snd.sound_lib_id, vol);
    }
}

void AudioManager::SetListenerPosition(float x, float y, float z) {
    prev_listener_pos_ = listener_pos_;
    listener_pos_ = glm::vec3(x, y, z);
    listener_vel_ = listener_pos_ - prev_listener_pos_;
}

void AudioManager::SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up) {
    listener_forward_ = glm::normalize(forward);
    listener_up_ = glm::normalize(up);
}

void AudioManager::StopSound(int handle) {
    auto it = active_sounds_.find(handle);
    if (it == active_sounds_.end()) return;
    if (sound_lib_) {
        sound_lib_->StopSFX(it->second.sound_lib_id);
    }
    active_sounds_.erase(it);
}

void AudioManager::SetDistanceModel(int model) {
    distance_model_ = model;
}

void AudioManager::SetDopplerFactor(float factor) {
    doppler_factor_ = std::max(0.0f, factor);
}

void AudioManager::SetMasterVolume(float vol) {
    master_vol_ = vol;
    sound_lib_->SetMasterVolume(vol);
}

void AudioManager::SetBGMVolume(float vol) {
    bgm_vol_ = vol;
}

void AudioManager::SetSFXVolume(float vol) {
    sfx_vol_ = vol;
}

void AudioManager::Update() {
    if (!sound_lib_) return;

    // Handle BGM crossfade
    if (crossfade_.active) {
        crossfade_.elapsed += 1.0f / 60.0f; // approximate frame step
        float t = std::min(crossfade_.elapsed / crossfade_.duration, 1.0f);
        float prev_vol = static_cast<int>((1.0f - t) * 100);
        float new_vol = static_cast<int>(t * 100);
        sound_lib_->SetBGMVolume(crossfade_.prev_bgm_id, std::max(0, static_cast<int>(prev_vol)));
        sound_lib_->SetBGMVolume(crossfade_.new_bgm_id, static_cast<int>(new_vol));
        if (t >= 1.0f) {
            sound_lib_->StopBGM(crossfade_.prev_bgm_id);
            current_bgm_id_ = crossfade_.new_bgm_id;
            current_bgm_ = crossfade_.new_bgm_name;
            crossfade_.active = false;
            spdlog::info("BGM: crossfade complete to '{}' id={}", current_bgm_, current_bgm_id_);
        }
    }

    // Update 3D sound volumes based on listener position
    for (auto& [handle, snd] : active_sounds_) {
        if (!snd.active) continue;
        if (!snd.is_3d) continue;

        int vol = CalculateAttenuation(snd);
        sound_lib_->SetSFXVolume(snd.sound_lib_id, vol);

        // Doppler shift: adjust volume based on relative velocity (pitch not supported by this SoundLib)
    }

    // Clean up finished sounds (if sound lib supports query)
    std::vector<int> to_remove;
    for (auto& [handle, snd] : active_sounds_) {
        // Check if sound has finished playing
        if (!sound_lib_->IsSFXPlaying(snd.sound_lib_id)) {
            to_remove.push_back(handle);
        }
    }
    for (int h : to_remove) active_sounds_.erase(h);
}

int AudioManager::CalculateAttenuation(const SoundInstance& snd) const {
    float dist = glm::distance(listener_pos_, snd.position);
    float vol = 1.0f;

    if (distance_model_ == 0) {
        // Linear attenuation
        if (dist <= snd.reference_distance) {
            vol = 1.0f;
        } else if (dist >= snd.max_distance) {
            vol = 0.0f;
        } else {
            vol = 1.0f - (dist - snd.reference_distance) / (snd.max_distance - snd.reference_distance);
        }
    }
    // Clamp and convert to 0-100 scale for sound lib
    vol = std::max(0.0f, std::min(1.0f, vol));
    return (int)(vol * 100);
}

void AudioManager::ScanAudioDirectory() {
    // Implementation for directory scanning
    sfx_count_ = 0;
    std::string folders[] = {"Interface", "Weapon", "Character", "Monster", "SFX"};
    for (auto& f : folders) {
        std::string p = ASSETS_PATH + std::string("audio/") + f + "/";
        if (std::filesystem::exists(p)) {
            for (auto const& dir_entry : std::filesystem::directory_iterator{p}) {
                if (dir_entry.is_regular_file()) sfx_count_++;
            }
        }
    }
}
