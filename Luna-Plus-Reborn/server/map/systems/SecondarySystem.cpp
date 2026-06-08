#include "SecondarySystem.hpp"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include <Family_generated.h>
#include <Pet_generated.h>
#include <Fishing_generated.h>
#include <Secondary_generated.h>
#include <Farm_generated.h>
#include <PacketType_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <random>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <random>

namespace {
std::mt19937& Rng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

std::string SqlEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\'') out += "''";
        else out += c;
    }
    return out;
}
}

std::vector<SecondarySystem::SeedDef> SecondarySystem::DefaultSeeds() {
    return {
        {1, "Tomato", 1001, 2, 5, 120.0f, 4},
        {2, "Carrot", 1002, 3, 6, 90.0f, 4},
        {3, "Wheat", 1003, 4, 8, 180.0f, 4},
        {4, "Magic Herb", 1004, 1, 3, 300.0f, 4},
        {5, "Golden Flower", 1005, 1, 2, 600.0f, 4},
    };
}

void SecondarySystem::Init(Database* db, int map_id) {
    db_ = db;
    map_id_ = map_id;
    members_.clear();
    families_.clear();
    pets_.clear();
    farms_.clear();
    next_family_id_ = 1;
    proposal_from_ = 0;
    proposal_to_ = 0;
    fish_table_.clear();
    territories_.clear();
    tournaments_.clear();
    houses_.clear();
    furniture_.clear();
    house_templates_.clear();
    furniture_catalog_.clear();
    selected_house_.clear();
    next_house_id_ = 1;
    next_furniture_id_ = 1;
    house_templates_.push_back({0, "Cozy Cottage", 10000, 16, 51});
    house_templates_.push_back({1, "Town House", 25000, 24, 51});
    house_templates_.push_back({2, "Grand Villa", 50000, 32, 51});
    furniture_catalog_.push_back({9001, "Wooden Table", "Tables"});
    furniture_catalog_.push_back({9002, "Comfy Chair", "Seating"});
    furniture_catalog_.push_back({9003, "Lamp", "Lighting"});
    furniture_catalog_.push_back({9004, "Rug", "Decor"});
    furniture_catalog_.push_back({9005, "Bookshelf", "Storage"});
    furniture_catalog_.push_back({9006, "Bed", "Bedroom"});
    furniture_catalog_.push_back({9007, "Desk", "Tables"});
    furniture_catalog_.push_back({9008, "Cabinet", "Storage"});
    shop_items_.clear();
    seeds_ = DefaultSeeds();
    farm_tick_timer_ = 0.0f;

    fish_table_.push_back({21000001, "Common Carp", 0, 1.0f});
    fish_table_.push_back({21000002, "Silver Trout", 1, 1.5f});
    fish_table_.push_back({21000003, "Golden Koi", 2, 2.0f});
    territories_.push_back({1, "Alker Harbor", 0, "", 10, 0, 0, "", false, 5, 0});
    territories_.push_back({2, "Red Orc Outpost", 0, "", 12, 0, 0, "", false, 7, 0});
    territories_.push_back({3, "Siege Castle", 0, "", 15, 0, 0, "", true, 10, 0});
    tournaments_.push_back({1, "Weekly Arena", 0, 0, 8, 5000, 0, 0, 1, 6, 0, false});
    tournaments_.push_back({2, "Guild Championship", 0, 0, 16, 15000, 0, 0, 1, 6, 0, false});
    shop_items_.push_back({1001, "Health Potion", "Restore 200 HP", 50, "Consumables", 0, 1, 99, false, 0});
    shop_items_.push_back({1002, "Mana Potion", "Restore 150 MP", 50, "Consumables", 0, 1, 99, false, 0});
    shop_items_.push_back({201, "Pet Summon Scroll", "Summon a pet companion", 200, "Pets", 0, 1, 5, false, 0});
    shop_items_.push_back({301, "Costume Box", "Random cosmetic costume", 150, "Cosmetics", 1, 1, 3, false, 0});
    shop_items_.push_back({401, "Fishing Bait x10", "Ten fishing baits", 30, "Misc", 0, 10, 99, false, 0});
    player_cash_.clear();
    purchase_counts_.clear();
    purchases_loaded_.clear();

    if (!db_) {
        spdlog::info("SecondarySystem: map {} initialized (no db)", map_id_);
        return;
    }

    auto rows = db_->Query("SELECT FamilyIdx, FamilyName, MasterIdx FROM TB_FAMILY LIMIT 64");
    for (const auto& row : rows) {
        if (row.size() < 3) continue;
        FamilyGroup g;
        g.family_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        g.name = row[1];
        g.master_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
        families_.push_back(g);
        if (g.family_id >= next_family_id_) next_family_id_ = g.family_id + 1;
    }

    LoadMembersFromDb();

    rows = db_->Query("SELECT item_id, name, rarity, weight FROM phase6_fish_types LIMIT 32");
    if (!rows.empty()) {
        fish_table_.clear();
        for (const auto& row : rows) {
            if (row.size() < 4) continue;
            FishTableEntry e;
            e.item_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            e.name = row[1];
            e.rarity = static_cast<uint8_t>(std::atoi(row[2].c_str()));
            e.weight = std::stof(row[3]);
            fish_table_.push_back(e);
        }
    }

    rows = db_->Query(
        "SELECT territory_id, name, owner_guild_id, owner_guild_name, tax_rate, "
        "siege_time, attacker_guild_id, attacker_guild_name, is_castle, defense_bonus, tax_accumulated "
        "FROM phase6_territories LIMIT 16");
    if (!rows.empty()) {
        territories_.clear();
        for (const auto& row : rows) {
            if (row.size() < 5) continue;
            TerritoryState t;
            t.id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            t.name = row[1];
            t.owner_guild_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
            t.owner_guild_name = row[3];
            t.tax_rate = static_cast<uint16_t>(std::atoi(row[4].c_str()));
            if (row.size() >= 11) {
                t.siege_time = static_cast<uint64_t>(std::strtoull(row[5].c_str(), nullptr, 10));
                t.attacker_guild_id = static_cast<uint32_t>(std::atoi(row[6].c_str()));
                t.attacker_guild_name = row[7];
                t.is_castle = std::atoi(row[8].c_str()) != 0;
                t.defense_bonus = static_cast<uint8_t>(std::atoi(row[9].c_str()));
                t.tax_accumulated = static_cast<uint32_t>(std::atoi(row[10].c_str()));
            }
            territories_.push_back(t);
        }
    }

    rows = db_->Query(
        "SELECT item_id, name, price, category, description, currency_type, stack_count, "
        "max_purchase, on_sale, sale_price FROM phase6_shop_items LIMIT 64");
    if (!rows.empty()) {
        shop_items_.clear();
        for (const auto& row : rows) {
            if (row.size() < 4) continue;
            ShopItem i;
            i.item_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            i.name = row[1];
            i.price = static_cast<uint32_t>(std::atoi(row[2].c_str()));
            i.category = row[3];
            if (row.size() >= 5) i.description = row[4];
            if (row.size() >= 6) i.currency_type = static_cast<uint8_t>(std::atoi(row[5].c_str()));
            if (row.size() >= 7) i.stack_count = static_cast<uint16_t>(std::atoi(row[6].c_str()));
            if (row.size() >= 8) i.max_purchase = static_cast<uint16_t>(std::atoi(row[7].c_str()));
            if (row.size() >= 9) i.on_sale = std::atoi(row[8].c_str()) != 0;
            if (row.size() >= 10) i.sale_price = static_cast<uint32_t>(std::atoi(row[9].c_str()));
            shop_items_.push_back(i);
        }
    }

    LoadTournamentsFromDb();

    rows = db_->Query(
        "SELECT template_id, name, price, max_furniture, map_id "
        "FROM phase6_house_templates LIMIT 16");
    if (!rows.empty()) {
        house_templates_.clear();
        for (const auto& row : rows) {
            if (row.size() < 5) continue;
            HouseTemplate t;
            t.template_id = static_cast<uint8_t>(std::atoi(row[0].c_str()));
            t.name = row[1];
            t.price = static_cast<uint32_t>(std::atoi(row[2].c_str()));
            t.max_furniture = static_cast<uint16_t>(std::atoi(row[3].c_str()));
            t.map_id = static_cast<uint16_t>(std::atoi(row[4].c_str()));
            house_templates_.push_back(t);
        }
    }

    rows = db_->Query(
        "SELECT item_id, name, category FROM phase6_furniture_catalog LIMIT 64");
    if (!rows.empty()) {
        furniture_catalog_.clear();
        for (const auto& row : rows) {
            if (row.size() < 3) continue;
            FurnitureCatalogEntry e;
            e.item_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            e.name = row[1];
            e.category = row[2];
            furniture_catalog_.push_back(e);
        }
    }

    rows = db_->Query(
        "SELECT HouseIdx, CharacterIdx, MapIdx, PosX, PosY, HouseType, FurnitureCount "
        "FROM TB_HOUSE LIMIT 32");
    houses_.clear();
    for (const auto& row : rows) {
        if (row.size() < 7) continue;
        HouseState h;
        h.house_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        h.owner_id = static_cast<uint32_t>(std::atoi(row[1].c_str()));
        h.map_id = static_cast<uint16_t>(std::atoi(row[2].c_str()));
        h.pos_x = std::stof(row[3]);
        h.pos_y = std::stof(row[4]);
        h.house_type = static_cast<uint8_t>(std::atoi(row[5].c_str()));
        h.furniture_count = static_cast<uint16_t>(std::atoi(row[6].c_str()));
        if (const HouseTemplate* tmpl = FindHouseTemplate(h.house_type)) {
            h.name = tmpl->name;
            h.max_furniture = tmpl->max_furniture;
        } else {
            h.name = HouseTypeName(h.house_type);
        }
        houses_.push_back(h);
        if (h.house_id >= next_house_id_) next_house_id_ = h.house_id + 1;
    }

    rows = db_->Query(
        "SELECT FurnitureIdx, HouseIdx, ItemDBIdx, PosX, PosY, RotY "
        "FROM TB_HOUSE_FURNITURE LIMIT 256");
    furniture_.clear();
    for (const auto& row : rows) {
        if (row.size() < 6) continue;
        FurnitureState f;
        f.furniture_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        f.house_id = static_cast<uint32_t>(std::atoi(row[1].c_str()));
        f.item_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
        f.pos_x = std::stof(row[3]);
        f.pos_y = std::stof(row[4]);
        f.rot_y = std::stof(row[5]);
        if (const FurnitureCatalogEntry* cat = FindFurnitureCatalog(f.item_id))
            f.name = cat->name;
        else
            f.name = "Furniture";
        furniture_.push_back(f);
        if (f.furniture_id >= next_furniture_id_) next_furniture_id_ = f.furniture_id + 1;
    }
    for (auto& h : houses_) SyncHouseFurnitureCount(h);

    spdlog::info("SecondarySystem: map {} — {} members, {} fish, {} shop items, {} houses",
        map_id_, members_.size(), fish_table_.size(), shop_items_.size(), houses_.size());
}

void SecondarySystem::Update(float dt) {
    farm_tick_timer_ += dt;
    pet_tick_timer_ += dt;
    if (farm_tick_timer_ >= 5.0f) {
        farm_tick_timer_ = 0.0f;
        for (auto& [cid, plots] : farms_) {
            TickFarmGrowth(cid, 5.0f);
            (void)plots;
        }
    }
    if (pet_tick_timer_ >= 5.0f) {
        pet_tick_timer_ = 0.0f;
        for (auto& [cid, pet] : pets_) {
            if (!pet.summoned) continue;
            TickPet(cid, pet, 5.0f);
            SavePetToDb(cid, pet);
        }
    }
    siege_tick_timer_ += dt;
    if (siege_tick_timer_ >= 30.0f) {
        siege_tick_timer_ = 0.0f;
        TickSieges();
    }
    tournament_tick_timer_ += dt;
    if (tournament_tick_timer_ >= 60.0f) {
        tournament_tick_timer_ = 0.0f;
        TickTournaments();
    }
}

void SecondarySystem::LoadMembersFromDb() {
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT character_id, name, relation, partner_id, partner_name, family_id, family_name, married_date "
        "FROM player_family LIMIT 512");
    for (const auto& row : rows) {
        if (row.size() < 7) continue;
        MemberState m;
        m.character_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        m.name = row[1];
        m.relation = static_cast<uint8_t>(std::atoi(row[2].c_str()));
        m.partner_id = static_cast<uint32_t>(std::atoi(row[3].c_str()));
        m.partner_name = row[4];
        m.family_id = static_cast<uint32_t>(std::atoi(row[5].c_str()));
        m.family_name = row[6];
        if (row.size() >= 8)
            m.married_date = static_cast<uint64_t>(std::strtoull(row[7].c_str(), nullptr, 10));
        if (!FindMember(m.character_id)) members_.push_back(m);
    }
}

void SecondarySystem::LoadMemberFromDb(uint32_t character_id, MemberState& out) {
    out = {};
    out.character_id = character_id;
    out.relation = 1;
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT name, relation, partner_id, partner_name, family_id, family_name, married_date "
        "FROM player_family WHERE character_id=" + std::to_string(character_id));
    if (rows.empty() || rows[0].size() < 6) return;
    const auto& row = rows[0];
    out.name = row[0];
    out.relation = static_cast<uint8_t>(std::atoi(row[1].c_str()));
    out.partner_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
    out.partner_name = row[3];
    out.family_id = static_cast<uint32_t>(std::atoi(row[4].c_str()));
    out.family_name = row[5];
    if (row.size() >= 7)
        out.married_date = static_cast<uint64_t>(std::strtoull(row[6].c_str(), nullptr, 10));
}

void SecondarySystem::SaveMemberToDb(const MemberState& member) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_family "
        "(character_id, name, relation, partner_id, partner_name, family_id, family_name, married_date) VALUES ("
        + std::to_string(member.character_id) + ", '"
        + SqlEscape(member.name) + "', "
        + std::to_string(member.relation) + ", "
        + std::to_string(member.partner_id) + ", '"
        + SqlEscape(member.partner_name) + "', "
        + std::to_string(member.family_id) + ", '"
        + SqlEscape(member.family_name) + "', "
        + std::to_string(member.married_date) + ")");
}

const char* SecondarySystem::RelationLabel(uint8_t relation) {
    switch (relation) {
    case 2: return "Engaged";
    case 3: return "Married";
    case 4: return "Divorced";
    default: return "Single";
    }
}

void SecondarySystem::SaveFamilyGroupToDb(const FamilyGroup& group) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR IGNORE INTO TB_FAMILY (FamilyIdx, FamilyName, MasterIdx) VALUES ("
        + std::to_string(group.family_id) + ", '"
        + SqlEscape(group.name) + "', "
        + std::to_string(group.master_id) + ")");
}

void SecondarySystem::LoadPetFromDb(uint32_t character_id, PetState& out) {
    out = {};
    out.pet_id = 1;
    out.template_id = 1;
    out.name = "Fluffy";
    out.level = 1;
    out.hp = 100;
    out.max_hp = 100;
    out.satiation = 100;
    out.evolution = 1;
    out.exp = 0;
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT pet_id, template_id, name, level, hp, max_hp, satiation, summoned, evolution, exp "
        "FROM player_pet WHERE character_id=" + std::to_string(character_id));
    if (rows.empty() || rows[0].size() < 8) return;
    const auto& row = rows[0];
    out.pet_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
    out.template_id = static_cast<uint32_t>(std::atoi(row[1].c_str()));
    out.name = row[2].empty() ? "Fluffy" : row[2];
    out.level = static_cast<uint16_t>(std::atoi(row[3].c_str()));
    out.hp = static_cast<uint16_t>(std::atoi(row[4].c_str()));
    out.max_hp = static_cast<uint16_t>(std::atoi(row[5].c_str()));
    out.satiation = static_cast<uint16_t>(std::atoi(row[6].c_str()));
    out.summoned = std::atoi(row[7].c_str()) != 0;
    if (row.size() >= 10) {
        out.evolution = static_cast<uint8_t>(std::max(1, std::atoi(row[8].c_str())));
        out.exp = static_cast<uint32_t>(std::atoi(row[9].c_str()));
    }
    out.initialized = true;
}

void SecondarySystem::SavePetToDb(uint32_t character_id, const PetState& pet) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_pet "
        "(character_id, pet_id, template_id, name, level, hp, max_hp, satiation, summoned, evolution, exp) VALUES ("
        + std::to_string(character_id) + ", "
        + std::to_string(pet.pet_id) + ", "
        + std::to_string(pet.template_id) + ", '"
        + SqlEscape(pet.name) + "', "
        + std::to_string(pet.level) + ", "
        + std::to_string(pet.hp) + ", "
        + std::to_string(pet.max_hp) + ", "
        + std::to_string(pet.satiation) + ", "
        + std::to_string(pet.summoned ? 1 : 0) + ", "
        + std::to_string(pet.evolution) + ", "
        + std::to_string(pet.exp) + ")");
}

uint32_t SecondarySystem::PetExpToNext(uint16_t level) {
    return static_cast<uint32_t>(level) * 50u;
}

uint32_t SecondarySystem::PetFeedCost() {
    return 50;
}

uint32_t SecondarySystem::PetEvolveCost(uint8_t evolution) {
    return 500u * static_cast<uint32_t>(std::max(1, static_cast<int>(evolution)));
}

bool SecondarySystem::CanSummonPet(const PetState& pet) {
    return pet.satiation >= 20;
}

void SecondarySystem::ApplyPetExpLevel(PetState& pet, uint32_t gained_exp) {
    pet.exp += gained_exp;
    while (pet.exp >= PetExpToNext(pet.level) && pet.level < 99) {
        pet.exp -= PetExpToNext(pet.level);
        pet.level = static_cast<uint16_t>(pet.level + 1);
        pet.max_hp = static_cast<uint16_t>(pet.max_hp + 5);
        pet.hp = pet.max_hp;
    }
}

void SecondarySystem::TickPet(uint32_t character_id, PetState& pet, float dt) {
    (void)character_id;
    (void)dt;
    if (!pet.summoned) return;
    if (pet.satiation > 0)
        pet.satiation = static_cast<uint16_t>(std::max(0, static_cast<int>(pet.satiation) - 2));
    if (pet.satiation == 0) {
        pet.summoned = false;
        return;
    }
    if (pet.satiation > 40)
        ApplyPetExpLevel(pet, 5);
}

flatbuffers::Offset<luna::protocol::PetInfo> SecondarySystem::MakePetInfoOffset(
    flatbuffers::FlatBufferBuilder& fbb, const PetState& pet) const {
    using namespace luna::protocol;
    return CreatePetInfo(fbb, pet.pet_id, pet.template_id, fbb.CreateString(pet.name),
        pet.level, pet.hp, pet.max_hp, pet.satiation, pet.summoned, pet.evolution, pet.exp,
        PetExpToNext(pet.level), PetFeedCost(), PetEvolveCost(pet.evolution), CanSummonPet(pet));
}

void SecondarySystem::LoadFarmFromDb(uint32_t character_id, std::vector<FarmPlotState>& plots) {
    plots.clear();
    plots.resize(kFarmPlotCount);
    for (int i = 0; i < kFarmPlotCount; ++i) plots[i].plot_id = static_cast<uint8_t>(i);
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT plot_id, seed_id, plant_name, growth_stage, max_stages, growth_timer, "
        "growth_time, watered, harvested FROM player_farm_plot WHERE character_id="
        + std::to_string(character_id));
    for (const auto& row : rows) {
        if (row.size() < 9) continue;
        int pid = std::atoi(row[0].c_str());
        if (pid < 0 || pid >= kFarmPlotCount) continue;
        auto& p = plots[pid];
        p.plot_id = static_cast<uint8_t>(pid);
        p.seed_id = static_cast<uint32_t>(std::atoi(row[1].c_str()));
        p.plant_name = row[2];
        p.growth_stage = static_cast<uint8_t>(std::atoi(row[3].c_str()));
        p.max_stages = static_cast<uint8_t>(std::atoi(row[4].c_str()));
        p.growth_timer = std::stof(row[5]);
        p.growth_time = std::stof(row[6]);
        p.watered = std::atoi(row[7].c_str()) != 0;
        p.harvested = std::atoi(row[8].c_str()) != 0;
    }
}

void SecondarySystem::SaveFarmPlotToDb(uint32_t character_id, const FarmPlotState& plot) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_farm_plot "
        "(character_id, plot_id, seed_id, plant_name, growth_stage, max_stages, "
        "growth_timer, growth_time, watered, harvested) VALUES ("
        + std::to_string(character_id) + ", "
        + std::to_string(plot.plot_id) + ", "
        + std::to_string(plot.seed_id) + ", '"
        + SqlEscape(plot.plant_name) + "', "
        + std::to_string(plot.growth_stage) + ", "
        + std::to_string(plot.max_stages) + ", "
        + std::to_string(plot.growth_timer) + ", "
        + std::to_string(plot.growth_time) + ", "
        + std::to_string(plot.watered ? 1 : 0) + ", "
        + std::to_string(plot.harvested ? 1 : 0) + ")");
}

SecondarySystem::MemberState* SecondarySystem::FindMember(uint32_t character_id) {
    for (auto& m : members_) {
        if (m.character_id == character_id) return &m;
    }
    return nullptr;
}

const SecondarySystem::MemberState* SecondarySystem::FindMember(uint32_t character_id) const {
    for (const auto& m : members_) {
        if (m.character_id == character_id) return &m;
    }
    return nullptr;
}

SecondarySystem::MemberState& SecondarySystem::EnsureMember(uint32_t character_id,
                                                            const std::string& name) {
    if (auto* m = FindMember(character_id)) return *m;
    MemberState loaded;
    LoadMemberFromDb(character_id, loaded);
    loaded.character_id = character_id;
    if (loaded.name.empty()) loaded.name = name;
    members_.push_back(loaded);
    return members_.back();
}

SecondarySystem::PetState& SecondarySystem::EnsurePet(uint32_t character_id) {
    auto it = pets_.find(character_id);
    if (it != pets_.end()) return it->second;
    PetState pet;
    LoadPetFromDb(character_id, pet);
    pet.initialized = true;
    auto [inserted, _] = pets_.emplace(character_id, pet);
    return inserted->second;
}

std::vector<SecondarySystem::FarmPlotState>& SecondarySystem::EnsureFarm(uint32_t character_id) {
    auto it = farms_.find(character_id);
    if (it != farms_.end()) return it->second;
    std::vector<FarmPlotState> plots;
    LoadFarmFromDb(character_id, plots);
    auto [inserted, _] = farms_.emplace(character_id, std::move(plots));
    return inserted->second;
}

const SecondarySystem::SeedDef* SecondarySystem::FindSeed(uint32_t seed_id) const {
    for (const auto& s : seeds_) {
        if (s.id == seed_id) return &s;
    }
    return seeds_.empty() ? nullptr : &seeds_[0];
}

uint8_t SecondarySystem::CalcGrowthPct(const FarmPlotState& plot) {
    if (plot.seed_id == 0 || plot.harvested || plot.max_stages == 0) return 0;
    if (plot.growth_stage >= plot.max_stages) return 100;
    float stage_progress = plot.growth_time > 0.0f ? plot.growth_timer / plot.growth_time : 0.0f;
    float completed = static_cast<float>(std::max(0, plot.growth_stage - 1)) + stage_progress;
    return static_cast<uint8_t>(std::min(99.0f, (completed / plot.max_stages) * 100.0f));
}

bool SecondarySystem::IsPlotReady(const FarmPlotState& plot) {
    return plot.seed_id != 0 && !plot.harvested && plot.growth_stage >= plot.max_stages;
}

bool SecondarySystem::IsPlotEmpty(const FarmPlotState& plot) {
    return plot.seed_id == 0 || plot.harvested;
}

flatbuffers::Offset<luna::protocol::FarmPlotInfo> SecondarySystem::MakeFarmPlotOffset(
    flatbuffers::FlatBufferBuilder& fbb, const FarmPlotState& plot) const {
    using namespace luna::protocol;
    return CreateFarmPlotInfo(fbb, plot.plot_id, plot.seed_id, fbb.CreateString(plot.plant_name),
        plot.growth_stage, plot.max_stages, CalcGrowthPct(plot), plot.growth_timer, plot.growth_time,
        plot.watered, plot.harvested, IsPlotReady(plot));
}

void SecondarySystem::TickFarmGrowthPlot(FarmPlotState& plot, float dt) {
    if (IsPlotEmpty(plot) || !plot.watered || plot.growth_stage >= plot.max_stages) return;
    plot.growth_timer += dt;
    while (plot.growth_time > 0.0f && plot.growth_timer >= plot.growth_time
           && plot.growth_stage < plot.max_stages) {
        plot.growth_timer -= plot.growth_time;
        plot.growth_stage++;
        plot.watered = false;
    }
}

void SecondarySystem::TickFarmGrowth(uint32_t character_id, float dt) {
    auto& plots = EnsureFarm(character_id);
    for (auto& plot : plots) {
        if (plot.seed_id == 0) continue;
        TickFarmGrowthPlot(plot, dt);
        SaveFarmPlotToDb(character_id, plot);
    }
}

flatbuffers::Offset<luna::protocol::FamilyInfo> SecondarySystem::MakeFamilyInfoOffset(
    flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player) const {
    using namespace luna::protocol;

    std::vector<flatbuffers::Offset<FamilyMemberInfo>> member_offs;
    uint32_t family_id = 0;
    std::string family_name;
    uint32_t master_id = 0;

    const MemberState* self = FindMember(player.character_id);
    if (self) {
        family_id = self->family_id;
        family_name = self->family_name;
        if (family_id) {
            for (const auto& f : families_) {
                if (f.family_id == family_id) {
                    master_id = f.master_id;
                    family_name = f.name;
                    break;
                }
            }
            for (const auto& m : members_) {
                if (m.family_id != family_id) continue;
                auto nm = fbb.CreateString(m.name.empty() ? "Member" : m.name);
                auto pn = fbb.CreateString(m.partner_name);
                auto fn = fbb.CreateString(m.family_name);
                member_offs.push_back(CreateFamilyMemberInfo(
                    fbb, m.character_id, nm, m.relation, m.partner_id, pn, m.family_id, fn,
                    m.married_date, m.character_id == master_id));
            }
        } else {
            auto nm = fbb.CreateString(player.name);
            auto pn = fbb.CreateString(self->partner_name);
            member_offs.push_back(CreateFamilyMemberInfo(
                fbb, player.character_id, nm, self->relation, self->partner_id, pn, 0,
                fbb.CreateString(""), self->married_date, false));
        }
    } else {
        auto nm = fbb.CreateString(player.name);
        member_offs.push_back(CreateFamilyMemberInfo(
            fbb, player.character_id, nm, 1, 0, fbb.CreateString(""), 0, fbb.CreateString(""),
            0, false));
    }

    const bool can_accept = self && self->relation == 2 && self->partner_id != 0;
    const bool can_reject = can_accept;
    const bool can_divorce = self && self->relation == 3;
    const bool can_create = self && self->family_id == 0;
    bool can_leave = false;
    if (self && self->family_id && self->character_id != master_id)
        can_leave = true;

    uint32_t engaged_partner_id = can_accept ? self->partner_id : 0;
    std::string engaged_partner_name = can_accept ? self->partner_name : std::string();

    return CreateFamilyInfo(fbb, family_id, fbb.CreateString(family_name), master_id,
        fbb.CreateVector(member_offs), can_accept, can_reject, can_divorce, can_leave, can_create,
        engaged_partner_id, fbb.CreateString(engaged_partner_name));
}

void SecondarySystem::SendFamilyState(NetworkLayer* network, const MapPlayerContext& player,
                                      uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto fam = MakeFamilyInfoOffset(fbb, player);
    auto resp = CreateFamilyResponse(fbb, result, fam, fbb.CreateString(message));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_FAMILY_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendPetState(NetworkLayer* network, const MapPlayerContext& player,
                                   uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    auto& pet = EnsurePet(player.character_id);
    flatbuffers::FlatBufferBuilder fbb;
    auto info = MakePetInfoOffset(fbb, pet);
    auto resp = CreatePetResponse(fbb, result, info, fbb.CreateString(message));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_PET_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendFarmState(NetworkLayer* network, const MapPlayerContext& player,
                                    uint8_t result, uint32_t harvest_item, uint16_t harvest_count,
                                    const std::string& message) {
    using namespace luna::protocol;
    auto& plots = EnsureFarm(player.character_id);
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<FarmPlotInfo>> offs;
    std::vector<flatbuffers::Offset<FarmSeedInfo>> seed_offs;
    for (const auto& s : seeds_) {
        seed_offs.push_back(CreateFarmSeedInfo(fbb, s.id, fbb.CreateString(s.name),
            static_cast<uint16_t>(s.growth_time), s.harvest_item));
    }
    for (const auto& p : plots)
        offs.push_back(MakeFarmPlotOffset(fbb, p));
    auto resp = CreateFarmResponse(fbb, result, fbb.CreateVector(offs), fbb.CreateVector(seed_offs),
        harvest_item, harvest_count, fbb.CreateString(message));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_FARM_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendFishingResult(NetworkLayer* network, const MapPlayerContext& player,
                                        uint8_t result, uint32_t fish_id, const std::string& fish_name,
                                        uint8_t rarity, const std::string& message) {
    using namespace luna::protocol;
    (void)player;
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = CreateFishingCastResponse(fbb, result, fish_id, fbb.CreateString(fish_name),
        rarity, fbb.CreateString(message));
    fbb.Finish(resp);
    uint16_t pkt = result == 0 ? PacketType_MP_FISHING_CAST_ACK : PacketType_MP_FISHING_CAST_NACK;
    network->SendPacket(pkt, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendSiegeInfo(NetworkLayer* network, const MapPlayerContext& player,
                                    uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = MakeSiegeInfoResponseOffset(fbb, player, result, message);
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_SIEGE_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

SecondarySystem::TerritoryState* SecondarySystem::FindTerritory(uint32_t territory_id) {
    for (auto& t : territories_)
        if (t.id == territory_id) return &t;
    return nullptr;
}

const SecondarySystem::TerritoryState* SecondarySystem::FindTerritory(uint32_t territory_id) const {
    for (const auto& t : territories_)
        if (t.id == territory_id) return &t;
    return nullptr;
}

void SecondarySystem::SaveTerritoryToDb(const TerritoryState& territory) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO phase6_territories "
        "(territory_id, name, owner_guild_id, owner_guild_name, tax_rate, siege_time, "
        "attacker_guild_id, attacker_guild_name, is_castle, defense_bonus, tax_accumulated) VALUES ("
        + std::to_string(territory.id) + ", '"
        + SqlEscape(territory.name) + "', "
        + std::to_string(territory.owner_guild_id) + ", '"
        + SqlEscape(territory.owner_guild_name) + "', "
        + std::to_string(territory.tax_rate) + ", "
        + std::to_string(territory.siege_time) + ", "
        + std::to_string(territory.attacker_guild_id) + ", '"
        + SqlEscape(territory.attacker_guild_name) + "', "
        + std::to_string(territory.is_castle ? 1 : 0) + ", "
        + std::to_string(territory.defense_bonus) + ", "
        + std::to_string(territory.tax_accumulated) + ")");
}

uint32_t SecondarySystem::SecondsUntilSiege(uint64_t siege_time) {
    if (siege_time == 0) return 0;
    const uint64_t now = static_cast<uint64_t>(time(nullptr));
    if (siege_time <= now) return 0;
    const uint64_t delta = siege_time - now;
    return delta > UINT32_MAX ? UINT32_MAX : static_cast<uint32_t>(delta);
}

void SecondarySystem::ResolveSiege(TerritoryState& territory) {
    if (territory.attacker_guild_id == 0) {
        territory.siege_time = 0;
        SaveTerritoryToDb(territory);
        return;
    }
    uint32_t winner = territory.owner_guild_id;
    if (territory.owner_guild_id == 0) {
        winner = territory.attacker_guild_id;
    } else {
        const int roll = std::rand() % 100;
        const int defender_chance = std::min(90, 50 + static_cast<int>(territory.defense_bonus));
        if (roll >= defender_chance)
            winner = territory.attacker_guild_id;
    }
    if (winner == territory.attacker_guild_id) {
        territory.owner_guild_id = territory.attacker_guild_id;
        territory.owner_guild_name = territory.attacker_guild_name;
    }
    territory.attacker_guild_id = 0;
    territory.attacker_guild_name.clear();
    territory.siege_time = 0;
    SaveTerritoryToDb(territory);
}

void SecondarySystem::TickSieges() {
    const uint64_t now = static_cast<uint64_t>(time(nullptr));
    for (auto& t : territories_) {
        if (t.owner_guild_id != 0 && t.tax_rate > 0)
            t.tax_accumulated += static_cast<uint32_t>(t.tax_rate * 5);
        if (t.siege_time == 0 || t.attacker_guild_id == 0) continue;
        if (t.siege_time <= now)
            ResolveSiege(t);
        else
            SaveTerritoryToDb(t);
    }
}

flatbuffers::Offset<luna::protocol::SiegeInfoResponse> SecondarySystem::MakeSiegeInfoResponseOffset(
    flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
    uint8_t result, const std::string& message) const {
    using namespace luna::protocol;
    const uint64_t now = static_cast<uint64_t>(time(nullptr));

    std::vector<flatbuffers::Offset<SiegeTerritoryInfo>> terr_offs;
    std::vector<flatbuffers::Offset<SiegeScheduleInfo>> sched_offs;
    bool can_declare = false;
    bool can_set_tax = false;

    for (const auto& t : territories_) {
        const bool owned = player.guild_id != 0 && t.owner_guild_id == player.guild_id;
        const bool has_future_siege = t.siege_time > now && t.attacker_guild_id != 0;
        const bool can_attack = player.guild_id != 0 && t.owner_guild_id != player.guild_id
            && !has_future_siege;
        if (owned) can_set_tax = true;
        if (owned) can_set_tax = true;
        if (can_attack) can_declare = true;

        terr_offs.push_back(CreateSiegeTerritoryInfo(
            fbb, t.id, fbb.CreateString(t.name), t.owner_guild_id,
            fbb.CreateString(t.owner_guild_name), t.tax_rate, t.siege_time,
            t.is_castle, t.defense_bonus, t.attacker_guild_id,
            fbb.CreateString(t.attacker_guild_name), SecondsUntilSiege(t.siege_time),
            t.tax_accumulated, owned, can_attack, owned));

        if (has_future_siege) {
            sched_offs.push_back(CreateSiegeScheduleInfo(
                fbb, t.id, fbb.CreateString(t.name), t.attacker_guild_id,
                fbb.CreateString(t.attacker_guild_name), t.owner_guild_id,
                fbb.CreateString(t.owner_guild_name), t.siege_time,
                SecondsUntilSiege(t.siege_time)));
        }
    }

    return CreateSiegeInfoResponse(
        fbb, result, fbb.CreateString(message), fbb.CreateVector(terr_offs),
        fbb.CreateVector(sched_offs), player.guild_id, fbb.CreateString(player.guild_name),
        can_declare && player.guild_id != 0, can_set_tax);
}

void SecondarySystem::LoadTournamentsFromDb() {
    if (!db_) {
        const uint64_t now = static_cast<uint64_t>(time(nullptr));
        for (auto& t : tournaments_) {
            if (t.registration_end == 0)
                t.registration_end = now + (t.id == 1 ? 3600 : 7200);
        }
        return;
    }
    tournament_regs_.clear();
    tournament_matches_.clear();
    auto rows = db_->Query(
        "SELECT tournament_id, name, state, max_teams, prize_gold, registration_end, "
        "current_round, min_team_size, max_team_size, winner_guild_id, prize_claimed "
        "FROM phase6_tournaments LIMIT 32");
    if (!rows.empty()) {
        tournaments_.clear();
        for (const auto& row : rows) {
            if (row.size() < 10) continue;
            TournamentState t;
            t.id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            t.name = row[1];
            t.state = static_cast<uint8_t>(std::atoi(row[2].c_str()));
            t.max_teams = static_cast<uint16_t>(std::atoi(row[3].c_str()));
            t.prize_gold = static_cast<uint32_t>(std::atoi(row[4].c_str()));
            t.registration_end = static_cast<uint64_t>(std::strtoull(row[5].c_str(), nullptr, 10));
            t.current_round = static_cast<uint8_t>(std::atoi(row[6].c_str()));
            t.min_team_size = static_cast<uint8_t>(std::atoi(row[7].c_str()));
            t.max_team_size = static_cast<uint8_t>(std::atoi(row[8].c_str()));
            t.winner_guild_id = static_cast<uint32_t>(std::atoi(row[9].c_str()));
            if (row.size() >= 11)
                t.prize_claimed = std::atoi(row[10].c_str()) != 0;
            SyncTournamentRegisteredCount(t);
            tournaments_.push_back(t);
        }
    } else {
        const uint64_t now = static_cast<uint64_t>(time(nullptr));
        for (auto& t : tournaments_) {
            if (t.registration_end == 0)
                t.registration_end = now + (t.id == 1 ? 3600 : 7200);
            SaveTournamentToDb(t);
        }
    }
    rows = db_->Query(
        "SELECT tournament_id, guild_id, guild_name, team_leader_id, seed, eliminated "
        "FROM phase6_tournament_registrations LIMIT 256");
    for (const auto& row : rows) {
        if (row.size() < 6) continue;
        TournamentRegistration r;
        r.tournament_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        r.guild_id = static_cast<uint32_t>(std::atoi(row[1].c_str()));
        r.guild_name = row[2];
        r.team_leader_id = static_cast<uint32_t>(std::atoi(row[3].c_str()));
        r.seed = static_cast<uint16_t>(std::atoi(row[4].c_str()));
        r.eliminated = std::atoi(row[5].c_str()) != 0;
        tournament_regs_.push_back(r);
    }
    for (auto& t : tournaments_) SyncTournamentRegisteredCount(t);
    rows = db_->Query(
        "SELECT tournament_id, round_num, match_index, team1_guild_id, team2_guild_id, "
        "winner_guild_id, completed FROM phase6_tournament_matches LIMIT 512");
    for (const auto& row : rows) {
        if (row.size() < 7) continue;
        TournamentMatchState m;
        m.tournament_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        m.round = static_cast<uint8_t>(std::atoi(row[1].c_str()));
        m.match_index = static_cast<uint8_t>(std::atoi(row[2].c_str()));
        m.team1_guild_id = static_cast<uint32_t>(std::atoi(row[3].c_str()));
        m.team2_guild_id = static_cast<uint32_t>(std::atoi(row[4].c_str()));
        m.winner_guild_id = static_cast<uint32_t>(std::atoi(row[5].c_str()));
        m.completed = std::atoi(row[6].c_str()) != 0;
        tournament_matches_.push_back(m);
    }
}

void SecondarySystem::SaveTournamentToDb(const TournamentState& tournament) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO phase6_tournaments "
        "(tournament_id, name, state, max_teams, prize_gold, registration_end, current_round, "
        "min_team_size, max_team_size, winner_guild_id, prize_claimed) VALUES ("
        + std::to_string(tournament.id) + ", '"
        + SqlEscape(tournament.name) + "', "
        + std::to_string(tournament.state) + ", "
        + std::to_string(tournament.max_teams) + ", "
        + std::to_string(tournament.prize_gold) + ", "
        + std::to_string(tournament.registration_end) + ", "
        + std::to_string(tournament.current_round) + ", "
        + std::to_string(tournament.min_team_size) + ", "
        + std::to_string(tournament.max_team_size) + ", "
        + std::to_string(tournament.winner_guild_id) + ", "
        + std::to_string(tournament.prize_claimed ? 1 : 0) + ")");
}

void SecondarySystem::SaveTournamentRegistrationToDb(const TournamentRegistration& reg) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO phase6_tournament_registrations "
        "(tournament_id, guild_id, guild_name, team_leader_id, seed, eliminated) VALUES ("
        + std::to_string(reg.tournament_id) + ", "
        + std::to_string(reg.guild_id) + ", '"
        + SqlEscape(reg.guild_name) + "', "
        + std::to_string(reg.team_leader_id) + ", "
        + std::to_string(reg.seed) + ", "
        + std::to_string(reg.eliminated ? 1 : 0) + ")");
}

void SecondarySystem::DeleteTournamentRegistrationFromDb(uint32_t tournament_id, uint32_t guild_id) {
    if (!db_) return;
    db_->Execute(
        "DELETE FROM phase6_tournament_registrations WHERE tournament_id="
        + std::to_string(tournament_id) + " AND guild_id=" + std::to_string(guild_id));
}

void SecondarySystem::SaveTournamentMatchToDb(const TournamentMatchState& match) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO phase6_tournament_matches "
        "(tournament_id, round_num, match_index, team1_guild_id, team2_guild_id, "
        "winner_guild_id, completed) VALUES ("
        + std::to_string(match.tournament_id) + ", "
        + std::to_string(match.round) + ", "
        + std::to_string(match.match_index) + ", "
        + std::to_string(match.team1_guild_id) + ", "
        + std::to_string(match.team2_guild_id) + ", "
        + std::to_string(match.winner_guild_id) + ", "
        + std::to_string(match.completed ? 1 : 0) + ")");
}

void SecondarySystem::DeleteTournamentMatchesFromDb(uint32_t tournament_id) {
    if (!db_) return;
    db_->Execute(
        "DELETE FROM phase6_tournament_matches WHERE tournament_id="
        + std::to_string(tournament_id));
}

SecondarySystem::TournamentState* SecondarySystem::FindTournament(uint32_t tournament_id) {
    for (auto& t : tournaments_)
        if (t.id == tournament_id) return &t;
    return nullptr;
}

bool SecondarySystem::IsGuildRegistered(uint32_t tournament_id, uint32_t guild_id) const {
    for (const auto& r : tournament_regs_)
        if (r.tournament_id == tournament_id && r.guild_id == guild_id) return true;
    return false;
}

void SecondarySystem::SyncTournamentRegisteredCount(TournamentState& tournament) {
    uint16_t count = 0;
    for (const auto& r : tournament_regs_)
        if (r.tournament_id == tournament.id) count++;
    tournament.registered = count;
}

uint32_t SecondarySystem::SecondsUntilTournamentStart(uint64_t registration_end) {
    if (registration_end == 0) return 0;
    const uint64_t now = static_cast<uint64_t>(time(nullptr));
    if (registration_end <= now) return 0;
    const uint64_t delta = registration_end - now;
    return delta > UINT32_MAX ? UINT32_MAX : static_cast<uint32_t>(delta);
}

void SecondarySystem::GenerateTournamentBracket(TournamentState& tournament) {
    DeleteTournamentMatchesFromDb(tournament.id);
    tournament_matches_.erase(
        std::remove_if(tournament_matches_.begin(), tournament_matches_.end(),
            [&](const TournamentMatchState& m) { return m.tournament_id == tournament.id; }),
        tournament_matches_.end());

    std::vector<TournamentRegistration*> regs;
    for (auto& r : tournament_regs_) {
        if (r.tournament_id == tournament.id) regs.push_back(&r);
    }
    if (regs.size() < 2) return;

    std::shuffle(regs.begin(), regs.end(), std::mt19937{std::random_device{}()});
    for (size_t i = 0; i < regs.size(); ++i) {
        regs[i]->seed = static_cast<uint16_t>(i + 1);
        regs[i]->eliminated = false;
        SaveTournamentRegistrationToDb(*regs[i]);
    }

    int bracket_size = 1;
    while (bracket_size < static_cast<int>(regs.size())) bracket_size *= 2;
    const int num_matches = bracket_size / 2;
    for (int i = 0; i < num_matches; ++i) {
        TournamentMatchState m;
        m.tournament_id = tournament.id;
        m.round = 1;
        m.match_index = static_cast<uint8_t>(i);
        m.team1_guild_id = (i * 2 < static_cast<int>(regs.size())) ? regs[i * 2]->guild_id : 0;
        m.team2_guild_id = (i * 2 + 1 < static_cast<int>(regs.size())) ? regs[i * 2 + 1]->guild_id : 0;
        if (m.team2_guild_id == 0) {
            m.winner_guild_id = m.team1_guild_id;
            m.completed = true;
        } else if (m.team1_guild_id == 0) {
            m.winner_guild_id = m.team2_guild_id;
            m.completed = true;
        }
        tournament_matches_.push_back(m);
        SaveTournamentMatchToDb(m);
    }
    tournament.current_round = 1;
}

void SecondarySystem::StartTournament(TournamentState& tournament) {
    if (tournament.state != 0) return;
    SyncTournamentRegisteredCount(tournament);
    if (tournament.registered < 2) return;
    tournament.state = 1;
    GenerateTournamentBracket(tournament);
    SaveTournamentToDb(tournament);
}

void SecondarySystem::AdvanceTournamentRound(TournamentState& tournament) {
    bool round_complete = true;
    int matches_in_round = 0;
    for (const auto& m : tournament_matches_) {
        if (m.tournament_id != tournament.id || m.round != tournament.current_round) continue;
        matches_in_round++;
        if (!m.completed) round_complete = false;
    }
    if (!round_complete || matches_in_round == 0) return;

    if (matches_in_round == 1) {
        CompleteTournament(tournament);
        return;
    }

    std::vector<uint32_t> winners;
    for (const auto& m : tournament_matches_) {
        if (m.tournament_id == tournament.id && m.round == tournament.current_round)
            winners.push_back(m.winner_guild_id);
    }
    const uint8_t next_round = static_cast<uint8_t>(tournament.current_round + 1);
    const int next_matches = static_cast<int>(winners.size()) / 2;
    for (int i = 0; i < next_matches; ++i) {
        TournamentMatchState nm;
        nm.tournament_id = tournament.id;
        nm.round = next_round;
        nm.match_index = static_cast<uint8_t>(i);
        nm.team1_guild_id = winners[i * 2];
        nm.team2_guild_id = winners[i * 2 + 1];
        if (nm.team2_guild_id == 0) {
            nm.winner_guild_id = nm.team1_guild_id;
            nm.completed = true;
        }
        tournament_matches_.push_back(nm);
        SaveTournamentMatchToDb(nm);
    }
    tournament.current_round = next_round;
    SaveTournamentToDb(tournament);
}

void SecondarySystem::CompleteTournament(TournamentState& tournament) {
    tournament.winner_guild_id = 0;
    for (auto it = tournament_matches_.rbegin(); it != tournament_matches_.rend(); ++it) {
        if (it->tournament_id == tournament.id && it->completed && it->winner_guild_id != 0) {
            tournament.winner_guild_id = it->winner_guild_id;
            break;
        }
    }
    tournament.state = 2;
    SaveTournamentToDb(tournament);
}

void SecondarySystem::TickTournaments() {
    const uint64_t now = static_cast<uint64_t>(time(nullptr));
    for (auto& t : tournaments_) {
        if (t.state == 0) {
            if (t.registration_end > 0 && t.registration_end <= now)
                StartTournament(t);
            continue;
        }
        if (t.state != 1) continue;

        for (auto& m : tournament_matches_) {
            if (m.tournament_id != t.id || m.completed) continue;
            if (m.team1_guild_id == 0 && m.team2_guild_id == 0) continue;
            if (m.team1_guild_id == 0) {
                m.winner_guild_id = m.team2_guild_id;
            } else if (m.team2_guild_id == 0) {
                m.winner_guild_id = m.team1_guild_id;
            } else {
                m.winner_guild_id = (std::rand() % 2 == 0) ? m.team1_guild_id : m.team2_guild_id;
            }
            m.completed = true;
            SaveTournamentMatchToDb(m);
            for (auto& r : tournament_regs_) {
                if (r.tournament_id != t.id) continue;
                if (r.guild_id != m.winner_guild_id
                    && (r.guild_id == m.team1_guild_id || r.guild_id == m.team2_guild_id)) {
                    r.eliminated = true;
                    SaveTournamentRegistrationToDb(r);
                }
            }
            break;
        }
        AdvanceTournamentRound(t);
    }
}

flatbuffers::Offset<luna::protocol::TournamentListResponse>
SecondarySystem::MakeTournamentListResponseOffset(
    flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
    uint8_t result, const std::string& message) const {
    using namespace luna::protocol;

    std::vector<flatbuffers::Offset<TournamentEntryInfo>> tour_offs;
    std::vector<flatbuffers::Offset<TournamentTeamInfo>> team_offs;
    std::vector<flatbuffers::Offset<TournamentMatchInfo>> match_offs;

    for (const auto& t : tournaments_) {
        const bool player_reg = player.guild_id != 0
            && IsGuildRegistered(t.id, player.guild_id);
        const bool can_reg = t.state == 0 && player.guild_id != 0 && !player_reg
            && t.registered < t.max_teams;
        const bool can_unreg = t.state == 0 && player_reg;
        const bool can_claim = t.state == 2 && !t.prize_claimed && player.guild_id != 0
            && t.winner_guild_id == player.guild_id;

        tour_offs.push_back(CreateTournamentEntryInfo(
            fbb, t.id, fbb.CreateString(t.name), t.state, t.registered, t.max_teams,
            t.prize_gold, t.registration_end, t.current_round, t.min_team_size, t.max_team_size,
            t.winner_guild_id, player_reg, can_reg, can_unreg, can_claim,
            SecondsUntilTournamentStart(t.registration_end)));
    }
    for (const auto& r : tournament_regs_) {
        team_offs.push_back(CreateTournamentTeamInfo(
            fbb, r.tournament_id, r.guild_id, fbb.CreateString(r.guild_name),
            r.seed, r.eliminated));
    }
    for (const auto& m : tournament_matches_) {
        match_offs.push_back(CreateTournamentMatchInfo(
            fbb, m.tournament_id, m.round, m.match_index, m.team1_guild_id, m.team2_guild_id,
            m.winner_guild_id, m.completed));
    }

    return CreateTournamentListResponse(
        fbb, result, fbb.CreateString(message), fbb.CreateVector(tour_offs),
        fbb.CreateVector(team_offs), fbb.CreateVector(match_offs), player.guild_id);
}

void SecondarySystem::SendTournamentList(NetworkLayer* network, const MapPlayerContext& player,
                                         uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = MakeTournamentListResponseOffset(fbb, player, result, message);
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_TOURNAMENT_LIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendHousingInfo(NetworkLayer* network, const MapPlayerContext& player,
                                    uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = MakeHousingInfoResponseOffset(fbb, player, result, message);
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_HOUSING_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

std::string SecondarySystem::HouseTypeName(uint8_t house_type) {
    switch (house_type) {
    case 0: return "Cozy Cottage";
    case 1: return "Town House";
    case 2: return "Grand Villa";
    default: return "House";
    }
}

SecondarySystem::HouseState* SecondarySystem::FindHouse(uint32_t house_id) {
    for (auto& h : houses_) {
        if (h.house_id == house_id) return &h;
    }
    return nullptr;
}

const SecondarySystem::HouseState* SecondarySystem::FindHouse(uint32_t house_id) const {
    for (const auto& h : houses_) {
        if (h.house_id == house_id) return &h;
    }
    return nullptr;
}

const SecondarySystem::HouseTemplate* SecondarySystem::FindHouseTemplate(uint8_t template_id) const {
    for (const auto& t : house_templates_) {
        if (t.template_id == template_id) return &t;
    }
    return nullptr;
}

const SecondarySystem::FurnitureCatalogEntry* SecondarySystem::FindFurnitureCatalog(
    uint32_t item_id) const {
    for (const auto& e : furniture_catalog_) {
        if (e.item_id == item_id) return &e;
    }
    return nullptr;
}

bool SecondarySystem::PlayerOwnsHouse(uint32_t character_id) const {
    for (const auto& h : houses_) {
        if (h.owner_id == character_id) return true;
    }
    return false;
}

SecondarySystem::HouseState* SecondarySystem::FindOwnedHouse(uint32_t character_id) {
    for (auto& h : houses_) {
        if (h.owner_id == character_id) return &h;
    }
    return nullptr;
}

std::vector<const SecondarySystem::FurnitureState*> SecondarySystem::FurnitureForHouse(
    uint32_t house_id) const {
    std::vector<const FurnitureState*> out;
    for (const auto& f : furniture_) {
        if (f.house_id == house_id) out.push_back(&f);
    }
    return out;
}

void SecondarySystem::SyncHouseFurnitureCount(HouseState& house) {
    uint16_t count = 0;
    for (const auto& f : furniture_) {
        if (f.house_id == house.house_id) ++count;
    }
    house.furniture_count = count;
}

void SecondarySystem::SaveHouseToDb(const HouseState& house) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO TB_HOUSE "
        "(HouseIdx, CharacterIdx, MapIdx, PosX, PosY, HouseType, FurnitureCount) VALUES ("
        + std::to_string(house.house_id) + ", "
        + std::to_string(house.owner_id) + ", "
        + std::to_string(house.map_id) + ", "
        + std::to_string(house.pos_x) + ", "
        + std::to_string(house.pos_y) + ", "
        + std::to_string(house.house_type) + ", "
        + std::to_string(house.furniture_count) + ")");
}

void SecondarySystem::SaveFurnitureToDb(const FurnitureState& furniture) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO TB_HOUSE_FURNITURE "
        "(FurnitureIdx, HouseIdx, ItemDBIdx, PosX, PosY, PosZ, RotY) VALUES ("
        + std::to_string(furniture.furniture_id) + ", "
        + std::to_string(furniture.house_id) + ", "
        + std::to_string(furniture.item_id) + ", "
        + std::to_string(furniture.pos_x) + ", "
        + std::to_string(furniture.pos_y) + ", 0, "
        + std::to_string(furniture.rot_y) + ")");
}

flatbuffers::Offset<luna::protocol::HousingInfoResponse>
SecondarySystem::MakeHousingInfoResponseOffset(
    flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
    uint8_t result, const std::string& message) const {
    using namespace luna::protocol;

    const uint32_t player_gold = player.gold ? *player.gold : 0;
    const bool owns = PlayerOwnsHouse(player.character_id);
    uint32_t selected = 0;
    auto sel_it = selected_house_.find(player.character_id);
    if (sel_it != selected_house_.end()) selected = sel_it->second;
    if (!selected) {
        if (const HouseState* owned = const_cast<SecondarySystem*>(this)->FindOwnedHouse(
                player.character_id))
            selected = owned->house_id;
    }

    std::vector<flatbuffers::Offset<HousingInfo>> house_offs;
    for (const auto& h : houses_) {
        if (h.owner_id != player.character_id && h.owner_id != 0) continue;
        const bool is_owner = h.owner_id == player.character_id;
        std::vector<flatbuffers::Offset<HousingFurnitureInfo>> furn_offs;
        if (is_owner) {
            for (const auto* f : FurnitureForHouse(h.house_id)) {
                furn_offs.push_back(CreateHousingFurnitureInfo(
                    fbb, f->furniture_id, f->item_id, fbb.CreateString(f->name),
                    f->pos_x, f->pos_y, f->rot_y));
            }
        }
        house_offs.push_back(CreateHousingInfo(
            fbb, h.house_id, h.owner_id, fbb.CreateString(h.name), h.map_id,
            h.pos_x, h.pos_y, h.house_type, h.furniture_count, h.max_furniture,
            is_owner, is_owner || h.owner_id == 0, fbb.CreateVector(furn_offs)));
    }

    std::vector<flatbuffers::Offset<HousingTemplateInfo>> tmpl_offs;
    for (const auto& t : house_templates_) {
        tmpl_offs.push_back(CreateHousingTemplateInfo(
            fbb, t.template_id, fbb.CreateString(t.name), t.price,
            t.max_furniture, t.map_id));
    }

    return CreateHousingInfoResponse(
        fbb, result, fbb.CreateString(message), fbb.CreateVector(house_offs),
        fbb.CreateVector(tmpl_offs), player_gold, selected, !owns);
}

void SecondarySystem::SendCashShopList(NetworkLayer* network, const MapPlayerContext& player,
                                       uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    auto resp = MakeCashShopListResponseOffset(fbb, player, result, message);
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_CASHSHOP_LIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

uint32_t SecondarySystem::EffectiveShopPrice(const ShopItem& item) {
    if (item.on_sale && item.sale_price > 0) return item.sale_price;
    return item.price;
}

const SecondarySystem::ShopItem* SecondarySystem::FindShopItem(uint32_t item_id) const {
    for (const auto& i : shop_items_) {
        if (i.item_id == item_id) return &i;
    }
    return nullptr;
}

void SecondarySystem::LoadPlayerCashFromDb(uint32_t character_id, PlayerCashShopState& out) {
    out = PlayerCashShopState{};
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT luna_points, battle_pass_level, battle_pass_xp, battle_pass_active "
        "FROM player_cash_shop WHERE character_id=" + std::to_string(character_id) + " LIMIT 1");
    if (rows.empty() || rows[0].size() < 4) return;
    out.luna_points = static_cast<uint32_t>(std::atoi(rows[0][0].c_str()));
    out.battle_pass_level = static_cast<uint16_t>(std::atoi(rows[0][1].c_str()));
    out.battle_pass_xp = static_cast<uint32_t>(std::atoi(rows[0][2].c_str()));
    out.battle_pass_active = std::atoi(rows[0][3].c_str()) != 0;
}

void SecondarySystem::SavePlayerCashToDb(uint32_t character_id, const PlayerCashShopState& state) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_cash_shop "
        "(character_id, luna_points, battle_pass_level, battle_pass_xp, battle_pass_active) "
        "VALUES (" + std::to_string(character_id) + ", "
        + std::to_string(state.luna_points) + ", "
        + std::to_string(state.battle_pass_level) + ", "
        + std::to_string(state.battle_pass_xp) + ", "
        + std::to_string(state.battle_pass_active ? 1 : 0) + ")");
}

void SecondarySystem::LoadPurchasesFromDb(uint32_t character_id) {
    if (!db_ || purchases_loaded_.count(character_id)) return;
    purchase_counts_[character_id].clear();
    auto rows = db_->Query(
        "SELECT shop_item_id, purchase_count FROM player_cashshop_purchases "
        "WHERE character_id=" + std::to_string(character_id));
    for (const auto& row : rows) {
        if (row.size() < 2) continue;
        uint32_t item_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        uint16_t count = static_cast<uint16_t>(std::atoi(row[1].c_str()));
        purchase_counts_[character_id][item_id] = count;
    }
    purchases_loaded_.insert(character_id);
}

SecondarySystem::PlayerCashShopState& SecondarySystem::EnsurePlayerCash(uint32_t character_id) {
    auto it = player_cash_.find(character_id);
    if (it == player_cash_.end()) {
        PlayerCashShopState st;
        LoadPlayerCashFromDb(character_id, st);
        player_cash_[character_id] = st;
    }
    LoadPurchasesFromDb(character_id);
    return player_cash_[character_id];
}

uint16_t SecondarySystem::GetPurchaseCount(uint32_t character_id, uint32_t item_id) const {
    auto cit = purchase_counts_.find(character_id);
    if (cit == purchase_counts_.end()) return 0;
    auto iit = cit->second.find(item_id);
    return iit == cit->second.end() ? 0 : iit->second;
}

void SecondarySystem::SetPurchaseCount(uint32_t character_id, uint32_t item_id, uint16_t count) {
    purchase_counts_[character_id][item_id] = count;
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_cashshop_purchases "
        "(character_id, shop_item_id, purchase_count) VALUES ("
        + std::to_string(character_id) + ", "
        + std::to_string(item_id) + ", "
        + std::to_string(count) + ")");
}

flatbuffers::Offset<luna::protocol::CashShopListResponse>
SecondarySystem::MakeCashShopListResponseOffset(
    flatbuffers::FlatBufferBuilder& fbb, const MapPlayerContext& player,
    uint8_t result, const std::string& message) const {
    using namespace luna::protocol;

    const PlayerCashShopState& cash = const_cast<SecondarySystem*>(this)->EnsurePlayerCash(
        player.character_id);
    const uint32_t player_gold = player.gold ? *player.gold : 0;

    std::vector<flatbuffers::Offset<CashShopItemInfo>> offs;
    for (const auto& i : shop_items_) {
        const uint32_t price = EffectiveShopPrice(i);
        const uint16_t purchased = GetPurchaseCount(player.character_id, i.item_id);
        const bool at_limit = i.max_purchase > 0 && purchased >= i.max_purchase;
        bool can_afford = !at_limit;
        if (can_afford) {
            if (i.currency_type == 0)
                can_afford = player_gold >= price;
            else
                can_afford = cash.luna_points >= price;
        }
        offs.push_back(CreateCashShopItemInfo(
            fbb, i.item_id, fbb.CreateString(i.name), fbb.CreateString(i.description),
            price, fbb.CreateString(i.category), i.currency_type, i.stack_count,
            i.max_purchase, purchased, can_afford, i.on_sale));
    }

    return CreateCashShopListResponse(
        fbb, result, fbb.CreateString(message), fbb.CreateVector(offs),
        player_gold, cash.luna_points, cash.battle_pass_level, cash.battle_pass_xp,
        1000, fbb.CreateString("Season 1"));
}

void SecondarySystem::HandlePacket(NetworkLayer* network, const MapPlayerContext& player,
                                   uint16_t type, const uint8_t* payload, size_t len) {
    using namespace luna::protocol;
    (void)len;

    if (type == PacketType_MP_FAMILY_INFO_SYN) {
        EnsureMember(player.character_id, player.name);
        SendFamilyState(network, player, 0);
        return;
    }
    if (type == PacketType_MP_FAMILY_CREATE_SYN) {
        auto req = flatbuffers::GetRoot<FamilyCreateRequest>(payload);
        std::string fname = req->family_name() ? req->family_name()->str() : "Family";
        for (const auto& f : families_) {
            if (f.name == fname) {
                SendFamilyState(network, player, 1, "Family name taken");
                network->SendPacket(PacketType_MP_FAMILY_CREATE_NACK, payload, len);
                return;
            }
        }
        MemberState& member = EnsureMember(player.character_id, player.name);
        if (member.family_id) {
            SendFamilyState(network, player, 1, "Already in a family");
            network->SendPacket(PacketType_MP_FAMILY_CREATE_NACK, payload, len);
            return;
        }
        FamilyGroup g;
        g.family_id = next_family_id_++;
        g.name = fname;
        g.master_id = player.character_id;
        families_.push_back(g);
        SaveFamilyGroupToDb(g);
        member.family_id = g.family_id;
        member.family_name = fname;
        SaveMemberToDb(member);
        SendFamilyState(network, player, 0, "Family created");
        flatbuffers::FlatBufferBuilder fbb;
        auto fam = MakeFamilyInfoOffset(fbb, player);
        auto ack = CreateFamilyResponse(fbb, 0, fam, fbb.CreateString("Family created"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_CREATE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_FAMILY_PROPOSE_SYN) {
        auto req = flatbuffers::GetRoot<FamilyProposeRequest>(payload);
        uint32_t target_id = req->target_id();
        std::string target_name = req->target_name() ? req->target_name()->str() : "Partner";
        MemberState& from = EnsureMember(player.character_id, player.name);
        if (from.relation == 2 || from.relation == 3) {
            SendFamilyState(network, player, 1, "You are already in a relationship");
            network->SendPacket(PacketType_MP_FAMILY_PROPOSE_NACK, payload, len);
            return;
        }
        MemberState& to = EnsureMember(target_id, target_name);
        if (to.relation == 2 || to.relation == 3) {
            SendFamilyState(network, player, 1, "Target is unavailable");
            network->SendPacket(PacketType_MP_FAMILY_PROPOSE_NACK, payload, len);
            return;
        }
        proposal_from_ = player.character_id;
        proposal_to_ = target_id;
        from.relation = 2;
        from.partner_id = target_id;
        from.partner_name = target_name;
        SaveMemberToDb(from);
        to.relation = 2;
        to.partner_id = player.character_id;
        to.partner_name = player.name;
        SaveMemberToDb(to);
        SendFamilyState(network, player, 0, "Proposal sent");
        flatbuffers::FlatBufferBuilder fbb;
        auto fam = MakeFamilyInfoOffset(fbb, player);
        auto ack = CreateFamilyResponse(fbb, 0, fam, fbb.CreateString("Proposal sent"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_PROPOSE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_FAMILY_ACCEPT_SYN) {
        MemberState* self = FindMember(player.character_id);
        if (!self || self->relation != 2 || self->partner_id == 0) {
            SendFamilyState(network, player, 1, "No pending proposal");
            return;
        }
        MemberState* partner = FindMember(self->partner_id);
        const uint64_t now = static_cast<uint64_t>(time(nullptr));
        self->relation = 3;
        self->married_date = now;
        SaveMemberToDb(*self);
        if (partner) {
            partner->relation = 3;
            partner->married_date = now;
            SaveMemberToDb(*partner);
        }
        proposal_from_ = 0;
        proposal_to_ = 0;
        SendFamilyState(network, player, 0, "Married!");
        flatbuffers::FlatBufferBuilder fbb;
        auto fam = MakeFamilyInfoOffset(fbb, player);
        auto ack = CreateFamilyResponse(fbb, 0, fam, fbb.CreateString("Married!"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_ACCEPT_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_FAMILY_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<FamilyActionRequest>(payload);
        uint8_t action = req->action();
        MemberState* self = FindMember(player.character_id);
        if (!self) {
            SendFamilyState(network, player, 1, "Unknown character");
            network->SendPacket(PacketType_MP_FAMILY_ACTION_NACK, payload, len);
            return;
        }
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_FAMILY_ACTION_ACK;
        switch (action) {
        case 0: // divorce
            if (self->relation != 3 || self->partner_id == 0) {
                result = 1; msg = "Not married"; ack = PacketType_MP_FAMILY_ACTION_NACK;
                break;
            }
            if (MemberState* partner = FindMember(self->partner_id)) {
                partner->relation = 4;
                partner->partner_id = 0;
                partner->partner_name.clear();
                partner->married_date = 0;
                SaveMemberToDb(*partner);
            }
            self->relation = 4;
            self->partner_id = 0;
            self->partner_name.clear();
            self->married_date = 0;
            SaveMemberToDb(*self);
            msg = "Divorced";
            break;
        case 1: // reject proposal
            if (self->relation != 2 || self->partner_id == 0) {
                result = 1; msg = "No proposal to reject"; ack = PacketType_MP_FAMILY_ACTION_NACK;
                break;
            }
            if (MemberState* partner = FindMember(self->partner_id)) {
                partner->relation = 1;
                partner->partner_id = 0;
                partner->partner_name.clear();
                SaveMemberToDb(*partner);
            }
            self->relation = 1;
            self->partner_id = 0;
            self->partner_name.clear();
            SaveMemberToDb(*self);
            proposal_from_ = 0;
            proposal_to_ = 0;
            msg = "Proposal rejected";
            break;
        case 2: // leave family
            if (!self->family_id) {
                result = 1; msg = "Not in a family"; ack = PacketType_MP_FAMILY_ACTION_NACK;
                break;
            }
            for (const auto& f : families_) {
                if (f.family_id == self->family_id && f.master_id == self->character_id) {
                    result = 1; msg = "Family master cannot leave (disband first)";
                    ack = PacketType_MP_FAMILY_ACTION_NACK;
                    goto family_action_done;
                }
            }
            self->family_id = 0;
            self->family_name.clear();
            SaveMemberToDb(*self);
            msg = "Left family";
            break;
        default:
            result = 1; msg = "Unknown family action"; ack = PacketType_MP_FAMILY_ACTION_NACK;
            break;
        }
    family_action_done:
        SendFamilyState(network, player, result, msg);
        flatbuffers::FlatBufferBuilder fbb;
        auto fam = MakeFamilyInfoOffset(fbb, player);
        auto resp = CreateFamilyResponse(fbb, result, fam, fbb.CreateString(msg));
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    if (type == PacketType_MP_PET_INFO_SYN) {
        SendPetState(network, player, 0);
        return;
    }
    if (type == PacketType_MP_PET_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<PetActionRequest>(payload);
        uint8_t action = req->action();
        PetState& pet = EnsurePet(player.character_id);
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_PET_ACTION_ACK;
        switch (action) {
        case 0:
            if (!CanSummonPet(pet)) {
                result = 1; msg = "Pet too hungry to summon (need 20+ satiation)";
                ack = PacketType_MP_PET_ACTION_NACK;
            } else {
                pet.summoned = true;
                msg = "Pet summoned";
            }
            break;
        case 1:
            pet.summoned = false;
            msg = "Pet dismissed";
            break;
        case 2: {
            const uint32_t feed_cost = PetFeedCost();
            if (player.gold && *player.gold < feed_cost) {
                result = 1; msg = "Not enough gold to feed pet"; ack = PacketType_MP_PET_ACTION_NACK;
            } else {
                if (player.gold) *player.gold -= feed_cost;
                pet.satiation = std::min<uint16_t>(100, static_cast<uint16_t>(pet.satiation + 30));
                ApplyPetExpLevel(pet, 10);
                msg = "Pet fed";
            }
            break;
        }
        case 3: {
            if (pet.evolution >= 3) {
                result = 1; msg = "Pet at max evolution"; ack = PacketType_MP_PET_ACTION_NACK;
            } else {
                const uint16_t level_req = static_cast<uint16_t>(pet.evolution * 10);
                const uint32_t evolve_cost = PetEvolveCost(pet.evolution);
                if (pet.level < level_req) {
                    result = 1;
                    msg = "Need level " + std::to_string(level_req) + " to evolve";
                    ack = PacketType_MP_PET_ACTION_NACK;
                } else if (player.gold && *player.gold < evolve_cost) {
                    result = 1; msg = "Not enough gold to evolve pet"; ack = PacketType_MP_PET_ACTION_NACK;
                } else {
                    if (player.gold) *player.gold -= evolve_cost;
                    pet.evolution = static_cast<uint8_t>(pet.evolution + 1);
                    pet.level = 1;
                    pet.exp = 0;
                    pet.max_hp = static_cast<uint16_t>(pet.max_hp + 30);
                    pet.hp = pet.max_hp;
                    msg = "Pet evolved to form " + std::to_string(pet.evolution) + "!";
                }
            }
            break;
        }
        default:
            result = 1; msg = "Unknown pet action"; ack = PacketType_MP_PET_ACTION_NACK;
            break;
        }
        SavePetToDb(player.character_id, pet);
        flatbuffers::FlatBufferBuilder fbb;
        auto info = MakePetInfoOffset(fbb, pet);
        auto resp = CreatePetResponse(fbb, result, info, fbb.CreateString(msg));
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    if (type == PacketType_MP_FARM_INFO_SYN) {
        SendFarmState(network, player, 0);
        return;
    }
    if (type == PacketType_MP_FARM_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<FarmActionRequest>(payload);
        uint8_t action = req->action();
        uint8_t plot_id = req->plot_id();
        uint32_t seed_id = req->seed_id();
        auto& plots = EnsureFarm(player.character_id);
        if (plot_id >= plots.size()) {
            SendFarmState(network, player, 1, 0, 0, "Invalid plot");
            return;
        }
        FarmPlotState& plot = plots[plot_id];
        uint32_t harvest_item = 0;
        uint16_t harvest_count = 0;
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_FARM_ACTION_ACK;

        if (action == 1) {
            const SeedDef* seed = FindSeed(seed_id ? seed_id : 1);
            if (!seed || !IsPlotEmpty(plot)) {
                result = 1; msg = "Cannot plant here"; ack = PacketType_MP_FARM_ACTION_NACK;
            } else {
                plot = FarmPlotState{};
                plot.plot_id = plot_id;
                plot.seed_id = seed->id;
                plot.plant_name = seed->name;
                plot.growth_stage = 1;
                plot.growth_timer = 0;
                plot.growth_time = seed->growth_time;
                plot.max_stages = seed->max_stages;
                plot.watered = false;
                plot.harvested = false;
                msg = "Planted " + seed->name;
                SaveFarmPlotToDb(player.character_id, plot);
            }
        } else if (action == 2) {
            if (IsPlotEmpty(plot) || IsPlotReady(plot)) {
                result = 1; msg = "Nothing to water"; ack = PacketType_MP_FARM_ACTION_NACK;
            } else {
                plot.watered = true;
                msg = "Plot watered";
                SaveFarmPlotToDb(player.character_id, plot);
            }
        } else if (action == 3) {
            const SeedDef* seed = FindSeed(plot.seed_id);
            if (!seed || !IsPlotReady(plot)) {
                result = 1; msg = "Not ready to harvest"; ack = PacketType_MP_FARM_ACTION_NACK;
            } else {
                harvest_item = seed->harvest_item;
                harvest_count = static_cast<uint16_t>(
                    seed->min_yield + (Rng()() % (seed->max_yield - seed->min_yield + 1)));
                if (player.grant_loot) player.grant_loot(harvest_item, harvest_count);
                plot = FarmPlotState{};
                plot.plot_id = plot_id;
                msg = "Harvest complete";
                SaveFarmPlotToDb(player.character_id, plot);
            }
        } else {
            result = 1; msg = "Unknown farm action"; ack = PacketType_MP_FARM_ACTION_NACK;
        }

        flatbuffers::FlatBufferBuilder fbb;
        std::vector<flatbuffers::Offset<FarmPlotInfo>> offs;
        std::vector<flatbuffers::Offset<FarmSeedInfo>> seed_offs;
        for (const auto& s : seeds_) {
            seed_offs.push_back(CreateFarmSeedInfo(fbb, s.id, fbb.CreateString(s.name),
                static_cast<uint16_t>(s.growth_time), s.harvest_item));
        }
        for (const auto& p : plots)
            offs.push_back(MakeFarmPlotOffset(fbb, p));
        auto resp = CreateFarmResponse(fbb, result, fbb.CreateVector(offs), fbb.CreateVector(seed_offs),
            harvest_item, harvest_count, fbb.CreateString(msg));
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }

    if (type == PacketType_MP_FISHING_CAST_SYN) {
        (void)flatbuffers::GetRoot<FishingCastRequest>(payload);
        if (fish_table_.empty()) {
            SendFishingResult(network, player, 1, 0, "", 0, "No fish in this area");
            return;
        }
        std::discrete_distribution<size_t> dist(fish_table_.size(), 0, fish_table_.size(),
            [this](size_t i) { return fish_table_[i].weight; });
        const auto& fish = fish_table_[dist(Rng())];
        if (player.grant_loot) player.grant_loot(fish.item_id, 1);
        SendFishingResult(network, player, 0, fish.item_id, fish.name, fish.rarity,
            "Caught " + fish.name + "!");
        return;
    }

    if (type == PacketType_MP_SIEGE_INFO_SYN) {
        SendSiegeInfo(network, player, 0);
        return;
    }
    if (type == PacketType_MP_SIEGE_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<SiegeActionRequest>(payload);
        uint8_t action = req->action();
        uint32_t territory_id = req->territory_id();
        uint32_t param = req->param();
        TerritoryState* t = FindTerritory(territory_id);
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_SIEGE_ACTION_ACK;
        const uint64_t now = static_cast<uint64_t>(time(nullptr));

        if (!t) {
            result = 1; msg = "Unknown territory"; ack = PacketType_MP_SIEGE_ACTION_NACK;
        } else switch (action) {
        case 0: // declare siege
            if (!player.guild_id) {
                result = 1; msg = "Join a guild first"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else if (t->owner_guild_id == player.guild_id) {
                result = 1; msg = "Cannot siege your own territory"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else if (t->siege_time > now && t->attacker_guild_id != 0) {
                result = 1; msg = "Siege already scheduled"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else if (player.gold && *player.gold < 5000) {
                result = 1; msg = "Need 5000g to declare siege"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else {
                if (player.gold) *player.gold -= 5000;
                t->attacker_guild_id = player.guild_id;
                t->attacker_guild_name = player.guild_name.empty() ? "Guild" : player.guild_name;
                t->siege_time = now + 3600;
                SaveTerritoryToDb(*t);
                msg = "Siege declared on " + t->name + " (1 hour)";
            }
            break;
        case 1: // set tax
            if (!player.guild_id || t->owner_guild_id != player.guild_id) {
                result = 1; msg = "Your guild does not own this territory";
                ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else {
                const uint16_t rate = static_cast<uint16_t>(std::min<uint32_t>(30, param));
                t->tax_rate = rate;
                SaveTerritoryToDb(*t);
                msg = "Tax rate set to " + std::to_string(rate) + "%";
            }
            break;
        case 2: // collect tax
            if (!player.guild_id || t->owner_guild_id != player.guild_id) {
                result = 1; msg = "Your guild does not own this territory";
                ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else if (t->tax_accumulated == 0) {
                result = 1; msg = "No tax to collect"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else {
                if (player.gold) *player.gold += t->tax_accumulated;
                msg = "Collected " + std::to_string(t->tax_accumulated) + "g tax";
                t->tax_accumulated = 0;
                SaveTerritoryToDb(*t);
            }
            break;
        case 3: // cancel siege
            if (!player.guild_id || t->attacker_guild_id != player.guild_id) {
                result = 1; msg = "Cannot cancel this siege"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            } else {
                t->attacker_guild_id = 0;
                t->attacker_guild_name.clear();
                t->siege_time = 0;
                SaveTerritoryToDb(*t);
                msg = "Siege cancelled";
            }
            break;
        default:
            result = 1; msg = "Unknown siege action"; ack = PacketType_MP_SIEGE_ACTION_NACK;
            break;
        }

        flatbuffers::FlatBufferBuilder fbb;
        auto info = MakeSiegeInfoResponseOffset(fbb, player, result, msg);
        auto resp = CreateSiegeActionResponse(fbb, result, fbb.CreateString(msg), info);
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_TOURNAMENT_LIST_SYN) {
        SendTournamentList(network, player, 0);
        return;
    }
    if (type == PacketType_MP_TOURNAMENT_REGISTER_SYN) {
        auto req = flatbuffers::GetRoot<TournamentRegisterRequest>(payload);
        uint32_t tid = req->tournament_id();
        TournamentState* t = FindTournament(tid);
        flatbuffers::FlatBufferBuilder fbb;
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_TOURNAMENT_REGISTER_ACK;

        if (!t || t->state != 0) {
            result = 1; msg = "Registration closed"; ack = PacketType_MP_TOURNAMENT_REGISTER_ACK;
        } else if (player.guild_id == 0) {
            result = 1; msg = "Join a guild first";
        } else if (IsGuildRegistered(tid, player.guild_id)) {
            result = 1; msg = "Already registered";
        } else if (t->registered >= t->max_teams) {
            result = 1; msg = "Tournament full";
        } else if (player.gold && *player.gold < 500) {
            result = 1; msg = "Need 500g registration fee";
        } else {
            if (player.gold) *player.gold -= 500;
            TournamentRegistration reg;
            reg.tournament_id = tid;
            reg.guild_id = player.guild_id;
            reg.guild_name = player.guild_name.empty() ? "Guild" : player.guild_name;
            reg.team_leader_id = player.character_id;
            tournament_regs_.push_back(reg);
            SaveTournamentRegistrationToDb(reg);
            SyncTournamentRegisteredCount(*t);
            SaveTournamentToDb(*t);
            msg = "Registered for " + t->name;
        }

        auto list = MakeTournamentListResponseOffset(fbb, player, result, msg);
        auto resp = CreateTournamentRegisterResponse(fbb, result, tid, fbb.CreateString(msg), list);
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_TOURNAMENT_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<TournamentActionRequest>(payload);
        uint8_t action = req->action();
        uint32_t tid = req->tournament_id();
        TournamentState* t = FindTournament(tid);
        std::string msg;
        uint8_t result = 0;
        uint16_t ack = PacketType_MP_TOURNAMENT_ACTION_ACK;

        if (!t) {
            result = 1; msg = "Unknown tournament"; ack = PacketType_MP_TOURNAMENT_ACTION_NACK;
        } else if (action == 0) {
            if (t->state != 0 || !IsGuildRegistered(tid, player.guild_id)) {
                result = 1; msg = "Cannot unregister"; ack = PacketType_MP_TOURNAMENT_ACTION_NACK;
            } else {
                tournament_regs_.erase(
                    std::remove_if(tournament_regs_.begin(), tournament_regs_.end(),
                        [&](const TournamentRegistration& r) {
                            return r.tournament_id == tid && r.guild_id == player.guild_id;
                        }),
                    tournament_regs_.end());
                DeleteTournamentRegistrationFromDb(tid, player.guild_id);
                SyncTournamentRegisteredCount(*t);
                SaveTournamentToDb(*t);
                msg = "Unregistered from tournament";
            }
        } else if (action == 1) {
            if (t->state != 2 || t->prize_claimed || t->winner_guild_id != player.guild_id) {
                result = 1; msg = "Prize not available"; ack = PacketType_MP_TOURNAMENT_ACTION_NACK;
            } else {
                if (player.gold) *player.gold += t->prize_gold;
                t->prize_claimed = true;
                SaveTournamentToDb(*t);
                msg = "Claimed " + std::to_string(t->prize_gold) + "g prize!";
            }
        } else {
            result = 1; msg = "Unknown tournament action"; ack = PacketType_MP_TOURNAMENT_ACTION_NACK;
        }

        flatbuffers::FlatBufferBuilder fbb;
        auto list = MakeTournamentListResponseOffset(fbb, player, result, msg);
        auto resp = CreateTournamentActionResponse(fbb, result, fbb.CreateString(msg), list);
        fbb.Finish(resp);
        network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_HOUSING_INFO_SYN) {
        SendHousingInfo(network, player);
        return;
    }
    if (type == PacketType_MP_HOUSING_ACTION_SYN) {
        auto req = flatbuffers::GetRoot<HousingActionRequest>(payload);
        uint8_t action = req->action();
        uint32_t house_id = req->house_id();
        uint8_t template_id = req->template_id();
        uint32_t furniture_item_id = req->furniture_item_id();
        float pos_x = req->pos_x();
        float pos_y = req->pos_y();

        flatbuffers::FlatBufferBuilder fbb;
        auto send_action = [&](uint8_t result, const std::string& msg, uint16_t ack) {
            auto info = MakeHousingInfoResponseOffset(fbb, player, result, msg);
            auto resp = CreateHousingActionResponse(fbb, result, action,
                fbb.CreateString(msg), info);
            fbb.Finish(resp);
            network->SendPacket(ack, fbb.GetBufferPointer(), fbb.GetSize());
        };

        if (action == 0) {
            if (PlayerOwnsHouse(player.character_id)) {
                send_action(1, "You already own a house", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            const HouseTemplate* tmpl = FindHouseTemplate(template_id);
            if (!tmpl) {
                send_action(1, "Invalid house template", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            if (!player.gold || *player.gold < tmpl->price) {
                send_action(1, "Not enough gold", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            *player.gold -= tmpl->price;
            HouseState h;
            h.house_id = next_house_id_++;
            h.owner_id = player.character_id;
            h.name = tmpl->name;
            h.map_id = tmpl->map_id;
            h.house_type = tmpl->template_id;
            h.max_furniture = tmpl->max_furniture;
            h.pos_x = static_cast<float>((h.house_id % 10) * 12.0f);
            h.pos_y = static_cast<float>((h.house_id % 7) * 8.0f);
            houses_.push_back(h);
            SaveHouseToDb(h);
            selected_house_[player.character_id] = h.house_id;
            send_action(0, "Purchased " + h.name + "!", PacketType_MP_HOUSING_ACTION_ACK);
            return;
        }
        if (action == 1) {
            HouseState* h = FindHouse(house_id);
            if (!h || h->owner_id != player.character_id) {
                send_action(1, "House not found", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            selected_house_[player.character_id] = house_id;
            send_action(0, "Entered " + h->name + " at map " + std::to_string(h->map_id),
                PacketType_MP_HOUSING_ACTION_ACK);
            return;
        }
        if (action == 2) {
            HouseState* h = FindHouse(house_id);
            if (!h || h->owner_id != player.character_id) {
                send_action(1, "Cannot decorate this house", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            if (h->furniture_count >= h->max_furniture) {
                send_action(1, "Furniture limit reached", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            const FurnitureCatalogEntry* cat = FindFurnitureCatalog(furniture_item_id);
            if (!cat) {
                send_action(1, "Unknown furniture item", PacketType_MP_HOUSING_ACTION_NACK);
                return;
            }
            FurnitureState f;
            f.furniture_id = next_furniture_id_++;
            f.house_id = house_id;
            f.item_id = furniture_item_id;
            f.name = cat->name;
            f.pos_x = pos_x;
            f.pos_y = pos_y;
            furniture_.push_back(f);
            SyncHouseFurnitureCount(*h);
            SaveFurnitureToDb(f);
            SaveHouseToDb(*h);
            selected_house_[player.character_id] = house_id;
            send_action(0, "Placed " + cat->name, PacketType_MP_HOUSING_ACTION_ACK);
            return;
        }
        send_action(1, "Unknown housing action", PacketType_MP_HOUSING_ACTION_NACK);
        return;
    }
    if (type == PacketType_MP_CASHSHOP_LIST_SYN) {
        SendCashShopList(network, player);
        return;
    }
    if (type == PacketType_MP_CASHSHOP_BUY_SYN) {
        auto req = flatbuffers::GetRoot<CashShopBuyRequest>(payload);
        uint32_t item_id = req->item_id();
        const ShopItem* item = FindShopItem(item_id);
        flatbuffers::FlatBufferBuilder fbb;
        auto send_buy = [&](uint8_t result, const std::string& msg) {
            auto list = MakeCashShopListResponseOffset(fbb, player, result, msg);
            auto resp = CreateCashShopBuyResponse(fbb, result, item_id,
                fbb.CreateString(msg), list);
            fbb.Finish(resp);
            network->SendPacket(PacketType_MP_CASHSHOP_BUY_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        };

        if (!item) {
            send_buy(1, "Item not found");
            return;
        }
        PlayerCashShopState& cash = EnsurePlayerCash(player.character_id);
        const uint32_t price = EffectiveShopPrice(*item);
        const uint16_t purchased = GetPurchaseCount(player.character_id, item_id);
        if (item->max_purchase > 0 && purchased >= item->max_purchase) {
            send_buy(1, "Purchase limit reached");
            return;
        }
        if (item->currency_type == 0) {
            if (!player.gold || *player.gold < price) {
                send_buy(1, "Not enough gold");
                return;
            }
            *player.gold -= price;
        } else {
            if (cash.luna_points < price) {
                send_buy(1, "Not enough Luna Points");
                return;
            }
            cash.luna_points -= price;
            SavePlayerCashToDb(player.character_id, cash);
        }
        if (player.grant_loot) player.grant_loot(item->item_id, item->stack_count);
        SetPurchaseCount(player.character_id, item_id, purchased + 1);
        send_buy(0, "Purchased " + item->name);
        return;
    }
}
