#include "AvatarDialog.hpp"
#include <cstdio>

static const char* hair_styles[] = {"Short", "Long", "Ponytail", "Bangs", "Mohawk", "Bald"};
static const char* face_types[] = {"Neutral", "Smile", "Serious", "Cute", "Cool", "Mysterious"};
static const char* skin_tones[] = {"Fair", "Light", "Tan", "Olive", "Brown", "Dark"};

void AvatarDialog::Open(GameState* state, WindowManager* wm) {
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/BodyChangeDialog.bin.txt",
        "Avatar Customization", 200, 60, 420, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Customize Your Character", 10, 4, 0xff88ccff);

    // Preview
    preview_label_ = window_->AddWidget<Label>("", 10, 28, 0xffffffff);

    // Hair grid
    auto* hair_grid = window_->AddWidget<Grid>(2, 3, 110, 24, 10, 120);
    hair_grid->SetPadding(3);
    for (int i = 0; i < 6; i++) {
        GridSlot gs; gs.empty = false; gs.text = hair_styles[i];
        hair_grid->SetSlot(i / 3, i % 3, gs);
    }
    hair_grid->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            hair_idx_ = row * 3 + col;
            Refresh(nullptr);
        }
    });

    auto* face_grid = window_->AddWidget<Grid>(2, 3, 110, 24, 10, 200);
    face_grid->SetPadding(3);
    for (int i = 0; i < 6; i++) {
        GridSlot gs; gs.empty = false; gs.text = face_types[i];
        face_grid->SetSlot(i / 3, i % 3, gs);
    }
    face_grid->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            face_idx_ = row * 3 + col;
            Refresh(nullptr);
        }
    });

    auto* skin_grid = window_->AddWidget<Grid>(2, 3, 110, 24, 10, 280);
    skin_grid->SetPadding(3);
    for (int i = 0; i < 6; i++) {
        GridSlot gs; gs.empty = false; gs.text = skin_tones[i];
        skin_grid->SetSlot(i / 3, i % 3, gs);
    }
    skin_grid->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            skin_idx_ = row * 3 + col;
            Refresh(nullptr);
        }
    });

    // Labels
    window_->AddWidget<Label>("Hair Style", 10, 104, 0xffffcc88);
    window_->AddWidget<Label>("Face Type", 10, 184, 0xffffcc88);
    window_->AddWidget<Label>("Skin Tone", 10, 264, 0xffffcc88);

    auto* apply_btn = window_->AddWidget<Button>("Apply Changes", 10, 340, 130, 24);
    apply_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    apply_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            state->chat_messages.push_back("Appearance updated!");
            Refresh(state);
        }
    });

    auto* random_btn = window_->AddWidget<Button>("Randomize", 160, 340, 100, 24);
    random_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            hair_idx_ = rand() % 6;
            face_idx_ = rand() % 6;
            skin_idx_ = rand() % 6;
            Refresh(nullptr);
        }
    });

    Refresh(state);
}

void AvatarDialog::Refresh(GameState* state) {
    (void)state;
    if (!preview_label_) return;
    char buf[256];
    snprintf(buf, sizeof(buf),
        "=== Current Appearance ===\n\n"
        "Hair: %s\n"
        "Face: %s\n"
        "Skin: %s\n\n"
        "Click on options to change.\nApply to save changes.",
        hair_styles[hair_idx_ % 6], face_types[face_idx_ % 6], skin_tones[skin_idx_ % 6]);
    preview_label_->SetText(buf);
}

void AvatarDialog::UpdateFromState(GameState*) {}
