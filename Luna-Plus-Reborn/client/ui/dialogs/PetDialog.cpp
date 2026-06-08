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
    hunger_bar_->SetText("Satiation");

    exp_bar_ = window_->AddWidget<ProgressBar>(10, 200, 340, 16);
    exp_bar_->SetProgress(0.0f);
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

    feed_btn_ = window_->AddWidget<Button>("Feed (+30)", 190, 280, 90, 24);
    feed_btn_->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    feed_btn_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (feed_fn_) feed_fn_();
            else if (pet_) {
                if (state->gold >= 50) {
                    state->gold -= 50;
                    pet_->Feed(30);
                    if (status_label_) status_label_->SetText("Pet fed! -50g");
                    Refresh(state);
                } else {
                    if (status_label_) status_label_->SetText("Not enough gold!");
                }
            }
        }
    });

    evolve_btn_ = window_->AddWidget<Button>("Evolve", 290, 280, 80, 24);
    evolve_btn_->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    evolve_btn_->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (evolve_fn_) evolve_fn_();
            else if (pet_) {
                if (state->gold >= 1000) {
                    state->gold -= 1000;
                    state->chat_messages.push_back("Pet evolved!");
                    if (status_label_) status_label_->SetText("Pet evolved to next form!");
                    Refresh(state);
                } else {
                    if (status_label_) status_label_->SetText("Need 1000g to evolve!");
                }
            }
        }
    });

    Refresh(state);
}

void PetDialog::Refresh(GameState* state) {
    if (!info_label_) return;

    const bool use_network = state && !state->offline_mode
        && (state->network_pet.pet_id != 0 || !state->network_pet.name.empty());

    char buf[320];
    if (use_network) {
        const auto& p = state->network_pet;
        snprintf(buf, sizeof(buf),
            "Name: %s\n"
            "Level: %d | Evolution: %d\n"
            "HP: %d / %d\n"
            "Satiation: %d/100\n"
            "EXP: %u / %u\n"
            "Active: %s\n\n"
            "Summon needs 20+ satiation.\n"
            "Evolve at level %d (cost %ug).",
            p.name.empty() ? "Fluffy" : p.name.c_str(),
            p.level, p.evolution,
            p.hp, p.max_hp,
            p.satiation,
            p.exp, p.exp_to_next,
            p.summoned ? "Yes" : "No",
            p.evolution * 10, p.evolve_cost);
        if (hunger_bar_) hunger_bar_->SetProgress(p.satiation / 100.0f);
        if (exp_bar_) {
            float pct = p.exp_to_next > 0 ? static_cast<float>(p.exp) / p.exp_to_next : 0.0f;
            exp_bar_->SetProgress(pct);
        }
        if (feed_btn_) {
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "Feed (%ug)", p.feed_cost);
            feed_btn_->SetText(lbl);
        }
        if (evolve_btn_) {
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "Evolve (%ug)", p.evolve_cost);
            evolve_btn_->SetText(lbl);
        }
    } else if (pet_) {
        snprintf(buf, sizeof(buf),
            "Name: %s\n"
            "Level: %d | Evolution: %d\n"
            "Satiation: %d/100\n"
            "Active: %s\n\n"
            "Tip: Feed your pet to keep it\n"
            "healthy. Evolve at max level!",
            pet_->GetName().c_str(),
            pet_->GetLevel(),
            pet_->GetEvolution(),
            pet_->GetHunger(),
            pet_->IsActive() ? "Yes" : "No");
        if (hunger_bar_) hunger_bar_->SetProgress(pet_->GetHunger() / 100.0f);
        if (exp_bar_) {
            float pct = pet_->GetExpNext() > 0
                ? static_cast<float>(pet_->GetExp()) / pet_->GetExpNext() : 0.0f;
            exp_bar_->SetProgress(pct);
        }
    } else {
        return;
    }
    info_label_->SetText(buf);
}

void PetDialog::UpdateFromState(GameState* state) {
    Refresh(state);
}
