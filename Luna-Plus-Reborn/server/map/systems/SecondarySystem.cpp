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
    shop_items_.clear();
    seeds_ = DefaultSeeds();
    farm_tick_timer_ = 0.0f;

    fish_table_.push_back({21000001, "Common Carp", 0, 1.0f});
    fish_table_.push_back({21000002, "Silver Trout", 1, 1.5f});
    fish_table_.push_back({21000003, "Golden Koi", 2, 2.0f});
    territories_.push_back({1, "Alker Harbor", 0, "", 10, 0});
    territories_.push_back({2, "Red Orc Outpost", 0, "", 12, 0});
    territories_.push_back({3, "Siege Castle", 0, "", 15, 0});
    tournaments_.push_back({1, "Weekly Arena", 0, 0, 8, 5000});
    tournaments_.push_back({2, "Guild Championship", 0, 0, 16, 15000});
    shop_items_.push_back({1001, "Health Potion", 50, "Consumables"});
    shop_items_.push_back({1002, "Mana Potion", 50, "Consumables"});
    shop_items_.push_back({201, "Pet Summon Scroll", 200, "Pets"});
    shop_items_.push_back({301, "Costume Box", 500, "Cosmetics"});
    shop_items_.push_back({401, "Fishing Bait x10", 30, "Misc"});

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
        "SELECT territory_id, name, owner_guild_id, owner_guild_name, tax_rate "
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
            territories_.push_back(t);
        }
    }

    rows = db_->Query("SELECT item_id, name, price, category FROM phase6_shop_items LIMIT 64");
    if (!rows.empty()) {
        shop_items_.clear();
        for (const auto& row : rows) {
            if (row.size() < 4) continue;
            ShopItem i;
            i.item_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
            i.name = row[1];
            i.price = static_cast<uint32_t>(std::atoi(row[2].c_str()));
            i.category = row[3];
            shop_items_.push_back(i);
        }
    }

    rows = db_->Query(
        "SELECT HouseIdx, CharacterIdx, MapIdx, PosX, PosY, HouseType, FurnitureCount "
        "FROM TB_HOUSE LIMIT 32");
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
        houses_.push_back(h);
    }

    spdlog::info("SecondarySystem: map {} — {} members, {} fish, {} shop items",
        map_id_, members_.size(), fish_table_.size(), shop_items_.size());
}

void SecondarySystem::Update(float dt) {
    farm_tick_timer_ += dt;
    if (farm_tick_timer_ < 5.0f) return;
    farm_tick_timer_ = 0.0f;
    for (auto& [cid, plots] : farms_) {
        TickFarmGrowth(cid, 5.0f);
        (void)plots;
    }
}

void SecondarySystem::LoadMembersFromDb() {
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT character_id, name, relation, partner_id, partner_name, family_id, family_name "
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
        if (!FindMember(m.character_id)) members_.push_back(m);
    }
}

void SecondarySystem::LoadMemberFromDb(uint32_t character_id, MemberState& out) {
    out = {};
    out.character_id = character_id;
    out.relation = 1;
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT name, relation, partner_id, partner_name, family_id, family_name "
        "FROM player_family WHERE character_id=" + std::to_string(character_id));
    if (rows.empty() || rows[0].size() < 6) return;
    const auto& row = rows[0];
    out.name = row[0];
    out.relation = static_cast<uint8_t>(std::atoi(row[1].c_str()));
    out.partner_id = static_cast<uint32_t>(std::atoi(row[2].c_str()));
    out.partner_name = row[3];
    out.family_id = static_cast<uint32_t>(std::atoi(row[4].c_str()));
    out.family_name = row[5];
}

void SecondarySystem::SaveMemberToDb(const MemberState& member) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_family "
        "(character_id, name, relation, partner_id, partner_name, family_id, family_name) VALUES ("
        + std::to_string(member.character_id) + ", '"
        + SqlEscape(member.name) + "', "
        + std::to_string(member.relation) + ", "
        + std::to_string(member.partner_id) + ", '"
        + SqlEscape(member.partner_name) + "', "
        + std::to_string(member.family_id) + ", '"
        + SqlEscape(member.family_name) + "')");
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
    if (!db_) return;
    auto rows = db_->Query(
        "SELECT pet_id, template_id, name, level, hp, max_hp, satiation, summoned "
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
    out.initialized = true;
}

void SecondarySystem::SavePetToDb(uint32_t character_id, const PetState& pet) {
    if (!db_) return;
    db_->Execute(
        "INSERT OR REPLACE INTO player_pet "
        "(character_id, pet_id, template_id, name, level, hp, max_hp, satiation, summoned) VALUES ("
        + std::to_string(character_id) + ", "
        + std::to_string(pet.pet_id) + ", "
        + std::to_string(pet.template_id) + ", '"
        + SqlEscape(pet.name) + "', "
        + std::to_string(pet.level) + ", "
        + std::to_string(pet.hp) + ", "
        + std::to_string(pet.max_hp) + ", "
        + std::to_string(pet.satiation) + ", "
        + std::to_string(pet.summoned ? 1 : 0) + ")");
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

void SecondarySystem::TickFarmGrowth(uint32_t character_id, float dt) {
    auto& plots = EnsureFarm(character_id);
    for (auto& plot : plots) {
        if (plot.seed_id == 0 || plot.harvested || !plot.watered) continue;
        plot.growth_timer += dt;
        int stage = static_cast<int>(plot.growth_timer / plot.growth_time);
        plot.growth_stage = static_cast<uint8_t>(std::min<int>(plot.max_stages, stage + 1));
        SaveFarmPlotToDb(character_id, plot);
    }
}

void SecondarySystem::SendFamilyState(NetworkLayer* network, const MapPlayerContext& player,
                                      uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;

    std::vector<flatbuffers::Offset<FamilyMemberInfo>> member_offs;
    uint32_t family_id = 0;
    std::string family_name;
    uint32_t master_id = 0;

    if (auto* self = FindMember(player.character_id)) {
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
                    fbb, m.character_id, nm, m.relation, m.partner_id, pn, m.family_id, fn));
            }
        } else {
            auto nm = fbb.CreateString(player.name);
            auto pn = fbb.CreateString(self->partner_name);
            member_offs.push_back(CreateFamilyMemberInfo(
                fbb, player.character_id, nm, self->relation, self->partner_id, pn, 0,
                fbb.CreateString("")));
        }
    } else {
        auto nm = fbb.CreateString(player.name);
        member_offs.push_back(CreateFamilyMemberInfo(
            fbb, player.character_id, nm, 1, 0, fbb.CreateString(""), 0, fbb.CreateString("")));
    }

    auto fam = CreateFamilyInfo(fbb, family_id, fbb.CreateString(family_name), master_id,
        fbb.CreateVector(member_offs));
    auto resp = CreateFamilyResponse(fbb, result, fam, fbb.CreateString(message));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_FAMILY_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendPetState(NetworkLayer* network, const MapPlayerContext& player,
                                   uint8_t result, const std::string& message) {
    using namespace luna::protocol;
    auto& pet = EnsurePet(player.character_id);
    flatbuffers::FlatBufferBuilder fbb;
    auto nm = fbb.CreateString(pet.name);
    auto info = CreatePetInfo(fbb, pet.pet_id, pet.template_id, nm, pet.level, pet.hp,
        pet.max_hp, pet.satiation, pet.summoned);
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
    for (const auto& p : plots) {
        uint8_t pct = 0;
        if (p.max_stages > 0)
            pct = static_cast<uint8_t>(std::min(100, (p.growth_stage * 100) / p.max_stages));
        offs.push_back(CreateFarmPlotInfo(fbb, p.plot_id, p.seed_id, fbb.CreateString(p.plant_name),
            p.growth_stage, p.max_stages, pct, p.watered, p.harvested));
    }
    auto resp = CreateFarmResponse(fbb, result, fbb.CreateVector(offs), harvest_item, harvest_count,
        fbb.CreateString(message));
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

void SecondarySystem::SendSiegeInfo(NetworkLayer* network, const MapPlayerContext& player) {
    using namespace luna::protocol;
    (void)player;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<SiegeTerritoryInfo>> offs;
    for (const auto& t : territories_) {
        offs.push_back(CreateSiegeTerritoryInfo(fbb, t.id, fbb.CreateString(t.name),
            t.owner_guild_id, fbb.CreateString(t.owner_guild_name), t.tax_rate, t.siege_time));
    }
    auto resp = CreateSiegeInfoResponse(fbb, 0, fbb.CreateVector(offs));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_SIEGE_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendTournamentList(NetworkLayer* network, const MapPlayerContext& player) {
    using namespace luna::protocol;
    (void)player;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<TournamentEntryInfo>> offs;
    for (const auto& t : tournaments_) {
        offs.push_back(CreateTournamentEntryInfo(fbb, t.id, fbb.CreateString(t.name), t.state,
            t.registered, t.max_teams, t.prize_gold));
    }
    auto resp = CreateTournamentListResponse(fbb, 0, fbb.CreateVector(offs));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_TOURNAMENT_LIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendHousingInfo(NetworkLayer* network, const MapPlayerContext& player) {
    using namespace luna::protocol;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<HousingInfo>> offs;
    for (const auto& h : houses_) {
        if (h.owner_id != player.character_id && h.owner_id != 0) continue;
        offs.push_back(CreateHousingInfo(fbb, h.house_id, h.map_id, h.pos_x, h.pos_y,
            h.house_type, h.furniture_count));
    }
    if (offs.empty()) {
        HouseState demo;
        demo.house_id = 1;
        demo.owner_id = player.character_id;
        demo.map_id = static_cast<uint16_t>(map_id_);
        offs.push_back(CreateHousingInfo(fbb, demo.house_id, demo.map_id, demo.pos_x, demo.pos_y,
            demo.house_type, demo.furniture_count));
    }
    auto resp = CreateHousingInfoResponse(fbb, 0, fbb.CreateVector(offs));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_HOUSING_INFO_ACK, fbb.GetBufferPointer(), fbb.GetSize());
}

void SecondarySystem::SendCashShopList(NetworkLayer* network, const MapPlayerContext& player) {
    using namespace luna::protocol;
    (void)player;
    flatbuffers::FlatBufferBuilder fbb;
    std::vector<flatbuffers::Offset<CashShopItemInfo>> offs;
    for (const auto& i : shop_items_) {
        offs.push_back(CreateCashShopItemInfo(fbb, i.item_id, fbb.CreateString(i.name),
            i.price, fbb.CreateString(i.category)));
    }
    auto resp = CreateCashShopListResponse(fbb, 0, fbb.CreateVector(offs));
    fbb.Finish(resp);
    network->SendPacket(PacketType_MP_CASHSHOP_LIST_ACK, fbb.GetBufferPointer(), fbb.GetSize());
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
        auto ack = CreateFamilyResponse(fbb, 0, 0, fbb.CreateString("Family created"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_CREATE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_FAMILY_PROPOSE_SYN) {
        auto req = flatbuffers::GetRoot<FamilyProposeRequest>(payload);
        uint32_t target_id = req->target_id();
        std::string target_name = req->target_name() ? req->target_name()->str() : "Partner";
        MemberState& from = EnsureMember(player.character_id, player.name);
        if (from.relation == 3) {
            SendFamilyState(network, player, 1, "Already married");
            network->SendPacket(PacketType_MP_FAMILY_PROPOSE_NACK, payload, len);
            return;
        }
        proposal_from_ = player.character_id;
        proposal_to_ = target_id;
        from.relation = 2;
        from.partner_id = target_id;
        from.partner_name = target_name;
        SaveMemberToDb(from);
        MemberState& to = EnsureMember(target_id, target_name);
        to.relation = 2;
        to.partner_id = player.character_id;
        to.partner_name = player.name;
        SaveMemberToDb(to);
        SendFamilyState(network, player, 0, "Proposal sent");
        flatbuffers::FlatBufferBuilder fbb;
        auto ack = CreateFamilyResponse(fbb, 0, 0, fbb.CreateString("Proposal sent"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_PROPOSE_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_FAMILY_ACCEPT_SYN) {
        if (proposal_to_ != player.character_id && proposal_from_ != player.character_id) {
            SendFamilyState(network, player, 1, "No pending proposal");
            return;
        }
        uint32_t partner_id = (player.character_id == proposal_to_) ? proposal_from_ : proposal_to_;
        MemberState* self = FindMember(player.character_id);
        MemberState* partner = FindMember(partner_id);
        if (!self || self->relation != 2) {
            SendFamilyState(network, player, 1, "Not engaged");
            return;
        }
        self->relation = 3;
        SaveMemberToDb(*self);
        if (partner) {
            partner->relation = 3;
            SaveMemberToDb(*partner);
        }
        proposal_from_ = 0;
        proposal_to_ = 0;
        SendFamilyState(network, player, 0, "Married!");
        flatbuffers::FlatBufferBuilder fbb;
        auto ack = CreateFamilyResponse(fbb, 0, 0, fbb.CreateString("Married!"));
        fbb.Finish(ack);
        network->SendPacket(PacketType_MP_FAMILY_ACCEPT_ACK, fbb.GetBufferPointer(), fbb.GetSize());
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
        case 0: pet.summoned = true; msg = "Pet summoned"; break;
        case 1: pet.summoned = false; msg = "Pet dismissed"; break;
        case 2:
            if (player.gold && *player.gold < 50) {
                result = 1; msg = "Not enough gold to feed pet"; ack = PacketType_MP_PET_ACTION_NACK;
            } else {
                if (player.gold) *player.gold -= 50;
                pet.satiation = std::min<uint16_t>(100, static_cast<uint16_t>(pet.satiation + 25));
                msg = "Pet fed";
            }
            break;
        case 3:
            if (pet.level < 50) {
                pet.level = static_cast<uint16_t>(pet.level + 1);
                pet.max_hp = static_cast<uint16_t>(pet.max_hp + 20);
                pet.hp = pet.max_hp;
                msg = "Pet evolved!";
            } else {
                result = 1; msg = "Pet max level"; ack = PacketType_MP_PET_ACTION_NACK;
            }
            break;
        default:
            result = 1; msg = "Unknown pet action"; ack = PacketType_MP_PET_ACTION_NACK;
            break;
        }
        SavePetToDb(player.character_id, pet);
        flatbuffers::FlatBufferBuilder fbb;
        auto nm = fbb.CreateString(pet.name);
        auto info = CreatePetInfo(fbb, pet.pet_id, pet.template_id, nm, pet.level, pet.hp,
            pet.max_hp, pet.satiation, pet.summoned);
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
            if (!seed || plot.seed_id != 0) {
                result = 1; msg = "Cannot plant here"; ack = PacketType_MP_FARM_ACTION_NACK;
            } else {
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
            if (plot.seed_id == 0 || plot.harvested) {
                result = 1; msg = "Nothing to water"; ack = PacketType_MP_FARM_ACTION_NACK;
            } else {
                plot.watered = true;
                TickFarmGrowth(player.character_id, plot.growth_time * 0.5f);
                msg = "Plot watered";
                SaveFarmPlotToDb(player.character_id, plot);
            }
        } else if (action == 3) {
            const SeedDef* seed = FindSeed(plot.seed_id);
            if (!seed || plot.growth_stage < plot.max_stages || plot.harvested) {
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
        for (const auto& p : plots) {
            uint8_t pct = p.max_stages ? static_cast<uint8_t>((p.growth_stage * 100) / p.max_stages) : 0;
            offs.push_back(CreateFarmPlotInfo(fbb, p.plot_id, p.seed_id, fbb.CreateString(p.plant_name),
                p.growth_stage, p.max_stages, pct, p.watered, p.harvested));
        }
        auto resp = CreateFarmResponse(fbb, result, fbb.CreateVector(offs), harvest_item, harvest_count,
            fbb.CreateString(msg));
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
        SendSiegeInfo(network, player);
        return;
    }
    if (type == PacketType_MP_TOURNAMENT_LIST_SYN) {
        SendTournamentList(network, player);
        return;
    }
    if (type == PacketType_MP_TOURNAMENT_REGISTER_SYN) {
        auto req = flatbuffers::GetRoot<TournamentRegisterRequest>(payload);
        uint32_t tid = req->tournament_id();
        TournamentState* t = nullptr;
        for (auto& entry : tournaments_) {
            if (entry.id == tid) { t = &entry; break; }
        }
        flatbuffers::FlatBufferBuilder fbb;
        if (!t || t->registered >= t->max_teams) {
            auto resp = CreateTournamentRegisterResponse(fbb, 1, tid,
                fbb.CreateString("Registration failed"));
            fbb.Finish(resp);
            network->SendPacket(PacketType_MP_TOURNAMENT_REGISTER_ACK, fbb.GetBufferPointer(),
                fbb.GetSize());
            return;
        }
        if (player.guild_id == 0) {
            auto resp = CreateTournamentRegisterResponse(fbb, 1, tid,
                fbb.CreateString("Join a guild first"));
            fbb.Finish(resp);
            network->SendPacket(PacketType_MP_TOURNAMENT_REGISTER_ACK, fbb.GetBufferPointer(),
                fbb.GetSize());
            return;
        }
        t->registered++;
        auto resp = CreateTournamentRegisterResponse(fbb, 0, tid,
            fbb.CreateString("Registered for tournament"));
        fbb.Finish(resp);
        network->SendPacket(PacketType_MP_TOURNAMENT_REGISTER_ACK, fbb.GetBufferPointer(),
            fbb.GetSize());
        return;
    }
    if (type == PacketType_MP_HOUSING_INFO_SYN) {
        SendHousingInfo(network, player);
        return;
    }
    if (type == PacketType_MP_CASHSHOP_LIST_SYN) {
        SendCashShopList(network, player);
        return;
    }
    if (type == PacketType_MP_CASHSHOP_BUY_SYN) {
        auto req = flatbuffers::GetRoot<CashShopBuyRequest>(payload);
        uint32_t item_id = req->item_id();
        const ShopItem* item = nullptr;
        for (const auto& i : shop_items_) {
            if (i.item_id == item_id) { item = &i; break; }
        }
        flatbuffers::FlatBufferBuilder fbb;
        if (!item) {
            auto resp = CreateCashShopBuyResponse(fbb, 1, item_id,
                fbb.CreateString("Item not found"));
            fbb.Finish(resp);
            network->SendPacket(PacketType_MP_CASHSHOP_BUY_ACK, fbb.GetBufferPointer(), fbb.GetSize());
            return;
        }
        if (!player.gold || *player.gold < item->price) {
            auto resp = CreateCashShopBuyResponse(fbb, 1, item_id,
                fbb.CreateString("Not enough gold"));
            fbb.Finish(resp);
            network->SendPacket(PacketType_MP_CASHSHOP_BUY_ACK, fbb.GetBufferPointer(), fbb.GetSize());
            return;
        }
        *player.gold -= item->price;
        if (player.grant_loot) player.grant_loot(item->item_id, 1);
        auto resp = CreateCashShopBuyResponse(fbb, 0, item_id,
            fbb.CreateString("Purchased " + item->name));
        fbb.Finish(resp);
        network->SendPacket(PacketType_MP_CASHSHOP_BUY_ACK, fbb.GetBufferPointer(), fbb.GetSize());
        return;
    }
}
