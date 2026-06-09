# AGENT-2 — UI Visual: Window Chrome + Skin Texture (merger FINAL-02 + LAST-02)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old UI punya: Win32 GDI window chrome (gradient title bar, 3D border, close button merah) + texture-based skin dari image_path.bin. Reborn UI: solid colors. Perbaiki agar visual UI match Old.

## Aturan Ketat

1. BACA `client/ui/Window.cpp` — cari Render() method
2. ✅ Jika sudah ada gradient dan 3D border → skip bagian itu
3. ✅ Jika UiSkinManager sudah ada → skip bagian skin
4. 🔧 Jika belum → implementasi sesuai di bawah
5. Build verify

## File target (tidak ada conflict dengan agent lain)

- `client/ui/Window.cpp` — UPDATE Render() chrome
- `client/ui/Window.hpp` — mungkin perlu tambah method
- `client/ui/skin/UiSkinManager.hpp` — BARU
- `client/ui/skin/UiSkinManager.cpp` — BARU
- `client/rendering/UIRenderer.hpp` — mungkin perlu tambah method DrawTexturedRect

### Part A: Window Chrome (di Window.cpp Render())

```cpp
// Cari method Window::Render() atau Window::DrawChrome()
// Di dalamnya, gambar:
// 1. Title bar gradient
ui.DrawRect(x_, y_, w_, title_h_, {42, 42, 110, 230});          // top color
ui.DrawRect(x_, y_ + title_h_/2, w_, title_h_/2, {26, 26, 78, 230}); // bottom (darker)

// 2. 3D border (raised style)
ui.DrawBorder(x_, y_, w_, h_, {150, 200, 255, 200}, 1); // top-left: light
ui.DrawBorder(x_, y_, w_, h_, {50, 80, 120, 200}, 2);    // bottom-right: dark

// 3. Close button (pojok kanan atas)
float cb_x = x_ + w_ - 20, cb_y = y_ + 2, cb_s = 16;
ui.DrawRect(cb_x, cb_y, cb_s, cb_s, {180, 20, 20, 220});   // red bg
ui.DrawText(cb_x + 4, cb_y + 1, 0xFFFFFFFF, "X");

// 4. Title text putih + shadow
uint32_t title_color = 0xFFFFFFFF;
ui.DrawText(x_ + 6, y_ + 4, 0xFF000000, "%s", title_.c_str()); // shadow
ui.DrawText(x_ + 5, y_ + 3, title_color, "%s", title_.c_str()); // text
```

### Part B: UiSkinManager (port image_path.bin texture)

```cpp
// UiSkinManager.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <bgfx/bgfx.h>
#include <rendering/UIRenderer.hpp>

struct SkinElement {
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
    float u1=0, v1=0, u2=1, v2=1;
    bool has_texture = false;
};

class UiSkinManager {
public:
    static UiSkinManager& Instance();
    bool LoadSkin(const std::string& image_path_file);
    const SkinElement* GetElement(const std::string& name) const;
    void DrawElement(UIRenderer& ui, const std::string& name, float x, float y, float w, float h);
private:
    std::unordered_map<std::string, SkinElement> elements_;
    bool ParseImagePath(const std::string& path);
};
```

```cpp
// UiSkinManager.cpp — implementasi
// Cari file assets/interface/image_path.bin.txt (hasil convert dari .bin)
// Format: element_name, u1, v1, u2, v2, texture_file
// Parse → load texture → simpan SkinElement
```

### Part C: Integrasi

Di `Window.cpp` Render(), setelah gambar chrome, panggil:
```cpp
auto* skin = UiSkinManager::Instance().GetElement("window_bg");
if (skin && skin->has_texture) {
    UiSkinManager::Instance().DrawElement(ui, "window_bg", x_, y_, w_, h_);
} else {
    // Fallback ke solid color (existing)
    ui.DrawRect(x_, y_, w_, h_, body_color_);
}
```

## ✅ Kembalikan: "AGENT-2 done: UI chrome + skin texture implemented, 0 build errors"
