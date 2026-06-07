#include "ModLoader.h"
#include "GltfWriter.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

struct ChxManifest {
    std::vector<std::string> modFiles;
    std::vector<std::string> animFiles;
};

static bool ParseChx(const std::string& path, ChxManifest& out) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == '\t' || line.back() == ' '))
            line.pop_back();
        if (line.empty()) continue;
        if (line.rfind("*MOD_FILE_NAME", 0) == 0) {
            auto pos = line.find('\t');
            if (pos == std::string::npos) pos = line.find(' ');
            if (pos != std::string::npos) {
                auto name = line.substr(pos + 1);
                while (!name.empty() && (name.front() == '\t' || name.front() == ' ')) name.erase(name.begin());
                while (!name.empty() && (name.back() == '\t' || name.back() == ' ')) name.pop_back();
                out.modFiles.push_back(name);
            }
        } else if (line.rfind("*MOTION_NUM", 0) == 0) {
            // ANM files follow as plain lines
        } else if (line[0] != '*') {
            while (!line.empty() && (line.front() == '\t' || line.front() == ' ')) line.erase(line.begin());
            if (!line.empty() && line != "NULL.ANM" && line != "NULL.ANM\r") {
                while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == '\t' || line.back() == ' '))
                    line.pop_back();
                if (!line.empty()) out.animFiles.push_back(line);
            }
        }
    }
    return !out.modFiles.empty();
}

static std::string FindModFile(const std::string& modName, const std::vector<std::string>& searchDirs) {
    // Search directories for the .mod file, case-insensitive on macOS
    for (auto& dir : searchDirs) {
        if (!fs::exists(dir)) continue;
        // First try exact match
        fs::path exact = fs::path(dir) / modName;
        if (fs::exists(exact)) return exact.string();
        // Case-insensitive fallback
        std::string lower = modName;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        for (auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            std::string fn = entry.path().filename().string();
            std::string fnLower = fn;
            std::transform(fnLower.begin(), fnLower.end(), fnLower.begin(), ::tolower);
            if (fnLower == lower) return entry.path().string();
        }
    }
    return {};
}

static int ConvertOne(const std::string& chxPath, const std::string& outDir,
                       const std::vector<std::string>& searchDirs, float scale) {
    ChxManifest manifest;
    if (!ParseChx(chxPath, manifest)) {
        std::cerr << "  FAIL: cannot parse " << chxPath << std::endl;
        return 1;
    }

    ModMesh mesh;
    for (auto& modName : manifest.modFiles) {
        std::string modPath = FindModFile(modName, searchDirs);
        if (modPath.empty()) {
            std::cerr << "  FAIL: mod not found: " << modName << std::endl;
            return 1;
        }
        if (!ModLoader::Load(modPath, mesh)) {
            std::cerr << "  FAIL: cannot load mod: " << modPath << std::endl;
            return 1;
        }
    }

    if (mesh.positions.empty() || mesh.indices.empty()) {
        std::cerr << "  FAIL: no geometry in mod" << std::endl;
        return 1;
    }

    fs::path stem = fs::path(chxPath).stem();
    std::string glbPath = (fs::path(outDir) / stem).string() + ".glb";
    if (!GltfWriter::WriteGLB(glbPath, mesh, scale)) {
        std::cerr << "  FAIL: cannot write glb" << std::endl;
        return 1;
    }

    std::cout << "  OK: " << stem.string()
              << " (" << mesh.positions.size() << "v "
              << mesh.indices.size() / 3 << "t) -> " << glbPath << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    std::string input;
    std::string outDir = "assets_converted/glb";
    float scale = 0.03f;

    std::vector<std::string> searchDirs = {
        "assets/unpacked/npc",
        "assets/unpacked/character",
        "assets/unpacked/monster",
        "assets/unpacked/farm",
    };

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) outDir = argv[++i];
        else if (arg == "-s" && i + 1 < argc) scale = std::stof(argv[++i]);
        else if ((arg == "-d" || arg == "--dirs") && i + 1 < argc) {
            searchDirs.clear();
            i++;
            while (i < argc && argv[i][0] != '-') searchDirs.push_back(argv[i++]);
            i--;
        } else input = arg;
    }

    fs::create_directories(outDir);
    // Also make search dirs relative to project root if not absolute
    auto makeAbs = [](std::string& d) {
        if (!d.empty() && d[0] != '/') d = "../../" + d;
    };
    for (auto& d : searchDirs) makeAbs(d);

    if (input.empty()) {
        std::cerr << "Usage: chx_to_gltf <input.chx|input_dir> [-o out_dir] [-s scale] [-d dir1 dir2 ...]" << std::endl;
        std::cerr << "\nConverts .chx/.mod files to glTF 2.0 Binary (.glb)." << std::endl;
        std::cerr << "Default search dirs: ";
        for (auto& d : searchDirs) std::cerr << d << " ";
        std::cerr << std::endl;
        return 1;
    }

    fs::path inPath(input);
    int total = 0, ok = 0;

    if (fs::is_regular_file(inPath)) {
        total = 1;
        ok = (ConvertOne(inPath.string(), outDir, searchDirs, scale) == 0) ? 1 : 0;
    } else if (fs::is_directory(inPath)) {
        std::vector<fs::path> chxFiles;
        for (auto& entry : fs::recursive_directory_iterator(inPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".chx") chxFiles.push_back(entry.path());
            }
        }
        std::sort(chxFiles.begin(), chxFiles.end());
        std::cout << "Found " << chxFiles.size() << " .chx files in " << inPath.string() << std::endl;
        total = static_cast<int>(chxFiles.size());
        for (auto& chx : chxFiles) {
            if (ConvertOne(chx.string(), outDir, searchDirs, scale) == 0) ok++;
        }
    } else {
        std::cerr << "Error: input not found: " << input << std::endl;
        return 1;
    }

    std::cout << "\nResult: " << ok << "/" << total << " OK" << std::endl;
    return (ok == total) ? 0 : 1;
}
