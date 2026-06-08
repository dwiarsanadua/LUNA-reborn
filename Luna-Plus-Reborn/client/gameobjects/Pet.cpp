#include "Pet.hpp"
#include <rendering/UIRenderer.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>

void Pet::Init(const std::string& name, uint32_t id) {
    name_ = name;
    id_ = id;
    level_ = 1;
    max_hp_ = 100;
    hunger_ = 100;
    evolution_ = 1;
    exp_ = 0;
    exp_next_ = 50;
}

void Pet::ApplyNetworkState(uint16_t level, uint16_t hp, uint16_t max_hp, uint16_t satiation,
                            uint8_t evolution, uint32_t exp, uint32_t exp_to_next) {
    level_ = level;
    max_hp_ = max_hp;
    hunger_ = satiation;
    evolution_ = evolution;
    exp_ = exp;
    exp_next_ = exp_to_next > 0 ? exp_to_next : 50;
    (void)hp;
}

void Pet::Update(float dt, float follow_x, float follow_y, float follow_z) {
    if (!active_) return;
    timer_ += dt;

    float dx = follow_x - x_, dz = follow_z - z_;
    float dist = sqrtf(dx * dx + dz * dz);
    if (dist > follow_dist_) {
        float speed = std::min(4.0f * dt, dist - follow_dist_);
        x_ += (dx / dist) * speed;
        z_ += (dz / dist) * speed;
        y_ = follow_y + 0.3f;
    } else {
        y_ = follow_y + 0.3f;
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
    float hunger_pct = hunger_ / 100.0f;
    ui.DrawBar(sx, sy + 14, 60, 5, hunger_pct, {255, 200, 60, 255}, {60, 40, 0, 180});
}
