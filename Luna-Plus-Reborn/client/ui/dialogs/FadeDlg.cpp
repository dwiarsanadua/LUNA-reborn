#include "FadeDlg.hpp"
#include <algorithm>

FadeDlg::FadeDlg() {}

void FadeDlg::FadeIn(float duration) {
    fading_ = true;
    complete_ = false;
    duration_ = duration;
    progress_ = 1.0f;
    dir_ = FADE_IN;
}

void FadeDlg::FadeOut(float duration) {
    fading_ = true;
    complete_ = false;
    duration_ = duration;
    progress_ = 0.0f;
    dir_ = FADE_OUT;
}

void FadeDlg::FadeTo(uint32_t color, float duration) {
    fade_color_ = color;
    fading_ = true;
    complete_ = false;
    duration_ = duration;
    progress_ = 0.0f;
    dir_ = FADE_OUT;
}

void FadeDlg::Update(float dt) {
    if (!fading_) return;

    switch (dir_) {
    case FADE_OUT:
        progress_ += dt / duration_;
        if (progress_ >= 1.0f) {
            progress_ = 1.0f;
            complete_ = true;
            fading_ = false;
            if (callback_) callback_();
        }
        break;
    case FADE_IN:
        progress_ -= dt / duration_;
        if (progress_ <= 0.0f) {
            progress_ = 0.0f;
            complete_ = true;
            fading_ = false;
            if (callback_) callback_();
        }
        break;
    default:
        break;
    }
}

void FadeDlg::Render(UIRenderer& ui) {
    if (!fading_ && !complete_ && progress_ <= 0) return;

    float alpha = progress_;
    alpha = std::max(0.0f, std::min(1.0f, alpha));

    uint8_t a = (uint8_t)(alpha * 255.0f);
    uint8_t r = (fade_color_ >> 16) & 0xFF;
    uint8_t g = (fade_color_ >> 8) & 0xFF;
    uint8_t b = fade_color_ & 0xFF;

    ui.DrawRect(0, 0, 1280, 720, {r, g, b, a});
}
