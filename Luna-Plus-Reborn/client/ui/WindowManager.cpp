#include "WindowManager.hpp"
#include "UiScriptParser.hpp"
#include "UiFunctionRegistry.hpp"
#include "UiAtlasRegistry.hpp"
#include "UiSoundIndex.hpp"
#include "UiScriptWidgetBuilder.hpp"
#include "widgets/Button.hpp"
#include <ui/skin/UiSkinManager.hpp>
#include <ui/UiTooltip.hpp>
#include <ui/widgets/Widget.hpp>
#include <audio/AudioManager.hpp>
#include <engine/gx_render/VFS.h>
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <unordered_set>
#include <spdlog/spdlog.h>
namespace fs = std::filesystem;

extern AudioManager* g_audio;

static void CollectAtlasIDs(const UiElement& elem, std::unordered_set<int>& ids) {
    if (elem.basic_img.atlas != -1) ids.insert(elem.basic_img.atlas);
    if (elem.over_img.atlas != -1) ids.insert(elem.over_img.atlas);
    if (elem.press_img.atlas != -1) ids.insert(elem.press_img.atlas);
    if (elem.icon_cell_bg.atlas != -1) ids.insert(elem.icon_cell_bg.atlas);
    if (elem.dragover_bg.atlas != -1) ids.insert(elem.dragover_bg.atlas);
    for (const auto& child : elem.children)
        CollectAtlasIDs(child, ids);
}

static TextureInfo LoadScriptImage(UIRenderer& ui, const UiScriptUV& uv) {
    if (uv.atlas < 0) return {};
    return UiAtlasRegistry::LoadAtlasTexture(ui, uv.atlas);
}

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
  const UiElement* cached = UiSkinManager::GetLayoutByPath(path);
    UiElement root = cached ? *cached : UiScriptParser::ParseFile(path);
    if (root.type.empty()) return nullptr;

    std::string win_id = !root.id.empty() ? root.id : UiScriptParser::WidgetTypeName(root.type);
    if (auto* existing = Find(win_id)) {
        BringToFront(existing);
        return existing;
    }

    Window* win = Open(win_id, root.rect.x, root.rect.y, root.rect.w, root.rect.h);
    win->SetMovable(root.moveable);
    win->SetVisible(root.active);
    win->SetScriptLayout(true);
    win->SetDrawChrome(false);
    win->SetClosable(root.close_sound >= 0 || root.children.empty());

    if (root.caption_rect.h > 0) win->SetTitleBarH(root.caption_rect.h);

    if (root.basic_img.atlas != -1 && g_ui) {
        UiScriptUV bg_uv = root.basic_img;
        win->SetCustomBackground([bg_uv](UIRenderer& ui, float x, float y, float w, float h) {
            TextureInfo tex = LoadScriptImage(ui, bg_uv);
            if (bgfx::isValid(tex.handle))
                ui.DrawImageUV(x, y, w, h, tex.handle, bg_uv.u1, bg_uv.v1, bg_uv.u2, bg_uv.v2);
        });
    }

    if (root.open_sound >= 0 && g_audio) {
        std::string sfx = UiSoundIndex::Resolve(root.open_sound);
        if (!sfx.empty()) g_audio->PlaySFXByCategory(AudioManager::SFX_UI, sfx);
    }

    win->OnClose([root]() {
        if (root.close_sound >= 0 && g_audio) {
            std::string sfx = UiSoundIndex::Resolve(root.close_sound);
            if (!sfx.empty()) g_audio->PlaySFXByCategory(AudioManager::SFX_UI, sfx);
        }
    });

    UiScriptWidgetBuilder::AddTree(win, root);

    return win;
}

Window* WindowManager::LoadFromScriptOrOpen(const std::string& path, const std::string& fallback_title,
                                            float x, float y, float w, float h) {
    if (Window* win = LoadFromScript(path)) return win;
    spdlog::warn("WindowManager: script load failed for {}, using fallback", path);
    return Open(fallback_title, x, y, w, h);
}

void WindowManager::PreloadUI(const std::string& interface_path) {
    std::string dir = VFS::Find(interface_path + "/Windows");
    if (dir.empty()) dir = interface_path + "/Windows";
    if (!fs::is_directory(dir)) {
        spdlog::warn("PreloadUI: directory not found {}", dir);
        return;
    }

    std::unordered_set<int> all_atlases;
    int files = 0;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        const std::string name = entry.path().filename().string();
        if (name.size() <= 8 || name.substr(name.size() - 8) != ".bin.txt") continue;
        ++files;

        const UiElement* cached = UiSkinManager::GetLayout(name.substr(0, name.size() - 8));
        if (cached) CollectAtlasIDs(*cached, all_atlases);
        else CollectAtlasIDs(UiScriptParser::ParseFile(entry.path().string()), all_atlases);
    }

    if (g_ui) {
        for (int atlas_id : all_atlases)
            UiAtlasRegistry::LoadAtlasTexture(*g_ui, atlas_id);
    }

    spdlog::info("PreloadUI: {} layout files, {} atlas textures preloaded",
        files, all_atlases.size());
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
    std::sort(windows_.begin(), windows_.end(),
        [](auto& a, auto& b) { return a->GetZOrder() < b->GetZOrder(); });
}

static void CollectTooltip(Window* win, float mx, float my) {
    if (!win || !win->IsVisible()) return;
    float ox = win->GetX();
    float oy = win->UsesScriptLayout() ? win->GetY() : (win->GetY() + win->GetTitleBarH());
    for (auto& w : win->GetWidgets()) {
        if (!w->IsVisible() || w->GetTooltip().empty()) continue;
        float wx = ox + w->GetX(), wy = oy + w->GetY();
        if (mx >= wx && mx <= wx + w->GetW() && my >= wy && my <= wy + w->GetH()) {
            UiTooltip::Set(mx, my, w->GetTooltip());
            return;
        }
    }
}

void WindowManager::Update(float dt, float mx, float my, bool mousedown, bool mousepressed) {
    UiTooltip::Clear();
    for (auto it = windows_.rbegin(); it != windows_.rend(); ++it) {
        if (!(*it)->IsVisible()) continue;
        if (HasModal() && !(*it)->IsModal()) continue;
        (*it)->Update(dt, mx, my, mousedown, mousepressed);
        if (mousepressed && (*it)->HitTest(mx, my)) {
            BringToFront(it->get());
            break;
        }
    }
    for (auto& w : windows_) CollectTooltip(w.get(), mx, my);
}

void WindowManager::Render(UIRenderer& ui) {
    for (auto& w : windows_) {
        if (!w->IsVisible()) continue;
        if (HasModal() && !w->IsModal())
            ui.DrawRect(0, 0, 1280, 720, {0, 0, 0, 120});
        w->Render(ui);
    }
    UiTooltip::Render(ui);
}
