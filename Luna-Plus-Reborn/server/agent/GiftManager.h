// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>

struct EventInfo {
    uint32_t index;
    uint32_t delay_sec;
    uint32_t next_index;
    uint32_t item_id;
    uint32_t item_count;
};

class GiftEvent {
public:
    GiftEvent();
    ~GiftEvent();

    bool Init(uint32_t character_id, uint32_t event_id, const EventInfo* info);
    bool Execute();
    uint32_t GetID() const { return id_; }
    uint32_t GetCharacterID() const { return character_id_; }
    void Release();

private:
    uint32_t id_ = 0;
    uint32_t character_id_ = 0;
    const EventInfo* info_ = nullptr;
    uint64_t delay_ms_ = 0;
    uint64_t check_time_ms_ = 0;
};

class GiftManager {
public:
    GiftManager();
    ~GiftManager();

    bool Init(const std::string& config_path);
    void Release();
    void AddEvent(uint32_t character_id, uint32_t event_index);
    void RemoveEvent(uint32_t event_id);
    void ExecuteEvent(uint32_t character_id, uint32_t event_index);
    void Process();

    uint32_t GetStartEventCount() const { return start_event_count_; }
    const uint32_t* GetStartEventIndices() const { return start_event_indices_.data(); }

private:
    std::unordered_map<uint32_t, EventInfo*> event_info_table_;
    std::unordered_map<uint32_t, GiftEvent*> gift_event_table_;
    uint32_t next_event_id_ = 10000;
    uint32_t start_event_count_ = 0;
    std::vector<uint32_t> start_event_indices_;
};
