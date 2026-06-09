#include "GuildMarkDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstring>
#include <algorithm>

const uint32_t GuildMarkDialog::palette_[NUM_COLORS] = {
    0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00,
    0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff,
    0xffc0c0c0, 0xff808080, 0xff800000, 0xff008000,
    0xff000080, 0xff808000, 0xff800080, 0xff008080,
};

void GuildMarkDialog::Open(WindowManager* wm, uint32_t guild_id) {
    if (window_) return;
    guild_id_ = guild_id;

    if (wm) {
        window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GuildMarkDlg.bin.txt",
            "Guild Emblem", 300, 160, 340, 340);
    } else {
        window_ = new Window("Guild Emblem", 300, 160, 340, 340);
    }
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    // Emblem preview area
    auto* preview_label = window_->AddWidget<Label>("Current Emblem:", 20, 30);
    preview_label->SetColor(0xffcccccc);

    // Color palette buttons
    auto* color_label = window_->AddWidget<Label>("Select Color:", 20, 130);
    color_label->SetColor(0xffcccccc);

    for (int i = 0; i < NUM_COLORS; i++) {
        int col = i % 8;
        int row = i / 8;
        float bx = 20 + col * 28;
        float by = 150 + row * 28;
        auto* color_btn = window_->AddWidget<Button>("", bx, by, 24, 24);
        color_btn->SetColors(
            UIColor::FromRGBA(palette_[i]),
            UIColor::FromRGBA(palette_[i]),
            UIColor::FromRGBA(palette_[i]));
        color_btn->OnEvent([this, i](const UIEvent& e) {
            if (e.type == UIEvent::Click) SelectPreset(i);
        });
    }

    // Preset emblems
    auto* preset_label = window_->AddWidget<Label>("Presets:", 20, 210);
    preset_label->SetColor(0xffcccccc);

    const char* preset_names[] = {"Star", "Cross", "Diamond", "Circle", "Shield", "Crown"};
    for (int i = 0; i < 6; i++) {
        float bx = 20 + (i % 3) * 105;
        float by = 230 + (i / 3) * 30;
        auto* preset_btn = window_->AddWidget<Button>(preset_names[i], bx, by, 95, 24);
        preset_btn->SetColors({50,50,80,220}, {70,70,120,220}, {40,40,60,220});
        preset_btn->OnEvent([this, i](const UIEvent& e) {
            if (e.type == UIEvent::Click) {
                GenerateDefaultEmblem();
                // Modify based on preset index
                uint32_t c = palette_[selected_color_];
                uint8_t cr = (c >> 0) & 0xff;
                uint8_t cg = (c >> 8) & 0xff;
                uint8_t cb = (c >> 16) & 0xff;
                for (int py = 0; py < emblem_h_; py++) {
                    for (int px = 0; px < emblem_w_; px++) {
                        int idx = (py * emblem_w_ + px) * 4;
                        bool fill = false;
                        switch (i) {
                            case 0: // Star
                                fill = (abs(px - 8) + abs(py - 8)) < 7;
                                break;
                            case 1: // Cross
                                fill = (abs(px - 8) < 2 || abs(py - 8) < 2);
                                break;
                            case 2: // Diamond
                                fill = (abs(px - 8) + abs(py - 8)) < 9;
                                break;
                            case 3: // Circle
                                fill = ((px - 8) * (px - 8) + (py - 8) * (py - 8)) < 49;
                                break;
                            case 4: // Shield
                                fill = (py > 2 && py < 14 && px > 2 && px < 14);
                                break;
                            case 5: // Crown
                                fill = (py < 6 && (px % 4 == 0 || px == 8));
                                break;
                        }
                        if (fill) {
                            emblem_rgba_[idx + 0] = cr;
                            emblem_rgba_[idx + 1] = cg;
                            emblem_rgba_[idx + 2] = cb;
                            emblem_rgba_[idx + 3] = 255;
                        }
                    }
                }
            }
        });
    }

    // Upload button
    auto* upload_btn = window_->AddWidget<Button>("Upload", 60, 280, 100, 28);
    upload_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    upload_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && on_upload_) {
            std::string encoded = EncodeEmblemBase64();
            on_upload_(guild_id_, encoded);
            spdlog::info("GuildMarkDialog: uploading emblem for guild {}", guild_id_);
        }
    });

    // Cancel button
    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 180, 280, 100, 28);
    cancel_btn->SetColors({80,40,40,220}, {130,60,60,220}, {50,30,30,220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });

    if (emblem_rgba_.empty()) {
        GenerateDefaultEmblem();
    }
}

void GuildMarkDialog::Close() {
    window_ = nullptr;
}

void GuildMarkDialog::SetNetworkCallbacks(
    std::function<void(uint32_t, const std::string&)> upload_emblem,
    std::function<void(uint32_t)> download_emblem) {
    on_upload_ = upload_emblem;
    on_download_ = download_emblem;
}

void GuildMarkDialog::SetEmblemData(const std::vector<uint8_t>& rgba_data, int w, int h) {
    emblem_rgba_ = rgba_data;
    emblem_w_ = std::max(8, std::min(64, w));
    emblem_h_ = std::max(8, std::min(64, h));
}

void GuildMarkDialog::ClearEmblem() {
    emblem_rgba_.clear();
}

void GuildMarkDialog::GenerateDefaultEmblem() {
    emblem_w_ = 16;
    emblem_h_ = 16;
    emblem_rgba_.assign(emblem_w_ * emblem_h_ * 4, 0);
    // Transparent background
    for (int y = 0; y < emblem_h_; y++) {
        for (int x = 0; x < emblem_w_; x++) {
            int idx = (y * emblem_w_ + x) * 4;
            emblem_rgba_[idx + 0] = 0;
            emblem_rgba_[idx + 1] = 0;
            emblem_rgba_[idx + 2] = 0;
            emblem_rgba_[idx + 3] = 0;
        }
    }
    // Draw a simple border
    uint32_t c = palette_[selected_color_];
    for (int x = 0; x < emblem_w_; x++) {
        emblem_rgba_[(0 * emblem_w_ + x) * 4 + 0] = (c >> 0) & 0xff;
        emblem_rgba_[(0 * emblem_w_ + x) * 4 + 1] = (c >> 8) & 0xff;
        emblem_rgba_[(0 * emblem_w_ + x) * 4 + 2] = (c >> 16) & 0xff;
        emblem_rgba_[(0 * emblem_w_ + x) * 4 + 3] = 255;
        emblem_rgba_[((emblem_h_ - 1) * emblem_w_ + x) * 4 + 0] = (c >> 0) & 0xff;
        emblem_rgba_[((emblem_h_ - 1) * emblem_w_ + x) * 4 + 1] = (c >> 8) & 0xff;
        emblem_rgba_[((emblem_h_ - 1) * emblem_w_ + x) * 4 + 2] = (c >> 16) & 0xff;
        emblem_rgba_[((emblem_h_ - 1) * emblem_w_ + x) * 4 + 3] = 255;
    }
    for (int y = 0; y < emblem_h_; y++) {
        emblem_rgba_[(y * emblem_w_ + 0) * 4 + 0] = (c >> 0) & 0xff;
        emblem_rgba_[(y * emblem_w_ + 0) * 4 + 1] = (c >> 8) & 0xff;
        emblem_rgba_[(y * emblem_w_ + 0) * 4 + 2] = (c >> 16) & 0xff;
        emblem_rgba_[(y * emblem_w_ + 0) * 4 + 3] = 255;
        emblem_rgba_[(y * emblem_w_ + (emblem_w_ - 1)) * 4 + 0] = (c >> 0) & 0xff;
        emblem_rgba_[(y * emblem_w_ + (emblem_w_ - 1)) * 4 + 1] = (c >> 8) & 0xff;
        emblem_rgba_[(y * emblem_w_ + (emblem_w_ - 1)) * 4 + 2] = (c >> 16) & 0xff;
        emblem_rgba_[(y * emblem_w_ + (emblem_w_ - 1)) * 4 + 3] = 255;
    }
}

void GuildMarkDialog::SelectPreset(int index) {
    selected_color_ = std::max(0, std::min(NUM_COLORS - 1, index));
}

std::string GuildMarkDialog::EncodeEmblemBase64() const {
    if (emblem_rgba_.empty()) return "";
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    size_t data_size = emblem_rgba_.size();
    // Header: width,height (2 bytes each, little-endian) + pixel data
    std::vector<uint8_t> header;
    header.push_back(static_cast<uint8_t>(emblem_w_));
    header.push_back(static_cast<uint8_t>(emblem_h_));
    header.insert(header.end(), emblem_rgba_.begin(), emblem_rgba_.end());

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

void GuildMarkDialog::RenderEmblemPreview(UIRenderer& ui, float x, float y, float size) {
    if (emblem_rgba_.empty()) return;
    float cell_w = size / emblem_w_;
    float cell_h = size / emblem_h_;
    for (int py = 0; py < emblem_h_; py++) {
        for (int px = 0; px < emblem_w_; px++) {
            int idx = (py * emblem_w_ + px) * 4;
            uint8_t r = emblem_rgba_[idx + 0];
            uint8_t g = emblem_rgba_[idx + 1];
            uint8_t b = emblem_rgba_[idx + 2];
            uint8_t a = emblem_rgba_[idx + 3];
            if (a > 0) {
                float alpha = a / 255.0f;
                ui.DrawRect(x + px * cell_w, y + py * cell_h,
                            cell_w + 0.5f, cell_h + 0.5f,
                            UIColor{r, g, b, static_cast<uint8_t>(alpha * 255)});
            }
        }
    }
    ui.DrawBorder(x - 1, y - 1, size + 2, size + 2, UIColor{150, 150, 150, 200});
}

void GuildMarkDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    if (window_ && !window_->IsVisible()) {
        Close();
    }
}
