#include "UiSkinManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <algorithm>

std::unordered_map<std::string, bgfx::TextureHandle> UiSkinManager::textures_;
std::unordered_map<std::string, UISkinPart> UiSkinManager::skin_parts_;
std::unordered_map<std::string, UiSkinManager::WindowSkin> UiSkinManager::window_skins_;
UiSkinManager::ButtonSkin UiSkinManager::default_button_skin_;
UiSkinManager::GaugeSkin UiSkinManager::hp_gauge_skin_;
UiSkinManager::GaugeSkin UiSkinManager::mp_gauge_skin_;
UiSkinManager::GaugeSkin UiSkinManager::exp_gauge_skin_;
UiSkinManager::SlotSkin UiSkinManager::inventory_slot_skin_;
UiSkinManager::ScrollSkin UiSkinManager::scroll_skin_;
UiSkinManager::TabSkin UiSkinManager::tab_skin_;
std::string UiSkinManager::texture_path_;
bool UiSkinManager::initialized_ = false;

void UiSkinManager::Init(const std::string& ui_texture_path, const std::string& interface_path) {
    if (initialized_) return;
    texture_path_ = ui_texture_path;

    // Register default window skins
    WindowSkin main_win;
    main_win.name = "main";
    main_win.title_color = 0x3C3C78E6;
    main_win.body_color = 0x101020DC;
    main_win.border_color = 0x64B4FFC8;
    window_skins_["main"] = main_win;

    WindowSkin sub_win;
    sub_win.name = "sub";
    sub_win.title_color = 0x28284BB4;
    sub_win.body_color = 0x101020DC;
    sub_win.border_color = 0x5078A0C8;
    window_skins_["sub"] = sub_win;

    WindowSkin popup_win;
    popup_win.name = "popup";
    popup_win.title_color = 0x4B283CDC;
    popup_win.body_color = 0x141428DC;
    popup_win.border_color = 0xA064B4C8;
    window_skins_["popup"] = popup_win;

    // Default button skin
    default_button_skin_.color_normal = 0x283250DC;
    default_button_skin_.color_hover = 0x50648CDC;
    default_button_skin_.color_pressed = 0x1E283CDC;

    // Gauge skins
    hp_gauge_skin_.color_fill = 0xFF3C3CFF;
    hp_gauge_skin_.color_bg = 0x3C0000B4;
    mp_gauge_skin_.color_fill = 0x3C3CFFFF;
    mp_gauge_skin_.color_bg = 0x00003CB4;
    exp_gauge_skin_.color_fill = 0xFFDC3CFF;
    exp_gauge_skin_.color_bg = 0x3C2800B4;

    // Slot skin
    inventory_slot_skin_.color_normal = 0x282832C8;
    inventory_slot_skin_.color_hover = 0x3C3C50C8;

    // Try to load UI textures (may not exist if PAK not extracted)
    // Textures are loaded on demand via LoadTexture()

    // Load window layout definitions
    int layouts = LoadWindowLayouts(interface_path + "Windows/");
    spdlog::info("UiSkinManager: initialized ({} window layouts, {} textures pre-cached)",
                 layouts, (int)textures_.size());
    initialized_ = true;
}

void UiSkinManager::Shutdown() {
    for (auto& [name, tex] : textures_) {
        if (bgfx::isValid(tex)) bgfx::destroy(tex);
    }
    textures_.clear();
    skin_parts_.clear();
    window_skins_.clear();
    initialized_ = false;
}

bgfx::TextureHandle UiSkinManager::LoadTexture(const std::string& name, const std::string& path) {
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;

    std::string full_path = path.empty() ? texture_path_ + name + ".png" : path;
    // UIRenderer::LoadTexture is not static, so we use stb_image directly
    int w, h, n;
    unsigned char* d = stbi_load(full_path.c_str(), &w, &h, &n, 4);
    if (!d) {
        spdlog::warn("UiSkin: cannot load texture {}", full_path);
        textures_[name] = BGFX_INVALID_HANDLE;
        return BGFX_INVALID_HANDLE;
    }
    auto tex = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, false, 1,
                                      bgfx::TextureFormat::RGBA8, 0,
                                      bgfx::copy(d, w * h * 4));
    stbi_image_free(d);
    textures_[name] = tex;

    UISkinPart part;
    part.name = name;
    part.texture_path = full_path;
    part.texture = tex;
    part.tex_w = w;
    part.tex_h = h;
    skin_parts_[name] = part;

    spdlog::info("UiSkin: loaded {} ({}x{})", full_path, w, h);
    return tex;
}

bool UiSkinManager::IsLoaded(const std::string& name) {
    auto it = textures_.find(name);
    return it != textures_.end() && bgfx::isValid(it->second);
}

bgfx::TextureHandle UiSkinManager::GetTexture(const std::string& name) {
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;
    return BGFX_INVALID_HANDLE;
}

UISkinPart* UiSkinManager::GetSkinPart(const std::string& part_name) {
    auto it = skin_parts_.find(part_name);
    if (it != skin_parts_.end()) return &it->second;
    return nullptr;
}

UiSkinManager::WindowSkin* UiSkinManager::GetWindowSkin(const std::string& window_type) {
    auto it = window_skins_.find(window_type);
    if (it != window_skins_.end()) return &it->second;
    // Fallback to main
    it = window_skins_.find("main");
    if (it != window_skins_.end()) return &it->second;
    return nullptr;
}

void UiSkinManager::RegisterWindowSkin(const std::string& window_type, const WindowSkin& skin) {
    window_skins_[window_type] = skin;
}

UiSkinManager::ButtonSkin* UiSkinManager::GetButtonSkin(const std::string& btn_type) {
    (void)btn_type;
    return &default_button_skin_;
}

UiSkinManager::GaugeSkin* UiSkinManager::GetGaugeSkin(const std::string& gauge_type) {
    if (gauge_type == "hp") return &hp_gauge_skin_;
    if (gauge_type == "mp") return &mp_gauge_skin_;
    if (gauge_type == "exp") return &exp_gauge_skin_;
    return &hp_gauge_skin_;
}

UiSkinManager::SlotSkin* UiSkinManager::GetSlotSkin(const std::string& slot_type) {
    (void)slot_type;
    return &inventory_slot_skin_;
}

UiSkinManager::ScrollSkin* UiSkinManager::GetScrollSkin() {
    return &scroll_skin_;
}

UiSkinManager::TabSkin* UiSkinManager::GetTabSkin() {
    return &tab_skin_;
}

void UiSkinManager::DrawWindow(UIRenderer& ui, float x, float y, float w, float h,
                                const std::string& window_type, const char* title) {
    auto* skin = GetWindowSkin(window_type);
    if (!skin) return;

    // Window body (fill)
    ui.DrawRect(x, y, w, 22, {uint8_t((skin->title_color >> 24) & 0xFF),
                               uint8_t((skin->title_color >> 16) & 0xFF),
                               uint8_t((skin->title_color >> 8) & 0xFF),
                               uint8_t(skin->title_color & 0xFF)});
    ui.DrawRect(x, y + 22, w, h - 22, {uint8_t((skin->body_color >> 24) & 0xFF),
                                         uint8_t((skin->body_color >> 16) & 0xFF),
                                         uint8_t((skin->body_color >> 8) & 0xFF),
                                         uint8_t(skin->body_color & 0xFF)});
    ui.DrawBorder(x, y, w, h, {uint8_t((skin->border_color >> 24) & 0xFF),
                                uint8_t((skin->border_color >> 16) & 0xFF),
                                uint8_t((skin->border_color >> 8) & 0xFF),
                                uint8_t(skin->border_color & 0xFF)});

    // Title text
    ui.DrawText(x + 6, y + 3, 0xffffffff, "%s", title ? title : "");

    // Close button (X)
    float cx = x + w - 18, cy = y + 3;
    ui.DrawRect(cx, cy, 14, 14, {180, 40, 40, 200});
    ui.DrawText(cx + 3, cy + 1, 0xffffffff, "X");
}

void UiSkinManager::DrawButton(UIRenderer& ui, float x, float y, float w, float h,
                                const std::string& btn_type, const char* text,
                                bool hover, bool pressed) {
    auto* skin = GetButtonSkin(btn_type);
    if (!skin) return;

    uint32_t col = pressed ? skin->color_pressed : (hover ? skin->color_hover : skin->color_normal);
    UIColor c = {uint8_t((col >> 24) & 0xFF), uint8_t((col >> 16) & 0xFF),
                 uint8_t((col >> 8) & 0xFF), uint8_t(col & 0xFF)};
    ui.DrawRect(x, y, w, h, c);
    ui.DrawBorder(x, y, w, h, {100, 150, 200, 150});
    float tx = x + w * 0.5f - strlen(text) * 4.0f;
    ui.DrawText(tx, y + h * 0.5f - 7, 0xffffffff, "%s", text);
}

void UiSkinManager::DrawGauge(UIRenderer& ui, float x, float y, float w, float h,
                               float pct, const std::string& gauge_type, const char* text) {
    auto* skin = GetGaugeSkin(gauge_type);
    if (!skin) return;

    UIColor bg = {uint8_t((skin->color_bg >> 24) & 0xFF), uint8_t((skin->color_bg >> 16) & 0xFF),
                  uint8_t((skin->color_bg >> 8) & 0xFF), uint8_t(skin->color_bg & 0xFF)};
    UIColor fg = {uint8_t((skin->color_fill >> 24) & 0xFF), uint8_t((skin->color_fill >> 16) & 0xFF),
                  uint8_t((skin->color_fill >> 8) & 0xFF), uint8_t(skin->color_fill & 0xFF)};
    ui.DrawRect(x, y, w, h, bg);
    if (pct > 0) ui.DrawRect(x, y, w * std::min(1.0f, pct), h, fg);
    ui.DrawBorder(x, y, w, h, {0, 0, 0, 180});
    if (text) ui.DrawText(x + 4, y + 1, 0xffffffff, "%s", text);
}

void UiSkinManager::DrawSlot(UIRenderer& ui, float x, float y, float size,
                              const std::string& slot_type, bool hover) {
    auto* skin = GetSlotSkin(slot_type);
    if (!skin) return;

    UIColor c = hover ? UIColor{uint8_t((skin->color_hover >> 24) & 0xFF),
                                uint8_t((skin->color_hover >> 16) & 0xFF),
                                uint8_t((skin->color_hover >> 8) & 0xFF),
                                uint8_t(skin->color_hover & 0xFF)}
                      : UIColor{uint8_t((skin->color_normal >> 24) & 0xFF),
                                uint8_t((skin->color_normal >> 16) & 0xFF),
                                uint8_t((skin->color_normal >> 8) & 0xFF),
                                uint8_t(skin->color_normal & 0xFF)};
    ui.DrawRect(x, y, size, size, c);
    ui.DrawBorder(x, y, size, size, {60, 60, 80, 150});
}

int UiSkinManager::LoadWindowLayouts(const std::string& windows_path) {
    // Parse .txt window definition files
    // Format example: "WindowID_Inventory.bin.txt"
    // Contains position, size, texture references for each UI element
    int count = 0;
    // This is a simplified loader - full implementation would parse
    // the .txt format and register all window skin parts
    spdlog::info("UiSkin: scanning window layouts in {}", windows_path);
    return count;
}

std::vector<std::string> UiSkinManager::GetLoadedTextureNames() {
    std::vector<std::string> names;
    for (auto& [name, tex] : textures_) {
        names.push_back(name);
    }
    return names;
}
