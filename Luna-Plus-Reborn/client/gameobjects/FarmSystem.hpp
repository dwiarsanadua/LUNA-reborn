#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

struct FarmPlot {
    int id = 0;
    int x = 0, y = 0; // Grid position
    int seed_id = 0;
    std::string plant_name;
    int growth_stage = 0;
    int max_stages = 4;
    float growth_timer = 0;
    float growth_time = 60.0f; // seconds per stage
    bool watered = false;
    bool harvested = false;
    time_t planted_time = 0;
    time_t last_water_time = 0;
};

struct SeedData {
    int id = 0;
    std::string name;
    int growth_time = 60;
    int harvest_item = 0;
    int min_yield = 1, max_yield = 3;
};

class FarmSystem {
public:
    void Init();
    void Update(float dt);
    
    // Plot management
    bool Plant(int plot_id, int seed_id);
    bool Water(int plot_id);
    bool Harvest(int plot_id, int& item_id, int& count);
    bool ClearPlot(int plot_id);
    
    // Queries
    FarmPlot* GetPlot(int plot_id);
    const std::vector<FarmPlot>& GetAllPlots() const { return plots_; }
    int GetAvailablePlots() const;
    
    // Seeds
    static SeedData GetSeed(int seed_id);
    static std::vector<SeedData> GetAllSeeds();
    
    // Save/Load
    std::string Serialize() const;
    void Deserialize(const std::string& data);
    
    int GetPlotCount() const { return (int)plots_.size(); }
    void AllocatePlots(int count) { plots_.resize(count); }

private:
    std::vector<FarmPlot> plots_;
    float tick_timer_ = 0;
};
