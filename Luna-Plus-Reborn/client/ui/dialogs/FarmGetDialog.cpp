#include "FarmGetDialog.hpp"
#include <gameobjects/FarmSystem.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <cstdio>

FarmGetDialog::FarmGetDialog()
    : selected_seed_(1) {}

void FarmGetDialog::Open(GameState* state, WindowManager* wm,
                         std::function<void(uint8_t, uint32_t)> on_plant,
                         std::function<void(uint8_t)> on_water,
                         std::function<void(uint8_t)> on_harvest) {
    if (window_) return;
    if (!wm) return;

    plant_cb_ = std::move(on_plant);
    water_cb_ = std::move(on_water);
    harvest_cb_ = std::move(on_harvest);

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Farm_Get.bin.txt",
        "Farm Harvest", 300, 160, 380, 280);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    status_label_ = window_->AddWidget<Label>("Ready", 10, 4, ColorPalette::TEXT_NORMAL);
    status_label_->SetID("status_label");

    seeds_label_ = window_->AddWidget<Label>("", 10, 22, ColorPalette::TEXT_GOLD);
    seeds_label_->SetID("seeds_label");

    auto* hint = window_->AddWidget<Label>(
        "[P]lant  [W]ater  [H]arvest  [C]lose", 10, 40, ColorPalette::TEXT_DISABLED);
    hint->SetID("hint_label");

    // 3x3 plot grid
    float gx = 10, gy = 58;
    float slot_w = 115, slot_h = 62;
    float gap = 6;

    for (int p = 0; p < 9; p++) {
        float px = gx + (p % 3) * (slot_w + gap);
        float py = gy + (p / 3) * (slot_h + gap);

        char id[32];

        // Plot background placeholder
        snprintf(id, sizeof(id), "plot_%d", p);
        auto* plabel = window_->AddWidget<Label>("", px, py, 0xffffffff);
        plabel->SetID(id);
        plabel->SetRect(px, py, slot_w, slot_h);

        // Progress bar for growth
        snprintf(id, sizeof(id), "bar_%d", p);
        auto* pbar = window_->AddWidget<ProgressBar>(px + 4, py + 14, slot_w - 8, 10);
        pbar->SetID(id);
        pbar->SetColors({80, 200, 80, 255}, {40, 40, 40, 200});

        // Stage text label
        snprintf(id, sizeof(id), "stg_%d", p);
        auto* stg = window_->AddWidget<Label>("", px + 4, py + 26, 0xffcccccc);
        stg->SetID(id);

        // Plant button (visible when empty)
        snprintf(id, sizeof(id), "plant_%d", p);
        auto* plant_btn = window_->AddWidget<Button>("Plant", px + 4, py + 40, 50, 18);
        plant_btn->SetID(id);
        plant_btn->SetColors({50, 80, 50, 220}, {80, 130, 80, 220}, {30, 60, 30, 220});
        int plot_id = p;
        plant_btn->OnEvent([this, plot_id](const UIEvent& e) {
            if (e.type == UIEvent::Click && plant_cb_) {
                plant_cb_(static_cast<uint8_t>(plot_id), selected_seed_);
            }
        });

        // Water button (visible when planted and not watered)
        snprintf(id, sizeof(id), "water_%d", p);
        auto* water_btn = window_->AddWidget<Button>("Water", px + 4, py + 40, 50, 18);
        water_btn->SetID(id);
        water_btn->SetColors({30, 60, 100, 220}, {50, 100, 160, 220}, {20, 40, 70, 220});
        water_btn->OnEvent([this, plot_id](const UIEvent& e) {
            if (e.type == UIEvent::Click && water_cb_) {
                water_cb_(static_cast<uint8_t>(plot_id));
            }
        });

        // Harvest button (visible when ready)
        snprintf(id, sizeof(id), "harvest_%d", p);
        auto* harvest_btn = window_->AddWidget<Button>("Harvest", px + 58, py + 40, 55, 18);
        harvest_btn->SetID(id);
        harvest_btn->SetColors({80, 60, 30, 220}, {130, 100, 50, 220}, {60, 40, 20, 220});
        harvest_btn->OnEvent([this, plot_id](const UIEvent& e) {
            if (e.type == UIEvent::Click && harvest_cb_) {
                harvest_cb_(static_cast<uint8_t>(plot_id));
            }
        });

        // Wait label (visible when planted but not ready, for cooldown display)
        snprintf(id, sizeof(id), "wait_%d", p);
        auto* wait_lbl = window_->AddWidget<Label>("...", px + 58, py + 40, 0xff888888);
        wait_lbl->SetID(id);
    }

    Refresh(state);
}

void FarmGetDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
    plant_cb_ = nullptr;
    water_cb_ = nullptr;
    harvest_cb_ = nullptr;
}

void FarmGetDialog::SelectSeed(uint32_t seed_id) {
    selected_seed_ = seed_id;
}

void FarmGetDialog::Refresh(GameState* state) {
    if (!window_ || !state) return;

    // Update seeds list
    char seed_buf[512];
    int off = 0;
    off += snprintf(seed_buf + off, sizeof(seed_buf) - off, "Seed[%u]: ", selected_seed_);

    if (!state->network_farm_seeds.empty()) {
        for (size_t i = 0; i < state->network_farm_seeds.size() && off < (int)sizeof(seed_buf) - 32; i++) {
            off += snprintf(seed_buf + off, sizeof(seed_buf) - off,
                "[%zu]%s ", i + 1, state->network_farm_seeds[i].name.c_str());
        }
    } else {
        auto fallback = FarmSystem::GetAllSeeds();
        for (size_t i = 0; i < fallback.size() && off < (int)sizeof(seed_buf) - 32; i++) {
            off += snprintf(seed_buf + off, sizeof(seed_buf) - off,
                "[%zu]%s ", i + 1, fallback[i].name.c_str());
        }
    }

    if (seeds_label_) seeds_label_->SetText(seed_buf);

    // Update each plot
    for (int p = 0; p < 9; p++) {
        const GameState::NetworkFarmPlot* np = (p < (int)state->network_farm_plots.size())
            ? &state->network_farm_plots[p] : nullptr;

        bool planted = (np && np->seed_id > 0);
        bool ready = planted && np->ready;
        bool watered = planted && np->watered;

        char id[32];

        // Plot label
        char buf[128];
        snprintf(buf, sizeof(buf), "Plot %d: %s%s%s",
            p + 1,
            planted ? np->plant_name.c_str() : "Empty",
            watered ? " ~" : "",
            ready ? " READY!" : "");

        snprintf(id, sizeof(id), "plot_%d", p);
        auto* plabel = window_->FindWidget<Label>(id);
        if (plabel) plabel->SetText(buf);

        // Progress bar
        float progress = 0.0f;
        if (planted) {
            progress = np->growth_pct / 100.0f;
        }
        snprintf(id, sizeof(id), "bar_%d", p);
        auto* pbar = window_->FindWidget<ProgressBar>(id);
        if (pbar) pbar->SetProgress(progress);

        // Stage text
        snprintf(id, sizeof(id), "stg_%d", p);
        auto* stg = window_->FindWidget<Label>(id);
        if (stg) {
            if (planted) {
                char stg_buf[32];
                snprintf(stg_buf, sizeof(stg_buf), "%u%% S%d/%d",
                    np->growth_pct, np->growth_stage, np->max_stages);
                stg->SetText(stg_buf);
            } else {
                stg->SetText("");
            }
        }

        // Plant button - visible when empty
        snprintf(id, sizeof(id), "plant_%d", p);
        auto* plant_btn = window_->FindWidget<Button>(id);
        if (plant_btn) plant_btn->SetVisible(!planted);

        // Water button - visible when planted and not fully grown
        snprintf(id, sizeof(id), "water_%d", p);
        auto* water_btn = window_->FindWidget<Button>(id);
        if (water_btn) water_btn->SetVisible(planted && !watered && !ready);

        // Harvest button - visible when ready
        snprintf(id, sizeof(id), "harvest_%d", p);
        auto* harvest_btn = window_->FindWidget<Button>(id);
        if (harvest_btn) harvest_btn->SetVisible(ready);

        // Wait label - visible when planted but waiting for growth
        snprintf(id, sizeof(id), "wait_%d", p);
        auto* wait_lbl = window_->FindWidget<Label>(id);
        if (wait_lbl) wait_lbl->SetVisible(planted && watered && !ready);
    }
}
