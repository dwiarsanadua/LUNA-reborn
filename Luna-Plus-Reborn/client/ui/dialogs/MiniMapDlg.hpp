#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

struct MiniMapEntity {
    uint32_t id;
    glm::vec2 pos;
    uint32_t color;
    float size;
    bool is_player;
};

class MiniMapDlg {
public:
    MiniMapDlg();
    ~MiniMapDlg();

    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return window_ != nullptr; }

    void SetPlayerPos(float world_x, float world_z);
    void AddEntity(uint32_t id, float x, float z, uint32_t color, float size = 4.0f);
    void ClearEntities();
    void SetFogData(const std::vector<bool>& revealed, int map_w, int map_h);

    void Update(float dt);
    void Render(UIRenderer& ui);

private:
    Window* window_ = nullptr;
    float player_x_ = 0, player_z_ = 0;
    float map_scale_ = 0.5f;
    float map_offset_x_ = 0, map_offset_y_ = 0;
    float map_w_ = 180, map_h_ = 180;

    std::vector<MiniMapEntity> entities_;
    std::vector<bool> fog_revealed_;
    int fog_w_ = 0, fog_h_ = 0;

    bgfx::TextureHandle render_target_ = BGFX_INVALID_HANDLE;
    bgfx::FrameBufferHandle fbo_ = BGFX_INVALID_HANDLE;
    bool fb_ready_ = false;

    void CreateRenderTarget();
    void RenderTerrainView();
    void DrawFogOverlay(UIRenderer& ui, float sx, float sy);
};
