#pragma once
#include <string>
#include <memory>

class LuaEngine;

class MapScriptRuntime {
public:
    MapScriptRuntime();
    ~MapScriptRuntime();

    bool Initialize();
    void Shutdown();
    bool RunScript(const std::string& path, uint32_t player_id, uint32_t quest_id = 0);
    bool RunQuestFsm(uint32_t quest_id, uint32_t player_id, const char* event,
                     uint32_t param0 = 0, uint32_t param1 = 0);

private:
    std::unique_ptr<LuaEngine> lua_;
    bool ready_ = false;
};
