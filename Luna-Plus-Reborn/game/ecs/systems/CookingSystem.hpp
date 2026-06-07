#pragma once
#include <entt/entt.hpp>
#include <cstdint>
#include <vector>

struct Recipe {
    uint32_t recipe_id;
    std::string name;
    uint32_t result_item_id;
    uint16_t result_count;
    std::vector<std::pair<uint32_t, uint16_t>> ingredients;
    uint16_t min_level;
    uint32_t buff_id;
    float buff_duration;
};

class CookingSystem {
public:
    CookingSystem();
    bool LearnRecipe(entt::registry& reg, entt::entity player, uint32_t recipe_id);
    bool Cook(entt::registry& reg, entt::entity player, uint32_t recipe_id);
    bool HasIngredients(entt::registry& reg, entt::entity player, const Recipe& recipe);
    void ConsumeIngredients(entt::registry& reg, entt::entity player, const Recipe& recipe);
    void Update(entt::registry& reg, float dt);
    std::vector<Recipe> GetAvailableRecipes(entt::entity player) const;

private:
    std::vector<Recipe> recipes_;
};
