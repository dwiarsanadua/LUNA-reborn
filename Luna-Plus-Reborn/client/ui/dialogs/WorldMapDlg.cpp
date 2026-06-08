#include "WorldMapDlg.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>

WorldMapDlg::WorldMapDlg() {}

void WorldMapDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/WorldMapDlg.bin.txt",
        "World Map", 0, 0, 1280, 720);
    window_->SetClosable(true);
    window_->SetMovable(false);
    window_->SetTitleBarH(0);
}

void WorldMapDlg::Close() {
    window_ = nullptr;
}

void WorldMapDlg::SetPlayerPos(float world_x, float world_z) {
    player_x_ = world_x;
    player_z_ = world_z;
}

void WorldMapDlg::SetZones(const std::vector<WorldZone>& zones) {
    zones_ = zones;
}

void WorldMapDlg::DiscoverZone(const std::string& name) {
    for (auto& z : zones_) {
        if (z.name == name) {
            z.discovered = true;
            break;
        }
    }
}

void WorldMapDlg::WorldToMap(float wx, float wz, float& mx, float& mz) const {
    float nx = (wx - world_ox_) / world_w_;
    float nz = (wz - world_oz_) / world_h_;
    mx = map_x_ + nx * map_w_;
    mz = map_y_ + nz * map_h_;
}

void WorldMapDlg::Render(UIRenderer& ui) {
    if (!window_) return;

    // Full-screen dark overlay
    ui.DrawRect(0, 0, 1280, 720, {10, 10, 20, 240});

    // Map background
    ui.DrawRect(map_x_, map_y_, map_w_, map_h_, {30, 45, 25, 255});
    ui.DrawBorder(map_x_, map_y_, map_w_, map_h_, {100, 180, 255, 150}, 2.0f);

    // Grid lines
    int num_cells = 8;
    float cell_w = map_w_ / num_cells;
    float cell_h = map_h_ / num_cells;
    for (int i = 1; i < num_cells; i++) {
        ui.DrawRect(map_x_ + i * cell_w, map_y_, 1, map_h_, {60, 80, 50, 100});
        ui.DrawRect(map_x_, map_y_ + i * cell_h, map_w_, 1, {60, 80, 50, 100});
    }

    // Draw zones
    for (auto& z : zones_) {
        float mx, mz;
        WorldToMap(z.center_x, z.center_z, mx, mz);
        float mr = z.radius * (map_w_ / world_w_);

        uint8_t r = (z.color >> 16) & 0xFF;
        uint8_t g = (z.color >> 8) & 0xFF;
        uint8_t b = z.color & 0xFF;
        uint8_t alpha = z.discovered ? (uint8_t)120 : (uint8_t)40;

        ui.DrawRect(mx - mr, mz - mr, mr * 2, mr * 2, {r, g, b, alpha});
        ui.DrawBorder(mx - mr, mz - mr, mr * 2, mr * 2, {r, g, b, (uint8_t)(alpha + 60)});

        if (z.discovered) {
            ui.DrawText(mx - 20, mz - 6, (r << 24) | (g << 16) | (b << 8) | 0xFF, z.name.c_str());
        }
    }

    // Undiscovered areas overlay
    for (auto& z : zones_) {
        if (!z.discovered) {
            float mx, mz;
            WorldToMap(z.center_x, z.center_z, mx, mz);
            float mr = z.radius * (map_w_ / world_w_);
            ui.DrawRect(mx - mr, mz - mr, mr * 2, mr * 2, {0, 0, 0, 160});
        }
    }

    // Player marker
    {
        float mx, mz;
        WorldToMap(player_x_, player_z_, mx, mz);
        float ps = 8.0f;
        ui.DrawRect(mx - ps, mz - ps, ps * 2, ps * 2, {100, 255, 100, 255});
        ui.DrawBorder(mx - ps - 1, mz - ps - 1, ps * 2 + 2, ps * 2 + 2, {255, 255, 255, 180});

        char buf[64];
        snprintf(buf, sizeof(buf), "You (%.0f, %.0f)", player_x_, player_z_);
        ui.DrawText(mx + 12, mz - 6, 0xffffffff, buf);
    }

    // Title
    ui.DrawTextCentered(10, 0xffffcc88, "=== WORLD MAP ===");
    ui.DrawText(10, 690, 0xff888888, "ESC or right-click to close");
}
