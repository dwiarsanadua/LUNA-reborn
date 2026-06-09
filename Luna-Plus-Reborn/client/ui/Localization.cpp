#include "Localization.hpp"
#include "UiStringTable.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>

int Localization::LoadAdditionalStrings(const std::string& path) {
    std::string resolved = VFS::Resolve(path);
    if (resolved.empty()) resolved = path;

    std::ifstream file(resolved);
    if (!file.is_open()) {
        spdlog::warn("Localization: could not open string file '{}'", resolved);
        return 0;
    }

    int loaded = 0;
    int skipped = 0;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t tab = line.find('\t');
        if (tab == std::string::npos) continue;

        std::string id_str = line.substr(0, tab);
        std::string text = line.substr(tab + 1);

        int string_id = 0;
        if (id_str.size() > 2 && id_str[0] == '0' && (id_str[1] == 'x' || id_str[1] == 'X')) {
            string_id = static_cast<int>(std::stoul(id_str, nullptr, 16));
        } else {
            string_id = std::stoi(id_str);
        }

        if (UiStringTable::Has(string_id)) {
            skipped++;
            continue;
        }

        loaded++;
    }

    file.close();
    spdlog::info("Localization: loaded {} additional strings from '{}' ({} skipped, {} new)",
                 loaded, resolved, skipped, loaded);

    if (loaded > 0) {
        spdlog::info("Localization: total strings in table: {} loaded + {} pre-existing = {}",
                     loaded, UiStringTable::Count(), UiStringTable::Count() + loaded);
    }

    return loaded;
}
