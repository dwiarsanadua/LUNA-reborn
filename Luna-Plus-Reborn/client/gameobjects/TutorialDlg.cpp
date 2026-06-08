#include "TutorialDlg.hpp"
#include <cmath>
#include <algorithm>

TutorialDlg::TutorialDlg() {}

void TutorialDlg::Start(const std::string& tutorial_id, const std::vector<TutorialStep>& steps) {
    tutorial_id_ = tutorial_id;
    steps_ = steps;
    current_step_ = 0;
    active_ = true;
    anim_timer_ = 0;

    if (!steps_.empty() && steps_[0].on_enter) {
        steps_[0].on_enter();
    }
}

void TutorialDlg::NextStep() {
    if (!active_ || steps_.empty()) return;
    if (current_step_ + 1 < (int)steps_.size()) {
        current_step_++;
        anim_timer_ = 0;
        if (steps_[current_step_].on_enter) {
            steps_[current_step_].on_enter();
        }
    } else {
        Complete();
    }
}

void TutorialDlg::PrevStep() {
    if (!active_ || steps_.empty()) return;
    if (current_step_ > 0) {
        current_step_--;
        anim_timer_ = 0;
    }
}

void TutorialDlg::Skip() {
    active_ = false;
    steps_.clear();
}

void TutorialDlg::Complete() {
    active_ = false;
    if (on_complete_) on_complete_(tutorial_id_);
}

void TutorialDlg::Reset() {
    active_ = false;
    current_step_ = 0;
    steps_.clear();
    tutorial_id_.clear();
}

void TutorialDlg::Update(float dt) {
    if (!active_) return;
    anim_timer_ += dt;
    pulse_anim_ = sinf(anim_timer_ * 4.0f) * 0.3f + 0.7f;

    // Check completion condition
    if (!steps_.empty() && current_step_ < (int)steps_.size()) {
        auto& step = steps_[current_step_];
        if (step.completion_condition && step.completion_condition()) {
            NextStep();
        }
    }
}

void TutorialDlg::Render(UIRenderer& ui) {
    if (!active_ || steps_.empty()) return;
    if (current_step_ >= (int)steps_.size()) return;

    auto& step = steps_[current_step_];

    // Dark overlay behind tutorial
    ui.DrawRect(0, 0, 1280, 720, {0, 0, 0, 100});

    RenderHighlight(ui, step);
    RenderArrowOverlay(ui, step);
    RenderPopup(ui, step);
}

void TutorialDlg::RenderHighlight(UIRenderer& ui, const TutorialStep& step) {
    float px = pulse_anim_ * 4.0f;
    float hw = step.highlight_w > 0 ? step.highlight_w : 100;
    float hh = step.highlight_h > 0 ? step.highlight_h : 40;

    // Draw highlight border with glow
    UIColor glow = {100, 255, 100, (uint8_t)(80 * pulse_anim_)};
    ui.DrawBorder(step.highlight_x - px, step.highlight_y - px,
        hw + px * 2, hh + px * 2, glow, 2.0f);
    ui.DrawBorder(step.highlight_x - px - 1, step.highlight_y - px - 1,
        hw + px * 2 + 2, hh + px * 2 + 2, {255, 255, 200, 40});
}

void TutorialDlg::RenderArrowOverlay(UIRenderer& ui, const TutorialStep& step) {
    if (step.arrow_target_x == 0 && step.arrow_target_y == 0) return;

    float ax = step.arrow_target_x;
    float ay = step.arrow_target_y;
    float arrow_size = 20.0f;

    // Draw a simple triangle arrow
    float angle = step.arrow_angle;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    // Three points of arrow
    float p1x = ax + cos_a * arrow_size;
    float p1y = ay + sin_a * arrow_size;
    float p2x = ax + cos_a * (-arrow_size * 0.5f) - sin_a * (arrow_size * 0.4f);
    float p2y = ay + sin_a * (-arrow_size * 0.5f) + cos_a * (arrow_size * 0.4f);
    float p3x = ax + cos_a * (-arrow_size * 0.5f) + sin_a * (arrow_size * 0.4f);
    float p3y = ay + sin_a * (-arrow_size * 0.5f) - cos_a * (arrow_size * 0.4f);

    // Approximate triangle via borders (simplified)
    ui.DrawRect(p1x - 1, p1y - 1, 3, 3, {100, 255, 100, 220});
    (void)p2x; (void)p2y; (void)p3x; (void)p3y;
}

void TutorialDlg::RenderPopup(UIRenderer& ui, const TutorialStep& step) {
    int sw = 1280, sh = 720;

    // Position popup at bottom center
    popup_x_ = sw * 0.5f - popup_w_ * 0.5f;
    popup_y_ = sh - popup_h_ - 30;

    // Popup background
    ui.DrawRect(popup_x_, popup_y_, popup_w_, popup_h_, {16, 16, 32, 230});
    ui.DrawBorder(popup_x_, popup_y_, popup_w_, popup_h_, {100, 200, 100, 200}, 2.0f);

    // Title
    ui.DrawText(popup_x_ + 15, popup_y_ + 10, 0xffffcc88, step.title.c_str());

    // Description (word-wrapped approximation)
    ui.DrawText(popup_x_ + 15, popup_y_ + 40, 0xffffffff, step.description.c_str());

    // Step counter
    char buf[64];
    snprintf(buf, sizeof(buf), "Step %d / %d", current_step_ + 1, GetTotalSteps());
    ui.DrawText(popup_x_ + popup_w_ - 100, popup_y_ + popup_h_ - 30, 0xff888888, buf);

    // Next/Skip buttons
    ui.DrawText(popup_x_ + popup_w_ - 160, popup_y_ + popup_h_ - 24, 0xff88ff88, "[Next >]");
    ui.DrawText(popup_x_ + popup_w_ - 80, popup_y_ + popup_h_ - 24, 0xffff6666, "[Skip]");
}
