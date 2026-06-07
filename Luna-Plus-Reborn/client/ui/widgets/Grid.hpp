#pragma once
#include "Widget.hpp"
#include <input/InputSystem.hpp>
#include <vector>

struct GridSlot {
    int row = 0, col = 0;
    uint32_t color = 0xffffffff;
    std::string text;
    int count = 0;
    int userdata = 0;
    TextureInfo texture;
    bool empty = true;
};

class Grid : public Widget {
public:
    Grid(int rows, int cols, float slot_w, float slot_h, float x = 0, float y = 0);
    GridSlot* GetSlot(int row, int col);
    void SetPadding(float p) { padding_ = p; }
    void SetSlot(int row, int col, const GridSlot& slot);
    void ClearSlot(int row, int col);
    void ClearAll();
    int GetRows() const { return rows_; }
    int GetCols() const { return cols_; }
    float GetSlotW() const { return slot_w_; }
    float GetSlotH() const { return slot_h_; }
    
    void Update(float dt, float mx, float my, bool mousedown, bool mousepressed) override;
    void Render(UIRenderer& ui) override;
    
    using SlotCallback = std::function<void(int row, int col, const UIEvent&)>;
    void OnSlotEvent(SlotCallback cb) { slot_cb_ = cb; }
    
    using TooltipCallback = std::function<std::string(int row, int col)>;
    void OnTooltip(TooltipCallback cb) { tooltip_cb_ = cb; }
    int GetHoverRow() const { return hover_row_; }
    int GetHoverCol() const { return hover_col_; }
    
    // Right-click / context menu
    using ContextMenuCallback = std::function<void(int row, int col)>;
    void OnContextMenu(ContextMenuCallback cb) { ctx_cb_ = cb; }

private:
    int rows_, cols_;
    float slot_w_, slot_h_;
    float padding_ = 2;
    std::vector<GridSlot> slots_;
    int hover_row_ = -1, hover_col_ = -1;
    SlotCallback slot_cb_;
    TooltipCallback tooltip_cb_;
    ContextMenuCallback ctx_cb_;
    
    int Index(int row, int col) const { return row * cols_ + col; }
};
