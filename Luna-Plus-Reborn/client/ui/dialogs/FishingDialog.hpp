#pragma once
#include <ui/Window.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

struct FishItemInfo {
    uint32_t item_id = 0;
    uint32_t item_db_idx = 0;
    std::string name;
    int durability = 0;
    int supply_value = 0;
    int point_value = 0;
};

class FishingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, class WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; active_ = false; point_active_ = false; }
    void Update(float dt);
    void Render(UIRenderer& ui);

    bool IsActive() const { return active_; }
    bool IsCatching() const { return catching_; }
    float GetProgress() const { return progress_; }
    void StartCasting() { casting_ = true; cast_timer_ = 0; }

    void SetResultCallback(std::function<void(bool success, int fish_type)> cb) { result_cb_ = cb; }
    void SetCastCallback(std::function<void()> cb) { cast_cb_ = std::move(cb); }

    void OpenPointDialog();
    void ClosePointDialog();
    bool IsPointActive() const { return point_active_; }

    void SetCurrentPoints(uint32_t points);
    uint32_t GetCurrentPoints() const { return current_points_; }
    uint32_t GetAddPoints() const { return add_points_; }

    bool AddFishToSlot(const FishItemInfo& fish);
    void ClearFishSlots();
    void SendConvertPoints();
    void OnConvertAck(uint32_t added_points);
    void OnConvertNack();
    int GetFishSlotCount() const { return MAX_FISH_SLOTS; }
    const FishItemInfo* GetFishSlot(int idx) const;

private:
    Window* window_ = nullptr;
    Label* status_label_ = nullptr;
    Label* fish_label_ = nullptr;
    ProgressBar* progress_bar_ = nullptr;

    bool active_ = false;
    bool casting_ = false;
    bool catching_ = false;
    bool caught_ = false;

    float cast_timer_ = 0;
    float wait_timer_ = 0;
    float bite_timer_ = 0;
    float catch_window_ = 0;
    float progress_ = 0;
    float progress_dir_ = 1.0f;
    int fish_type_ = 0;

    std::function<void(bool, int)> result_cb_;
    std::function<void()> cast_cb_;

    static constexpr float MAX_WAIT = 8.0f;
    static constexpr float CATCH_WINDOW = 0.3f;

    // Fishing Point system
    static constexpr int MAX_FISH_SLOTS = 12;
    static constexpr uint32_t MAX_FISH_POINT = 9999999;

    bool point_active_ = false;
    uint32_t current_points_ = 0;
    uint32_t add_points_ = 0;
    FishItemInfo fish_slots_[12];
    int fill_slot_count_ = 0;

    void RenderPointDialog(UIRenderer& ui);
    void RecalculatePoints();
};
