# FINAL-02 — UI Chrome: Win32-style Window Frame

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old UI window pakai Win32 GDI window chrome: title bar gradient (biru tua), border 3D (raised), close button merah. Reborn punya Window class dengan DrawChrome(). Perlu update rendering chrome agar mirip Old.

## Aturan Ketat

1. BACA `client/ui/Window.cpp` — cari method Render() atau DrawChrome()
2. ✅ Jika sudah mirip — skip
3. 🔧 Jika belum — update DrawWindow() atau custom render

## File yang harus dicek

```bash
cat client/ui/Window.hpp | head -60
rg "DrawChrome\|Render\|DrawWindow\|draw_chrome" client/ui/Window.* --type cpp
```

## Yang harus diupdate

### 1. Title Bar Gradient
Old: gradient dari #2A2A6E ke #1A1A4E (dark blue). Reborn: solid color.
```cpp
// Di Window::Render() atau DrawChrome():
// Gambar gradient title bar:
// - Top: WINDOW_TITLE (42,42,110)
// - Bottom: darker (26,26,78)
// Gunakan UIRenderer::DrawRect() dengan warna berbeda
```

### 2. Window Border 3D
Old: border raised (light top/left, dark bottom/right).
```cpp
// Gambar 4 garis border:
// - Top: light blue (150,200,255)
// - Left: light blue (150,200,255) 
// - Bottom: dark blue (50,80,120)
// - Right: dark blue (50,80,120)
```

### 3. Close Button
Old: merah (#CC0000) dengan X putih.
```cpp
// Close button di pojok kanan atas:
// - Background: merah tua (180,20,20)
// - Hover: merah terang (220,40,40)
// - Text/X: putih (255,255,255)
```

### 4. Title Text
Old: putih, bold, shadow.
```cpp
// DrawText dengan white + shadow offset 1px
```

## Output

✅ Kembalikan: "FINAL-02 done: UI chrome aligned — gradient title bar, 3D border, close button"
