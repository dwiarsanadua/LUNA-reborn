#include "TriggerSystem.hpp"
#include <rendering/UIRenderer.hpp>
#include <cmath>
#include <algorithm>
#include <spdlog/spdlog.h>

void TriggerSystem::Init() {
    regions_.clear();
    active_events_.clear();
    spdlog::info("TriggerSystem: initialized");
}

void TriggerSystem::Update(float dt, const glm::vec3& player_pos, float terrain_height) {
    (void)terrain_height;
    
    // Check each region against player
    for (auto& region : regions_) {
        if (!region.active) continue;
        
        bool inside = TestPoint(region, player_pos);
        auto* evt = FindEvent(region.id, 1); // entity_id = 1 for player
        
        if (inside && !evt) {
            // Player entered region
            TriggerEvent e;
            e.region_id = region.id;
            e.entity_id = 1;
            e.enter_time = 0;
            e.inside = true;
            active_events_.push_back(e);
            TriggerEnter(region.id, 1);
        } else if (!inside && evt) {
            // Player left region
            TriggerExit(region.id, 1);
            active_events_.erase(
                std::remove_if(active_events_.begin(), active_events_.end(),
                    [&](auto& e) { return e.region_id == region.id && e.entity_id == 1; }),
                active_events_.end());
        } else if (inside && evt) {
            // Player staying in region
            evt->enter_time += dt;
            evt->tick_timer += dt;
            if (evt->tick_timer >= region.tick_interval) {
                evt->tick_timer = 0;
                if (on_tick_) on_tick_(region.id, 1, region.action);
            }
        }
    }
}

void TriggerSystem::Render(UIRenderer& ui, const glm::vec3& player_pos) {
    (void)ui; (void)player_pos;
    // Visual rendering of trigger zones would go here
}

uint32_t TriggerSystem::AddRegion(const TriggerRegion& region) {
    TriggerRegion r = region;
    r.id = next_id_++;
    regions_.push_back(r);
    return r.id;
}

void TriggerSystem::RemoveRegion(uint32_t id) {
    regions_.erase(
        std::remove_if(regions_.begin(), regions_.end(),
            [&](auto& r) { return r.id == id; }),
        regions_.end());
    active_events_.erase(
        std::remove_if(active_events_.begin(), active_events_.end(),
            [&](auto& e) { return e.region_id == id; }),
        active_events_.end());
}

void TriggerSystem::ClearRegions() {
    regions_.clear();
    active_events_.clear();
}

TriggerRegion* TriggerSystem::GetRegion(uint32_t id) {
    for (auto& r : regions_) if (r.id == id) return &r;
    return nullptr;
}

std::vector<TriggerRegion> TriggerSystem::GetRegionsAt(const glm::vec3& pos) const {
    std::vector<TriggerRegion> result;
    for (auto& r : regions_) {
        if (r.active && TestPoint(r, pos)) result.push_back(r);
    }
    return result;
}

bool TriggerSystem::TestPoint(const TriggerRegion& region, const glm::vec3& point) const {
    glm::vec3 delta = point - region.position;
    switch (region.shape) {
    case TriggerShape::Sphere:
        return glm::length(delta) <= region.size.x;
    case TriggerShape::Box:
        return fabs(delta.x) <= region.size.x * 0.5f &&
               fabs(delta.y) <= region.size.y * 0.5f &&
               fabs(delta.z) <= region.size.z * 0.5f;
    case TriggerShape::Cylinder: {
        float dxz = sqrtf(delta.x * delta.x + delta.z * delta.z);
        return dxz <= region.size.x && fabs(delta.y) <= region.size.y * 0.5f;
    }
    }
    return false;
}

std::vector<TriggerEvent> TriggerSystem::GetEntityEvents(uint32_t entity_id) const {
    std::vector<TriggerEvent> result;
    for (auto& e : active_events_) if (e.entity_id == entity_id) result.push_back(e);
    return result;
}

TriggerEvent* TriggerSystem::FindEvent(uint32_t region_id, uint32_t entity_id) {
    for (auto& e : active_events_) {
        if (e.region_id == region_id && e.entity_id == entity_id) return &e;
    }
    return nullptr;
}

void TriggerSystem::TriggerEnter(uint32_t region_id, uint32_t entity_id) {
    for (auto& r : regions_) {
        if (r.id == region_id) {
            spdlog::info("Trigger: entity {} entered '{}'", entity_id, r.name);
            if (on_enter_) on_enter_(region_id, entity_id, r.action);
            if (r.one_shot) r.active = false;
            break;
        }
    }
}

void TriggerSystem::TriggerExit(uint32_t region_id, uint32_t entity_id) {
    for (auto& r : regions_) {
        if (r.id == region_id) {
            spdlog::info("Trigger: entity {} exited '{}'", entity_id, r.name);
            if (on_exit_) on_exit_(region_id, entity_id, r.action);
            break;
        }
    }
}
