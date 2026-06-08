#include "TriggerSystem.hpp"
#include "../MapServer.h"
#include "QuestSystem.h"
#include "server/shared/Database.h"
#include <ecs/components/QuestLog.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>

static float JsonNumber(const std::string& blob, const char* key, float fallback = 0) {
    std::string pat = std::string("\"") + key + "\"";
    size_t p = blob.find(pat);
    if (p == std::string::npos) return fallback;
    p = blob.find(':', p);
    if (p == std::string::npos) return fallback;
    return static_cast<float>(std::atof(blob.c_str() + p + 1));
}

static uint32_t JsonUInt(const std::string& blob, const char* key, uint32_t fallback = 0) {
    return static_cast<uint32_t>(JsonNumber(blob, key, static_cast<float>(fallback)));
}

static std::string JsonString(const std::string& blob, const char* key) {
    std::string pat = std::string("\"") + key + "\"";
    size_t p = blob.find(pat);
    if (p == std::string::npos) return {};
    p = blob.find(':', p);
    if (p == std::string::npos) return {};
    p = blob.find('"', p);
    if (p == std::string::npos) return {};
    size_t e = blob.find('"', p + 1);
    if (e == std::string::npos) return {};
    return blob.substr(p + 1, e - p - 1);
}

void TriggerSystem::LoadForMap(Database& db, int map_id) {
    triggers_.clear();
    fired_once_.clear();

    db.Execute(
        "CREATE TABLE IF NOT EXISTS map_triggers ("
        " trigger_id INTEGER PRIMARY KEY,"
        " map_id INTEGER NOT NULL,"
        " trigger_type INTEGER DEFAULT 0,"
        " pos_x REAL, pos_z REAL, radius REAL DEFAULT 8,"
        " param0 INTEGER DEFAULT 0, param1 INTEGER DEFAULT 0,"
        " script_path TEXT, repeatable INTEGER DEFAULT 0)");

    auto rows = db.Query("SELECT trigger_id,map_id,trigger_type,pos_x,pos_z,radius,param0,param1,script_path,repeatable "
                         "FROM map_triggers WHERE map_id=" + std::to_string(map_id));
    for (const auto& row : rows) {
        if (row.size() < 10) continue;
        MapTriggerDef t;
        t.trigger_id = static_cast<uint32_t>(std::atoi(row[0].c_str()));
        t.map_id = std::atoi(row[1].c_str());
        t.type = static_cast<MapTriggerType>(std::atoi(row[2].c_str()));
        t.x = static_cast<float>(std::atof(row[3].c_str()));
        t.z = static_cast<float>(std::atof(row[4].c_str()));
        t.radius = static_cast<float>(std::atof(row[5].c_str()));
        t.param0 = static_cast<uint32_t>(std::atoi(row[6].c_str()));
        t.param1 = static_cast<uint32_t>(std::atoi(row[7].c_str()));
        t.script_path = row[8];
        t.repeatable = std::atoi(row[9].c_str()) != 0;
        triggers_.push_back(t);
    }

    if (triggers_.empty())
        LoadSeedFile(map_id, "assets/data/map_triggers_seed.json");

    spdlog::info("TriggerSystem: {} triggers loaded for map {}", triggers_.size(), map_id);
}

void TriggerSystem::LoadSeedFile(int map_id, const std::string& json_path) {
    std::ifstream in(json_path);
    if (!in) return;
    std::stringstream ss;
    ss << in.rdbuf();
    std::string blob = ss.str();

    size_t pos = 0;
    while ((pos = blob.find('{', pos)) != std::string::npos) {
        size_t end = blob.find('}', pos);
        if (end == std::string::npos) break;
        std::string obj = blob.substr(pos, end - pos + 1);
        int obj_map = static_cast<int>(JsonUInt(obj, "map_id", map_id));
        if (obj_map != map_id) { pos = end + 1; continue; }

        MapTriggerDef t;
        t.trigger_id = JsonUInt(obj, "trigger_id", static_cast<uint32_t>(triggers_.size() + 1));
        t.map_id = obj_map;
        t.type = static_cast<MapTriggerType>(JsonUInt(obj, "type", 0));
        t.x = JsonNumber(obj, "x");
        t.z = JsonNumber(obj, "z");
        t.radius = JsonNumber(obj, "radius", 8.0f);
        t.param0 = JsonUInt(obj, "param0");
        t.param1 = JsonUInt(obj, "param1");
        t.script_path = JsonString(obj, "script");
        t.repeatable = JsonUInt(obj, "repeatable") != 0;
        triggers_.push_back(t);
        pos = end + 1;
    }
}

void TriggerSystem::Update(MapServer* server, entt::entity player, float player_x, float player_z) {
    if (!server || player == entt::null) return;
    for (const auto& t : triggers_) {
        if (!t.repeatable && fired_once_.count(t.trigger_id)) continue;
        float dx = player_x - t.x;
        float dz = player_z - t.z;
        if (dx * dx + dz * dz > t.radius * t.radius) continue;

        fired_once_.insert(t.trigger_id);
        server->SendTriggerNotify(t.trigger_id, static_cast<uint8_t>(t.type), t.param0, t.param1);
        switch (t.type) {
        case MapTriggerType::QuestStart:
            server->GetQuestSystem().StartQuest(server->GetRegistry(), player, t.param0);
            spdlog::info("TriggerSystem: quest {} started via trigger {}", t.param0, t.trigger_id);
            break;
        case MapTriggerType::SpawnMonster:
            server->SpawnMonster(static_cast<int>(t.param0), {t.x, 0.0f, t.z});
            break;
        case MapTriggerType::Teleport:
            server->TeleportPlayer(t.param0 ? t.param0 : static_cast<uint32_t>(server->GetMapId()),
                                   t.x, 0.0f, t.z);
            break;
        case MapTriggerType::Script:
            server->RunMapScript(t.script_path, player);
            break;
        default:
            spdlog::debug("TriggerSystem: region enter trigger {}", t.trigger_id);
            break;
        }
    }
}
