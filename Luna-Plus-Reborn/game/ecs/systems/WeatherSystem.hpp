#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <entt/entt.hpp>

enum class WeatherType : uint8_t {
    CLEAR = 0,
    CLOUDY = 1,
    RAIN = 2,
    STORM = 3,
    FOG = 4,
    SNOW = 5
};

struct WeatherState {
    WeatherType type = WeatherType::CLEAR;
    float intensity = 0.0f;
    float duration = 0.0f;
    float elapsed = 0.0f;
    float wind_x = 0.0f;
    float wind_z = 0.0f;
};

struct WeatherConfig {
    float cycle_duration = 120.0f;
    float min_intensity = 0.0f;
    float max_intensity = 1.0f;
    float move_speed_mult_rain = 0.75f;
    float move_speed_mult_snow = 0.80f;
    float move_speed_mult_storm = 0.60f;
    float visibility_fog = 0.30f;
    float visibility_rain = 0.70f;
};

class WeatherSystem {
public:
    WeatherSystem();

    void SetConfig(const WeatherConfig& config);
    const WeatherConfig& GetConfig() const;

    void Update(entt::registry& reg, float dt, int map_id);

    WeatherType GetCurrentWeather(int map_id) const;
    float GetIntensity(int map_id) const;

    void ForceWeather(int map_id, WeatherType type, float intensity, float duration);

private:
    WeatherConfig config_;
    std::unordered_map<int, WeatherState> map_weather_;
    std::unordered_map<int, float> cycle_timers_;

    void TransitionWeather(int map_id);
    void ApplyEffects(entt::registry& reg, int map_id);
    void BroadcastWeatherChange(int map_id);
};
