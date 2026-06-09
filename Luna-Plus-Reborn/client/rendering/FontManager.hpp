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

// Font size constants — match Old GDI ClearType
static constexpr float FONT_SIZE_TITLE   = 12.0f;
static constexpr float FONT_SIZE_BUTTON  = 12.0f;
static constexpr float FONT_SIZE_CHAT    = 12.0f;
static constexpr float FONT_SIZE_TOOLTIP = 11.0f;
static constexpr float FONT_SIZE_DAMAGE  = 14.0f;

class FontManager {
public:
    static FontManager& Instance();

    bool Init();
    void SetLanguage(Language lang);
    Language GetLanguage() const { return current_lang_; }

    void* GetFont() const { return current_font_; }
    float GetFontSize() const { return font_size_; }

    static constexpr float FontSizeTitle()   { return FONT_SIZE_TITLE; }
    static constexpr float FontSizeButton()  { return FONT_SIZE_BUTTON; }
    static constexpr float FontSizeChat()    { return FONT_SIZE_CHAT; }
    static constexpr float FontSizeTooltip() { return FONT_SIZE_TOOLTIP; }
    static constexpr float FontSizeDamage()  { return FONT_SIZE_DAMAGE; }

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
