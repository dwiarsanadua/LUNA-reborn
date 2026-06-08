#include "UiSoundIndex.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cctype>

static std::unordered_map<int, std::string> s_sounds;
bool UiSoundIndex::ready_ = false;

static std::string NormalizeSoundPath(const std::string& raw) {
    std::string path = raw;
    for (char& c : path) {
        if (c == '\\') c = '/';
        else c = (char)std::tolower((unsigned char)c);
    }
    auto slash = path.find('/');
    if (slash != std::string::npos) path = path.substr(slash + 1);
    return path;
}

void UiSoundIndex::Init(const std::string& path) {
    if (ready_) return;
    s_sounds.clear();

    std::string resolved = VFS::Find(path);
    if (resolved.empty()) resolved = path;

    std::ifstream in(resolved);
    if (!in.is_open()) {
        spdlog::warn("UiSoundIndex: cannot open {}", resolved);
        ready_ = true;
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        int id = 0;
        char file[512] = {};
        if (sscanf(line.c_str(), "%d\t%511[^\t]", &id, file) < 2) continue;
        if (file[0] == '\0') continue;
        s_sounds[id] = NormalizeSoundPath(file);
    }

    spdlog::info("UiSoundIndex: loaded {} sound entries", s_sounds.size());
    ready_ = true;
}

void UiSoundIndex::Shutdown() {
    s_sounds.clear();
    ready_ = false;
}

std::string UiSoundIndex::Resolve(int sound_id) {
    auto it = s_sounds.find(sound_id);
    if (it == s_sounds.end()) return {};
    return it->second;
}
