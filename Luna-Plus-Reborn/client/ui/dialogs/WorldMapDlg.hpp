#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct WorldZone {
    std::string name;
    float center_x, center_z;
    float radius;
    uint32_t color;
    bool discovered;
};

class WorldMapDlg {
public:
    WorldMapDlg();
    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return window_ != nullptr; }

    void SetPlayerPos(float world_x, float world_z);
    void SetZones(const std::vector<WorldZone>& zones);
    void DiscoverZone(const std::string& name);

    void Render(UIRenderer& ui);

private:
    Window* window_ = nullptr;
    float player_x_ = 0, player_z_ = 0;
    float world_w_ = 2000.0f, world_h_ = 2000.0f;
    float world_ox_ = -1000.0f, world_oz_ = -1000.0f;
    float map_x_ = 40, map_y_ = 40;
    float map_w_ = 1200, map_h_ = 640;
    std::vector<WorldZone> zones_;

    void WorldToMap(float wx, float wz, float& mx, float& mz) const;
};
