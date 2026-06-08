#include "GridSystem.hpp"
#include <algorithm>

int64_t GridSystem::CellKey(int cx, int cz) {
    return (static_cast<int64_t>(cx) << 32) | static_cast<uint32_t>(cz);
}

void GridSystem::WorldToCell(float x, float z, int& cx, int& cz) {
    cx = static_cast<int>(std::floor(x / kCellSize));
    cz = static_cast<int>(std::floor(z / kCellSize));
}

void GridSystem::Clear() {
    entity_cells_.clear();
    cells_.clear();
}

void GridSystem::Insert(uint32_t entity_id, float x, float z) {
    Remove(entity_id);
    int cx = 0, cz = 0;
    WorldToCell(x, z, cx, cz);
    int64_t key = CellKey(cx, cz);
    entity_cells_[entity_id] = key;
    cells_[key].push_back(entity_id);
}

void GridSystem::Remove(uint32_t entity_id) {
    auto it = entity_cells_.find(entity_id);
    if (it == entity_cells_.end()) return;
    auto cell_it = cells_.find(it->second);
    if (cell_it != cells_.end()) {
        auto& vec = cell_it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), entity_id), vec.end());
        if (vec.empty()) cells_.erase(cell_it);
    }
    entity_cells_.erase(it);
}

void GridSystem::Update(uint32_t entity_id, float x, float z) {
    int cx = 0, cz = 0;
    WorldToCell(x, z, cx, cz);
    int64_t key = CellKey(cx, cz);
    auto it = entity_cells_.find(entity_id);
    if (it != entity_cells_.end() && it->second == key) return;
    Insert(entity_id, x, z);
}

std::vector<uint32_t> GridSystem::QueryCell(int cx, int cz) const {
    auto it = cells_.find(CellKey(cx, cz));
    return it != cells_.end() ? it->second : std::vector<uint32_t>{};
}

std::vector<uint32_t> GridSystem::QueryRadius(float x, float z, float radius) const {
    std::vector<uint32_t> out;
    int min_cx = 0, min_cz = 0, max_cx = 0, max_cz = 0;
    WorldToCell(x - radius, z - radius, min_cx, min_cz);
    WorldToCell(x + radius, z + radius, max_cx, max_cz);
    float r2 = radius * radius;
    for (int cz = min_cz; cz <= max_cz; ++cz) {
        for (int cx = min_cx; cx <= max_cx; ++cx) {
            for (uint32_t id : QueryCell(cx, cz)) {
                if (std::find(out.begin(), out.end(), id) == out.end())
                    out.push_back(id);
            }
        }
    }
    (void)r2;
    return out;
}
