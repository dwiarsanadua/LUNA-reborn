#include "UiSoundIndex.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

static std::unordered_map<int, std::string> s_sounds;
static std::unordered_map<int, std::string> s_sound_names;
static std::unordered_map<std::string, std::vector<int>> s_sounds_by_category;
bool UiSoundIndex::ready_ = false;

static std::string NormalizeSoundPath(const std::string& raw) {
    std::string path = raw;
    for (char& c : path) {
        if (c == '\\') c = '/';
        else c = (char)std::tolower((unsigned char)c);
    }
    auto slash = path.find('/');
    if (slash != std::string::npos) path = path.substr(slash + 1);
    // Remove extension for lookup
    auto dot = path.rfind('.');
    if (dot != std::string::npos) path = path.substr(0, dot);
    return path;
}

static std::string Trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static bool ParseTabLine(const std::string& line, int& id, std::string& file, std::string& name) {
    return sscanf(line.c_str(), "%d\t%511[^\t]", &id, file.data()) >= 2;
}

static std::vector<std::string> SplitCSV(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, ',')) {
        parts.push_back(Trim(part));
    }
    return parts;
}

static int ParseSoundLine(const std::string& line, std::string& file_out, std::string& name_out) {
    std::string trimmed = Trim(line);
    if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '/' || trimmed[0] == '#')
        return -1;

    int id = -1;
    file_out.clear();
    name_out.clear();

    // Format 1: tab-separated (SoundList.bin.txt)
    // id\tfilename
    // id\tfilename\tsound_name
    char file_buf[512] = {}, name_buf[256] = {};
    int parsed = sscanf(trimmed.c_str(), "%d\t%511s\t%255[^\t\r\n]", &id, file_buf, name_buf);
    if (parsed >= 2) {
        file_out = file_buf;
        name_out = (parsed >= 3) ? name_buf : file_buf;
        return id;
    }

    // Format 2: tab-separated, filename may have spaces
    parsed = sscanf(trimmed.c_str(), "%d\t%511[^\t]", &id, file_buf);
    if (parsed >= 2 && file_buf[0] != '\0') {
        file_out = file_buf;
        name_out = file_buf;
        return id;
    }

    // Format 3: comma-separated (CSV)
    auto parts = SplitCSV(trimmed);
    if (parts.size() >= 2) {
        try {
            id = std::stoi(parts[0]);
            file_out = parts[1];
            name_out = (parts.size() >= 3) ? parts[2] : parts[1];
            return id;
        } catch (...) {}
    }

    // Format 4: space-separated (id filename)
    std::stringstream ss(trimmed);
    int first_val = 0;
    std::string second_val;
    if (ss >> first_val >> second_val) {
        id = first_val;
        file_out = second_val;
        name_out = second_val;
        return id;
    }

    // Format 5: filename only (no ID, assign auto-increment)
    if (trimmed.find_first_of("\\/") != std::string::npos || trimmed.find('.') != std::string::npos) {
        static int auto_id = 100000;
        id = auto_id++;
        file_out = trimmed;
        name_out = trimmed;
        return id;
    }

    return -1;
}

static void TryLoadFromDirectory(const std::string& category, const std::string& dir_path) {
    std::string resolved = VFS::Find(dir_path);
    if (resolved.empty() || !fs::exists(resolved)) return;

    int count = 0;
    for (auto& entry : fs::directory_iterator(resolved)) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext != ".wav" && ext != ".ogg" && ext != ".mp3" && ext != ".flac") continue;

        int id = std::hash<std::string>{}(entry.path().stem().string()) & 0x7fffffff;
        std::string filename = entry.path().filename().string();
        std::string normalized = NormalizeSoundPath(filename);

        if (s_sounds.find(id) == s_sounds.end()) {
            s_sounds[id] = normalized;
            s_sound_names[id] = entry.path().stem().string();
            s_sounds_by_category[category].push_back(id);
            count++;
        }
    }
    if (count > 0) {
        spdlog::debug("UiSoundIndex: scanned {} sounds from directory '{}'", count, dir_path);
    }
}

void UiSoundIndex::Init(const std::string& path) {
    if (ready_) return;
    s_sounds.clear();
    s_sound_names.clear();
    s_sounds_by_category.clear();
    // 1. Load from SoundList file (primary source)
    std::string resolved = VFS::Find(path);
    if (resolved.empty()) resolved = path;

    std::ifstream in(resolved);
    if (in.is_open()) {
        std::string line;
        int line_num = 0;
        int loaded = 0;
        int skipped = 0;

        while (std::getline(in, line)) {
            line_num++;
            std::string file_path, sound_name;
            int id = ParseSoundLine(line, file_path, sound_name);
            if (id < 0) {
                skipped++;
                continue;
            }

            std::string normalized = NormalizeSoundPath(file_path);

            // Detect duplicate IDs
            if (s_sounds.find(id) != s_sounds.end()) {
                spdlog::warn("UiSoundIndex: duplicate sound ID {} at line {} (was '{}', now '{}')",
                             id, line_num, s_sounds[id], normalized);
                skipped++;
                continue;
            }

            s_sounds[id] = normalized;
            s_sound_names[id] = sound_name.empty() ? normalized : sound_name;

            // Categorize by first path component if available
            std::string category = "uncategorized";
            auto slash = file_path.find('/');
            if (slash != std::string::npos) {
                category = file_path.substr(0, slash);
                std::transform(category.begin(), category.end(), category.begin(), ::tolower);
            }
            s_sounds_by_category[category].push_back(id);
            loaded++;
        }

        spdlog::info("UiSoundIndex: loaded {} sounds from '{}' ({} skipped, {} lines)",
                     loaded, path, skipped, line_num);
    } else {
        spdlog::warn("UiSoundIndex: cannot open '{}', will scan audio directories", resolved);
    }

    // 2. Supplement by scanning audio directories for additional sounds
    const std::string audio_dirs[] = {
        "audio/Interface/", "audio/Effect/", "audio/Character/", "audio/Monster/",
        "audio/Weapon/", "audio/Vehicle/", "audio/Ambient/", "audio/NPC/"
    };
    for (auto& dir : audio_dirs) {
        TryLoadFromDirectory(dir, dir);
    }

    spdlog::info("UiSoundIndex: loaded {} total sounds across {} categories",
                 s_sounds.size(), s_sounds_by_category.size());
    ready_ = true;
}

void UiSoundIndex::Shutdown() {
    s_sounds.clear();
    s_sound_names.clear();
    s_sounds_by_category.clear();
    ready_ = false;
}

std::string UiSoundIndex::Resolve(int sound_id) {
    auto it = s_sounds.find(sound_id);
    if (it == s_sounds.end()) return {};
    return it->second;
}
