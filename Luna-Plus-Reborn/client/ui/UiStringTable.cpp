#include "UiStringTable.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

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
    int line_num = 0;
    int skipped = 0;

    while (std::getline(in, line)) {
        line_num++;

        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        auto tab = line.find('\t');
        if (tab == std::string::npos) {
            skipped++;
            continue;
        }

        std::string id_str = line.substr(0, tab);
        std::string text = line.substr(tab + 1);

        if (text.empty()) {
            skipped++;
            continue;
        }

        int id = 0;
        if (id_str.size() > 2 && id_str[0] == '0' && (id_str[1] == 'x' || id_str[1] == 'X')) {
            id = static_cast<int>(std::strtoul(id_str.c_str(), nullptr, 16));
        } else {
            id = std::atoi(id_str.c_str());
        }

        if (id == 0 && line_num > 1) {
            skipped++;
            continue;
        }

        s_strings[id] = std::move(text);
    }

    spdlog::info("UiStringTable: loaded {} UI strings from {} ({} lines, {} skipped)",
                 s_strings.size(), resolved, line_num, skipped);
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

std::string UiStringTable::Format(int id, ...) {
    va_list args;
    va_start(args, id);
    std::string result = FormatV(id, args);
    va_end(args);
    return result;
}

std::string UiStringTable::FormatV(int id, va_list args) {
    const char* fmt = Get(id);
    if (!fmt) return "(missing string " + std::to_string(id) + ")";

    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (len < 0) return fmt;

    std::string result(static_cast<size_t>(len) + 1, '\0');
    std::vsnprintf(&result[0], result.size(), fmt, args);
    result.resize(static_cast<size_t>(len));
    return result;
}

bool UiStringTable::Has(int id) {
    return s_strings.find(id) != s_strings.end();
}

int UiStringTable::Count() {
    return static_cast<int>(s_strings.size());
}

std::vector<StringEntry> UiStringTable::GetAll() {
    std::vector<StringEntry> entries;
    entries.reserve(s_strings.size());
    for (const auto& [id, text] : s_strings) {
        entries.push_back({id, text});
    }
    std::sort(entries.begin(), entries.end(),
              [](const StringEntry& a, const StringEntry& b) { return a.id < b.id; });
    return entries;
}

void UiStringTable::DumpMissing() {
    spdlog::info("UiStringTable: {} strings loaded", s_strings.size());
}
