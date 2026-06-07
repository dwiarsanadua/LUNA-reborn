#include "CookingSystem.hpp"
#include "../components/Inventory.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>

static std::vector<Recipe> InitRecipes() {
    std::vector<Recipe> recipes;
    recipes.push_back({1, "HP Potion", 20001, 3, {{1001, 2}, {1002, 1}}, 1, 0, 0});
    recipes.push_back({2, "MP Potion", 20002, 3, {{1003, 2}, {1004, 1}}, 1, 0, 0});
    recipes.push_back({3, "Buff Stew", 20003, 1, {{1001, 3}, {1002, 2}, {1005, 1}}, 5, 101, 60.0f});
    recipes.push_back({4, "EXP Elixir", 20004, 1, {{1003, 3}, {1004, 2}, {1005, 1}}, 10, 102, 300.0f});
    return recipes;
}

CookingSystem::CookingSystem() : recipes_(InitRecipes()) {}

bool CookingSystem::LearnRecipe(entt::registry& reg, entt::entity player, uint32_t recipe_id) {
    (void)reg; (void)player;
    spdlog::info("Player learned recipe {}", recipe_id);
    return true;
}

bool CookingSystem::HasIngredients(entt::registry& reg, entt::entity player, const Recipe& recipe) {
    if (!reg.valid(player) || !reg.all_of<Inventory>(player)) return false;
    auto& inv = reg.get<Inventory>(player);
    for (auto& [item_id, count] : recipe.ingredients) {
        if (inv.FindItem(item_id) < 0) return false;
    }
    return true;
}

void CookingSystem::ConsumeIngredients(entt::registry& reg, entt::entity player, const Recipe& recipe) {
    if (!reg.valid(player)) return;
    auto& inv = reg.get<Inventory>(player);
    for (auto& [item_id, count] : recipe.ingredients) {
        int idx = inv.FindItem(item_id);
        if (idx >= 0) inv.RemoveItem(static_cast<size_t>(idx), count);
    }
}

bool CookingSystem::Cook(entt::registry& reg, entt::entity player, uint32_t recipe_id) {
    if (!reg.valid(player)) return false;
    Recipe* recipe = nullptr;
    for (auto& r : recipes_)
        if (r.recipe_id == recipe_id) { recipe = &r; break; }
    if (!recipe) return false;

    if (!HasIngredients(reg, player, *recipe)) {
        spdlog::warn("Missing ingredients for recipe {}", recipe_id);
        return false;
    }
    ConsumeIngredients(reg, player, *recipe);
    auto& inv = reg.get<Inventory>(player);
    inv.AddItem(recipe->result_item_id, recipe->result_count);
    spdlog::info("Cooked {} x{}", recipe->name, recipe->result_count);

    if (recipe->buff_id && reg.all_of<CharacterStats>(player)) {
        spdlog::info("Buff {} applied for {}s", recipe->buff_id, recipe->buff_duration);
    }
    return true;
}

std::vector<Recipe> CookingSystem::GetAvailableRecipes(entt::entity player) const {
    (void)player;
    return recipes_;
}

void CookingSystem::Update(entt::registry& reg, float dt) {
    (void)reg; (void)dt;
}
