#include "FishingDialog.hpp"
#include <ui/WindowManager.hpp>
#include <cstdlib>
#include <cstdio>

void FishingDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/FishingDlg.bin.txt",
            "FISHING", 440, 260, 400, 200);
    }
    if (!window_) {
        window_ = new Window("FISHING", 440, 260, 400, 200);
        window_->SetClosable(true);
        status_label_ = window_->AddWidget<Label>("Press SPACE to cast line", 20, 30, 0xffffcc88);
        fish_label_ = window_->AddWidget<Label>("", 20, 60, 0xffffffff);
        progress_bar_ = window_->AddWidget<ProgressBar>(20, 100, 360, 20);
    } else {
        window_->SetClosable(true);
        status_label_ = window_->FindWidget<Label>("");
        fish_label_ = window_->FindWidget<Label>("");
        progress_bar_ = window_->FindWidget<ProgressBar>("");
        if (!status_label_)
            status_label_ = window_->AddWidget<Label>("Press SPACE to cast line", 20, 30, 0xffffcc88);
        if (!progress_bar_)
            progress_bar_ = window_->AddWidget<ProgressBar>(20, 100, 360, 20);
    }

    active_ = true;
    casting_ = false;
    catching_ = false;
    caught_ = false;
    progress_ = 0;
    wait_timer_ = 0;
}

void FishingDialog::Update(float dt) {
    if (!active_) return;

    if (casting_) {
        cast_timer_ += dt;
        if (cast_timer_ > 1.0f) {
            casting_ = false;
            wait_timer_ = 0;
            if (status_label_) status_label_->SetText("Waiting for bite...");
        }
    } else if (!catching_ && !caught_) {
        wait_timer_ += dt;
        if (wait_timer_ > MAX_WAIT) {
            if (status_label_) status_label_->SetText("No bite. Try again.");
            active_ = false;
        } else if (wait_timer_ > 2.0f && (rand() % 100) < 3) {
            catching_ = true;
            bite_timer_ = 0;
            catch_window_ = CATCH_WINDOW;
            if (status_label_) status_label_->SetText("BITE! Press SPACE!");
        }
    } else if (catching_) {
        bite_timer_ += dt;
        catch_window_ -= dt;
        progress_ += progress_dir_ * dt * 2.0f;
        if (progress_ >= 1.0f) { progress_ = 1.0f; progress_dir_ = -1.0f; }
        if (progress_ <= 0.0f) { progress_ = 0.0f; progress_dir_ = 1.0f; }
        if (progress_bar_) progress_bar_->SetProgress(progress_);
        if (catch_window_ <= 0) {
            catching_ = false;
            if (status_label_) status_label_->SetText("Fish got away!");
            active_ = false;
        }
    }
}

void FishingDialog::Render(UIRenderer& ui) {
    if (active_ && window_) window_->Render(ui);
}
