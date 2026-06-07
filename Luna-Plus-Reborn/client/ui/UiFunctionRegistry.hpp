#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <ui/widgets/Widget.hpp>

namespace Luna {
    using UiCallback = std::function<void(Widget*, const UIEvent&)>;

    class UiFunctionRegistry {
    public:
        static UiFunctionRegistry& Get() {
            static UiFunctionRegistry instance;
            return instance;
        }

        void Register(const std::string& name, UiCallback cb) {
            callbacks_[name] = cb;
        }

        UiCallback GetCallback(const std::string& name) {
            if (callbacks_.count(name)) return callbacks_[name];
            return nullptr;
        }

    private:
        std::unordered_map<std::string, UiCallback> callbacks_;
    };
}
