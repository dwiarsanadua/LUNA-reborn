#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/Label.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

struct EmblemPixelData {
    uint8_t r = 0, g = 0, b = 0, a = 0;
};

class GuildMarkDialog {
public:
    Window* GetWindow() { return window_; }

    void Open(WindowManager* wm, uint32_t guild_id);
    void Close();
    void UpdateFromState(GameState* state);
    void RenderEmblemPreview(UIRenderer& ui, float x, float y, float size);

    void SetNetworkCallbacks(
        std::function<void(uint32_t, const std::string&)> upload_emblem,
        std::function<void(uint32_t)> download_emblem);

    void SetEmblemData(const std::vector<uint8_t>& rgba_data, int w, int h);
    void ClearEmblem();

    uint32_t GetGuildId() const { return guild_id_; }
    bool HasEmblem() const { return !emblem_rgba_.empty(); }

private:
    Window* window_ = nullptr;
    uint32_t guild_id_ = 0;
    int emblem_w_ = 16;
    int emblem_h_ = 16;
    std::vector<uint8_t> emblem_rgba_;

    int selected_color_ = 0;
    static const int NUM_COLORS = 16;
    static const uint32_t palette_[NUM_COLORS];

    std::function<void(uint32_t, const std::string&)> on_upload_;
    std::function<void(uint32_t)> on_download_;

    void GenerateDefaultEmblem();
    std::string EncodeEmblemBase64() const;
    void SelectPreset(int index);
};
