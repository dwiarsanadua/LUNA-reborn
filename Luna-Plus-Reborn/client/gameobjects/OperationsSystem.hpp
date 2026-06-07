#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <ctime>

struct ServerAnnouncement {
    std::string message;
    time_t start_time = 0;
    time_t end_time = 0;
    int interval_sec = 0;
    bool active = true;
};

class OperationsSystem {
public:
    void Init();
    void Update(float dt);
    
    // Announcements
    void AddAnnouncement(const std::string& msg, int interval_sec = 0);
    void RemoveAnnouncement(int index);
    bool HasActiveAnnouncement() const { return current_announcement_ >= 0; }
    std::string GetCurrentAnnouncement() const;
    
    // Maintenance mode
    void SetMaintenance(bool m) { maintenance_mode_ = m; maintenance_timer_ = 0; }
    bool IsMaintenance() const { return maintenance_mode_; }
    void SetMaintenanceEndTime(time_t t) { maintenance_end_ = t; }
    int GetMaintenanceRemaining() const;
    
    // Ban
    void BanPlayer(uint32_t player_id, const std::string& reason);
    bool IsBanned(uint32_t player_id) const;
    
    // Callbacks
    using AnnouncementCallback = std::function<void(const std::string&)>;
    void SetAnnouncementCallback(AnnouncementCallback cb) { announce_cb_ = cb; }

private:
    std::vector<ServerAnnouncement> announcements_;
    int current_announcement_ = -1;
    float announce_timer_ = 0;
    bool maintenance_mode_ = false;
    float maintenance_timer_ = 0;
    time_t maintenance_end_ = 0;
    std::vector<std::pair<uint32_t, std::string>> bans_;
    AnnouncementCallback announce_cb_;
};
