#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

struct AuctionListing {
    uint64_t id = 0;
    uint32_t seller_id = 0;
    std::string seller_name;
    uint32_t item_id = 0;
    std::string item_name;
    int item_count = 1;
    int enchant_level = 0;
    uint32_t buyout_price = 0;
    uint32_t bid_price = 0;
    uint32_t current_bid = 0;
    uint32_t bidder_id = 0;
    std::string bidder_name;
    float time_remaining = 86400.0f; // 24 hours
    bool sold = false;
    bool active = true;
};

class ConsignmentSystem {
public:
    void Init();
    void Update(float dt);
    
    // List an item
    bool ListItem(uint32_t seller_id, uint32_t item_id, int count, uint32_t price, uint32_t buyout = 0);
    bool CancelListing(uint64_t listing_id);
    
    // Bid/Buy
    bool PlaceBid(uint64_t listing_id, uint32_t bidder_id, uint32_t amount);
    bool Buyout(uint64_t listing_id, uint32_t buyer_id);
    
    // Queries
    std::vector<AuctionListing> SearchListings(const std::string& query = "");
    std::vector<AuctionListing> GetMyListings(uint32_t seller_id);
    std::vector<AuctionListing> GetMyBids(uint32_t bidder_id);
    
    // Fee calculation
    uint32_t CalculateFee(uint32_t price) const { return price * 5 / 100; } // 5% fee
    
    int GetActiveCount() const { return (int)listings_.size(); }

private:
    std::vector<AuctionListing> listings_;
    uint64_t next_id_ = 1;
    float tick_timer_ = 0;
    void ExpireListings(float dt);
};
