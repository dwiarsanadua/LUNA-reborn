#include "FamilyMarkDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

const uint32_t FamilyMarkDialog::palette_[NUM_COLORS] = {
    0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00,
    0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff,
    0xffc0c0c0, 0xff808080, 0xff800000, 0xff008000,
    0xff000080, 0xff808000, 0xff800080, 0xff008080,
};

void FamilyMarkDialog::Open(WindowManager* wm) {
    if (window_) return;

    window_ = new Window("Family Mark", 280, 140, 360, 380);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    float y = 10;

    // Mode indicator (Old: FDM_INFOTEXT)
    mode_label_ = window_->AddWidget<Label>("Mode: Family Mark", 15, y, ColorPalette::TEXT_NORMAL);
    y += 22;

    // Name input (Old: FDM_NAMEEDIT)
    window_->AddWidget<Label>("Mark Name:", 15, y, ColorPalette::TEXT_NORMAL);
    y += 18;
    name_input_ = window_->AddWidget<InputField>(15, y, 200, 22);
    name_input_->SetPlaceholder("Enter family mark name");
    y += 30;

    // Color palette (following GuildMarkDialog pattern)
    window_->AddWidget<Label>("Select Color:", 15, y, ColorPalette::TEXT_NORMAL);
    y += 18;
    for (int i = 0; i < NUM_COLORS; i++) {
        int col = i % 8;
        int row = i / 8;
        float bx = 15 + col * 28;
        float by = y + row * 28;
        auto* color_btn = window_->AddWidget<Button>("", bx, by, 24, 24);
        color_btn->SetColors(
            UIColor::FromRGBA(palette_[i]),
            UIColor::FromRGBA(palette_[i]),
            UIColor::FromRGBA(palette_[i]));
        color_btn->OnEvent([this, i](const UIEvent& e) {
            if (e.type == UIEvent::Click) SelectColor(i);
        });
    }
    y += 60;

    // Preset patterns (following GuildMarkDialog pattern)
    window_->AddWidget<Label>("Presets:", 15, y, ColorPalette::TEXT_NORMAL);
    y += 18;
    const char* preset_names[] = {"Heart", "Star", "Cross", "Diamond", "Circle", "Wing"};
    for (int i = 0; i < 6; i++) {
        float bx = 15 + (i % 3) * 100;
        float by = y + (i / 3) * 28;
        auto* preset_btn = window_->AddWidget<Button>(preset_names[i], bx, by, 90, 24);
        preset_btn->SetColors({50,50,80,220}, {70,70,120,220}, {40,40,60,220});
        preset_btn->OnEvent([this, i](const UIEvent& e) {
            if (e.type == UIEvent::Click) ApplyPreset(i);
        });
    }
    y += 64;

    // Info text area (Old: FDM_INFOTEXT)
    info_text_ = window_->AddWidget<Label>("Set a name and pick a pattern, then register.", 15, y, ColorPalette::TEXT_DISABLED);
    y += 22;

    // Register buttons (Old: FDM_REGISTOKBTN / FUM_REGISTOKBTN)
    btn_register_ = window_->AddWidget<Button>("Register Mark", 30, y, 130, 28);
    btn_register_->SetColors({50,90,130,220}, {80,120,160,220}, {30,60,100,220});
    btn_register_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnRegister();
    });

    btn_union_register_ = window_->AddWidget<Button>("Register Union", 180, y, 130, 28);
    btn_union_register_->SetColors({80,50,130,220}, {110,80,160,220}, {60,30,100,220});
    btn_union_register_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            union_mode_ = true;
            OnRegister();
        }
    });

    GenerateDefaultMark();
    ShowFamilyMark();
}

void FamilyMarkDialog::ShowFamilyMark() {
    union_mode_ = false;
    if (mode_label_) mode_label_->SetText("Mode: Family Mark");
    if (btn_register_) btn_register_->SetVisible(true);
    if (btn_union_register_) btn_union_register_->SetVisible(false);
}

void FamilyMarkDialog::ShowFamilyUnionMark() {
    union_mode_ = true;
    if (mode_label_) mode_label_->SetText("Mode: Family Union Mark");
    if (btn_register_) btn_register_->SetVisible(false);
    if (btn_union_register_) btn_union_register_->SetVisible(true);
}

void FamilyMarkDialog::SelectColor(int index) {
    selected_color_ = std::max(0, std::min(NUM_COLORS - 1, index));
    spdlog::debug("FamilyMarkDialog: selected color {}", selected_color_);
}

void FamilyMarkDialog::ApplyPreset(int index) {
    GenerateDefaultMark();
    uint32_t c = palette_[selected_color_];
    uint8_t cr = (c >> 0) & 0xff;
    uint8_t cg = (c >> 8) & 0xff;
    uint8_t cb = (c >> 16) & 0xff;

    for (int py = 0; py < mark_h_; py++) {
        for (int px = 0; px < mark_w_; px++) {
            int idx = (py * mark_w_ + px) * 4;
            bool fill = false;
            switch (index) {
                case 0: // Heart
                    fill = (px == 4 || px == 11) && py >= 2 && py <= 6;
                    break;
                case 1: // Star
                    fill = (abs(px - 8) + abs(py - 6)) < 6;
                    break;
                case 2: // Cross
                    fill = (abs(px - 8) < 2 || abs(py - 6) < 2);
                    break;
                case 3: // Diamond
                    fill = (abs(px - 8) + abs(py - 6)) < 8;
                    break;
                case 4: // Circle
                    fill = ((px - 8) * (px - 8) + (py - 6) * (py - 6)) < 30;
                    break;
                case 5: // Wing
                    fill = (py > 2 && py < 10 && (px % 5 == 0 || px == 8));
                    break;
            }
            if (fill) {
                mark_rgba_[idx + 0] = cr;
                mark_rgba_[idx + 1] = cg;
                mark_rgba_[idx + 2] = cb;
                mark_rgba_[idx + 3] = 255;
            }
        }
    }
}

void FamilyMarkDialog::GenerateDefaultMark() {
    mark_w_ = 16;
    mark_h_ = 12;
    mark_rgba_.assign(mark_w_ * mark_h_ * 4, 0);
    uint32_t c = palette_[selected_color_];
    // Simple border
    for (int x = 0; x < mark_w_; x++) {
        int top = (0 * mark_w_ + x) * 4;
        int bot = ((mark_h_ - 1) * mark_w_ + x) * 4;
        mark_rgba_[top + 0] = (c >> 0) & 0xff;
        mark_rgba_[top + 1] = (c >> 8) & 0xff;
        mark_rgba_[top + 2] = (c >> 16) & 0xff;
        mark_rgba_[top + 3] = 255;
        mark_rgba_[bot + 0] = (c >> 0) & 0xff;
        mark_rgba_[bot + 1] = (c >> 8) & 0xff;
        mark_rgba_[bot + 2] = (c >> 16) & 0xff;
        mark_rgba_[bot + 3] = 255;
    }
    for (int y = 0; y < mark_h_; y++) {
        int left = (y * mark_w_ + 0) * 4;
        int right = (y * mark_w_ + (mark_w_ - 1)) * 4;
        mark_rgba_[left + 0] = (c >> 0) & 0xff;
        mark_rgba_[left + 1] = (c >> 8) & 0xff;
        mark_rgba_[left + 2] = (c >> 16) & 0xff;
        mark_rgba_[left + 3] = 255;
        mark_rgba_[right + 0] = (c >> 0) & 0xff;
        mark_rgba_[right + 1] = (c >> 8) & 0xff;
        mark_rgba_[right + 2] = (c >> 16) & 0xff;
        mark_rgba_[right + 3] = 255;
    }
}

void FamilyMarkDialog::OnRegister() {
    if (!name_input_ || name_input_->GetText().empty()) {
        if (info_text_) info_text_->SetText("Please enter a mark name.");
        return;
    }
    std::string encoded = EncodeBase64();
    std::string name = name_input_->GetText();
    uint32_t name_hash = 0;
    for (char ch : name) name_hash = name_hash * 31 + static_cast<uint8_t>(ch);

    if (union_mode_) {
        if (on_union_register_) {
            on_union_register_(name_hash, encoded);
            spdlog::info("FamilyMarkDialog: registered union mark '{}'", name);
        }
    } else {
        if (on_register_) {
            on_register_(name_hash, encoded);
            spdlog::info("FamilyMarkDialog: registered family mark '{}'", name);
        }
    }
    if (info_text_) info_text_->SetText("Mark registered successfully.");
}

std::string FamilyMarkDialog::EncodeBase64() const {
    if (mark_rgba_.empty()) return "";
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    std::vector<uint8_t> header;
    header.push_back(static_cast<uint8_t>(mark_w_));
    header.push_back(static_cast<uint8_t>(mark_h_));
    header.insert(header.end(), mark_rgba_.begin(), mark_rgba_.end());

    for (size_t i = 0; i < header.size(); i += 3) {
        unsigned int b = (static_cast<unsigned int>(header[i]) << 16);
        if (i + 1 < header.size()) b |= (static_cast<unsigned int>(header[i + 1]) << 8);
        if (i + 2 < header.size()) b |= static_cast<unsigned int>(header[i + 2]);
        result += b64[(b >> 18) & 0x3f];
        result += b64[(b >> 12) & 0x3f];
        result += (i + 1 < header.size()) ? b64[(b >> 6) & 0x3f] : '=';
        result += (i + 2 < header.size()) ? b64[b & 0x3f] : '=';
    }
    return result;
}

void FamilyMarkDialog::RenderMarkPreview(UIRenderer& ui, float x, float y, float size) {
    if (mark_rgba_.empty()) return;
    float cell_w = size / mark_w_;
    float cell_h = size / mark_h_;
    for (int py = 0; py < mark_h_; py++) {
        for (int px = 0; px < mark_w_; px++) {
            int idx = (py * mark_w_ + px) * 4;
            uint8_t r = mark_rgba_[idx + 0];
            uint8_t g = mark_rgba_[idx + 1];
            uint8_t b = mark_rgba_[idx + 2];
            uint8_t a = mark_rgba_[idx + 3];
            if (a > 0) {
                ui.DrawRect(x + px * cell_w, y + py * cell_h,
                            cell_w + 0.5f, cell_h + 0.5f,
                            UIColor{r, g, b, a});
            }
        }
    }
    ui.DrawBorder(x - 1, y - 1, size + 2, size + 2, UIColor{150, 150, 150, 200});
}

void FamilyMarkDialog::SetNetworkCallbacks(
    std::function<void(uint32_t, const std::string&)> on_register_mark,
    std::function<void(uint32_t, const std::string&)> on_register_union_mark) {
    on_register_ = std::move(on_register_mark);
    on_union_register_ = std::move(on_register_union_mark);
}

void FamilyMarkDialog::Close() {
    if (window_) {
        window_->Close();
        window_ = nullptr;
    }
}
