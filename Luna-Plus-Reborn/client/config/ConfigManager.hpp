#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct ConfigEntry {
    std::string key;
    std::string value;
    std::string section;
};

class ConfigManager {
public:
    static void Init(const std::string& config_path = "config/client.yaml");
    static void Shutdown();

    // Read
    static std::string GetString(const std::string& key, const std::string& def = "");
    static int GetInt(const std::string& key, int def = 0);
    static float GetFloat(const std::string& key, float def = 0);
    static bool GetBool(const std::string& key, bool def = false);

    // Write
    static void SetString(const std::string& key, const std::string& value);
    static void SetInt(const std::string& key, int value);
    static void SetFloat(const std::string& key, float value);
    static void SetBool(const std::string& key, bool value);

    // Persist
    static void Save();
    static void Load();

    // Sections
    static std::vector<std::string> GetSectionKeys(const std::string& section);

    // Default values
    static void LoadDefaults();

    // Access
    static const std::string& GetPath() { return config_path_; }

    // Video settings
    static int GetResolutionWidth() { return GetInt("video.width", 1280); }
    static int GetResolutionHeight() { return GetInt("video.height", 720); }
    static bool GetFullscreen() { return GetBool("video.fullscreen", false); }
    static bool GetVSync() { return GetBool("video.vsync", true); }
    static int GetFPSLimit() { return GetInt("video.fps_limit", 0); }

    // Audio settings
    static float GetMasterVolume() { return GetFloat("audio.master_volume", 1.0f); }
    static float GetBGMVolume() { return GetFloat("audio.bgm_volume", 0.7f); }
    static float GetSFXVolume() { return GetFloat("audio.sfx_volume", 0.8f); }

    // Gameplay settings
    static std::string GetLanguage() { return GetString("gameplay.language", "en"); }
    static float GetCameraSpeed() { return GetFloat("gameplay.camera_speed", 1.0f); }
    static bool ShowDamageNumbers() { return GetBool("gameplay.show_damage", true); }

private:
    static std::string config_path_;
    static std::unordered_map<std::string, ConfigEntry> entries_;
    static bool modified_;
    static std::string SectionKey(const std::string& section, const std::string& key);
};
