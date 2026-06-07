#include "WeatherSystem.hpp"
#include <rendering/UIRenderer.hpp>
#include <algorithm>

void WeatherSystem::Render(UIRenderer& ui, float player_x, float player_z) {
    if (weather_ == Clear) return;
    
    for (auto& d : drops_) {
        // World to screen (simplified isometric projection)
        float sx = (d.x - player_x) * 12.0f + 640.0f;
        float sy = (d.z - player_z) * 12.0f + 360.0f - d.y * 2.0f;
        
        if (sx < -20 || sx > 1300 || sy < -20 || sy > 740) continue;
        
        float alpha = std::min(1.0f, d.life) * 0.6f;
        uint8_t a = (uint8_t)(alpha * 255);
        
        // Rain streak
        float len = d.speed * 0.02f;
        ui.DrawRect(sx, sy - len, 1, len, {180, 200, 255, a});
    }
}
