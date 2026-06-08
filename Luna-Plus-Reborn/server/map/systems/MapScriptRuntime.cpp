#include "MapScriptRuntime.hpp"
#include <scripting/LuaEngine.h>
#include <spdlog/spdlog.h>
#include <filesystem>

namespace fs = std::filesystem;

MapScriptRuntime::MapScriptRuntime() = default;
MapScriptRuntime::~MapScriptRuntime() { Shutdown(); }

bool MapScriptRuntime::Initialize() {
    lua_ = std::make_unique<LuaEngine>();
    if (!lua_->Initialize()) {
        spdlog::warn("MapScriptRuntime: LuaEngine init failed (sol2 may be unavailable)");
        lua_.reset();
        ready_ = false;
        return false;
    }

    std::vector<std::string> roots = {
        "engine/scripting/stdlib",
        "assets/scripts",
        "assets/data/quest_fsms",
    };
    lua_->SetScriptRoots(roots);

    const char* fsm_paths[] = {
        "engine/scripting/stdlib/fsm_engine.lua",
        "assets/scripts/fsm_engine.lua",
    };
    for (const char* path : fsm_paths) {
        if (fs::exists(path) && lua_->PreloadModule("fsm_engine", path)) break;
    }

    ready_ = true;
    spdlog::info("MapScriptRuntime: quest/NPC runtime ready ({})",
        lua_->UsesLuaJit() ? "LuaJIT" : "Lua");
    return true;
}

void MapScriptRuntime::Shutdown() {
    if (lua_) {
        lua_->Shutdown();
        lua_.reset();
    }
    ready_ = false;
}

bool MapScriptRuntime::RunScript(const std::string& path, uint32_t player_id, uint32_t quest_id) {
    if (!ready_ || !lua_ || path.empty()) return false;
    lua_->SetGlobalInt("PLAYER_ID", static_cast<int>(player_id));
    lua_->SetGlobalInt("QUEST_ID", static_cast<int>(quest_id));
    if (!fs::exists(path)) {
        spdlog::debug("MapScriptRuntime: script missing {}", path);
        return false;
    }
    if (!lua_->ExecuteFile(path)) {
        spdlog::warn("MapScriptRuntime: failed to run {}", path);
        return false;
    }
    spdlog::debug("MapScriptRuntime: executed {} for player {}", path, player_id);
    return true;
}

bool MapScriptRuntime::RunQuestFsm(uint32_t quest_id, uint32_t player_id, const char* event,
                                   uint32_t param0, uint32_t param1) {
    if (!ready_ || !lua_ || !event) return false;

    char mod[32];
    snprintf(mod, sizeof(mod), "quest_%04u", quest_id);

    lua_->SetGlobalInt("PLAYER_ID", static_cast<int>(player_id));
    lua_->SetGlobalInt("QUEST_ID", static_cast<int>(quest_id));

    char bind[128];
    snprintf(bind, sizeof(bind), "quest_%u = require('%s')", quest_id, mod);
    if (!lua_->ExecuteString(bind)) return false;

    char dispatch[256];
    snprintf(dispatch, sizeof(dispatch),
        "if quest_%u and quest_%u.dispatch then quest_%u:dispatch('%s', "
        "{target_id=%u, item_id=%u, npc_id=%u}) end",
        quest_id, quest_id, quest_id, event, param0, param1,
        param0 ? param0 : param1);
    return lua_->ExecuteString(dispatch);
}
