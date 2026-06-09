#pragma once
#include <rendering/UIRenderer.hpp>
#include <ui/GameState.hpp>
#include <gameobjects/Monster.hpp>
#include <cstdint>
#include <vector>

enum class TargetType : uint8_t {
    None = 0,
    Monster = 1,
    Boss = 2,
    Player = 3,
    NPC = 4,
};

class TargetMonsterDialog {
public:
    void UpdateFromState(GameState* state, const std::vector<Monster>& monsters);
    void Render(UIRenderer& ui, float x, float y);

    int32_t GetTargetEntity() const { return target_entity_; }
    void SetTargetEntity(int32_t id) { target_entity_ = id; }
    bool HasTarget() const { return target_entity_ >= 0; }
    TargetType GetTargetType() const { return target_type_; }

private:
    int32_t target_entity_ = -1;
    TargetType target_type_ = TargetType::None;
    std::string target_name_;
    int target_level_ = 0;
    int target_hp_ = 0;
    int target_max_hp_ = 0;
    int target_hp_pct_ = 0;
    bool is_boss_ = false;
};
