#include "AudioManager.hpp"
#include <lib/soundlib/SoundLib.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <algorithm>
#include <filesystem>

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
}

void AudioManager::PlayBGM(const std::string& map_id) {
    if (current_bgm_id_ != -1) {
        sound_lib_->StopBGM(current_bgm_id_);
        current_bgm_id_ = -1;
    }
    std::string path = ASSETS_PATH + std::string("audio/BGM/") + map_id + ".mp3";
    int id = sound_lib_->OpenBGM(path.c_str());
    if (id >= 0) {
        sound_lib_->PlayBGM(id, true);
        current_bgm_id_ = id;
        current_bgm_ = map_id;
        spdlog::info("BGM: playing {} (id={})", map_id, id);
    }
}

void AudioManager::StopBGM() {
    if (current_bgm_id_ != -1) {
        sound_lib_->StopBGM(current_bgm_id_);
        current_bgm_id_ = -1;
    }
}

void AudioManager::PlaySFX(const std::string& name) {
    std::string path = ASSETS_PATH + std::string("audio/SFX/") + name;
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
    int id = sound_lib_->LoadSFX(path.c_str());
    if (id >= 0) {
        sound_lib_->PlaySFX(id);
    }
}

void AudioManager::Play3D(const std::string& name, float x, float y, float z) {
    // Basic 3D audio — for now just standard SFX
    PlaySFX(name);
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
    // No-op for now, miniaudio handles threads
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
