#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>

enum class TriggerShape { Box, Sphere, Cylinder };

enum class TriggerAction {
    BuffApply,        // Apply buff on enter
    DamageOverTime,   // DOT while inside
    SlowMovement,     // Reduce move speed
    Teleport,         // Teleport to another location
    DialogTrigger,    // Trigger NPC dialog
    SpawnMonster,     // Spawn monsters
    HealOverTime,     // HOT while inside
    EventFlag         // Set event flag
};

struct TriggerRegion {
    uint32_t id = 0;
    std::string name;
    TriggerShape shape = TriggerShape::Sphere;
    glm::vec3 position{0};
    glm::vec3 size{5.0f, 5.0f, 5.0f}; // Box: w/h/d, Sphere: radius, Cylinder: radius/height
    TriggerAction action = TriggerAction::DamageOverTime;
    int action_param = 0;     // buff_id, damage per tick, teleport map_id
    std::string action_str;   // teleport target name, dialog text
    float tick_interval = 1.0f;
    uint32_t target_map_id = 0;
    glm::vec3 target_position{0};
    bool one_shot = false;    // Remove after first trigger
    bool active = true;
    uint32_t faction = 0;     // 0 = all, 1 = player only, 2 = monster only
    
    // Visual
    uint32_t color = 0x44aaff44;
    bool show_radius = false;
};

struct TriggerEvent {
    uint32_t region_id = 0;
    uint32_t entity_id = 0;
    float enter_time = 0;
    bool inside = false;
    
    // DOT/HOT tracking
    float tick_timer = 0;
};

class TriggerSystem {
public:
    void Init();
    void Update(float dt, const glm::vec3& player_pos, float terrain_height = 0);
    void Render(class UIRenderer& ui, const glm::vec3& player_pos);
    
    // Region management
    uint32_t AddRegion(const TriggerRegion& region);
    void RemoveRegion(uint32_t id);
    void ClearRegions();
    TriggerRegion* GetRegion(uint32_t id);
    std::vector<TriggerRegion> GetRegionsAt(const glm::vec3& pos) const;
    
    // Test if point is inside a region
    bool TestPoint(const TriggerRegion& region, const glm::vec3& point) const;
    
    // Get active events for an entity
    std::vector<TriggerEvent> GetEntityEvents(uint32_t entity_id) const;
    
    // Callbacks
    using TriggerCallback = std::function<void(uint32_t region_id, uint32_t entity_id, TriggerAction action)>;
    void SetOnEnter(TriggerCallback cb) { on_enter_ = cb; }
    void SetOnExit(TriggerCallback cb) { on_exit_ = cb; }
    void SetOnTick(TriggerCallback cb) { on_tick_ = cb; }
    
    int GetRegionCount() const { return (int)regions_.size(); }
    int GetActiveEventCount() const { return (int)active_events_.size(); }

private:
    std::vector<TriggerRegion> regions_;
    std::vector<TriggerEvent> active_events_;
    uint32_t next_id_ = 1;
    TriggerCallback on_enter_, on_exit_, on_tick_;
    
    TriggerEvent* FindEvent(uint32_t region_id, uint32_t entity_id);
    void TriggerEnter(uint32_t region_id, uint32_t entity_id);
    void TriggerExit(uint32_t region_id, uint32_t entity_id);
};
