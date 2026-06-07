#pragma once
#include <cstdint>
#include <vector>

struct ItemDurability {
    uint32_t item_id = 0;
    int max_durability = 100;
    int current_durability = 100;
    bool destroyed = false;
    
    float GetPercent() const { return (float)current_durability / std::max(1, max_durability); }
    bool NeedsRepair() const { return current_durability < max_durability; }
    bool IsBroken() const { return current_durability <= 0; }
};

class DurabilitySystem {
public:
    void Init();
    
    // Get/set durability
    ItemDurability* GetDurability(uint32_t item_id);
    void SetDurability(uint32_t item_id, int current, int max_durability = 100);
    
    // Damage items on use
    void OnAttack(uint32_t weapon_id);
    void OnHit(uint32_t armor_id);
    void OnSkillUse(uint32_t item_id);
    
    // Repair
    bool Repair(uint32_t item_id);
    bool RepairAll(std::vector<uint32_t> item_ids);
    uint32_t GetRepairCost(uint32_t item_id) const;
    uint32_t GetRepairCostAll(const std::vector<uint32_t>& item_ids) const;
    
    // Break check
    bool IsBroken(uint32_t item_id) const;
    bool IsDestroyed(uint32_t item_id) const;
    
    int GetCount() const { return (int)durabilities_.size(); }
    
    static constexpr int MAX_DURABILITY = 100;
    static constexpr int ATTACK_DRAIN = 1;
    static constexpr int HIT_DRAIN = 2;
    static constexpr int REPAIR_COST_PER_POINT = 10; // 10 gold per durability point

private:
    std::vector<ItemDurability> durabilities_;
    ItemDurability* FindOrCreate(uint32_t item_id);
};
