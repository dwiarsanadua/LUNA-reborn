#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

struct EquipSlot {
    enum Type {
        Weapon, Shield, Helmet, Armor, Gloves, Boots,
        Cape, Mask, Ring1, Ring2, Earring1, Earring2,
        Necklace, Belt, Shoulder, Pants,
        FashionHead, FashionBody, FashionWeapon, FashionCape,
        COUNT
    };
    
    static const char* GetName(int slot) {
        static const char* names[] = {
            "Weapon", "Shield", "Helmet", "Armor", "Gloves", "Boots",
            "Cape", "Mask", "Ring1", "Ring2", "Earring1", "Earring2",
            "Necklace", "Belt", "Shoulder", "Pants",
            "FashionHead", "FashionBody", "FashionWeapon", "FashionCape"
        };
        if (slot >= 0 && slot < COUNT) return names[slot];
        return "Unknown";
    }
    
    static bool IsFashion(int slot) { return slot >= FashionHead; }
    static int GetEquipmentSlot(int fashion_slot) {
        switch (fashion_slot) {
        case FashionHead: return Helmet;
        case FashionBody: return Armor;
        case FashionWeapon: return Weapon;
        case FashionCape: return Cape;
        default: return fashion_slot;
        }
    }
};

struct CostumeLayer {
    int slot_type = 0;
    std::string glb_path;
    uint32_t color = 0xffffffff;
    float scale = 1.0f;
    bool visible = true;
};

class CostumeSystem {
public:
    void Init();
    
    // Equipment management
    void EquipItem(int slot, uint32_t item_id, const std::string& glb_path);
    void UnequipItem(int slot);
    bool IsEquipped(int slot) const;
    
    // Fashion/Costume overlay
    void ApplyFashion(int slot, const std::string& glb_path);
    void RemoveFashion(int slot);
    bool HasFashion(int slot) const;
    bool IsFashionActive() const { return fashion_active_; }
    void SetFashionActive(bool a) { fashion_active_ = a; }
    
    // Visual query — returns which .glb to render for each slot
    std::string GetVisualPath(int slot) const;
    uint32_t GetVisualColor(int slot) const;
    
    // All layers
    const std::vector<CostumeLayer>& GetEquipmentLayers() const { return equipment_layers_; }
    const std::vector<CostumeLayer>& GetFashionLayers() const { return fashion_layers_; }
    
    int GetEquipCount() const { return (int)equipment_layers_.size(); }
    int GetFashionCount() const { return (int)fashion_layers_.size(); }

private:
    std::vector<CostumeLayer> equipment_layers_;
    std::vector<CostumeLayer> fashion_layers_;
    bool fashion_active_ = true;
    
    int FindSlot(std::vector<CostumeLayer>& layers, int slot) const;
};
