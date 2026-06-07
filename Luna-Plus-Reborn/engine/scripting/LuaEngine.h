// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <memory>
#include <functional>

class LuaEngine {
public:
    LuaEngine();
    ~LuaEngine();

    bool Initialize();
    void Shutdown();
    bool ExecuteFile(const std::string& path);
    bool ExecuteString(const std::string& lua_code);

    template<typename Func>
    void RegisterFunction(const std::string& name, Func func) {
        RegisterFunctionImpl(name,
            std::function<int()>([f = std::move(func)]() mutable -> int {
                if constexpr (std::is_void_v<decltype(f())>) {
                    f();
                    return 0;
                } else {
                    return static_cast<int>(f());
                }
            }));
    }

    void CallFunction(const std::string& name);
    int GetGlobalInt(const std::string& name);
    void SetGlobalInt(const std::string& name, int value);

private:
    void RegisterFunctionImpl(const std::string& name, std::function<int()> func);

    class Impl;
    std::unique_ptr<Impl> impl_;
};
