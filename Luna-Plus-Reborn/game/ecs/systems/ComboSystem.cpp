#include "ComboSystem.hpp"
#include "../components/ComboComponent.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void ComboSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<ComboComponent>();
    for (auto entity : view) {
        auto& combo = view.get<ComboComponent>(entity);
        if (combo.combo_count > 0) {
            combo.combo_timer += dt;
            if (combo.IsExpired()) {
                int32_t expired_count = combo.combo_count;
                combo.Reset();
                spdlog::debug("COMBO: entity {} combo expired at {} hits",
                              static_cast<uint32_t>(entity), expired_count);
            }
        }
    }
}

void ComboSystem::RegisterHit(entt::registry& registry,
                              entt::entity attacker, entt::entity target) {
    if (!registry.valid(attacker)) return;

    auto* combo_ptr = registry.try_get<ComboComponent>(attacker);
    if (!combo_ptr) {
        combo_ptr = &registry.emplace<ComboComponent>(attacker);
    }
    auto& combo = *combo_ptr;

    if (target != combo.last_target && combo.last_target != entt::null) {
        int32_t lost_count = combo.combo_count;
        combo.Reset();
        spdlog::debug("COMBO: entity {} target switch, combo lost at {} hits",
                      static_cast<uint32_t>(attacker), lost_count);
    }

    combo.last_target = target;
    combo.RegisterHit();

    spdlog::debug("COMBO: entity {} hit target {} -> {} hits (max: {})",
                  static_cast<uint32_t>(attacker),
                  static_cast<uint32_t>(target),
                  combo.combo_count, combo.max_combo);
}

void ComboSystem::ResetCombo(entt::registry& registry, entt::entity attacker) {
    if (!registry.valid(attacker)) return;
    auto* combo = registry.try_get<ComboComponent>(attacker);
    if (combo) {
        spdlog::debug("COMBO: entity {} manual reset at {} hits",
                      static_cast<uint32_t>(attacker), combo->combo_count);
        combo->Reset();
    }
}

int32_t ComboSystem::GetComboCount(entt::registry& registry, entt::entity attacker) const {
    if (!registry.valid(attacker)) return 0;
    auto* combo = registry.try_get<ComboComponent>(attacker);
    return combo ? combo->combo_count : 0;
}

int32_t ComboSystem::GetMaxCombo(entt::registry& registry, entt::entity attacker) const {
    if (!registry.valid(attacker)) return 0;
    auto* combo = registry.try_get<ComboComponent>(attacker);
    return combo ? combo->max_combo : 0;
}

float ComboSystem::GetComboMultiplier(entt::registry& registry, entt::entity attacker) const {
    if (!registry.valid(attacker)) return 1.0f;
    auto* combo = registry.try_get<ComboComponent>(attacker);
    if (!combo || combo->combo_count == 0) return 1.0f;

    float mult = 1.0f + combo->combo_count * MULTIPLIER_PER_HIT;
    return std::min(mult, MAX_MULTIPLIER);
}

float ComboSystem::GetComboProgress(entt::registry& registry, entt::entity attacker) const {
    if (!registry.valid(attacker)) return 0.0f;
    auto* combo = registry.try_get<ComboComponent>(attacker);
    if (!combo || combo->combo_count == 0) return 0.0f;
    return std::min(1.0f, combo->combo_timer / combo->combo_timeout);
}
