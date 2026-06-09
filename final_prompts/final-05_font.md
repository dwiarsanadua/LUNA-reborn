# FINAL-05 — Font Rendering: FreeType Hinting + Anti-Alias

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old pakai Windows GDI ClearType untuk font rendering. Reborn pakai FreeType + bgfx texture. Hasil rendering font bisa terlihat berbeda (tipis, kasar, atau kabur). Perlu tuning FreeType rendering parameter.

## Aturan Ketat

1. BACA file font rendering — cari FreeType config
2. ✅ Jika sudah optimal — skip
3. 🔧 Jika belum — update rendering mode

## File yang harus dicek

```bash
rg "freetype\|FreeType\|FT_\|font\|Font\|glyph\|Glyph\|texture_font" engine/gx_render/ --type cpp -l
rg "FT_LOAD_\|FT_RENDER_MODE\|SetFont\|font_size\|FontAtlas" engine/gx_render/ --type cpp
cat client/rendering/UIRenderer.hpp | head -30
```

## Parameter FreeType yang bisa di-tune

| Parameter | Old (ClearType) | Reborn (FreeType) | Action |
|-----------|----------------|-------------------|--------|
| Render mode | LCD (subpixel) | ? | 🔧 Set ke FT_RENDER_MODE_LCD |
| Hinting | Full hinting | ? | 🔧 Set ke FT_LOAD_TARGET_LCD |
| Anti-alias | 4x4 | ? | 🔧 Set ke 8-bit |
| Font size | 11pt (UI), 9pt (chat) | ? | 🔧 Sesuaikan |

### Implementasi

```cpp
// Cari font loading code — biasanya di UIRenderer atau FontManager:
// Set FreeType load flags:
// FT_LOAD_TARGET_LCD — untuk ClearType-like rendering
// FT_RENDER_MODE_LCD — subpixel rendering

// Contoh:
FT_Load_Glyph(face, glyph_index, FT_LOAD_TARGET_LCD);
FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD);
```

### Font Size Table

| UI Element | Old (pt) | Reborn (px) | Action |
|-----------|---------|-------------|--------|
| Title bar | 9 | ? | 🔧 Set ke ~12px |
| Button text | 9 | ? | 🔧 Set ke ~12px |
| Chat text | 9 | ? | 🔧 Set ke ~12px |
| Tooltip | 8 | ? | 🔧 Set ke ~11px |
| Item name | 9 | ? | 🔧 Set ke ~12px |
| Damage num | 11 bold | ? | 🔧 Set ke ~14px bold |

## Output

✅ Kembalikan: "FINAL-05 done: font rendering tuned — LCD mode, hinting, sizes"
