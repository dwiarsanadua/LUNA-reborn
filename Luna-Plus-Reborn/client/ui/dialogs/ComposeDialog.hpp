#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/Grid.hpp>
#include <vector>

class ComposeDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close();
    void SetKeyItem(uint32_t item_id, const std::string& name);
    void SetSourceItems(const std::vector<uint32_t>& item_ids);
    void SetResultPreview(const std::vector<std::string>& results);
    void SetOptionList(const std::vector<std::string>& options);
    void SetPage(int current, int total);
    void ShowResult(const std::string& item_name);
    void ShowFailure(const std::string& reason);
    void Reset();

private:
    Window* window_ = nullptr;

    // UI elements
    Label* key_item_label_ = nullptr;
    Grid* source_grid_ = nullptr;
    ListBox* option_list_ = nullptr;
    ListBox* result_list_ = nullptr;
    Label* page_label_ = nullptr;
    Button* btn_prev_ = nullptr;
    Button* btn_next_ = nullptr;
    Button* btn_compose_ = nullptr;
    Button* btn_close_ = nullptr;

    // State
    uint32_t key_item_id_ = 0;
    std::vector<uint32_t> source_items_;
    int current_page_ = 0;
    int total_pages_ = 1;

    void OnCompose();
    void OnPrevPage();
    void OnNextPage();
    void UpdatePageLabel();
};
