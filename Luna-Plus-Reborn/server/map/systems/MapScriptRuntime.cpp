#include "MapScriptRuntime.hpp"
#include <scripting/LuaEngine.h>
#include <spdlog/spdlog.h>

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
    ready_ = true;
    spdlog::info("MapScriptRuntime: Lua quest/NPC runtime ready");
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
    if (!lua_->ExecuteFile(path)) {
        spdlog::warn("MapScriptRuntime: failed to run {}", path);
        return false;
    }
    spdlog::debug("MapScriptRuntime: executed {} for player {}", path, player_id);
    return true;
}
