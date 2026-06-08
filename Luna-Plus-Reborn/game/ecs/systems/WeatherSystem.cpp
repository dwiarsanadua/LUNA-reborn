#include "WeatherSystem.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Movement.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>

static std::mt19937 s_rng(std::random_device{}());

WeatherSystem::WeatherSystem() {
    config_.cycle_duration = 120.0f;
    config_.min_intensity = 0.0f;
    config_.max_intensity = 1.0f;
    config_.move_speed_mult_rain = 0.75f;
    config_.move_speed_mult_snow = 0.80f;
    config_.move_speed_mult_storm = 0.60f;
    config_.visibility_fog = 0.30f;
    config_.visibility_rain = 0.70f;
}

void WeatherSystem::SetConfig(const WeatherConfig& config) {
    config_ = config;
}

const WeatherConfig& WeatherSystem::GetConfig() const {
    return config_;
}

void WeatherSystem::Update(entt::registry& reg, float dt, int map_id) {
    auto& state = map_weather_[map_id];
    state.elapsed += dt;

    auto& cycle_timer = cycle_timers_[map_id];
    cycle_timer += dt;

    if (cycle_timer >= config_.cycle_duration) {
        cycle_timer = 0.0f;
        TransitionWeather(map_id);
        BroadcastWeatherChange(map_id);
    }

    if (state.duration > 0.0f && state.elapsed >= state.duration) {
        cycle_timer = config_.cycle_duration;
        TransitionWeather(map_id);
        BroadcastWeatherChange(map_id);
    }

    ApplyEffects(reg, map_id);
}

WeatherType WeatherSystem::GetCurrentWeather(int map_id) const {
    auto it = map_weather_.find(map_id);
    return it != map_weather_.end() ? it->second.type : WeatherType::CLEAR;
}

float WeatherSystem::GetIntensity(int map_id) const {
    auto it = map_weather_.find(map_id);
    return it != map_weather_.end() ? it->second.intensity : 0.0f;
}

void WeatherSystem::ForceWeather(int map_id, WeatherType type, float intensity, float duration) {
    auto& state = map_weather_[map_id];
    state.type = type;
    state.intensity = std::clamp(intensity, 0.0f, 1.0f);
    state.duration = duration;
    state.elapsed = 0.0f;
    spdlog::info("WeatherSystem: forced weather type={} on map {}", static_cast<int>(type), map_id);
}

void WeatherSystem::TransitionWeather(int map_id) {
    static const WeatherType types[] = {
        WeatherType::CLEAR, WeatherType::CLOUDY, WeatherType::RAIN,
        WeatherType::STORM, WeatherType::FOG, WeatherType::SNOW
    };

    auto& state = map_weather_[map_id];
    WeatherType new_type;
    do {
        new_type = types[s_rng() % 6];
    } while (new_type == state.type && s_rng() % 3 != 0);

    state.type = new_type;
    state.intensity = config_.min_intensity +
        (config_.max_intensity - config_.min_intensity) *
        (static_cast<float>(s_rng() % 100) / 100.0f);
    state.elapsed = 0.0f;
    state.duration = 0.0f;

    spdlog::info("WeatherSystem: map {} transitions to weather type={} intensity={:.2f}",
                 map_id, static_cast<int>(new_type), state.intensity);
}

void WeatherSystem::ApplyEffects(entt::registry& reg, int map_id) {
    auto& state = map_weather_[map_id];

    auto view = reg.view<TagPlayer, CharacterStats, Movement>();
    for (auto entity : view) {
        auto& stats = view.get<CharacterStats>(entity);
        auto& move = view.get<Movement>(entity);

        float base_speed = stats.move_speed;
        float speed_mod = 1.0f;

        switch (state.type) {
            case WeatherType::RAIN:
                speed_mod = 1.0f - (1.0f - config_.move_speed_mult_rain) * state.intensity;
                break;
            case WeatherType::SNOW:
                speed_mod = 1.0f - (1.0f - config_.move_speed_mult_snow) * state.intensity;
                break;
            case WeatherType::STORM:
                speed_mod = 1.0f - (1.0f - config_.move_speed_mult_storm) * state.intensity;
                break;
            case WeatherType::FOG:
                speed_mod = 0.85f;
                break;
            default:
                speed_mod = 1.0f;
                break;
        }

        move.speed = base_speed * speed_mod;
    }
}

void WeatherSystem::BroadcastWeatherChange(int map_id) {
    auto& state = map_weather_[map_id];
    spdlog::info("WeatherSystem: broadcast weather change map={} type={} intensity={:.2f}",
                 map_id, static_cast<int>(state.type), state.intensity);
}
