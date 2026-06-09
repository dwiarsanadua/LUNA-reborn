#pragma once
#include <entt/entt.hpp>
#include <cstdint>

struct ComboComponent {
    int32_t combo_count = 0;
    int32_t max_combo = 0;
    float combo_timer = 0.0f;
    float combo_timeout = 3.0f;
    entt::entity last_target = entt::null;

    void Reset() {
        combo_count = 0;
        combo_timer = 0.0f;
        last_target = entt::null;
    }

    void RegisterHit() {
        combo_count++;
        combo_timer = 0.0f;
        if (combo_count > max_combo) max_combo = combo_count;
    }

    bool IsExpired() const {
        return combo_count > 0 && combo_timer >= combo_timeout;
    }
};
