#include "WeatherDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void WeatherDialog::Open(WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/WeatherDlg.bin.txt",
        "Weather", 400, 80, 320, 200);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title_lbl = window_->AddWidget<Label>("Weather Effect", 20, 36, ColorPalette::TEXT_GOLD);
    (void)title_lbl;

    const char* names[] = {"Clear", "Rain", "Heavy Rain", "Snow"};
    WeatherType types[] = {CLEAR, RAIN, HEAVY_RAIN, SNOW};
    float bx = 20;
    float by = 70;

    for (int i = 0; i < 4; i++) {
        auto* btn = window_->AddWidget<Button>(names[i], bx, by, 100, 28);
        btn->SetColors(
            {60, 80, 100, 220},
            {100, 130, 160, 220},
            {40, 50, 70, 220}
        );
        btn->OnEvent([this, type = types[i]](const UIEvent& e) {
            if (e.type == UIEvent::Click) SelectWeather(type);
        });
        bx += 115;
        if (bx + 100 > 300) { bx = 20; by += 40; }
    }

    auto* close_btn = window_->AddWidget<Button>("Close", 120, 150, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void WeatherDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

void WeatherDialog::SelectWeather(WeatherType type) {
    current_weather_ = type;
    const char* names[] = {"Clear", "Rain", "Heavy Rain", "Snow"};
    int idx = static_cast<int>(type);
    const char* name = (idx >= 0 && idx < 4) ? names[idx] : "Unknown";
    spdlog::info("WeatherDialog: weather changed to {} ({})", name, idx);
    if (on_weather_change_) on_weather_change_(type);
}
