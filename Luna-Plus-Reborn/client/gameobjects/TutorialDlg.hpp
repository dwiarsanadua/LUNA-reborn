#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <rendering/UIRenderer.hpp>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

struct TutorialStep {
    std::string title;
    std::string description;
    std::string highlight_widget_id; // UI element to highlight
    float highlight_x = 0, highlight_y = 0, highlight_w = 0, highlight_h = 0;
    float arrow_target_x = 0, arrow_target_y = 0; // where arrow points
    float arrow_angle = 0;
    std::function<void()> on_enter;
    std::function<bool()> completion_condition;
};

class TutorialDlg {
public:
    TutorialDlg();

    void Start(const std::string& tutorial_id, const std::vector<TutorialStep>& steps);
    void NextStep();
    void PrevStep();
    void Skip();
    void Complete();
    void Reset();

    void Update(float dt);
    void Render(UIRenderer& ui);

    bool IsActive() const { return active_; }
    int GetCurrentStep() const { return current_step_; }
    int GetTotalSteps() const { return (int)steps_.size(); }
    const std::string& GetCurrentTutorialId() const { return tutorial_id_; }

    using CompleteCallback = std::function<void(const std::string&)>;
    void SetCompleteCallback(CompleteCallback cb) { on_complete_ = cb; }

private:
    bool active_ = false;
    int current_step_ = 0;
    std::string tutorial_id_;
    std::vector<TutorialStep> steps_;
    CompleteCallback on_complete_;

    float popup_x_ = 100, popup_y_ = 100;
    float popup_w_ = 400, popup_h_ = 200;
    float anim_timer_ = 0;
    float pulse_anim_ = 0;

    void RenderHighlight(UIRenderer& ui, const TutorialStep& step);
    void RenderArrowOverlay(UIRenderer& ui, const TutorialStep& step);
    void RenderPopup(UIRenderer& ui, const TutorialStep& step);
};
