#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

class AmbientSystem {
public:
    void Init();
    void Update(float dt, int map_id, float time_of_day, float player_x, float player_y, float player_z);
    void Shutdown();
    
    // Manual triggers
    void PlayThunder(float delay = 0);
    void PlayBird(float x, float y, float z);
    void PlayInsect(float x, float y, float z);
    void PlayWind(float intensity);
    
    // Settings
    void SetEnabled(bool e) { enabled_ = e; }
    bool IsEnabled() const { return enabled_; }
    void SetVolume(float v) { volume_ = v; }
    
    // Per-map ambient config
    struct MapAmbient {
        int map_id = 0;
        std::string bgm;
        float wind_intensity = 0.3f;
        bool has_birds = true;
        bool has_insects = true;
        float thunder_chance = 0.001f; // per second
    };
    void SetMapAmbient(const MapAmbient& config);
    
    int GetActiveCount() const { return (int)active_sounds_.size(); }

private:
    struct AmbientSound {
        std::string name;
        float x, y, z;
        float volume;
        float lifetime;
        float elapsed;
        bool loop;
        bool active;
    };
    
    std::vector<AmbientSound> active_sounds_;
    MapAmbient current_map_;
    float volume_ = 0.5f;
    float wind_timer_ = 0;
    float thunder_timer_ = 0;
    bool enabled_ = true;
    float time_ = 0;
    
    void SpawnBird();
    void SpawnInsect();
    void TryThunder();
    void Play(const std::string& name, float x, float y, float z, float volume, float lifetime, bool loop);
};
