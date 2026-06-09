#include "ProgressDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void ProgressDialog::Open(GameState* state, WindowManager* wm) {
    OpenWithMessage(state, wm, "Processing...");
}

void ProgressDialog::OpenWithMessage(GameState* state, WindowManager* wm, const std::string& message) {
    if (window_) return;

    window_ = wm->LoadFromScript("assets/interface/Windows/ProgressDialog.bin.txt");
    if (!window_) {
        spdlog::warn("ProgressDialog: fallback to default window");
        window_ = new Window("Progress", 320, 200, 170, 80);
        window_->SetMovable(true);
        window_->SetClosable(false);
    }

    message_label_ = window_->AddWidget<Label>(message, 8, 8, ColorPalette::TEXT_NORMAL);
    progress_text_ = window_->AddWidget<Label>("0%", 8, 40, ColorPalette::TEXT_GOLD);

    if (on_cancel_) {
        auto* cancel_btn = window_->AddWidget<Button>("Cancel", 120, 55, 40, 18);
        cancel_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click && on_cancel_) on_cancel_();
        });
    }

    progress_ = 0.0f;
}

void ProgressDialog::SetProgress(float pct) {
    progress_ = pct;
    if (progress_text_) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d%%", (int)(pct * 100.0f));
        progress_text_->SetText(buf);
    }
}

void ProgressDialog::SetMessage(const std::string& msg) {
    if (message_label_) message_label_->SetText(msg);
}

void ProgressDialog::SetCancelCallback(std::function<void()> on_cancel) {
    on_cancel_ = on_cancel;
}

void ProgressDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void ProgressDialog::Close() {
    window_ = nullptr;
    message_label_ = nullptr;
    progress_text_ = nullptr;
    progress_ = 0.0f;
}

void ProgressBarUtil::Draw(float x, float y, float w, float h, float pct) {
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)pct;
}
