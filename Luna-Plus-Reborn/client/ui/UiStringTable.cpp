#include "UiStringTable.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>

static std::unordered_map<int, std::string> s_strings;
bool UiStringTable::ready_ = false;

void UiStringTable::Init(const std::string& path) {
    if (ready_) return;
    s_strings.clear();

    std::string resolved = VFS::Find(path);
    if (resolved.empty()) resolved = path;

    std::ifstream in(resolved);
    if (!in.is_open()) {
        spdlog::warn("UiStringTable: cannot open {}", resolved);
        ready_ = true;
        return;
    }

    std::string line;
    std::getline(in, line); // header line

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto tab = line.find('\t');
        if (tab == std::string::npos) continue;
        int id = std::atoi(line.substr(0, tab).c_str());
        std::string text = line.substr(tab + 1);
        if (!text.empty()) s_strings[id] = std::move(text);
    }

    spdlog::info("UiStringTable: loaded {} UI strings", s_strings.size());
    ready_ = true;
}

void UiStringTable::Shutdown() {
    s_strings.clear();
    ready_ = false;
}

const char* UiStringTable::Get(int id) {
    auto it = s_strings.find(id);
    if (it == s_strings.end()) return nullptr;
    return it->second.c_str();
}

const char* UiStringTable::GetOrEmpty(int id) {
    const char* s = Get(id);
    return s ? s : "";
}
