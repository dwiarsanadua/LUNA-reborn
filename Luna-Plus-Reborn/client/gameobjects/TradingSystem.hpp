#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

struct TradeItem {
    uint32_t id;
    std::string name;
    int count;
    int enchant;
    int inventory_slot;
};

struct TradeSession {
    uint32_t trader_a_id;
    uint32_t trader_b_id;
    std::string trader_a_name;
    std::string trader_b_name;
    
    // A offers to B
    std::vector<TradeItem> items_a;
    int gold_a = 0;
    bool confirmed_a = false;
    
    // B offers to A
    std::vector<TradeItem> items_b;
    int gold_b = 0;
    bool confirmed_b = false;
    
    bool active = true;
    bool completed = false;
};

class TradingSystem {
public:
    void Init();
    void Update(float dt);
    
    // Trade lifecycle
    uint32_t RequestTrade(uint32_t from_id, const std::string& from_name,
                          uint32_t to_id, const std::string& to_name);
    bool AcceptTrade(uint32_t session_id);
    bool RejectTrade(uint32_t session_id);
    bool CancelTrade(uint32_t session_id, uint32_t character_id);
    
    // Modify offers
    bool AddItem(uint32_t session_id, uint32_t character_id, const TradeItem& item);
    bool RemoveItem(uint32_t session_id, uint32_t character_id, int slot);
    bool SetGold(uint32_t session_id, uint32_t character_id, int gold);
    bool Confirm(uint32_t session_id, uint32_t character_id);
    bool Unconfirm(uint32_t session_id, uint32_t character_id);
    
    // Execution
    bool ExecuteTrade(uint32_t session_id);
    
    // Queries
    TradeSession* GetSession(uint32_t session_id);
    bool HasPendingRequest(uint32_t character_id) const;
    uint32_t FindPendingRequest(uint32_t character_id) const;
    int GetActiveTradeCount() const { return (int)sessions_.size(); }
    
    // Callbacks
    using TradeCallback = std::function<void(uint32_t session_id, const std::string& status)>;
    void SetTradeCallback(TradeCallback cb) { callback_ = cb; }

private:
    std::vector<TradeSession> sessions_;
    uint32_t next_id_ = 1;
    TradeCallback callback_;
    void CleanupSessions();
};
