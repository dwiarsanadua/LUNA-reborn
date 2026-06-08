#include "LuaEngine.h"
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <vector>
#include <map>
#include <functional>

namespace fs = std::filesystem;

class LuaEngine::Impl {
public:
    sol::state lua_;
    bool initialized_ = false;
    bool uses_luajit_ = false;
    std::vector<std::string> script_roots_;

    bool Init() {
        try {
            lua_.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math,
                                sol::lib::table, sol::lib::package);

            lua_["dofile"] = sol::lua_nil;
            lua_["loadfile"] = sol::lua_nil;
            lua_["load"] = sol::lua_nil;
            lua_["collectgarbage"] = sol::lua_nil;
            lua_["newproxy"] = sol::lua_nil;

            InstallSafeRequire();

#if defined(LUNA_LUA_JIT) && LUNA_LUA_JIT
            uses_luajit_ = true;
            spdlog::info("LuaEngine: LuaJIT backend active");
#else
            uses_luajit_ = false;
            spdlog::info("LuaEngine: Lua 5.x backend active (LuaJIT optional at build time)");
#endif
            initialized_ = true;
            return true;
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: init failed - {}", e.what());
            return false;
        }
    }

    void InstallSafeRequire() {
        sol::table package_tbl = lua_["package"];
        if (!package_tbl.valid()) {
            package_tbl = lua_.create_table();
            lua_["package"] = package_tbl;
        }
        sol::table loaded = package_tbl["loaded"];
        if (!loaded.valid()) {
            loaded = lua_.create_table();
            package_tbl["loaded"] = loaded;
        }

        lua_.set_function("require", [this, loaded](const std::string& mod) mutable -> sol::object {
            sol::object cached = loaded[mod];
            if (cached.valid() && cached.get_type() != sol::type::lua_nil) {
                return cached;
            }
            for (const auto& root : script_roots_) {
                fs::path candidate = fs::path(root) / (mod + ".lua");
                if (!fs::exists(candidate)) continue;
                auto result = lua_.script_file(candidate.string(), sol::script_pass_on_error);
                if (!result.valid()) {
                    sol::error err = result;
                    throw sol::error(err.what());
                }
                sol::object value = result;
                loaded[mod] = value;
                return value;
            }
            throw sol::error("module not found: " + mod);
        });
    }

    bool PreloadModuleFile(const std::string& module_name, const std::string& file_path) {
        if (!initialized_ || !fs::exists(file_path)) return false;
        try {
            auto result = lua_.script_file(file_path, sol::script_pass_on_error);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("LuaEngine: preload '{}' failed - {}", module_name, err.what());
                return false;
            }
            sol::table package_tbl = lua_["package"];
            sol::table loaded = package_tbl["loaded"];
            loaded[module_name] = result;
            spdlog::info("LuaEngine: preloaded module '{}'", module_name);
            return true;
        } catch (const std::exception& e) {
            spdlog::error("LuaEngine: preload '{}' exception - {}", module_name, e.what());
            return false;
        }
    }

    bool ExecuteFile(const std::string& path) {
        if (!initialized_) return false;
        try {
            auto result = lua_.script_file(path, sol::script_pass_on_error);
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
            auto result = lua_.script(lua_code, sol::script_pass_on_error);
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

void LuaEngine::SetScriptRoots(const std::vector<std::string>& roots) {
    impl_->script_roots_ = roots;
    if (impl_->initialized_) impl_->InstallSafeRequire();
}

bool LuaEngine::PreloadModule(const std::string& module_name, const std::string& file_path) {
    return impl_->PreloadModuleFile(module_name, file_path);
}

bool LuaEngine::UsesLuaJit() const {
    return impl_ && impl_->uses_luajit_;
}
