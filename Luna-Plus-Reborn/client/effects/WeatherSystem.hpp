#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <cstdlib>

class UIRenderer;

struct RainDrop {
    float x, y, z;
    float speed;
    float life;
};

class WeatherSystem {
public:
    enum Weather { Clear, Rain, HeavyRain, Snow };
    
    void SetWeather(Weather w) { weather_ = w; }
    Weather GetWeather() const { return weather_; }
    
    void Update(float dt, float player_x, float player_z) {
        (void)player_x; (void)player_z;
        
        if (weather_ == Clear) { drops_.clear(); return; }
        
        int target_count = (weather_ == HeavyRain) ? 300 : 100;
        
        // Spawn new drops
        while ((int)drops_.size() < target_count) {
            RainDrop d;
            d.x = (float)(rand() % 400 - 200);
            d.z = (float)(rand() % 400 - 200);
            d.y = (float)(rand() % 100 + 20);
            d.speed = (weather_ == HeavyRain) ? 80.0f : 50.0f;
            d.life = 2.0f;
            drops_.push_back(d);
        }
        
        // Update existing drops
        for (auto& d : drops_) {
            d.y -= d.speed * dt;
            d.x -= d.speed * 0.2f * dt; // wind effect
            d.life -= dt;
        }
        
        // Remove dead drops
        drops_.erase(
            std::remove_if(drops_.begin(), drops_.end(),
                [](auto& d) { return d.y < -5 || d.life <= 0; }),
            drops_.end());
    }
    
    void Render(UIRenderer& ui, float player_x, float player_z);
    
private:
    Weather weather_ = Clear;
    std::vector<RainDrop> drops_;
};
