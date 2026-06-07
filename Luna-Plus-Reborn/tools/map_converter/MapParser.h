#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/glm.hpp>

struct MapObject {
    std::string model;
    uint32_t material;
    glm::vec3 scale{1.0f};
    glm::vec3 position{0.0f};
    glm::vec4 rotation{0.0f, 1.0f, 0.0f, 0.0f};
};

struct MapData {
    glm::vec3 box_min{-1830, -1237, 0};
    glm::vec3 box_max{51200, 6284, 51200};
    std::string static_model;
    std::string heightfield_file;
    std::vector<MapObject> objects;
    struct Light { uint32_t color; glm::vec3 pos; float range; int shadow; };
    std::vector<Light> lights;
};

class MapParser {
public:
    static bool Parse(const std::string& path, MapData& out) {
        std::ifstream f(path);
        if (!f) return false;
        std::string text((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        auto trim = [](std::string& s) {
            while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) s.pop_back();
            while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
        };

        // Parse sections
        size_t pos = 0;
        while (pos < text.size()) {
            auto gx = text.find("GX_", pos);
            if (gx == std::string::npos) break;
            auto name_start = gx + 3;
            auto name_end = text.find_first_of(" \t\r\n", name_start);
            std::string section = text.substr(name_start, name_end - name_start);

            // Extract count if present on the GX_ line (e.g. "GX_OBJECT 1331")
            std::string countStr;
            auto countStart = text.find_first_of("0123456789", name_end);
            if (countStart != std::string::npos && countStart < text.find('{', name_end)) {
                auto countEnd = text.find_first_not_of("0123456789", countStart);
                countStr = text.substr(countStart, countEnd - countStart);
            }

            auto brace_start = text.find('{', name_end);
            if (brace_start == std::string::npos) break;
            int depth = 1;
            size_t brace_end = brace_start + 1;
            for (; brace_end < text.size() && depth > 0; brace_end++) {
                if (text[brace_end] == '{') depth++;
                else if (text[brace_end] == '}') depth--;
            }
            std::string content = text.substr(brace_start + 1, brace_end - brace_start - 2);

            if (section == "METADATA") {
                for (auto& line : {content}) {
                    std::stringstream ss(line);
                    std::string tok;
                    while (ss >> tok) {
                        if (tok == "BOX_MAX") ss >> out.box_max.x >> out.box_max.y >> out.box_max.z;
                        if (tok == "BOX_MIN") ss >> out.box_min.x >> out.box_min.y >> out.box_min.z;
                    }
                }
            }
            else if (section == "MAP") {
                std::stringstream ss(content);
                std::string tok;
                while (ss >> tok) {
                    if (tok == "STATIC_MODEL") ss >> out.static_model;
                    if (tok == "HEIGHT_FIELD") ss >> out.heightfield_file;
                }
            }
            else if (section == "OBJECT") {
                std::stringstream ss(content);
                int count = 0;
                if (!countStr.empty()) count = std::stoi(countStr);
                for (int i = 0; i < count && ss.good(); i++) {
                    MapObject obj;
                    ss >> obj.model >> obj.material
                       >> obj.scale.x >> obj.scale.y >> obj.scale.z
                       >> obj.position.x >> obj.position.y >> obj.position.z
                       >> obj.rotation.x >> obj.rotation.y >> obj.rotation.z >> obj.rotation.w;
                    int extra = 0; ss >> extra;
                    if (!obj.model.empty() && obj.model != "runa+stone") {
                        out.objects.push_back(obj);
                    }
                }
            }
            else if (section == "LIGHT") {
                std::stringstream ss(content);
                int count = 0; ss >> count;
                for (int i = 0; i < count && ss.good(); i++) {
                    MapData::Light lt;
                    std::string hex; ss >> hex;
                    lt.color = std::stoul(hex, nullptr, 16);
                    ss >> lt.pos.x >> lt.pos.y >> lt.pos.z
                       >> lt.range >> lt.shadow;
                    out.lights.push_back(lt);
                }
            }
            pos = brace_end;
        }
        return !out.heightfield_file.empty();
    }
};
