#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <functional>

class WeatherDialog {
public:
    enum WeatherType : int {
        CLEAR = 0,
        RAIN = 1,
        HEAVY_RAIN = 2,
        SNOW = 3,
    };

    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    void SetCurrentWeather(WeatherType w) { current_weather_ = w; }
    WeatherType GetCurrentWeather() const { return current_weather_; }

    using WeatherCallback = std::function<void(WeatherType type)>;
    void SetWeatherCallback(WeatherCallback cb) { on_weather_change_ = std::move(cb); }

private:
    Window* window_ = nullptr;
    WeatherType current_weather_ = CLEAR;
    WeatherCallback on_weather_change_;

    void SelectWeather(WeatherType type);
};
