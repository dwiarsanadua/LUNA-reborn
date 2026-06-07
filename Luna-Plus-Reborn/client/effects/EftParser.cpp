#include "EftParser.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>

std::unordered_map<std::string, EftDefinition> EftParser::cache_;

EftDefinition EftParser::Parse(const std::string& path) {
    if (cache_.count(path)) return cache_[path];

    std::ifstream f(path);
    if (!f) {
        spdlog::error("EftParser: Cannot open {}", path);
        return {};
    }

    EftDefinition def;
    def.name = path;
    
    std::string line;
    EftUnit* current_unit = nullptr;

    while (std::getline(f, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '@' || line[0] == ';') continue;

        if (line.find("#REPEAT") == 0) {
            def.loop = (line.find("TRUE") != std::string::npos || line.find("1") != std::string::npos);
        } else if (line.find("#NEWEFFECTUNIT") == 0 || line.find("#EFFECTUNIT") == 0 || line.find("#BILLBOARD") == 0 || line.find("#PARTICLE") == 0) {
            def.units.push_back(EftUnit{});
            current_unit = &def.units.back();
            if (line.find("#BILLBOARD") == 0) current_unit->type = EftUnit::Billboard;
            else if (line.find("#PARTICLE") == 0 || line.find("PARTICLE TRUE") != std::string::npos) current_unit->type = EftUnit::Particle;
        } else if (current_unit) {
            if (line.find("#LIFE") == 0) sscanf(line.c_str(), "#LIFE %f", &current_unit->life_time);
            else if (line.find("#SCALE") == 0) sscanf(line.c_str(), "#SCALE %f %f %f", &current_unit->scale_start.x, &current_unit->scale_start.y, &current_unit->scale_start.z);
            else if (line.find("#VELOCITY") == 0) sscanf(line.c_str(), "#VELOCITY %f %f", &current_unit->speed_min, &current_unit->speed_max);
            else if (line.find("#GRAVITY") == 0) sscanf(line.c_str(), "#GRAVITY %f", &current_unit->gravity);
            else if (line.find("#ROTATE") == 0) sscanf(line.c_str(), "#ROTATE %f %f %f", &current_unit->rot_offset.x, &current_unit->rot_offset.y, &current_unit->rot_offset.z);
            else if (line.find("#ALPHA") == 0) sscanf(line.c_str(), "#ALPHA %f %f", &current_unit->alpha_start, &current_unit->alpha_end);
            else if (line.find("#COLOR") == 0) {
                int r, g, b; sscanf(line.c_str(), "#COLOR %d %d %d", &r, &g, &b);
                current_unit->color_start = (255 << 24) | (b << 16) | (g << 8) | r;
            }
            else if (line.find("#FILE") == 0 || line.find("#TEXTURE") == 0) {
                char buf[128]; 
                if (sscanf(line.c_str(), "#FILE %s", buf) == 1 || sscanf(line.c_str(), "#TEXTURE %s", buf) == 1)
                    current_unit->asset_name = buf;
            }
        }
    }

    cache_[path] = def;
    return def;
}

std::vector<std::string> EftParser::GetParsedEffectNames() {
    std::vector<std::string> names;
    for (auto& [k, v] : cache_) names.push_back(k);
    return names;
}

std::string EftParser::Trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}
