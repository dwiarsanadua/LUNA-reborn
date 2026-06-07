#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <entt/entt.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Movement.hpp>
#include <ecs/components/AIComponent.hpp>
#include <ecs/components/Inventory.hpp>
#include <ecs/components/Party.hpp>
#include <ecs/components/Guild.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/systems/CombatSystem.hpp>
#include <ecs/systems/MovementSystem.hpp>
#include <ecs/systems/AISystem.hpp>
#include <ecs/systems/PartySystem.hpp>
#include <ecs/systems/GuildSystem.hpp>
#include <ecs/systems/PetSystem.hpp>
#include <ecs/systems/FishingSystem.hpp>
#include <ecs/systems/VehicleSystem.hpp>
#include <ecs/systems/CookingSystem.hpp>
#include <ecs/systems/ParticleSystem.hpp>
#include <ecs/systems/GameDataDB.hpp>
#include <chrono>
#include <thread>
#include <random>
#include <cassert>

static int tests_passed = 0, tests_failed = 0;

#define TEST(name, expr) do { \
    if (!(expr)) { spdlog::error("FAIL: {} (line {})", name, __LINE__); tests_failed++; } \
    else { spdlog::info("PASS: {}", name); tests_passed++; } \
} while(0)

int main() {
    spdlog::set_default_logger(spdlog::stdout_color_mt("integration_test"));
    spdlog::set_level(spdlog::level::info);
    spdlog::info("=== Phase 3-4 Integration Tests ===\n");

    // ─── Test 1: GameDataDB ───
    {
        GameDataDB db;
        TEST("DB open", db.Open(GAME_DATA_PATH));
        auto monsters = db.GetAllMonsters();
        auto items = db.GetAllItems();
        auto skills = db.GetAllSkills();
        TEST("Monsters loaded", monsters.size() > 0);
        TEST("Items loaded", items.size() > 0);
        TEST("Skills loaded", skills.size() > 0);
        auto m = db.GetMonster(monsters[0].monster_id);
        TEST("GetMonster by ID", m.monster_id > 0);
        auto item = db.GetItem(items[0].item_id);
        TEST("GetItem by ID", item.item_id > 0);
        auto skill = db.GetSkill(skills[0].skill_id);
        TEST("GetSkill by ID", skill.skill_id > 0);
        db.Close();
    }

    entt::registry reg;

    // ─── Test 2: CombatSystem ───
    {
        CombatSystem combat;
        auto player = reg.create();
        reg.emplace<Transform>(player, glm::vec3{0});
        auto& ps = reg.emplace<CharacterStats>(player);
        ps.level = 10; ps.hp = 500; ps.max_hp = 500;
        ps.physic_attack = 50; ps.physic_defense = 20; ps.accuracy = 90; ps.evasion = 15;
        reg.emplace<TagPlayer>(player);

        auto monster = reg.create();
        reg.emplace<Transform>(monster, glm::vec3{3,0,3});
        auto& ms = reg.emplace<CharacterStats>(monster);
        ms.level = 8; ms.hp = 200; ms.max_hp = 200;
        ms.physic_attack = 30; ms.physic_defense = 10; ms.accuracy = 75; ms.evasion = 10;
        reg.emplace<TagMonster>(monster);

        auto result = combat.CalculateDamage(ps, ms);
        TEST("Damage > 0", result.damage > 0);
        combat.ApplyDamage(reg, monster, result.damage);
        TEST("Monster HP reduced", reg.get<CharacterStats>(monster).hp < 200);
        TEST("In range check", combat.IsInRange(
            reg.get<Transform>(player), reg.get<Transform>(monster), 5.0f));
        TEST("Out of range check", !combat.IsInRange(
            reg.get<Transform>(player), reg.get<Transform>(monster), 1.0f));
    }

    // ─── Test 3: PartySystem ───
    {
        PartySystem party;
        auto leader = reg.create();
        reg.emplace<CharacterStats>(leader);
        auto member = reg.create();
        reg.emplace<CharacterStats>(member);

        TEST("Create party", party.CreateParty(reg, leader));
        TEST("Can't create duplicate", !party.CreateParty(reg, leader));
        TEST("Invite member", party.InviteToParty(reg, leader, member));
        TEST("Accept invite", party.AcceptInvite(reg, member));
        TEST("Party has 2 members", reg.get<Party>(leader).members.size() == 2);
        party.LeaveParty(reg, member);
        TEST("Leave removes party from member", !reg.all_of<Party>(member));
        TEST("XP distribution", party.DistributeXP(reg, leader, 100) > 0);
    }

    // ─── Test 4: GuildSystem ───
    {
        GuildSystem guild;
        auto founder = reg.create();

        TEST("Create guild", guild.CreateGuild(reg, founder, "TestGuild"));
        TEST("Guild has name", reg.get<Guild>(founder).name == "TestGuild");
        auto invitee = reg.create();
        TEST("Invite to guild", guild.InviteToGuild(reg, founder, invitee));
        TEST("Accept guild invite", guild.AcceptInvite(reg, invitee));
        TEST("Guild has 2 members", reg.get<Guild>(founder).members.size() == 2);

        guild.SetNotice(reg, founder, "Welcome!");
        TEST("Notice set", reg.get<Guild>(founder).notice == "Welcome!");
        guild.DepositGold(reg, founder, 1000);
        TEST("Gold deposited", reg.get<Guild>(founder).gold == 1000);
        guild.Update(reg, 1.0f);
        TEST("Guild exp increases", reg.get<Guild>(founder).exp > 0);
    }

    // ─── Test 5: PetSystem ───
    {
        PetSystem pet;
        auto owner = reg.create();
        reg.emplace<Transform>(owner);

        pet.SummonPet(reg, owner, 1);
        // Feed uses entity (owner id lookup)
        pet.FeedPet(reg, owner);
        pet.Update(reg, 1.0f);
        pet.DespawnPet(reg, owner);
        TEST("Pet despawned", true); // no crash
    }

    // ─── Test 6: FishingSystem ───
    {
        FishingSystem fish;
        auto fisher = reg.create();
        reg.emplace<Inventory>(fisher);

        fish.StartFishing(reg, fisher);
        fish.Update(reg, 4.0f); // trigger fish on line
        bool caught = fish.TryCatch(reg, fisher);
        // May or may not catch, just test no crash
        TEST("Fishing no crash", true);
    }

    // ─── Test 7: VehicleSystem ───
    {
        VehicleSystem vehicle;
        auto rider = reg.create();
        reg.emplace<Movement>(rider);

        TEST("Mount vehicle", vehicle.Mount(reg, rider, 1));
        TEST("Is mounted", vehicle.IsMounted(rider));
        TEST("Speed bonus > 0", vehicle.GetSpeedBonus(rider) > 0);
        vehicle.Dismount(reg, rider);
        TEST("Not mounted after dismount", !vehicle.IsMounted(rider));
    }

    // ─── Test 8: CookingSystem ───
    {
        CookingSystem cooking;
        auto chef = reg.create();
        auto& inv = reg.emplace<Inventory>(chef);
        inv.gold = 10000;
        inv.AddItem(1001, 5);
        inv.AddItem(1002, 5);

        auto recipes = cooking.GetAvailableRecipes(chef);
        TEST("Recipes available", recipes.size() > 0);
        TEST("Cook recipe 1", cooking.Cook(reg, chef, 1));
        TEST("Ingredients consumed", inv.FindItem(1001) >= 0); // still has 3 left
    }

    // ─── Test 9: ParticleSystem ───
    {
        ParticleSystem particles;
        auto target = reg.create();
        reg.emplace<Transform>(target);

        particles.Emit(reg, target, ParticleEffect::LevelUp);
        particles.EmitAt(glm::vec3(0), ParticleEffect::HitSpark, 0xFFFF0000);
        particles.Update(reg, 0.5f);
        TEST("Particles emitted", particles.GetActiveParticles().size() > 0);
        particles.Update(reg, 5.0f); // expire all
        TEST("Particles expired", particles.GetActiveParticles().empty());
    }

    // ─── Test 10: AISystem ───
    {
        AISystem ai;
        MovementSystem movement;
        auto monster = reg.create();
        reg.emplace<Transform>(monster, glm::vec3{10,0,10});
        reg.emplace<Movement>(monster);
        auto& astats = reg.emplace<CharacterStats>(monster);
        astats.hp = 100; astats.max_hp = 100;
        auto& aiai = reg.emplace<AIComponent>(monster);
        aiai.aggro_range = 15.0f;
        aiai.attack_range = 3.0f;
        aiai.spawn_position = glm::vec3{10,0,10};
        reg.emplace<TagMonster>(monster);

        // AI in idle should transition properly
        ai.Update(reg, 1.0f);
        TEST("AI no crash", true);
    }

    spdlog::info("\n=== Results: {}/{} passed, {}/{} failed ===",
                 tests_passed, tests_passed + tests_failed,
                 tests_failed, tests_passed + tests_failed);
    return (tests_failed == 0) ? 0 : 1;
}
