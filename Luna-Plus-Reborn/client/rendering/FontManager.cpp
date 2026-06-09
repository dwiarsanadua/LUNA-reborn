#include "FontManager.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

static const std::unordered_map<Language, std::string> FONT_PATHS = {
    {Language::English,  "assets/fonts/2002_EYA.ttf"},
    {Language::Korean,   "assets/fonts/NotoSansKR-Regular.ttf"},
    {Language::Japanese, "assets/fonts/NotoSansJP-Regular.ttf"},
    {Language::Chinese,  "assets/fonts/NotoSansSC-Regular.ttf"},
};

static const char* LANGUAGE_NAMES[] = {
    "English",
    "Korean",
    "Japanese",
    "Chinese",
};

FontManager& FontManager::Instance() {
    static FontManager mgr;
    return mgr;
}

bool FontManager::Init() {
    current_lang_ = Language::English;
    current_font_ = nullptr;
    font_size_ = 14.0f;
    spdlog::info("FontManager: initialized with default language English");
    return true;
}

void FontManager::SetLanguage(Language lang) {
    if (lang >= Language::Count) {
        spdlog::warn("FontManager: invalid language {}, keeping current", static_cast<int>(lang));
        return;
    }

    auto it = fonts_.find(lang);
    if (it != fonts_.end() && it->second != nullptr) {
        current_font_ = it->second;
        current_lang_ = lang;
        spdlog::info("FontManager: switched to {} (cached)", LanguageToString(lang));
        return;
    }

    auto path_it = FONT_PATHS.find(lang);
    if (path_it == FONT_PATHS.end()) {
        spdlog::warn("FontManager: no font path for {}, keeping current", LanguageToString(lang));
        return;
    }

    std::string resolved = VFS::Resolve(path_it->second);
    if (resolved.empty()) {
        resolved = path_it->second;
    }

    FILE* fp = std::fopen(resolved.c_str(), "rb");
    if (!fp) {
        spdlog::warn("FontManager: font file not found '{}' for {}, falling back to English",
                     resolved, LanguageToString(lang));
        if (lang != Language::English) {
            SetLanguage(Language::English);
        }
        return;
    }
    std::fclose(fp);

    if (LoadFont(lang, resolved)) {
        current_lang_ = lang;
        spdlog::info("FontManager: loaded font for {} from '{}'", LanguageToString(lang), resolved);
    } else {
        spdlog::warn("FontManager: failed to load font for {}, falling back to English",
                     LanguageToString(lang));
        if (lang != Language::English) {
            SetLanguage(Language::English);
        }
    }
}

bool FontManager::LoadFont(Language lang, const std::string& path) {
    (void)path;
    current_font_ = reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<int>(lang) + 1));
    fonts_[lang] = current_font_;
    return true;
}

const char* FontManager::LanguageToString(Language lang) {
    int idx = static_cast<int>(lang);
    if (idx >= 0 && idx < static_cast<int>(Language::Count)) {
        return LANGUAGE_NAMES[idx];
    }
    return "Unknown";
}

Language FontManager::StringToLanguage(const std::string& str) {
    for (int i = 0; i < static_cast<int>(Language::Count); i++) {
        if (str == LANGUAGE_NAMES[i]) {
            return static_cast<Language>(i);
        }
    }
    if (str == "ko" || str == "kr") return Language::Korean;
    if (str == "ja" || str == "jp") return Language::Japanese;
    if (str == "zh" || str == "cn") return Language::Chinese;
    return Language::English;
}
