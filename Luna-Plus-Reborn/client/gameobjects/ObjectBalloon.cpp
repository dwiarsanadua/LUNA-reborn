#include "ObjectBalloon.hpp"
#include <rendering/UIRenderer.hpp>
#include <cstdio>
#include <algorithm>
#include <cmath>

std::vector<BalloonData> ObjectBalloon::balloons_;

void ObjectBalloon::Add(const BalloonData& data) {
    balloons_.push_back(data);
}

void ObjectBalloon::Remove(int index) {
    if (index >= 0 && index < (int)balloons_.size())
        balloons_.erase(balloons_.begin() + index);
}

void ObjectBalloon::Clear() {
    balloons_.clear();
}

void ObjectBalloon::UpdateAll(float dt) {
    (void)dt;
    balloons_.erase(
        std::remove_if(balloons_.begin(), balloons_.end(),
            [](auto& b) { return !b.alive; }),
        balloons_.end());
}

void ObjectBalloon::RenderAll(UIRenderer& ui) {
    for (auto& b : balloons_) {
        // Project world → screen (simplified isometric)
        float sx = (b.world_x * 12.0f + 640.0f);
        float sy = (b.world_z * 12.0f + 360.0f) - b.world_y * 12.0f;
        if (sx < -100 || sx > 1380 || sy < -100 || sy > 820) continue;
        
        // Name
        char buf[128]; snprintf(buf, sizeof(buf), "%s Lv.%d", b.name.c_str(), b.level);
        ui.DrawText(sx - 30, sy - 24, b.name_color, "%s", buf);
        
        // Title
        if (!b.title.empty()) {
            ui.DrawText(sx - 30, sy - 12, b.title_color, "%s", b.title.c_str());
        }
        
        // HP bar
        if (b.show_hp && b.max_hp > 0) {
            float pct = (float)b.hp / b.max_hp;
            ui.DrawBar(sx - 25, sy, 50, 5, pct, {60, 200, 60, 200}, {40, 40, 40, 180});
        }
        
        // Quest marker
        if (b.quest_marker) {
            ui.DrawText(sx + 30, sy - 24, 0xff44ff44, "!");
        }
        
        // Billboard
        if (b.billboard_size > 0) {
            float bs = b.billboard_size;
            ui.DrawRect(sx - bs/2, sy - 24 - bs, bs, bs, {
                (uint8_t)((b.billboard_color >> 16) & 0xFF),
                (uint8_t)((b.billboard_color >> 8) & 0xFF),
                (uint8_t)(b.billboard_color & 0xFF), 200
            });
        }
    }
}
