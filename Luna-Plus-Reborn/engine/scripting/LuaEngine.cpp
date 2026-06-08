#include "LuaEngine.h"
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <map>

class LuaEngine::Impl {
public:
    sol::state lua_;
    bool initialized_ = false;

    static constexpr const char* SAFE_ENV = R"(
        _SAFE_ENV = {
            print = print,
            tostring = tostring,
            tonumber = tonumber,
            type = type,
            ipairs = ipairs,
            pairs = pairs,
            next = next,
            select = select,
            unpack = table.unpack or unpack,
            error = error,
            pcall = pcall,
            xpcall = xpcall,
            assert = assert,
            string = { byte = string.byte, char = string.char, find = string.find,
                       format = string.format, gmatch = string.gmatch, gsub = string.gsub,
                       len = string.len, lower = string.lower, match = string.match,
                       rep = string.rep, reverse = string.reverse, sub = string.sub,
                       upper = string.upper },
            math = { abs = math.abs, acos = math.acos, asin = math.asin, atan = math.atan,
                     atan2 = math.atan2, ceil = math.ceil, cos = math.cos, cosh = math.cosh,
                     deg = math.deg, exp = math.exp, floor = math.floor, fmod = math.fmod,
                     frexp = math.frexp, huge = math.huge, ldexp = math.ldexp, log = math.log,
                     max = math.max, min = math.min, modf = math.modf, pi = math.pi,
                     pow = math.pow, rad = math.rad, random = math.random,
                     randomseed = math.randomseed, sin = math.sin, sinh = math.sinh,
                     sqrt = math.sqrt, tan = math.tan, tanh = math.tanh },
            table = { concat = table.concat, insert = table.insert, maxn = table.maxn,
                      remove = table.remove, sort = table.sort }
        }
    )";

    bool Init() {
        try {
            lua_.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math,
                                sol::lib::table, sol::lib::coroutine);

            // Sandbox: run in restricted environment
            ExecuteString(SAFE_ENV);
            ExecuteString("setfenv(0, _SAFE_ENV)");

            // Remove dangerous globals
            lua_.set("dofile", sol::lua_nil);
            lua_.set("loadfile", sol::lua_nil);
            lua_.set("require", sol::lua_nil);
            lua_.set("module", sol::lua_nil);
            lua_.set("load", sol::lua_nil);
            lua_.set("collectgarbage", sol::lua_nil);
            lua_.set("newproxy", sol::lua_nil);

            initialized_ = true;
            spdlog::info("LuaEngine: initialized with sandboxed environment");
            return true;
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: init failed - {}", e.what());
            return false;
        }
    }

    bool ExecuteFile(const std::string& path) {
        if (!initialized_) return false;
        try {
            auto result = lua_.script_file(path);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("LuaEngine: file '{}' error - {}", path, err.what());
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: file '{}' exception - {}", path, e.what());
            return false;
        }
    }

    bool ExecuteString(const std::string& lua_code) {
        if (!initialized_) return false;
        try {
            auto result = lua_.script(lua_code);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("LuaEngine: script error - {}", err.what());
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: script exception - {}", e.what());
            return false;
        }
    }

    void RegisterFunc(const std::string& name, std::function<int()> func) {
        lua_.set_function(name, std::move(func));
    }

    void CallFunc(const std::string& name) {
        if (!initialized_) return;
        try {
            auto result = lua_.safe_script(name + "()", sol::script_pass_on_error);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("LuaEngine: call '{}' error - {}", name, err.what());
            }
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: call '{}' exception - {}", name, e.what());
        }
    }

    int GetInt(const std::string& name) {
        return lua_[name].get_or(0);
    }

    void SetInt(const std::string& name, int value) {
        lua_[name] = value;
    }
};

LuaEngine::LuaEngine() : impl_(std::make_unique<Impl>()) {}
LuaEngine::~LuaEngine() { Shutdown(); }

bool LuaEngine::Initialize() { return impl_->Init(); }
void LuaEngine::Shutdown() { if (impl_) impl_->initialized_ = false; }
bool LuaEngine::ExecuteFile(const std::string& path) { return impl_->ExecuteFile(path); }
bool LuaEngine::ExecuteString(const std::string& lua_code) { return impl_->ExecuteString(lua_code); }

void LuaEngine::RegisterFunctionImpl(const std::string& name, std::function<int()> func) {
    impl_->RegisterFunc(name, std::move(func));
}

void LuaEngine::CallFunction(const std::string& name) { impl_->CallFunc(name); }
int LuaEngine::GetGlobalInt(const std::string& name) { return impl_->GetInt(name); }
void LuaEngine::SetGlobalInt(const std::string& name, int value) { impl_->SetInt(name, value); }
