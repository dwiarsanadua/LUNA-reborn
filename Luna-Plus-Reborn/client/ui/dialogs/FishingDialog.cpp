#include "FishingDialog.hpp"
#include <cstdlib>
#include <cstdio>

void FishingDialog::Open(GameState* state) {
    (void)state;
    window_ = new Window("FISHING", 440, 260, 400, 200);
    window_->SetClosable(true);
    
    status_label_ = window_->AddWidget<Label>("Press SPACE to cast line", 20, 30, 0xffffcc88);
    fish_label_ = window_->AddWidget<Label>("", 20, 60, 0xffffffff);
    progress_bar_ = window_->AddWidget<ProgressBar>(20, 100, 360, 20);
    
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
        if (status_label_) {
            char buf[64]; snprintf(buf, sizeof(buf), "Casting... %.1f", cast_timer_);
            status_label_->SetText(buf);
        }
        if (cast_timer_ > 2.0f) {
            casting_ = false;
            wait_timer_ = 2.0f + (float)(rand() % (int)(MAX_WAIT * 10)) / 10.0f;
            if (status_label_) status_label_->SetText("Waiting for a bite...");
        }
        return;
    }
    
    if (!catching_ && !caught_) {
        wait_timer_ -= dt;
        bite_timer_ += dt;
        
        // Random fish bite
        if (wait_timer_ <= 0 && bite_timer_ > 1.0f) {
            catching_ = true;
            progress_ = 0.0f;
            catch_window_ = 0.3f + (float)(rand() % 20) / 100.0f;
            fish_type_ = rand() % 5;
            const char* fish_names[] = {"Minnow", "Perch", "Salmon", "Trout", "Gold Carp"};
            if (fish_label_) fish_label_->SetText(fish_names[fish_type_]);
            if (status_label_) status_label_->SetText("!! BITE !! Press SPACE rapidly!");
        }
    }
    
    if (catching_) {
        progress_ += progress_dir_ * dt * 0.8f;
        if (progress_ >= 1.0f || progress_ <= 0.0f) progress_dir_ *= -1.0f;
        progress_ = std::max(0.0f, std::min(1.0f, progress_));
        
        if (progress_bar_) progress_bar_->SetProgress(progress_);
        
        // Check if in catch zone
        float zone_center = 0.3f + (float)(fish_type_ % 3) * 0.15f;
        float in_zone = (progress_ >= zone_center - catch_window_ * 0.5f && 
                         progress_ <= zone_center + catch_window_ * 0.5f);
        
        // Auto-fail if out of time
        bite_timer_ += dt;
        if (bite_timer_ > 8.0f) {
            caught_ = true;
            if (status_label_) status_label_->SetText("Fish got away!");
            if (result_cb_) result_cb_(false, fish_type_);
        }
    }
}

void FishingDialog::Render(UIRenderer& ui) {
    if (!active_ || !window_) return;
    window_->Render(ui);
    
    // Draw catch zone indicator
    if (catching_ && window_) {
        float x = window_->GetX() + 20, y = window_->GetY() + 100;
        float w = 360, h = 20;
        float zone_center = 0.3f + (float)(fish_type_ % 3) * 0.15f;
        float zone_w = catch_window_ * w;
        float zone_x = x + zone_center * w - zone_w * 0.5f;
        ui.DrawRect(zone_x, y, zone_w, h, {60, 255, 60, 100});
    }
}
