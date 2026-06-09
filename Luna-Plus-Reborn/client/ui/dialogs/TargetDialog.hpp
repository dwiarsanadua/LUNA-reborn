#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <glm/glm.hpp>

struct TargetInfo {
    uint32_t entity_id = 0;
    std::string name;
    int level = 0;
    int hp = 0;
    int max_hp = 0;
    float hp_pct = 0.0f;
    int race = 0;
    int monster_kind = 0;
    uint32_t buff_flags = 0;
    bool is_player = false;
    bool is_boss = false;
    bool is_pk = false;
    bool has_target = false;
};

class TargetDialog {
public:
    TargetDialog();
    ~TargetDialog();

    void SetTarget(const TargetInfo& info);
    void ClearTarget();
    bool HasTarget() const { return has_target_; }
    const TargetInfo& GetTarget() const { return target_; }

    void SetSize(float w, float h);
    void SetPosition(float x, float y);

    void Render(class UIRenderer& ui, float dt);

    void SetOnDeselect(std::function<void()> callback) {
        on_deselect_ = callback;
    }

private:
    TargetInfo target_;
    bool has_target_ = false;

    float x_ = 0.0f;
    float y_ = 0.0f;
    float width_ = 220.0f;
    float height_ = 140.0f;
    float anim_timer_ = 0.0f;
    float fade_alpha_ = 1.0f;

    std::function<void()> on_deselect_;

    void RenderTargetName(class UIRenderer& ui);
    void RenderTargetLevel(class UIRenderer& ui);
    void RenderHpBar(class UIRenderer& ui);
    void RenderLifePoints(class UIRenderer& ui);
    void RenderBuffIcons(class UIRenderer& ui);
    void RenderContextMenu(class UIRenderer& ui);

    bool context_menu_open_ = false;
};

extern TargetDialog* g_target_dlg;
