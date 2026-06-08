#pragma once
#include <rendering/UIRenderer.hpp>
#include <string>
#include <unordered_map>

struct UiAtlasEntry {
    int id = -1;
    std::string filename;
    std::string resolved_path;
    int width = 1024;
    int height = 1024;
};

class UiAtlasRegistry {
public:
    static void Init(const std::string& image_path_file = "assets/interface/Windows/image_path.bin.txt");
    static void Shutdown();

    static bool IsReady();
    static const UiAtlasEntry* GetEntry(int atlas_id);
    static TextureInfo LoadAtlasTexture(UIRenderer& ui, int atlas_id);
    static void PreloadAtlases(UIRenderer& ui, const int* ids, size_t count);
    static void PreloadAtlases(UIRenderer& ui, const std::unordered_map<int, int>& usage);

    static float AtlasWidth(int atlas_id);
    static float AtlasHeight(int atlas_id);

private:
    static std::unordered_map<int, UiAtlasEntry> entries_;
    static std::unordered_map<int, TextureInfo> loaded_;
    static bool ready_;
};
