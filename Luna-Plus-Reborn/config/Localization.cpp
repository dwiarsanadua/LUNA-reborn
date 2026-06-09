#include <ui/Localization.hpp>
#include <config/ConfigManager.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <algorithm>

namespace {

static const char* LANG_CONFIG_KEY = "gameplay.language";

struct LangMapping {
    const char* config_value;
    Localization::Lang lang;
};

static const LangMapping LANG_MAP[] = {
    {"en", Localization::EN},
    {"id", Localization::ID},
};

static Localization::Lang ConfigToLang(const std::string& cfg) {
    std::string lower = cfg;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& m : LANG_MAP) {
        if (lower == m.config_value) return m.lang;
    }
    return Localization::EN;
}

static const char* LangToConfig(Localization::Lang lang) {
    for (const auto& m : LANG_MAP) {
        if (m.lang == lang) return m.config_value;
    }
    return "en";
}

} // anonymous namespace

void Localization_InitFromConfig() {
    std::string lang_str = ConfigManager::GetString(LANG_CONFIG_KEY, "en");
    Localization::Lang lang = ConfigToLang(lang_str);
    Localization::SetLanguage(lang);
    spdlog::info("Localization: language set to '{}' (from config: '{}')",
                 LangToConfig(lang), lang_str);
}

bool Localization_SwitchLanguage(const std::string& lang_code) {
    Localization::Lang lang = ConfigToLang(lang_code);
    if (lang < 0 || lang >= Localization::_COUNT) {
        spdlog::warn("Localization: unknown language '{}', falling back to en", lang_code);
        lang = Localization::EN;
    }

    Localization::SetLanguage(lang);
    ConfigManager::SetString(LANG_CONFIG_KEY, LangToConfig(lang));
    ConfigManager::Save();

    spdlog::info("Localization: switched to '{}'", LangToConfig(lang));
    return true;
}

bool Localization_SwitchLanguage(Localization::Lang lang) {
    if (lang < 0 || lang >= Localization::_COUNT) return false;
    Localization::SetLanguage(lang);
    ConfigManager::SetString(LANG_CONFIG_KEY, LangToConfig(lang));
    ConfigManager::Save();
    spdlog::info("Localization: switched to '{}'", LangToConfig(lang));
    return true;
}

const char* Localization_GetCurrentConfigValue() {
    return LangToConfig(Localization::GetLanguage());
}

int Localization_GetAvailableCount() {
    return Localization::_COUNT;
}

const char* Localization_GetAvailableCode(int index) {
    if (index < 0 || index >= Localization::_COUNT) return "";
    return LangToConfig(static_cast<Localization::Lang>(index));
}

const char* Localization_GetAvailableName(int index) {
    switch (static_cast<Localization::Lang>(index)) {
        case Localization::EN: return "English";
        case Localization::ID: return "Bahasa Indonesia";
        default: return "";
    }
}
