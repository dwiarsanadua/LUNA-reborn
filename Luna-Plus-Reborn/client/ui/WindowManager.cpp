#include "WindowManager.hpp"
#include "UiScriptParser.hpp"
#include "UiFunctionRegistry.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"
#include "widgets/Grid.hpp"
#include <algorithm>
#include <cstdio>

Window* WindowManager::Open(const std::string& title, float x, float y, float w, float h) {
    if (auto* existing = Find(title)) {
        BringToFront(existing);
        return existing;
    }
    auto win = std::make_unique<Window>(title, x, y, w, h);
    win->SetZOrder(next_z_++);
    Window* ptr = win.get();
    windows_.push_back(std::move(win));
    return ptr;
}

Window* WindowManager::LoadFromScript(const std::string& path) {
    UiElement root = UiScriptParser::ParseFile(path);
    if (root.type.empty()) return nullptr;

    Window* win = Open(root.id, root.rect.x, root.rect.y, root.rect.w, root.rect.h);
    win->SetMovable(root.moveable);
    win->SetVisible(root.active);

    // Apply caption rect as title bar height
    if (root.caption_rect.h > 0) win->SetTitleBarH(root.caption_rect.h);

    // If it has a basic image, set it as custom background
    if (root.basic_img.atlas != -1) {
        win->SetCustomBackground([root](UIRenderer& ui, float x, float y, float w, float h) {
            char atlas_name[32]; snprintf(atlas_name, 32, "b%d.tif", root.basic_img.atlas);
            TextureInfo tex = ui.LoadTexture(atlas_name, atlas_name);
            if (bgfx::isValid(tex.handle)) {
                ui.DrawImageUV(x, y, w, h, tex.handle, root.basic_img.u1, root.basic_img.v1, root.basic_img.u2, root.basic_img.v2);
            }
        });
    }

    // Process children (widgets)
    for (const auto& child : root.children) {
        Widget* added_widget = nullptr;
        if (child.type == "$STATIC") {
            added_widget = win->AddWidget<Label>(child.text, child.rect.x, child.rect.y, child.fg_color);
        } else if (child.type == "$BTN") {
            added_widget = win->AddWidget<Button>(child.text, child.rect.x, child.rect.y, child.rect.w, child.rect.h);
        } else if (child.type == "$WEAREDDLG") {
            added_widget = win->AddWidget<Grid>(1, (int)child.icon_cells.size(), child.rect.x, child.rect.y, 34, 34);
        }

        if (added_widget) {
            added_widget->SetID(child.id);
            // Wire #FUNC if present
            if (!child.func_name.empty()) {
                auto cb = Luna::UiFunctionRegistry::Get().GetCallback(child.func_name);
                if (cb) {
                    added_widget->OnEvent([cb, added_widget](const UIEvent& e) {
                        cb(added_widget, e);
                    });
                }
            }
        }
    }

    return win;
}

void WindowManager::Close(const std::string& title) {
    windows_.erase(
        std::remove_if(windows_.begin(), windows_.end(),
            [&](auto& w) { return w->GetTitle() == title; }),
        windows_.end());
}

void WindowManager::CloseAll() {
    windows_.clear();
}

Window* WindowManager::Find(const std::string& title) {
    for (auto& w : windows_)
        if (w->GetTitle() == title && w->IsVisible())
            return w.get();
    return nullptr;
}

bool WindowManager::IsOpen(const std::string& title) const {
    for (auto& w : windows_)
        if (w->GetTitle() == title && w->IsVisible())
            return true;
    return false;
}

bool WindowManager::HasModal() const {
    for (auto& w : windows_)
        if (w->IsVisible() && w->IsModal())
            return true;
    return false;
}

void WindowManager::BringToFront(Window* win) {
    win->SetZOrder(next_z_++);
    // Sort by Z-order for rendering
    std::sort(windows_.begin(), windows_.end(),
        [](auto& a, auto& b) { return a->GetZOrder() < b->GetZOrder(); });
}

void WindowManager::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    // Update windows in reverse Z-order (top first for input)
    for (auto it = windows_.rbegin(); it != windows_.rend(); ++it) {
        if (!(*it)->IsVisible()) continue;
        if (HasModal() && !(*it)->IsModal()) continue;
        (*it)->Update(dt, mx, my, mousedown, mousepressed);
        if (mousepressed && (*it)->HitTest(mx, my)) {
            BringToFront(it->get());
            break; // Only top window gets click
        }
    }
}

void WindowManager::Render(UIRenderer& ui) {
    // Render windows in Z-order (back to front)
    for (auto& w : windows_) {
        if (!w->IsVisible()) continue;
        if (HasModal() && !w->IsModal()) {
            // Dim background behind modal
            ui.DrawRect(0, 0, 1280, 720, {0, 0, 0, 120});
        }
        w->Render(ui);
    }
}
