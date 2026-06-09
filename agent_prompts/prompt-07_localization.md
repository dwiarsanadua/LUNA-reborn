# Agent-07 — Localization: CJK Font + String Table Completion

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Lengkapi localization support: CJK fonts dan string table. Dari verifikasi sebelumnya, UiStringTable loaded 1624 strings dari ~2000+ di Old. Font hanya 2002_EYA.ttf.

## Aturan Ketat

1. BACA dulu file yang akan diubah
2. ✅ Jika SUDAH ADA — skip, jangan duplikasi
3. 🔧 Jika BELUM ADA — tambah
4. JANGAN download font — cukup siapkan kode dengan path yang benar
5. Build verify

## File yang harus dicek

### client/rendering/FontManager.hpp
```bash
ls client/rendering/FontManager.* 2>/dev/null
```
Jika SUDAH ADA ✅ → skip, baca dulu apa yang sudah diimplementasi
Jika BELUM ADA 🔧 → buat:

```cpp
#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

enum class Language : uint8_t { English = 0, Korean, Japanese, Chinese, Count };

class FontManager {
public:
    static FontManager& Instance();
    bool Init();
    void SetLanguage(Language lang);
    Language GetLanguage() const { return current_lang_; }
    void* GetFont() const { return current_font_; }
private:
    void* current_font_ = nullptr;
    Language current_lang_ = Language::English;
    std::unordered_map<Language, void*> fonts_;
    bool LoadFont(Language lang, const std::string& path);
};
```

### client/rendering/FontManager.cpp
Jika belum ada, implementasi dengan fallback:
```cpp
bool FontManager::LoadFont(Language lang, const std::string& path) {
    // Try to load font, fallback to 2002_EYA.ttf
    (void)lang;
    (void)path;
    // Implementation depends on rendering backend
    return true;
}
```

### client/ui/Localization.cpp
```bash
ls client/ui/Localization.cpp 2>/dev/null
```
Cek apakah sudah ada. Jika belum:
```cpp
#include "Localization.hpp"
#include <ui/UiStringTable.hpp>
void Localization::SetLanguage(Language lang) {
    FontManager::Instance().SetLanguage(lang);
    current_lang_ = lang;
}
```

### client/ui/UiStringTable.cpp
Cek apakah sudah load semua strings:
```bash
grep -n "Load\|Parse\|InterfaceMsg" client/ui/UiStringTable.*
```
Pastikan file `InterfaceMsg.bin.txt` di-load dengan benar.

## Output

✅ Kembalikan: "Agent-07 done: [list file yang dibuat/dicek]"
