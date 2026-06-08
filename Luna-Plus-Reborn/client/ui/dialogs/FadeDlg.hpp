#pragma once
#include <rendering/UIRenderer.hpp>
#include <cstdint>
#include <functional>

class FadeDlg {
public:
    FadeDlg();

    void FadeIn(float duration = 0.5f);
    void FadeOut(float duration = 0.5f);
    void FadeTo(uint32_t color, float duration = 0.5f);

    void Update(float dt);
    void Render(UIRenderer& ui);

    bool IsFading() const { return fading_; }
    bool IsComplete() const { return complete_; }
    float GetProgress() const { return progress_; }

    using Callback = std::function<void()>;
    void SetCallback(Callback cb) { callback_ = cb; }

private:
    bool fading_ = false;
    bool complete_ = false;
    float duration_ = 0.5f;
    float progress_ = 0;
    uint32_t fade_color_ = 0x000000;
    enum FadeDir { FADE_NONE, FADE_IN, FADE_OUT } dir_ = FADE_NONE;
    Callback callback_;
};
