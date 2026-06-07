#include "PatchSystem.hpp"
#include <spdlog/spdlog.h>

void PatchSystem::Init() {
    current_ = {1, 1, 0};
    latest_ = {1, 2, 0};
    update_available_ = current_ < latest_;
    patch_notes_ = "v1.2.0 Changelog:\n"
                   "- New dungeons: Shadow Realm, Dragon's Lair\n"
                   "- Cash Shop with Luna Points\n"
                   "- Battle Pass Season 1\n"
                   "- Family & Couple system improvements\n"
                   "- Bug fixes and performance optimizations";
    spdlog::info("PatchSystem: v{} (latest: v{}, update: {})",
                 current_.ToString(), latest_.ToString(), update_available_);
}

void PatchSystem::SetCurrentVersion(int major, int minor, int patch) {
    current_ = {major, minor, patch};
    update_available_ = current_ < latest_;
}

bool PatchSystem::CheckForUpdate(const VersionInfo& latest) {
    latest_ = latest;
    update_available_ = current_ < latest_;
    return update_available_;
}

void PatchSystem::StartDownload() {
    downloading_ = true;
    download_progress_ = 0;
    spdlog::info("PatchSystem: downloading update v{}", latest_.ToString());
    if (update_cb_) update_cb_("Downloading...", 0);
}

void PatchSystem::UpdateDownload(float dt) {
    if (!downloading_) return;
    download_progress_ += download_speed_ * dt;
    if (download_progress_ >= 1.0f) {
        download_progress_ = 1.0f;
        downloading_ = false;
        ApplyUpdate();
    }
    if (update_cb_) update_cb_("Downloading...", download_progress_);
}

void PatchSystem::ApplyUpdate() {
    current_ = latest_;
    update_available_ = false;
    spdlog::info("PatchSystem: updated to v{}", current_.ToString());
    if (update_cb_) update_cb_("Update applied!", 1.0f);
}

std::string PatchSystem::GetChangelog() const {
    return patch_notes_;
}
