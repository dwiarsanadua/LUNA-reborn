#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

enum class ObjectKind : uint8_t {
    PLAYER,
    MONSTER,
    NPC,
    PET,
    DROPPED_ITEM,
    SUMMON,
    ALL
};

class GridSystem {
public:
    static constexpr float kCellSize = 64.0f;

    void Clear();
    void Insert(uint32_t entity_id, float x, float z);
    void Remove(uint32_t entity_id);
    void Update(uint32_t entity_id, float x, float z);
    std::vector<uint32_t> QueryRadius(float x, float z, float radius) const;
    std::vector<uint32_t> QueryCell(int cx, int cz) const;

    std::vector<entt::entity> GetTargetsInRange(
        const glm::vec3& center, float radius,
        entt::registry& registry,
        const std::unordered_set<ObjectKind>& kinds
    );

private:
    static int64_t CellKey(int cx, int cz);
    static void WorldToCell(float x, float z, int& cx, int& cz);

    std::unordered_map<uint32_t, int64_t> entity_cells_;
    std::unordered_map<int64_t, std::vector<uint32_t>> cells_;
};
