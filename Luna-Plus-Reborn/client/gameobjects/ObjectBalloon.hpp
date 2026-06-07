#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <vector>

class UIRenderer;

struct BalloonData {
    float world_x, world_y, world_z;
    std::string name;
    int hp = 100, max_hp = 100;
    int level = 1;
    uint32_t name_color = 0xffffffff;
    bool show_hp = true;
    std::string title;
    uint32_t title_color = 0xff888888;
    bool quest_marker = false;
    float billboard_size = 0;
    uint32_t billboard_color = 0;
    bool alive = true;
};

class ObjectBalloon {
public:
    static void Add(const BalloonData& data);
    static void Remove(int index);
    static void Clear();
    static void UpdateAll(float dt);
    static void RenderAll(UIRenderer& ui);

private:
    static std::vector<BalloonData> balloons_;
};
