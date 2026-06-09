# Agent D01 — CJK Font + String Table Completion

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Konteks

Localization: UiStringTable loaded 1624 strings dari ~2000+ di Old. Font hanya 2002_EYA.ttf (tidak support CJK). Perlu:
1. Bundle font CJK (Korean, Chinese, Japanese)
2. Load sisa ~400 string yang missing
3. Implement language switching runtime

## File yang harus diubah

### client/rendering/FontManager.hpp [BARU]

```cpp
#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

enum class Language : uint8_t {
    English = 0,
    Korean,
    Japanese,
    Chinese,
    Count
};

class FontManager {
public:
    static FontManager& Instance();
    
    bool Init();
    void SetLanguage(Language lang);
    Language GetLanguage() const { return current_lang_; }
    
    // Font pointer untuk rendering (bergantung pada rendering backend)
    void* GetFont() const { return current_font_; }
    float GetFontSize() const { return font_size_; }
    
    static const char* LanguageToString(Language lang);
    static Language StringToLanguage(const std::string& str);
    
private:
    FontManager() = default;
    void* current_font_ = nullptr;
    Language current_lang_ = Language::English;
    float font_size_ = 14.0f;
    std::unordered_map<Language, void*> fonts_;
    
    bool LoadFont(Language lang, const std::string& path);
};
```

### client/rendering/FontManager.cpp [BARU]

Implementasi:
```cpp
// Load font berdasarkan language:
const std::unordered_map<Language, std::string> FONT_PATHS = {
    {Language::English, "assets/fonts/2002_EYA.ttf"},
    {Language::Korean,  "assets/fonts/NotoSansKR-Regular.ttf"},
    {Language::Japanese,"assets/fonts/NotoSansJP-Regular.ttf"},
    {Language::Chinese, "assets/fonts/NotoSansSC-Regular.ttf"},
};

bool FontManager::LoadFont(Language lang, const std::string& path) {
    // Implementasi spesifik tergantung rendering backend (bgfx/bgfx+imgui)
    // Fallback ke 2002_EYA.ttf jika font spesifik tidak ditemukan
    auto it = FONT_PATHS.find(lang);
    if (it == FONT_PATHS.end() || !FileExists(it->second)) {
        // Fallback: default font
        current_font_ = LoadTTF(FONT_PATHS.at(Language::English));
        return false;
    }
    current_font_ = LoadTTF(it->second);
    fonts_[lang] = current_font_;
    return true;
}
```

### client/ui/Localization.cpp [UPDATE]

**🟡 Tambah missing strings** (Severity: M, Effort: 3 days)
- Baca InterfaceMsg.bin.txt di assets/ (file ini sudah di-convert dari .bin)
- Bandingkan string ID yang sudah di-load oleh UiStringTable
- Identifikasi ~400 string yang missing
- Tambah ke string table

Pseudo-code:
```cpp
// Di Localization::LoadStrings()
// Cari file InterfaceMsg.bin.txt yang mungkin terlewat
auto missing = LoadAdditionalStrings("assets/interface/InterfaceMsg.bin.txt");
spdlog::info("Localization: loaded {} additional strings", missing);
```

### client/ui/Localization.hpp [UPDATE]

**🟡 Tambah SetLanguage** (Severity: M, Effort: 2 days)
```cpp
void SetLanguage(Language lang) {
    FontManager::Instance().SetLanguage(lang);
    current_lang_ = lang;
    // Reload UI layout jika perlu
    // Force refresh all active dialogs
    ConfigManager::SetString("gameplay.language", LanguageToString(lang));
    ConfigManager::Save();
}

Language GetCurrentLanguage() const { return current_lang_; }
```

### client/rendering/UIRenderer.cpp [UPDATE]

**🟡 Integrasi FontManager** (Severity: M, Effort: 1 day)
- Di DrawText(), gunakan FontManager untuk font yang sesuai
- Fallback ke default font jika font language tidak ditemukan

## Prerequisite

Download NotoSans fonts:
```bash
cd assets/fonts/
curl -LO https://github.com/googlefonts/noto-cjk/releases/download/Sans2.004/03_NotoSansCJKkr.zip
# Extract NotoSansKR-Regular.otf, convert to .ttf
```

Untuk development, fallback font 2002_EYA.ttf tetap digunakan sebagai default.
Font CJK hanya di-load ketika SetLanguage dipanggil dengan Korean/Japanese/Chinese.

## Aturan

1. JANGAN download font — cukup buat kode dengan path yang benar
2. FontManager harus graceful fallback jika font file tidak ditemukan
3. String table: jangan duplikasi string yang sudah ada
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent D01 done: localization updated"
