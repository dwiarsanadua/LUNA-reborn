#include "UiCaptureMode.hpp"
#include "WindowManager.hpp"
#include "skin/UiSkinManager.hpp"
#include <util/ScreenshotCapture.hpp>
#include <engine/gx_render/RenderDevice.h>
#include <rendering/UIRenderer.hpp>
#include <engine/gx_render/VFS.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

static std::vector<std::string> LoadManifest(const std::string& manifest_path) {
    std::vector<std::string> layouts;
    std::string resolved = VFS::Find(manifest_path);
    if (resolved.empty()) resolved = manifest_path;

    std::ifstream in(resolved);
    if (in.is_open()) {
        try {
            nlohmann::json j; in >> j;
            if (j.contains("layouts") && j["layouts"].is_array()) {
                for (const auto& item : j["layouts"]) {
                    if (item.is_string()) layouts.push_back(item.get<std::string>());
                    else if (item.is_object() && item.contains("name"))
                        layouts.push_back(item["name"].get<std::string>());
                }
            }
        } catch (const std::exception& e) {
            spdlog::warn("UiCaptureMode: manifest parse failed: {}", e.what());
        }
    }

    if (layouts.empty()) {
        const char* defaults[] = {
            "Inven", "CharInfo", "Skill", "Quest", "PartySet", "Guild", "Friend",
            "Storage", "Trade", "Consignment", "Option", "MiniMap", "NpcImage",
            "CharGage", "HelpDlg", "KeySetting", "Note", "CookDlg", "PetInfo"
        };
        for (const char* n : defaults) layouts.push_back(n);
    }
    return layouts;
}

int RunUiCaptureMode(RenderDevice& device, UIRenderer& ui, const std::string& output_dir,
                     const std::string& manifest_path) {
    fs::create_directories(output_dir);
    std::string manifest = manifest_path.empty()
        ? VFS::Find("tools/ui_regression/capture_manifest.json")
        : manifest_path;
    if (manifest.empty()) manifest = "tools/ui_regression/capture_manifest.json";

    auto layouts = LoadManifest(manifest);
    WindowManager wm;
    int captured = 0;
    int failed = 0;
    std::string pending_tga;

    auto wait_for_file = [&](const std::string& tga_path, int seconds) -> bool {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
        while (std::chrono::steady_clock::now() < deadline) {
            device.BeginFrame();
            ui.BeginFrame();
            ui.DrawRect(0, 0, ui.width, ui.height, {24, 24, 32, 255});
            wm.Render(ui);
            ScreenshotCapture::Poll();
            device.EndFrame();
            if (fs::exists(tga_path)) return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        return fs::exists(tga_path);
    };

    spdlog::info("UiCaptureMode: capturing {} layouts to {}", layouts.size(), output_dir);

    for (const std::string& name : layouts) {
        if (!pending_tga.empty()) {
            if (wait_for_file(pending_tga, 8)) {
                spdlog::info("UiCaptureMode: saved {}", pending_tga);
                ++captured;
            } else {
                spdlog::warn("UiCaptureMode: timeout for {}", pending_tga);
                ++failed;
            }
            pending_tga.clear();
        }
        wm.CloseAll();
        std::string script = "assets/interface/Windows/" + name + ".bin.txt";
        Window* win = wm.LoadFromScript(script);
        if (!win) {
            spdlog::warn("UiCaptureMode: skip {} (script load failed)", name);
            ++failed;
            continue;
        }
        win->SetVisible(true);

        for (int i = 0; i < 3; ++i) {
            device.BeginFrame();
            ui.BeginFrame();
            ui.DrawRect(0, 0, ui.width, ui.height, {24, 24, 32, 255});
            wm.Render(ui);
            device.EndFrame();
        }

        std::string out_base = (fs::path(output_dir) / name).string();
        std::string out_png = out_base + ".png";
        std::string out_tga = out_png + ".tga";
        if (!ScreenshotCapture::Request(out_png)) {
            ++failed;
            continue;
        }
        pending_tga = out_tga;

        for (int i = 0; i < 3; ++i) {
            device.BeginFrame();
            ui.BeginFrame();
            ui.DrawRect(0, 0, ui.width, ui.height, {24, 24, 32, 255});
            wm.Render(ui);
            device.EndFrame();
        }
    }

    if (!pending_tga.empty()) {
        if (wait_for_file(pending_tga, 10)) {
            spdlog::info("UiCaptureMode: saved {}", pending_tga);
            ++captured;
        } else {
            spdlog::warn("UiCaptureMode: timeout for {}", pending_tga);
            ++failed;
        }
    }

    spdlog::info("UiCaptureMode: done ({} captured, {} failed)", captured, failed);
    return failed > 0 ? 1 : 0;
}
