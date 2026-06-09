# LAST-02 — UI Skin: Port image_path.bin Texture References

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old UI menggunakan **texture atlas** yang direferensi oleh file image_path.bin (dan varian: image_path_grey.bin, image_item_path.bin, dll.). Reborn UI saat ini pakai solid colors (`UIColor`). Untuk UI look yang sama, perlu port texture-based skin.

## Aturan Ketat

1. ✅ Jika skin system SUDAH ADA — skip
2. 🔧 Jika BELUM — buat minimal: load image_path.bin.txt → texture atlas → DrawWindow/DrawButton dengan texture
3. JANGAN ubah widget logic — hanya skin rendering
4. Build verify

## Cek Existing

```bash
# Cari apakah ada texture atlas file
ls assets/textures/ui/ assets/textures/interface/ 2>/dev/null | head -10
# Cari file image_path yang sudah di-convert ke .txt
find assets/ -name "image_path*" 2>/dev/null
# Cek UiSkinManager
ls client/ui/skin/ 2>/dev/null
```

## Yang harus dibuat

### 1. client/ui/skin/UiSkinManager.hpp [BARU]

```cpp
#pragma once
#include <string>
#include <unordered_map>
#include <bgfx/bgfx.h>

struct SkinElement {
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
    float u1=0, v1=0, u2=1, v2=1; // UV coordinates
    bool has_texture = false;
};

class UiSkinManager {
public:
    static UiSkinManager& Instance();
    bool LoadSkin(const std::string& image_path_file);
    
    const SkinElement* GetElement(const std::string& name) const;
    
    // Draw UI element with skin texture
    void DrawWindowFrame(UIRenderer& ui, float x, float y, float w, float h);
    void DrawButton(UIRenderer& ui, float x, float y, float w, float h, bool hover, bool pressed);
    void DrawCheckBox(UIRenderer& ui, float x, float y, bool checked);
    void DrawScrollBar(UIRenderer& ui, float x, float y, float w, float h);
    
private:
    std::unordered_map<std::string, SkinElement> elements_;
    bgfx::TextureHandle atlas_ = BGFX_INVALID_HANDLE;
    bool ParseImagePathFile(const std::string& path);
};
```

### 2. client/ui/skin/UiSkinManager.cpp [BARU]

```cpp
#include "UiSkinManager.hpp"
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

bool UiSkinManager::ParseImagePathFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    
    std::string line;
    while (std::getline(f, line)) {
        // Format image_path.bin.txt: element_name, atlas_idx, u1, v1, u2, v2
        // (format tergantung hasil convert .bin → .txt)
        std::stringstream ss(line);
        std::string name;
        // Parse sesuai format actual file
        // Simpan ke elements_[name]
    }
    return true;
}
```

### 3. Integrasi ke UIRenderer atau Window

Update `client/ui/Window.cpp` — di method Render():
```cpp
// Jika skin texture tersedia, pakai texture
// Fallback ke solid color jika tidak ada
if (skin_element && skin_element->has_texture) {
    // Draw with texture
} else {
    // Draw with solid color (existing)
}
```

## Output

✅ Kembalikan: "LAST-02 done: UI skin texture system created — loaded X elements"
