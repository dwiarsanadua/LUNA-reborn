#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <ctime>
#include <map>

struct TelemetryEvent {
    time_t timestamp;
    std::string category;
    std::string name;
    std::string value;
};

class TelemetrySystem {
public:
    void Init();
    void Update(float dt);
    
    // Log events
    void LogEvent(const std::string& category, const std::string& name, const std::string& value);
    void LogPlayerAction(const std::string& action);
    void LogError(const std::string& source, const std::string& message);
    
    // Performance tracking
    void RecordFPS(float fps);
    void RecordFrameTime(float ms);
    void RecordMemoryUsage(size_t bytes);
    
    // Queries
    std::vector<TelemetryEvent> GetRecentEvents(int count = 50) const;
    std::vector<TelemetryEvent> GetErrors(int count = 20) const;
    float GetAverageFPS() const { return total_frames_ > 0 ? total_fps_sum_ / total_frames_ : 0; }
    float GetAverageFrameTime() const { return total_frames_ > 0 ? total_frame_time_sum_ / total_frames_ : 0; }
    
    // Error report callback
    using ErrorCallback = std::function<void(const std::string& source, const std::string& msg)>;
    void SetErrorCallback(ErrorCallback cb) { error_cb_ = cb; }
    
    int GetEventCount() const { return (int)events_.size(); }
    int GetErrorCount() const { return (int)errors_.size(); }

private:
    std::vector<TelemetryEvent> events_;
    std::vector<TelemetryEvent> errors_;
    float total_fps_sum_ = 0;
    float total_frame_time_sum_ = 0;
    int total_frames_ = 0;
    float flush_timer_ = 0;
    ErrorCallback error_cb_;
    static constexpr int MAX_EVENTS = 1000;
    static constexpr int MAX_ERRORS = 200;
    void Flush();
};
