#include "MiniMapDlg.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>

MiniMapDlg::MiniMapDlg() {}

MiniMapDlg::~MiniMapDlg() {
    if (bgfx::isValid(fbo_)) bgfx::destroy(fbo_);
    if (bgfx::isValid(render_target_)) bgfx::destroy(render_target_);
}

void MiniMapDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/MiniMap.bin.txt",
        "MiniMap", 1090, 10, map_w_ + 20, map_h_ + 40);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(20);
    CreateRenderTarget();
}

void MiniMapDlg::Close() {
    window_ = nullptr;
}

void MiniMapDlg::SetPlayerPos(float world_x, float world_z) {
    player_x_ = world_x;
    player_z_ = world_z;
}

void MiniMapDlg::AddEntity(uint32_t id, float x, float z, uint32_t color, float size) {
    MiniMapEntity e;
    e.id = id;
    e.pos = glm::vec2(x, z);
    e.color = color;
    e.size = size;
    e.is_player = false;
    entities_.push_back(e);
}

void MiniMapDlg::ClearEntities() {
    entities_.clear();
}

void MiniMapDlg::SetFogData(const std::vector<bool>& revealed, int map_w, int map_h) {
    fog_revealed_ = revealed;
    fog_w_ = map_w;
    fog_h_ = map_h;
}

void MiniMapDlg::CreateRenderTarget() {
    if (fb_ready_) return;
    uint16_t rt_w = (uint16_t)map_w_;
    uint16_t rt_h = (uint16_t)map_h_;

    render_target_ = bgfx::createTexture2D(rt_w, rt_h, false, 1,
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);

    bgfx::TextureHandle fb_texs[] = { render_target_ };
    fbo_ = bgfx::createFrameBuffer(1, fb_texs, true);
    fb_ready_ = bgfx::isValid(fbo_);
}

void MiniMapDlg::RenderTerrainView() {
    if (!fb_ready_) return;
    // Render a top-down view of the terrain into the render target
    // For now we fill with a terrain-colored background
    bgfx::setViewFrameBuffer(static_cast<bgfx::ViewId>(50), fbo_);
    bgfx::setViewRect(static_cast<bgfx::ViewId>(50), 0, 0, (uint16_t)map_w_, (uint16_t)map_h_);
    bgfx::setViewClear(static_cast<bgfx::ViewId>(50), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0xff2d4a1e, 1.0f, 0);
    bgfx::touch(static_cast<bgfx::ViewId>(50));
}

void MiniMapDlg::Update(float dt) {
    (void)dt;
    if (!window_) return;
    RenderTerrainView();
}

void MiniMapDlg::Render(UIRenderer& ui) {
    if (!window_) return;

    float wx = window_->GetX();
    float wy = window_->GetY() + window_->GetTitleBarH();
    float cx = wx + 10;
    float cy = wy + 10;
    float half_w = map_w_ * 0.5f;
    float half_h = map_h_ * 0.5f;

    // Background
    ui.DrawRect(cx - 2, cy - 2, map_w_ + 4, map_h_ + 4, {0, 0, 0, 200});

    // Draw rendered terrain texture if available
    if (bgfx::isValid(render_target_)) {
        ui.DrawImage(cx, cy, map_w_, map_h_, render_target_, {255, 255, 255, 255});
    } else {
        ui.DrawRect(cx, cy, map_w_, map_h_, {45, 74, 30, 255});
    }

    // Draw fog of war
    DrawFogOverlay(ui, cx, cy);

    // Draw entities
    for (auto& e : entities_) {
        float ex = cx + half_w + (e.pos.x - player_x_) * map_scale_;
        float ey = cy + half_h + (e.pos.y - player_z_) * map_scale_;
        if (ex >= cx && ex <= cx + map_w_ && ey >= cy && ey <= cy + map_h_) {
            uint8_t r = (e.color >> 16) & 0xFF;
            uint8_t g = (e.color >> 8) & 0xFF;
            uint8_t b = e.color & 0xFF;
            ui.DrawRect(ex - e.size * 0.5f, ey - e.size * 0.5f, e.size, e.size, {r, g, b, 220});
        }
    }

    // Player dot (always center, bright white/green)
    float ps = 6.0f;
    ui.DrawRect(cx + half_w - ps * 0.5f, cy + half_h - ps * 0.5f, ps, ps, {100, 255, 100, 255});

    // Border
    ui.DrawBorder(cx - 2, cy - 2, map_w_ + 4, map_h_ + 4, {100, 180, 255, 180});
}

void MiniMapDlg::DrawFogOverlay(UIRenderer& ui, float sx, float sy) {
    if (fog_revealed_.empty() || fog_w_ <= 0) return;

    float cell_w = map_w_ / (float)fog_w_;
    float cell_h = map_h_ / (float)fog_h_;

    for (int y = 0; y < fog_h_; y++) {
        for (int x = 0; x < fog_w_; x++) {
            int idx = y * fog_w_ + x;
            if (idx >= (int)fog_revealed_.size()) break;
            if (!fog_revealed_[idx]) {
                ui.DrawRect(sx + x * cell_w, sy + y * cell_h, cell_w + 1, cell_h + 1, {0, 0, 0, 180});
            }
        }
    }
}
