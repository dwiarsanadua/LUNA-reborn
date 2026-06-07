#include "ConsignmentSystem.hpp"
#include <algorithm>
#include <cstdlib>
#include <spdlog/spdlog.h>

void ConsignmentSystem::Init() {
    listings_.clear();
    next_id_ = 1;
    spdlog::info("ConsignmentSystem: initialized");
}

void ConsignmentSystem::Update(float dt) {
    ExpireListings(dt);
}

bool ConsignmentSystem::ListItem(uint32_t seller_id, uint32_t item_id, int count, uint32_t price, uint32_t buyout) {
    AuctionListing listing;
    listing.id = next_id_++;
    listing.seller_id = seller_id;
    listing.seller_name = "Player_" + std::to_string(seller_id);
    listing.item_id = item_id;
    listing.item_name = "Item_" + std::to_string(item_id);
    listing.item_count = count;
    listing.bid_price = price;
    listing.buyout_price = buyout > 0 ? buyout : price * 2;
    listing.current_bid = 0;
    listing.time_remaining = 86400.0f;
    listings_.push_back(listing);
    spdlog::info("Consignment: listed item {} by seller {}", item_id, seller_id);
    return true;
}

bool ConsignmentSystem::CancelListing(uint64_t listing_id) {
    for (auto& l : listings_) {
        if (l.id == listing_id && l.active && !l.sold) {
            l.active = false;
            return true;
        }
    }
    return false;
}

bool ConsignmentSystem::PlaceBid(uint64_t listing_id, uint32_t bidder_id, uint32_t amount) {
    for (auto& l : listings_) {
        if (l.id == listing_id && l.active && !l.sold) {
            if (amount >= l.bid_price && amount > l.current_bid) {
                l.current_bid = amount;
                l.bidder_id = bidder_id;
                return true;
            }
        }
    }
    return false;
}

bool ConsignmentSystem::Buyout(uint64_t listing_id, uint32_t buyer_id) {
    for (auto& l : listings_) {
        if (l.id == listing_id && l.active && !l.sold && l.buyout_price > 0) {
            l.current_bid = l.buyout_price;
            l.bidder_id = buyer_id;
            l.sold = true;
            l.active = false;
            return true;
        }
    }
    return false;
}

std::vector<AuctionListing> ConsignmentSystem::SearchListings(const std::string& query) {
    std::vector<AuctionListing> results;
    for (auto& l : listings_) {
        if (!l.active || l.sold) continue;
        if (query.empty() || l.item_name.find(query) != std::string::npos) {
            results.push_back(l);
        }
    }
    return results;
}

std::vector<AuctionListing> ConsignmentSystem::GetMyListings(uint32_t seller_id) {
    std::vector<AuctionListing> results;
    for (auto& l : listings_) if (l.seller_id == seller_id) results.push_back(l);
    return results;
}

std::vector<AuctionListing> ConsignmentSystem::GetMyBids(uint32_t bidder_id) {
    std::vector<AuctionListing> results;
    for (auto& l : listings_) if (l.bidder_id == bidder_id) results.push_back(l);
    return results;
}

void ConsignmentSystem::ExpireListings(float dt) {
    for (auto& l : listings_) {
        if (!l.active || l.sold) continue;
        l.time_remaining -= dt;
        if (l.time_remaining <= 0) {
            l.active = false;
            if (l.current_bid > 0) {
                l.sold = true; // Won by highest bidder
            }
        }
    }
}
