#include "MapParser.h"
#include "HflParser.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

static std::string FindFile(const std::string& name, const std::vector<std::string>& dirs) {
    for (auto& dir : dirs) {
        fs::path p = fs::path(dir) / name;
        if (fs::exists(p)) return p.string();
        // case-insensitive
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (fs::is_directory(dir)) {
            for (auto& entry : fs::directory_iterator(dir)) {
                std::string fn = entry.path().filename().string();
                std::string fnl = fn;
                std::transform(fnl.begin(), fnl.end(), fnl.begin(), ::tolower);
                if (fnl == lower) return entry.path().string();
            }
        }
    }
    return {};
}

static void WriteSceneJSON(const std::string& path, const MapData& map, const HeightField* hf) {
    std::ofstream f(path);
    f << "{\n";
    f << "  \"map\": \"" << fs::path(path).stem().string() << "\",\n";
    if (hf) {
        f << "  \"heightfield\": {\"width\": " << hf->width << ", \"height\": " << hf->height
          << ", \"hmin\": " << hf->hmin << ", \"hmax\": " << hf->hmax << "},\n";
    }
    f << "  \"box_min\": [" << map.box_min.x << "," << map.box_min.y << "," << map.box_min.z << "],\n";
    f << "  \"box_max\": [" << map.box_max.x << "," << map.box_max.y << "," << map.box_max.z << "],\n";
    f << "  \"static_model\": \"" << map.static_model << "\",\n";
    f << "  \"lights\": [\n";
    for (size_t i = 0; i < map.lights.size(); i++) {
        auto& l = map.lights[i];
        f << "    {\"color\": " << l.color << ", \"pos\": [" << l.pos.x << "," << l.pos.y << "," << l.pos.z
          << "], \"range\": " << l.range << "}";
        if (i < map.lights.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ],\n";
    f << "  \"objects\": [\n";
    for (size_t i = 0; i < map.objects.size(); i++) {
        auto& o = map.objects[i];
        f << "    {\"model\": \"" << o.model << "\", \"pos\": [" << o.position.x << "," << o.position.y << "," << o.position.z
          << "], \"scale\": [" << o.scale.x << "," << o.scale.y << "," << o.scale.z
          << "], \"rot\": [" << o.rotation.x << "," << o.rotation.y << "," << o.rotation.z << "," << o.rotation.w << "]}";
        if (i < map.objects.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n";
    f << "}\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: map_converter <input.map> [-o out_dir] [-d mod_dir ...]\n";
        return 1;
    }

    std::string input;
    std::string outDir = "assets_converted/maps";
    std::vector<std::string> searchDirs = {
        "assets/unpacked/map",
        "assets/unpacked/npc",
        "assets/unpacked/character",
    };

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) outDir = argv[++i];
        else if ((arg == "-d" || arg == "--dirs") && i + 1 < argc) {
            searchDirs.clear();
            while (++i < argc && argv[i][0] != '-') searchDirs.push_back(argv[i]);
            i--;
        } else input = arg;
    }

    // Make paths absolute if relative
    auto makeAbs = [&](std::string& p) {
        if (!p.empty() && p[0] != '/') p = std::string("../../../") + p;
    };
    for (auto& d : searchDirs) makeAbs(d);
    makeAbs(input);
    fs::path inPath(input);

    MapData mapData;
    if (!MapParser::Parse(input, mapData)) {
        std::cerr << "FAIL: cannot parse " << input << std::endl;
        return 1;
    }

    fs::create_directories(outDir);
    std::string mapName = inPath.stem().string();

    // Load heightfield
    HeightField hf;
    std::string hflPath = FindFile(mapData.heightfield_file, searchDirs);
    bool hasHfl = false;
    if (!hflPath.empty()) {
        if (HflParser::Load(hflPath, hf)) {
            HflParser::WriteHGT(outDir + "/" + mapName + ".hgt", hf);
            HflParser::WriteOBJ(outDir + "/" + mapName + "_mesh.obj", hf, 0.01f);
            hasHfl = true;
            std::cout << "  Heightfield: " << hf.width << "x" << hf.height
                      << " range=[" << hf.hmin << "," << hf.hmax << "]" << std::endl;
        } else {
            std::cout << "  Heightfield: could not parse " << mapData.heightfield_file << std::endl;
        }
    } else {
        std::cout << "  Heightfield: " << mapData.heightfield_file << " not found" << std::endl;
    }

    // Write scene JSON
    WriteSceneJSON(outDir + "/" + mapName + ".json", mapData, hasHfl ? &hf : nullptr);

    std::cout << "  Objects: " << mapData.objects.size() << std::endl;
    std::cout << "  Lights: " << mapData.lights.size() << std::endl;
    std::cout << "  Output: " << outDir << "/" << mapName << ".json" << std::endl;
    if (hasHfl) {
        std::cout << "          " << outDir << "/" << mapName << ".hgt" << std::endl;
        std::cout << "          " << outDir << "/" << mapName << "_mesh.obj" << std::endl;
    }
    std::cout << "  OK" << std::endl;
    return 0;
}
