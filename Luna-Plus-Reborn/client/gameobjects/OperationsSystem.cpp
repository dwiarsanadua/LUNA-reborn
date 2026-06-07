#include "OperationsSystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void OperationsSystem::Init() {
    announcements_.clear();
    bans_.clear();
    current_announcement_ = -1;
    maintenance_mode_ = false;
    spdlog::info("OperationsSystem: initialized");
}

void OperationsSystem::Update(float dt) {
    // Rotate announcements
    if (!announcements_.empty()) {
        announce_timer_ += dt;
        auto& ann = announcements_[current_announcement_ >= 0 ? current_announcement_ : 0];
        int interval = ann.interval_sec > 0 ? ann.interval_sec : 60;
        if (announce_timer_ >= interval) {
            announce_timer_ = 0;
            current_announcement_ = (current_announcement_ + 1) % (int)announcements_.size();
            if (announce_cb_ && current_announcement_ >= 0)
                announce_cb_(announcements_[current_announcement_].message);
        }
    }
    
    // Maintenance timer
    if (maintenance_mode_) {
        maintenance_timer_ += dt;
    }
}

void OperationsSystem::AddAnnouncement(const std::string& msg, int interval_sec) {
    ServerAnnouncement a;
    a.message = msg;
    a.interval_sec = interval_sec > 0 ? interval_sec : 60;
    a.start_time = time(nullptr);
    announcements_.push_back(a);
    if (current_announcement_ < 0) current_announcement_ = 0;
}

void OperationsSystem::RemoveAnnouncement(int index) {
    if (index >= 0 && index < (int)announcements_.size()) {
        announcements_.erase(announcements_.begin() + index);
        if (current_announcement_ >= (int)announcements_.size())
            current_announcement_ = (int)announcements_.size() - 1;
    }
}

std::string OperationsSystem::GetCurrentAnnouncement() const {
    if (current_announcement_ >= 0 && current_announcement_ < (int)announcements_.size())
        return announcements_[current_announcement_].message;
    return "";
}

int OperationsSystem::GetMaintenanceRemaining() const {
    if (!maintenance_mode_) return 0;
    return (int)std::max(0.0, difftime(maintenance_end_, time(nullptr)));
}

void OperationsSystem::BanPlayer(uint32_t player_id, const std::string& reason) {
    bans_.push_back({player_id, reason});
    spdlog::info("Operations: banned player {}: {}", player_id, reason);
}

bool OperationsSystem::IsBanned(uint32_t player_id) const {
    for (auto& b : bans_) if (b.first == player_id) return true;
    return false;
}
