#include "Grid.hpp"
#include <input/InputSystem.hpp>
#include <algorithm>

Grid::Grid(int rows, int cols, float slot_w, float slot_h, float x, float y)
    : Widget(x, y, cols * (slot_w + 2.0f) + 2.0f, rows * (slot_h + 2.0f) + 2.0f)
    , rows_(rows), cols_(cols), slot_w_(slot_w), slot_h_(slot_h) {
    slots_.resize(rows * cols);
    visible_rows_ = rows;
}

GridSlot* Grid::GetSlot(int row, int col) {
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) return nullptr;
    return &slots_[Index(row, col)];
}

void Grid::SetSlot(int row, int col, const GridSlot& slot) {
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) return;
    slots_[Index(row, col)] = slot;
    slots_[Index(row, col)].empty = false;
}

void Grid::ClearSlot(int row, int col) {
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) return;
    slots_[Index(row, col)] = GridSlot{};
    slots_[Index(row, col)].empty = true;
}

void Grid::ClearAll() {
    for (auto& s : slots_) s = GridSlot{};
    for (auto& s : slots_) s.empty = true;
}

void Grid::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    if (!visible_ || !enabled_) return;
    hover_row_ = hover_col_ = -1;
    if (HitTest(mx, my)) {
        float lx = mx - x_, ly = my - y_;
        int c = (int)((lx - padding_) / (slot_w_ + padding_));
        int r = scroll_row_ + (int)((ly - padding_) / (slot_h_ + padding_));
        int vis_rows = visible_rows_ > 0 ? visible_rows_ : rows_;
        if (c >= 0 && c < cols_ && r >= scroll_row_ && r < scroll_row_ + vis_rows && r < rows_) {
            hover_row_ = r; hover_col_ = c;
            bool right_click = false;
            if (auto* isys = InputSystem::GetInstance()) {
                if (isys->IsMousePressed(1)) right_click = true;
            }
            if (right_click && ctx_cb_) {
                ctx_cb_(r, c);
            } else if (mousepressed && slot_cb_) {
                auto& slot = slots_[Index(r, c)];
                UIEvent e; e.type = UIEvent::Click; e.int_value = r * cols_ + c;
                slot_cb_(r, c, e);
            }
        }
    }
}

void Grid::Render(UIRenderer& ui) {
    if (!visible_) return;
    int vis_rows = visible_rows_ > 0 ? visible_rows_ : rows_;
    int r_end = scrollable_ ? std::min(rows_, scroll_row_ + vis_rows) : rows_;
    for (int r = scroll_row_; r < r_end; r++) {
        for (int c = 0; c < cols_; c++) {
            float sx = x_ + padding_ + c * (slot_w_ + padding_);
            float sy = y_ + padding_ + (r - scroll_row_) * (slot_h_ + padding_);
            auto& slot = slots_[Index(r, c)];
            
            if (slot.empty) {
                // Do not draw solid background if empty to let atlas shine through
            } else {
                if (bgfx::isValid(slot.texture.handle))
                    ui.DrawImage(sx, sy, slot_w_, slot_h_, slot.texture.handle);
                else
                    ui.DrawRect(sx, sy, slot_w_, slot_h_, {50, 50, 70, 200});
                
                if (slot.count > 1) {
                    char buf[16]; snprintf(buf, sizeof(buf), "x%d", slot.count);
                    ui.DrawText(sx + 2, sy + slot_h_ - 14, 0xffffffff, "%s", buf);
                }
            }
            
            // Render hover highlight unconditionally if hovered
            if (r == hover_row_ && c == hover_col_) {
                ui.DrawRect(sx, sy, slot_w_, slot_h_, {255, 255, 255, 60});
                ui.DrawBorder(sx, sy, slot_w_, slot_h_, {255, 255, 100, 200}, 2);
            }
        }
    }
    
    // Render tooltip for hovered slot
    if (hover_row_ >= 0 && hover_col_ >= 0 && tooltip_cb_) {
        std::string tip = tooltip_cb_(hover_row_, hover_col_);
        if (!tip.empty()) {
            float tx = 700, ty = hover_row_ * 20 + 100;
            if (ty > 600) ty = 600;
            float tw = 220, th = 70;
            ui.DrawRect(tx, ty, tw, th, {10, 10, 20, 230});
            ui.DrawBorder(tx, ty, tw, th, {100, 180, 255, 180});
            ui.DrawText(tx + 4, ty + 4, 0xffffffff, "%s", tip.c_str());
        }
    }
}
