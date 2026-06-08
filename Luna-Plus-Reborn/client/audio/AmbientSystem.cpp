#include "AmbientSystem.hpp"
#include "AudioManager.hpp"
#include <cstdlib>
#include <cmath>
#include <spdlog/spdlog.h>

extern AudioManager* g_audio;

void AmbientSystem::Init() {
    active_sounds_.clear();
    spdlog::info("AmbientSystem: initialized");
}

void AmbientSystem::Update(float dt, int map_id, float time_of_day, float px, float py, float pz) {
    if (!enabled_) return;
    time_ += dt;
    
    // Update active sounds
    for (auto& s : active_sounds_) {
        if (!s.active) continue;
        s.elapsed += dt;
        if (s.elapsed >= s.lifetime && !s.loop) s.active = false;
    }
    active_sounds_.erase(
        std::remove_if(active_sounds_.begin(), active_sounds_.end(),
            [](auto& s) { return !s.active; }),
        active_sounds_.end());
    
    // Ambient one-shots only when matching audio files exist (legacy had no Bird_*.wav assets).
    wind_timer_ -= dt;
    if (wind_timer_ <= 0 && g_audio && g_audio->CanPlaySFX("Wind.wav")) {
        wind_timer_ = 5.0f + (float)(rand() % 10);
        PlayWind(current_map_.wind_intensity);
    } else if (wind_timer_ <= 0) {
        wind_timer_ = 10.0f;
    }

    if (time_of_day > 0.25f && time_of_day < 0.75f && current_map_.has_birds && g_audio) {
        if (rand() % 300 == 0 && g_audio->CanPlaySFX("Bird_1.wav")) SpawnBird();
    }

    if ((time_of_day < 0.25f || time_of_day > 0.75f) && current_map_.has_insects && g_audio) {
        if (rand() % 200 == 0 && g_audio->CanPlaySFX("Insect_1.wav")) SpawnInsect();
    }
    
    // Thunder (random during any time)
    thunder_timer_ -= dt;
    if (thunder_timer_ <= 0) {
        thunder_timer_ = 30.0f + (float)(rand() % 120);
        if (g_audio && g_audio->CanPlaySFX("Thunder.wav") &&
            (float)(rand() % 1000) / 1000.0f < current_map_.thunder_chance * dt * 100) {
            PlayThunder(0);
        }
    }
    
    (void)map_id; (void)px; (void)py; (void)pz;
}

void AmbientSystem::PlayThunder(float delay) {
    (void)delay;
    float x = (float)(rand() % 200 - 100);
    float z = (float)(rand() % 200 - 100);
    Play("Thunder.wav", x, 0, z, 0.8f, 3.0f, false);
}

void AmbientSystem::PlayBird(float x, float y, float z) {
    int type = rand() % 3;
    std::string name = "Bird_" + std::to_string(type + 1) + ".wav";
    Play(name, x, y, z, 0.3f, 2.0f, false);
}

void AmbientSystem::PlayInsect(float x, float y, float z) {
    int type = rand() % 2;
    std::string name = "Insect_" + std::to_string(type + 1) + ".wav";
    Play(name, x, y, z, 0.2f, 3.0f, false);
}

void AmbientSystem::PlayWind(float intensity) {
    float vol = std::max(0.1f, std::min(1.0f, intensity));
    Play("Wind.wav", 0, 0, 0, vol, 4.0f, false);
}

void AmbientSystem::SpawnBird() {
    float x = (float)(rand() % 100 - 50);
    float z = (float)(rand() % 100 - 50);
    float y = 15.0f + (float)(rand() % 20);
    PlayBird(x, y, z);
}

void AmbientSystem::SpawnInsect() {
    float x = (float)(rand() % 30 - 15);
    float z = (float)(rand() % 30 - 15);
    PlayInsect(x, 1, z);
}

void AmbientSystem::Play(const std::string& name, float x, float y, float z, float volume, float lifetime, bool loop) {
    AmbientSound s;
    s.name = name; s.x = x; s.y = y; s.z = z;
    s.volume = std::max(0.0f, std::min(1.0f, volume * volume_));
    s.lifetime = lifetime; s.loop = loop; s.active = true;
    active_sounds_.push_back(s);
    
    if (g_audio) g_audio->Play3D(name, x, y, z);
}

void AmbientSystem::SetMapAmbient(const MapAmbient& config) {
    current_map_ = config;
}

void AmbientSystem::Shutdown() {
    active_sounds_.clear();
}
