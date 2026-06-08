#include "ScreenshotCapture.hpp"
#include <bgfx/bgfx.h>
#include <spdlog/spdlog.h>
#include <filesystem>

static std::function<void(const std::string&, bool)> s_completion;
static std::string s_pending_path;
static std::string s_pending_tga;
static bool s_waiting = false;

void ScreenshotCapture::SetCompletionHandler(std::function<void(const std::string&, bool)> cb) {
    s_completion = std::move(cb);
}

bool ScreenshotCapture::Request(const std::string& path) {
    if (s_waiting) return false;
    namespace fs = std::filesystem;
    fs::path p(path);
    if (p.has_parent_path()) fs::create_directories(p.parent_path());
    s_pending_path = path;
    s_pending_tga = path + ".tga";
    s_waiting = true;
    bgfx::requestScreenShot(BGFX_INVALID_HANDLE, path.c_str());
    spdlog::info("ScreenshotCapture: requested {} (bgfx writes {})", path, s_pending_tga);
    return true;
}

void ScreenshotCapture::Poll() {
    if (!s_waiting) return;
    namespace fs = std::filesystem;
    if (fs::exists(s_pending_tga)) {
        if (s_completion) s_completion(s_pending_tga, true);
        s_waiting = false;
        s_pending_path.clear();
        s_pending_tga.clear();
    } else if (fs::exists(s_pending_path)) {
        if (s_completion) s_completion(s_pending_path, true);
        s_waiting = false;
        s_pending_path.clear();
        s_pending_tga.clear();
    }
}
