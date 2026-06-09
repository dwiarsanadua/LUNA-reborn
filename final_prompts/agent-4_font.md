# AGENT-4 — Font Rendering: FreeType LCD Mode (FINAL-05)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old pakai GDI ClearType (subpixel rendering). Reborn pakai FreeType default (gray aliasing). Konfigurasi FreeType agar menghasilkan rendering identik dengan ClearType.

## Aturan Ketat

1. CARI file yang handle font rendering — bisa di `engine/gx_render/` atau `client/rendering/`
2. ✅ Jika FreeType sudah pakai FT_RENDER_MODE_LCD → skip
3. 🔧 Jika belum → update
4. Build verify

## File target (tidak ada conflict dengan agent lain)

Cari dengan:
```bash
rg "FT_Load_Glyph\|FT_Render_Glyph\|FT_LOAD_\|FT_RENDER_MODE" engine/ client/ --type cpp -l
rg "glyph\|font_size\|ttf\|FreeType\|freetype" engine/gx_render/ client/rendering/ --type cpp -l
```

### Yang harus diubah (di file font rendering yang ditemukan):

```cpp
// SEBELUM (FreeType default — gray aliasing):
FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

// SESUDAH (ClearType-like — LCD subpixel):
FT_Load_Glyph(face, glyph_index, FT_LOAD_TARGET_LCD);
FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);
// Buffer sekarang 3x lebar (RGB subpixel)
// Set texture format ke R8 atau RGB sesuai backend
```

### Font Size Table

Cari konstanta font size di UIRenderer.hpp atau file font:

```cpp
// Update font sizes agar match Old:
static constexpr float FONT_SIZE_TITLE   = 12.0f;  // title bar
static constexpr float FONT_SIZE_BUTTON  = 12.0f;  // button text
static constexpr float FONT_SIZE_CHAT    = 12.0f;  // chat text
static constexpr float FONT_SIZE_TOOLTIP = 11.0f;  // tooltip
static constexpr float FONT_SIZE_DAMAGE  = 14.0f;  // damage number (bold)
```

## ✅ Kembalikan: "AGENT-4 done: font rendering tuned — LCD mode + sizes, 0 build errors"
