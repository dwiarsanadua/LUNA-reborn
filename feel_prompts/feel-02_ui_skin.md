# FEEL-02 — UI Skin Alignment: Warna, Font, Border, Button Style

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya UI skin system via `.bin` texture references di `Luna-Plus-Old/LunaPlus/Data/Interface/Windows/image_path.bin`. Reborn pakai UIRenderer dengan warna default. UI "look & feel" berbeda.

## Aturan Ketat

1. BACA dulu file Reborn sebelum ubah
2. ✅ Jika sudah mirip Old — skip
3. 🔧 Jika berbeda — update konstanta warna, font size, border radius

## File yang harus dicek

### client/rendering/UIRenderer.hpp
Cari konstanta warna default:
```bash
rg "UIColor\|color_\|DrawRect\|DrawBorder\|DrawText" client/rendering/UIRenderer.hpp | head -20
```

### client/rendering/UIRenderer.cpp
Cari implementasi DrawRect, DrawBorder, DrawText:
```bash
rg "void UIRenderer::" client/rendering/UIRenderer.cpp
```

### client/ui/Window.hpp
Cari warna default title bar, body, border:
```bash
rg "title_color_\|body_color_\|border_color_" client/ui/Window.hpp
```

### client/ui/widgets/Button.hpp
Cari warna default button normal, hover, pressed:
```bash
rg "color_normal_\|color_hover_\|color_pressed_" client/ui/widgets/Button.hpp
```

## Yang harus diubah (jika berbeda dari Old)

```cpp
// Old UI color scheme (estimasi dari .bin texture references):
// Window title: dark blue gradient #2A2A6E
// Window body: dark navy #101020
// Window border: light blue #64B4FF
// Button normal: medium blue #325A82  
// Button hover: lighter blue #5078A0
// Button pressed: dark blue #1E3C64

// Update di UIRenderer.hpp atau Window.hpp:
struct UIColor { uint8_t r, g, b, a; };
// Tambah konstanta:
namespace OldColors {
    constexpr UIColor WINDOW_TITLE  = {42, 42, 110, 230};
    constexpr UIColor WINDOW_BODY   = {16, 16, 32, 220};
    constexpr UIColor WINDOW_BORDER = {100, 180, 255, 200};
    constexpr UIColor BTN_NORMAL    = {50, 90, 130, 220};
    constexpr UIColor BTN_HOVER     = {80, 120, 160, 220};
    constexpr UIColor BTN_PRESSED   = {30, 60, 100, 220};
}
```

## Output

✅ Kembalikan: "FEEL-02 done: updated X color constants, Y widget styles"
