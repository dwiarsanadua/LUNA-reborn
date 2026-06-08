#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <rendering/UIRenderer.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct CharMakeAppearance {
    int hair_style = 0;
    int face_style = 0;
    uint32_t hair_color = 0xff442200;
    uint32_t skin_color = 0xffe8c090;
    uint32_t eye_color = 0xff4488cc;
};

class CharMakeDlg {
public:
    CharMakeDlg();
    ~CharMakeDlg();

    void Open(WindowManager* wm);
    void Close();
    Window* GetWindow() { return window_; }
    bool IsOpen() const { return open_; }

    bool IsComplete() const { return complete_; }
    void ResetComplete() { complete_ = false; }
    bool FinalizeInto(GameState& state, CharInfo& out_char);
    void Render(UIRenderer& ui);
    bool HandleKey(int key, int action);
    bool HandleChar(unsigned int codepoint);

private:
    Window* window_ = nullptr;
    bool open_ = false;
    bool complete_ = false;

    // Selections
    int race_ = 0;
    int gender_ = 0;
    int class_ = 0;
    CharMakeAppearance appearance_;
    std::string name_;

    // Preview
    uint32_t preview_instance_ = 0;
    bool preview_created_ = false;

    // UI state
    int selected_tab_ = 0;

    void RenderClassSelection(UIRenderer& ui, float x, float y);
    void RenderAppearance(UIRenderer& ui, float x, float y);
    void RenderPreview(UIRenderer& ui, float x, float y);
    void UpdatePreview();
    void OnConfirm();
};
