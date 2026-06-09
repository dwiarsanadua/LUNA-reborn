#include "GiftManager.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <algorithm>

static uint64_t NowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

GiftEvent::GiftEvent() { Release(); }
GiftEvent::~GiftEvent() { Release(); }

bool GiftEvent::Init(uint32_t character_id, uint32_t event_id, const EventInfo* info) {
    if (!info) {
        spdlog::error("GiftEvent::Init: null EventInfo for character={} event={}", character_id, event_id);
        return false;
    }
    character_id_ = character_id;
    id_ = event_id;
    info_ = info;
    delay_ms_ = static_cast<uint64_t>(info->delay_sec) * 1000;
    check_time_ms_ = NowMs();

    spdlog::info("GiftEvent::Init: character={} event={} delay={}s item={} count={}",
                 character_id, event_id, info->delay_sec, info->item_id, info->item_count);
    return true;
}

bool GiftEvent::Execute() {
    auto now = NowMs();
    if (now - check_time_ms_ < delay_ms_) {
        delay_ms_ -= (now - check_time_ms_);
        check_time_ms_ = now;
        return false;
    }

    spdlog::info("GiftEvent::Execute: character={} event={} item={} count={}",
                 character_id_, info_->index, info_->item_id, info_->item_count);
    // In the old code, this sends MP_ITEM_GIFT_EVENT_NOTIFY to the map server
    // which triggers GiftItemInsert to give the item to the character.
    // Reborn would implement similar cross-server messaging.

    if (info_->next_index > 0) {
        // Chain to next event if configured
        // GIFTMGR->AddEvent(character_id_, info_->next_index);
    }
    return true;
}

void GiftEvent::Release() {
    id_ = 0;
    character_id_ = 0;
    info_ = nullptr;
    delay_ms_ = 0;
    check_time_ms_ = 0;
}

GiftManager::GiftManager() = default;
GiftManager::~GiftManager() { Release(); }

bool GiftManager::Init(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        spdlog::warn("GiftManager::Init: cannot open config file '{}'", config_path);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '#') continue;

        if (line.find("#StartEventCount") == 0) {
            std::istringstream iss(line.substr(16));
            iss >> start_event_count_;
            start_event_indices_.reserve(start_event_count_);
        } else if (line.find("#StartEventIndex") == 0) {
            std::istringstream iss(line.substr(15));
            for (uint32_t i = 0; i < start_event_count_; i++) {
                uint32_t idx = 0;
                if (!(iss >> idx)) break;
                start_event_indices_.push_back(idx);
            }
        } else if (line.find("#EventInfo") == 0) {
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') break;
                std::istringstream ev(line);
                auto info = std::make_unique<EventInfo>();
                if (!(ev >> info->index >> info->delay_sec >> info->next_index
                           >> info->item_id >> info->item_count)) {
                    break;
                }
                spdlog::info("GiftManager: loaded event idx={} delay={} next={} item={} count={}",
                             info->index, info->delay_sec, info->next_index,
                             info->item_id, info->item_count);
                event_info_table_[info->index] = info.release();
            }
        }
    }

    spdlog::info("GiftManager::Init: loaded {} event templates, {} start events",
                 event_info_table_.size(), start_event_indices_.size());
    return true;
}

void GiftManager::Release() {
    for (auto& [key, info] : event_info_table_) {
        delete info;
    }
    event_info_table_.clear();

    for (auto& [key, event] : gift_event_table_) {
        delete event;
    }
    gift_event_table_.clear();

    start_event_indices_.clear();
    start_event_count_ = 0;
}

void GiftManager::AddEvent(uint32_t character_id, uint32_t event_index) {
    auto it = event_info_table_.find(event_index);
    if (it == event_info_table_.end()) {
        spdlog::error("GiftManager::AddEvent: unknown event index={} for character={}",
                      event_index, character_id);
        return;
    }

    auto event = new GiftEvent();
    uint32_t id = next_event_id_++;
    if (event->Init(character_id, id, it->second)) {
        gift_event_table_[id] = event;
        spdlog::info("GiftManager::AddEvent: character={} event_idx={} -> event_id={}",
                     character_id, event_index, id);
    } else {
        delete event;
    }
}

void GiftManager::RemoveEvent(uint32_t event_id) {
    auto it = gift_event_table_.find(event_id);
    if (it != gift_event_table_.end()) {
        delete it->second;
        gift_event_table_.erase(it);
    }
}

void GiftManager::ExecuteEvent(uint32_t character_id, uint32_t event_index) {
    spdlog::info("GiftManager::ExecuteEvent: character={} event_idx={}", character_id, event_index);

    auto it = event_info_table_.find(event_index);
    if (it == event_info_table_.end()) {
        spdlog::error("GiftManager::ExecuteEvent: unknown event index={}", event_index);
        return;
    }

    // In the old code, this sends MP_ITEM_GIFT_EVENT_NOTIFY
    // with (character_id, event_index, item_id, item_count) to the map server.
    spdlog::info("GiftManager::ExecuteEvent: would send gift item={} count={} to character={}",
                 it->second->item_id, it->second->item_count, character_id);
}

void GiftManager::Process() {
    auto now = NowMs();
    std::vector<uint32_t> completed;

    for (auto& [id, event] : gift_event_table_) {
        if (event->Execute()) {
            completed.push_back(id);
        }
    }

    for (auto id : completed) {
        spdlog::info("GiftManager::Process: event {} completed", id);
        RemoveEvent(id);
    }
}
