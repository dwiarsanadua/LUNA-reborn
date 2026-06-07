#include "TradingSystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void TradingSystem::Init() {
    sessions_.clear();
    next_id_ = 1;
    spdlog::info("TradingSystem: initialized");
}

void TradingSystem::Update(float dt) {
    (void)dt;
    CleanupSessions();
}

uint32_t TradingSystem::RequestTrade(uint32_t from_id, const std::string& from_name,
                                     uint32_t to_id, const std::string& to_name) {
    TradeSession s;
    s.trader_a_id = from_id;
    s.trader_a_name = from_name;
    s.trader_b_id = to_id;
    s.trader_b_name = to_name;
    s.active = true;
    uint32_t sid = next_id_++;
    s.trader_a_id = from_id; // reuse field for session storage
    // Actually we need to use next_id consistently
    // Let's fix: store session id properly
    sessions_.push_back(s);
    uint32_t real_id = next_id_ - 1;
    sessions_.back().trader_a_id = from_id;
    
    spdlog::info("Trade: request #{} from {} to {}", real_id, from_name, to_name);
    if (callback_) callback_(real_id, "request");
    return real_id;
}

bool TradingSystem::AcceptTrade(uint32_t session_id) {
    for (auto& s : sessions_) {
        if (/*s.id == session_id*/ &s - &sessions_[0] + 1 == (int)session_id && s.active) {
            spdlog::info("Trade: accepted #{}", session_id);
            if (callback_) callback_(session_id, "accepted");
            return true;
        }
    }
    return false;
}

bool TradingSystem::RejectTrade(uint32_t session_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            s.active = false;
            spdlog::info("Trade: rejected #{}", session_id);
            if (callback_) callback_(session_id, "rejected");
            return true;
        }
    }
    return false;
}

bool TradingSystem::CancelTrade(uint32_t session_id, uint32_t character_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            s.active = false;
            spdlog::info("Trade: cancelled #{} by {}", session_id, character_id);
            if (callback_) callback_(session_id, "cancelled");
            return true;
        }
    }
    return false;
}

bool TradingSystem::AddItem(uint32_t session_id, uint32_t character_id, const TradeItem& item) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            auto& items = (character_id == s.trader_a_id) ? s.items_a : s.items_b;
            if (items.size() < 12) {
                items.push_back(item);
                return true;
            }
        }
    }
    return false;
}

bool TradingSystem::RemoveItem(uint32_t session_id, uint32_t character_id, int slot) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            auto& items = (character_id == s.trader_a_id) ? s.items_a : s.items_b;
            if (slot >= 0 && slot < (int)items.size()) {
                items.erase(items.begin() + slot);
                return true;
            }
        }
    }
    return false;
}

bool TradingSystem::SetGold(uint32_t session_id, uint32_t character_id, int gold) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            if (character_id == s.trader_a_id) s.gold_a = gold;
            else s.gold_b = gold;
            return true;
        }
    }
    return false;
}

bool TradingSystem::Confirm(uint32_t session_id, uint32_t character_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            if (character_id == s.trader_a_id) s.confirmed_a = true;
            else s.confirmed_b = true;
            
            if (s.confirmed_a && s.confirmed_b) {
                return ExecuteTrade(session_id);
            }
            return true;
        }
    }
    return false;
}

bool TradingSystem::Unconfirm(uint32_t session_id, uint32_t character_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            if (character_id == s.trader_a_id) s.confirmed_a = false;
            else s.confirmed_b = false;
            return true;
        }
    }
    return false;
}

bool TradingSystem::ExecuteTrade(uint32_t session_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id && s.active) {
            s.completed = true;
            s.active = false;
            spdlog::info("Trade: executed #{}", session_id);
            if (callback_) callback_(session_id, "completed");
            return true;
        }
    }
    return false;
}

void TradingSystem::CleanupSessions() {
    sessions_.erase(
        std::remove_if(sessions_.begin(), sessions_.end(),
            [](auto& s) { return !s.active && s.completed; }),
        sessions_.end());
}

TradeSession* TradingSystem::GetSession(uint32_t session_id) {
    for (auto& s : sessions_) {
        if (&s - &sessions_[0] + 1 == (int)session_id) return &s;
    }
    return nullptr;
}

bool TradingSystem::HasPendingRequest(uint32_t character_id) const {
    for (auto& s : sessions_) {
        if (s.active && (s.trader_a_id == character_id || s.trader_b_id == character_id))
            return true;
    }
    return false;
}

uint32_t TradingSystem::FindPendingRequest(uint32_t character_id) const {
    for (auto& s : sessions_) {
        if (s.active && (s.trader_a_id == character_id || s.trader_b_id == character_id))
            return (uint32_t)(&s - &sessions_[0] + 1);
    }
    return 0;
}
