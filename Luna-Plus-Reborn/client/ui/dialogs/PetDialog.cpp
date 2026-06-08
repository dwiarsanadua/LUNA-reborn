#include "PetDialog.hpp"
#include <cstdio>

void PetDialog::SetNetworkCallbacks(std::function<void()> summon_fn,
                                      std::function<void()> dismiss_fn,
                                      std::function<void()> feed_fn,
                                      std::function<void()> evolve_fn,
                                      std::function<void()> refresh_fn) {
    summon_fn_ = std::move(summon_fn);
    dismiss_fn_ = std::move(dismiss_fn);
    feed_fn_ = std::move(feed_fn);
    evolve_fn_ = std::move(evolve_fn);
    refresh_fn_ = std::move(refresh_fn);
}

void PetDialog::Open(GameState* state, WindowManager* wm, Pet* pet) {
    pet_ = pet;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/PetInfo.bin.txt",
        "Pet Manager", 200, 80, 380, 360);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Pet Management", 10, 4, 0xff88ff88);
    (void)title;

    info_label_ = window_->AddWidget<Label>("", 10, 28, 0xffffffff);
    hunger_bar_ = window_->AddWidget<ProgressBar>(10, 180, 340, 16);
    hunger_bar_->SetProgress(1.0f);
    hunger_bar_->SetColors({255, 180, 60, 255}, {60, 40, 20, 180});
    hunger_bar_->SetText("Hunger");

    exp_bar_ = window_->AddWidget<ProgressBar>(10, 200, 340, 16);
    exp_bar_->SetProgress(0.5f);
    exp_bar_->SetColors({100, 200, 255, 255}, {20, 40, 60, 180});
    exp_bar_->SetText("EXP");

    status_label_ = window_->AddWidget<Label>("Pet is ready!", 10, 220, 0xffcccccc);

    auto* summon_btn = window_->AddWidget<Button>("Summon", 10, 280, 80, 24);
    summon_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    summon_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (summon_fn_) summon_fn_();
            else if (pet_) {
                pet_->Summon();
                if (status_label_) status_label_->SetText("Pet summoned!");
                Refresh();
            }
        }
    });

    auto* dismiss_btn = window_->AddWidget<Button>("Dismiss", 100, 280, 80, 24);
    dismiss_btn->SetColors({80,40,40,220}, {130,80,80,220}, {50,30,30,220});
    dismiss_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (dismiss_fn_) dismiss_fn_();
            else if (pet_) {
                pet_->Dismiss();
                if (status_label_) status_label_->SetText("Pet dismissed.");
                Refresh();
            }
        }
    });

    auto* feed_btn = window_->AddWidget<Button>("Feed (+30)", 190, 280, 90, 24);
    feed_btn->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    feed_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (feed_fn_) feed_fn_();
            else if (pet_) {
                if (state->gold >= 50) {
                    state->gold -= 50;
                    pet_->Feed(30);
                    if (status_label_) status_label_->SetText("Pet fed! -50g");
                    Refresh();
                } else {
                    if (status_label_) status_label_->SetText("Not enough gold!");
                }
            }
        }
    });

    auto* evolve_btn = window_->AddWidget<Button>("Evolve", 290, 280, 80, 24);
    evolve_btn->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    evolve_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (evolve_fn_) evolve_fn_();
            else if (pet_) {
                if (state->gold >= 1000) {
                    state->gold -= 1000;
                    state->chat_messages.push_back("Pet evolved!");
                    if (status_label_) status_label_->SetText("Pet evolved to next form!");
                    Refresh();
                } else {
                    if (status_label_) status_label_->SetText("Need 1000g to evolve!");
                }
            }
        }
    });

    Refresh();
}

void PetDialog::Refresh() {
    if (!pet_ || !info_label_) return;
    char buf[256];
    snprintf(buf, sizeof(buf),
        "Name: %s\n"
        "Level: %d | Evolution: %d\n"
        "Hunger: %d/100\n"
        "Active: %s\n\n"
        "Tip: Feed your pet to keep it\n"
        "healthy. Evolve at max level!",
        pet_->GetName().c_str(),
        pet_->GetLevel(),
        pet_->GetEvolution(),
        pet_->GetHunger(),
        pet_->IsActive() ? "Yes" : "No");
    info_label_->SetText(buf);
    if (hunger_bar_) hunger_bar_->SetProgress(pet_->GetHunger() / 100.0f);
}

void PetDialog::UpdateFromState(GameState* state) {
    (void)state;
    if (window_) Refresh();
}
