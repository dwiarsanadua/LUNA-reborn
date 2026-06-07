#include "FarmSystem.hpp"
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <spdlog/spdlog.h>

void FarmSystem::Init() {
    plots_.clear();
    spdlog::info("FarmSystem: initialized");
}

void FarmSystem::Update(float dt) {
    tick_timer_ += dt;
    if (tick_timer_ < 5.0f) return; // Tick every 5 seconds
    tick_timer_ = 0;
    
    for (auto& plot : plots_) {
        if (plot.seed_id <= 0 || plot.harvested) continue;
        if (!plot.watered) continue;
        plot.growth_timer += 5.0f;
        int new_stage = std::min(plot.max_stages, (int)(plot.growth_timer / plot.growth_time));
        if (new_stage > plot.growth_stage) {
            plot.growth_stage = new_stage;
            spdlog::info("Farm: plot {} grew to stage {}", plot.id, plot.growth_stage);
        }
    }
}

bool FarmSystem::Plant(int plot_id, int seed_id) {
    if (plot_id < 0 || plot_id >= (int)plots_.size()) return false;
    auto& plot = plots_[plot_id];
    if (plot.seed_id > 0) return false;
    
    auto seed = GetSeed(seed_id);
    plot.seed_id = seed_id;
    plot.plant_name = seed.name;
    plot.growth_stage = 1;
    plot.growth_timer = 0;
    plot.growth_time = (float)seed.growth_time;
    plot.watered = false;
    plot.harvested = false;
    plot.planted_time = time(nullptr);
    spdlog::info("Farm: planted {} on plot {}", seed.name, plot_id);
    return true;
}

bool FarmSystem::Water(int plot_id) {
    if (plot_id < 0 || plot_id >= (int)plots_.size()) return false;
    auto& plot = plots_[plot_id];
    if (plot.seed_id <= 0) return false;
    plot.watered = true;
    plot.last_water_time = time(nullptr);
    return true;
}

bool FarmSystem::Harvest(int plot_id, int& item_id, int& count) {
    if (plot_id < 0 || plot_id >= (int)plots_.size()) return false;
    auto& plot = plots_[plot_id];
    if (plot.seed_id <= 0 || plot.growth_stage < plot.max_stages) return false;
    
    auto seed = GetSeed(plot.seed_id);
    item_id = seed.harvest_item;
    count = seed.min_yield + (rand() % (seed.max_yield - seed.min_yield + 1));
    plot.harvested = true;
    spdlog::info("Farm: harvested {} from plot {} (yield: {})", plot.plant_name, plot_id, count);
    return true;
}

bool FarmSystem::ClearPlot(int plot_id) {
    if (plot_id < 0 || plot_id >= (int)plots_.size()) return false;
    plots_[plot_id] = FarmPlot{};
    plots_[plot_id].id = plot_id;
    return true;
}

FarmPlot* FarmSystem::GetPlot(int plot_id) {
    if (plot_id < 0 || plot_id >= (int)plots_.size()) return nullptr;
    return &plots_[plot_id];
}

int FarmSystem::GetAvailablePlots() const {
    int avail = 0;
    for (auto& p : plots_) if (p.seed_id <= 0 || p.harvested) avail++;
    return avail;
}

SeedData FarmSystem::GetSeed(int seed_id) {
    static std::vector<SeedData> seeds = {
        {1, "Tomato", 120, 1001, 2, 5},
        {2, "Carrot", 90, 1002, 3, 6},
        {3, "Wheat", 180, 1003, 4, 8},
        {4, "Magic Herb", 300, 1004, 1, 3},
        {5, "Golden Flower", 600, 1005, 1, 2},
    };
    for (auto& s : seeds) if (s.id == seed_id) return s;
    return seeds[0];
}

std::vector<SeedData> FarmSystem::GetAllSeeds() {
    return {
        {1, "Tomato", 120, 1001, 2, 5},
        {2, "Carrot", 90, 1002, 3, 6},
        {3, "Wheat", 180, 1003, 4, 8},
        {4, "Magic Herb", 300, 1004, 1, 3},
        {5, "Golden Flower", 600, 1005, 1, 2},
    };
}
