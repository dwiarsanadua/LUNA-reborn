#include "test_harness.hpp"
#include <cstdio>
#include <fstream>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

static bool FileExists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) { fclose(f); return true; }
    return false;
}
static long FileSize(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}
void RunVisualTests() {
    TEST_SUITE("VISUAL: RESOURCE LOADING");
    TEST_STEP("Texture PNG");
    {
        const char* p = "assets/textures/1.png";
        bool ok = FileExists(p);
        TEST("Texture exists", ok);
        if (ok) {
            long sz = FileSize(p);
            TEST("Texture non-empty", sz > 100);
            spdlog::info("  Texture: {} - {} bytes", p, sz);
            FILE* f = fopen(p, "rb");
            if (f) {
                unsigned char h[8];
                size_t r = fread(h, 1, 8, f);
                bool v = (r == 8 && h[0]==0x89 && h[1]=='P' && h[2]=='N' && h[3]=='G');
                TEST("Valid PNG signature", v);
                fclose(f);
            }
        }
    }
    TEST_STEP("GLB model");
    {
        const char* p = "assets/models/00_alke_rock01.glb";
        bool ok = FileExists(p);
        TEST("GLB exists", ok);
        if (ok) { long sz = FileSize(p); TEST("GLB non-empty", sz > 100); spdlog::info("  Model: {} - {} bytes", p, sz); }
    }
    TEST_STEP("Animation JSON");
    {
        const char* p = "assets/animations/01_bird01.anm.json";
        std::ifstream f(p);
        TEST("Animation opens", f.is_open());
        if (f.is_open()) {
            try { nlohmann::json j; f >> j; TEST("JSON valid", !j.is_null()); if(j.contains("frames")) spdlog::info("  Frames: {}", j["frames"].size()); if(j.contains("duration")) spdlog::info("  Duration: {}", j["duration"].get<double>()); } catch(...){ TEST("JSON parse", false); } f.close();
        }
    }
    TEST_STEP("Font TTF");
    {
        const char* p = "assets/fonts/2002_EYA.ttf";
        bool ok = FileExists(p);
        TEST("Font exists", ok);
        if (ok) { long sz = FileSize(p); TEST("Font > 10KB", sz > 10240); spdlog::info("  Font: {} - {} bytes", p, sz); }
    }
    TEST_STEP("Shader binary");
    {
        const char* vp = "shaders/vs_skinned_preview.bin";
        const char* fp = "shaders/fs_preview.bin";
        TEST("VS exists", FileExists(vp)); TEST("FS exists", FileExists(fp));
        if (FileExists(vp)) spdlog::info("  VS: {} bytes", FileSize(vp));
        if (FileExists(fp)) spdlog::info("  FS: {} bytes", FileSize(fp));
    }
}
