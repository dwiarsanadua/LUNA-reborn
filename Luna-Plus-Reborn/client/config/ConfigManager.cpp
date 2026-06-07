#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <spdlog/spdlog.h>

std::string ConfigManager::config_path_;
std::unordered_map<std::string, ConfigEntry> ConfigManager::entries_;
bool ConfigManager::modified_ = false;

void ConfigManager::Init(const std::string& config_path) {
    config_path_ = config_path;
    LoadDefaults();
    Load();
}

void ConfigManager::Shutdown() {
    if (modified_) Save();
    entries_.clear();
}

void ConfigManager::LoadDefaults() {
    // Video
    SetInt("video.width", 1280);
    SetInt("video.height", 720);
    SetBool("video.fullscreen", false);
    SetBool("video.vsync", true);
    SetInt("video.fps_limit", 0);

    // Audio
    SetFloat("audio.master_volume", 1.0f);
    SetFloat("audio.bgm_volume", 0.7f);
    SetFloat("audio.sfx_volume", 0.8f);

    // Gameplay
    SetString("gameplay.language", "en");
    SetFloat("gameplay.camera_speed", 1.0f);
    SetBool("gameplay.show_damage", true);

    // Key bindings
    SetString("keys.move_forward", "W");
    SetString("keys.move_back", "S");
    SetString("keys.move_left", "A");
    SetString("keys.move_right", "D");
    SetString("keys.inventory", "I");
    SetString("keys.skills", "K");
    SetString("keys.quests", "J");
    SetString("keys.chat", "T");
    SetString("keys.npc_dialog", "N");
    SetString("keys.pk_toggle", "P");

    modified_ = false;
}

std::string ConfigManager::SectionKey(const std::string& section, const std::string& key) {
    return section + "." + key;
}

std::string ConfigManager::GetString(const std::string& key, const std::string& def) {
    auto it = entries_.find(key);
    if (it != entries_.end()) return it->second.value;
    return def;
}

int ConfigManager::GetInt(const std::string& key, int def) {
    auto it = entries_.find(key);
    if (it != entries_.end()) return std::stoi(it->second.value);
    return def;
}

float ConfigManager::GetFloat(const std::string& key, float def) {
    auto it = entries_.find(key);
    if (it != entries_.end()) return std::stof(it->second.value);
    return def;
}

bool ConfigManager::GetBool(const std::string& key, bool def) {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
        std::string val = it->second.value;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return val == "true" || val == "1" || val == "yes";
    }
    return def;
}

void ConfigManager::SetString(const std::string& key, const std::string& value) {
    auto& entry = entries_[key];
    entry.key = key;
    entry.value = value;
    modified_ = true;
}

void ConfigManager::SetInt(const std::string& key, int value) {
    SetString(key, std::to_string(value));
}

void ConfigManager::SetFloat(const std::string& key, float value) {
    SetString(key, std::to_string(value));
}

void ConfigManager::SetBool(const std::string& key, bool value) {
    SetString(key, value ? "true" : "false");
}

void ConfigManager::Save() {
    if (config_path_.empty()) return;
    std::ofstream file(config_path_);
    if (!file) {
        spdlog::warn("ConfigManager: cannot save to {}", config_path_);
        return;
    }

    std::string current_section;
    for (auto& [key, entry] : entries_) {
        size_t dot = key.find('.');
        std::string section = (dot != std::string::npos) ? key.substr(0, dot) : "";
        std::string k = (dot != std::string::npos) ? key.substr(dot + 1) : key;

        if (section != current_section) {
            file << "\n[" << section << "]\n";
            current_section = section;
        }
        file << k << " = " << entry.value << "\n";
    }

    modified_ = false;
    spdlog::info("ConfigManager: saved to {}", config_path_);
}

void ConfigManager::Load() {
    if (config_path_.empty()) return;
    std::ifstream file(config_path_);
    if (!file) {
        spdlog::info("ConfigManager: no config file {}, using defaults", config_path_);
        return;
    }

    std::string current_section;
    std::string line;
    while (std::getline(file, line)) {
        // Trim
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                current_section = line.substr(1, end - 1);
            }
            continue;
        }

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));

        std::string full_key = current_section.empty() ? key : current_section + "." + key;
        entries_[full_key].key = full_key;
        entries_[full_key].value = value;
        entries_[full_key].section = current_section;
    }

    spdlog::info("ConfigManager: loaded {} entries from {}", entries_.size(), config_path_);
    modified_ = false;
}

std::vector<std::string> ConfigManager::GetSectionKeys(const std::string& section) {
    std::vector<std::string> keys;
    std::string prefix = section + ".";
    for (auto& [key, _] : entries_) {
        if (key.find(prefix) == 0) {
            keys.push_back(key.substr(prefix.length()));
        }
    }
    return keys;
}
