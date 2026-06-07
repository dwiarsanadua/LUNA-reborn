#include "TelemetrySystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void TelemetrySystem::Init() {
    events_.clear();
    errors_.clear();
    total_fps_sum_ = 0;
    total_frame_time_sum_ = 0;
    total_frames_ = 0;
    spdlog::info("TelemetrySystem: initialized");
}

void TelemetrySystem::Update(float dt) {
    flush_timer_ += dt;
    if (flush_timer_ >= 300.0f) { // flush every 5 min
        flush_timer_ = 0;
        Flush();
    }
}

void TelemetrySystem::LogEvent(const std::string& category, const std::string& name, const std::string& value) {
    TelemetryEvent e;
    e.timestamp = time(nullptr);
    e.category = category;
    e.name = name;
    e.value = value;
    events_.push_back(e);
    if ((int)events_.size() > MAX_EVENTS) events_.erase(events_.begin());
}

void TelemetrySystem::LogPlayerAction(const std::string& action) {
    LogEvent("PlayerAction", action, "");
}

void TelemetrySystem::LogError(const std::string& source, const std::string& message) {
    TelemetryEvent e;
    e.timestamp = time(nullptr);
    e.category = "Error";
    e.name = source;
    e.value = message;
    errors_.push_back(e);
    if ((int)errors_.size() > MAX_ERRORS) errors_.erase(errors_.begin());
    spdlog::error("[Telemetry] {}: {}", source, message);
    if (error_cb_) error_cb_(source, message);
}

void TelemetrySystem::RecordFPS(float fps) {
    total_fps_sum_ += fps;
    total_frames_++;
}

void TelemetrySystem::RecordFrameTime(float ms) {
    total_frame_time_sum_ += ms;
}

std::vector<TelemetryEvent> TelemetrySystem::GetRecentEvents(int count) const {
    std::vector<TelemetryEvent> result;
    int start = std::max(0, (int)events_.size() - count);
    for (int i = start; i < (int)events_.size(); i++) result.push_back(events_[i]);
    return result;
}

std::vector<TelemetryEvent> TelemetrySystem::GetErrors(int count) const {
    std::vector<TelemetryEvent> result;
    int start = std::max(0, (int)errors_.size() - count);
    for (int i = start; i < (int)errors_.size(); i++) result.push_back(errors_[i]);
    return result;
}

void TelemetrySystem::Flush() {
    if (!events_.empty()) {
        spdlog::info("Telemetry: flushed {} events, {} errors", events_.size(), errors_.size());
    }
}
