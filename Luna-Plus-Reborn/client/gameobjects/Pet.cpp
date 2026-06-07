#include "Pet.hpp"
#include <rendering/UIRenderer.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>

void Pet::Init(const std::string& name, uint32_t id) {
    name_ = name; id_ = id;
    level_ = 1; hunger_ = 100; evolution_ = 1;
}

void Pet::Update(float dt, float follow_x, float follow_y, float follow_z) {
    if (!active_) return;
    timer_ += dt;
    
    // Follow with slight delay
    float dx = follow_x - x_, dz = follow_z - z_;
    float dist = sqrtf(dx * dx + dz * dz);
    if (dist > follow_dist_) {
        float speed = std::min(4.0f * dt, dist - follow_dist_);
        x_ += (dx / dist) * speed;
        z_ += (dz / dist) * speed;
        y_ = follow_y + 0.3f;
    }
    
    // Hunger decreases over time
    if (timer_ > 5.0f) {
        timer_ = 0;
        hunger_ = std::max(0, hunger_ - 1);
        if (hunger_ > 50) {
            exp_ += 1;
            if (exp_ >= level_ * 100) {
                exp_ = 0; level_++;
                if (level_ % 10 == 0 && evolution_ < 3) evolution_++;
            }
        }
    }
}

void Pet::RenderOverhead(UIRenderer& ui) {
    if (!active_) return;
    float sx = (x_ * 12.0f + 640.0f) - 30;
    float sy = (z_ * 12.0f + 360.0f) - 50;
    if (sx < -50 || sx > 1330 || sy < -50 || sy > 770) return;
    
    char buf[64];
    snprintf(buf, sizeof(buf), "%s Lv.%d (Evo%d)", name_.c_str(), level_, evolution_);
    ui.DrawText(sx, sy, 0xff88ff88, "%s", buf);
    // Hunger bar
    float hunger_pct = hunger_ / 100.0f;
    ui.DrawBar(sx, sy + 14, 60, 5, hunger_pct, {255, 200, 60, 255}, {60, 40, 0, 180});
}
