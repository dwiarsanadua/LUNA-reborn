#include "CostumeSystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void CostumeSystem::Init() {
    equipment_layers_.clear();
    fashion_layers_.clear();
    spdlog::info("CostumeSystem: initialized");
}

int CostumeSystem::FindSlot(std::vector<CostumeLayer>& layers, int slot) const {
    for (size_t i = 0; i < layers.size(); i++) if (layers[i].slot_type == slot) return (int)i;
    return -1;
}

void CostumeSystem::EquipItem(int slot, uint32_t item_id, const std::string& glb_path) {
    (void)item_id;
    int idx = FindSlot(equipment_layers_, slot);
    CostumeLayer layer;
    layer.slot_type = slot;
    layer.glb_path = glb_path;
    if (idx >= 0) equipment_layers_[idx] = layer;
    else equipment_layers_.push_back(layer);
}

void CostumeSystem::UnequipItem(int slot) {
    int idx = FindSlot(equipment_layers_, slot);
    if (idx >= 0) equipment_layers_.erase(equipment_layers_.begin() + idx);
}

bool CostumeSystem::IsEquipped(int slot) const {
    for (auto& l : equipment_layers_) if (l.slot_type == slot) return true;
    return false;
}

void CostumeSystem::ApplyFashion(int slot, const std::string& glb_path) {
    int idx = FindSlot(fashion_layers_, slot);
    CostumeLayer layer;
    layer.slot_type = slot;
    layer.glb_path = glb_path;
    layer.color = 0xffffffff;
    if (idx >= 0) fashion_layers_[idx] = layer;
    else fashion_layers_.push_back(layer);
}

void CostumeSystem::RemoveFashion(int slot) {
    int idx = FindSlot(fashion_layers_, slot);
    if (idx >= 0) fashion_layers_.erase(fashion_layers_.begin() + idx);
}

bool CostumeSystem::HasFashion(int slot) const {
    for (auto& l : fashion_layers_) if (l.slot_type == slot) return true;
    return false;
}

std::string CostumeSystem::GetVisualPath(int slot) const {
    // Fashion overrides equipment if active
    if (fashion_active_) {
        for (auto& l : fashion_layers_) if (l.slot_type == slot) return l.glb_path;
    }
    for (auto& l : equipment_layers_) if (l.slot_type == slot) return l.glb_path;
    return "";
}

uint32_t CostumeSystem::GetVisualColor(int slot) const {
    if (fashion_active_) {
        for (auto& l : fashion_layers_) if (l.slot_type == slot) return l.color;
    }
    for (auto& l : equipment_layers_) if (l.slot_type == slot) return l.color;
    return 0xffffffff;
}
