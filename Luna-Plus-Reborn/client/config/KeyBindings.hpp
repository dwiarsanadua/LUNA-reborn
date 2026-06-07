#pragma once
#include <string>
#include <vector>
#include <functional>

class KeyBindings {
public:
    struct BindingMod {
        static constexpr int NONE = 0;
        static constexpr int CTRL = 1;
        static constexpr int SHIFT = 2;
        static constexpr int ALT = 4;
    };

    struct Binding {
        std::string action;
        std::string display_name;
        int primary_key = 0;
        int primary_mods = 0;
        int secondary_key = 0;
        int secondary_mods = 0;
        std::string category;
        std::function<void()> handler;
    };

    static void Init();
    static void LoadFromConfig();
    static void SaveToConfig();

    static void Register(const std::string& action, const std::string& display_name,
                         int default_key, int default_mods = 0,
                         const std::string& category = "General");

    static void Bind(int key, int mods);
    static void Unbind(const std::string& action);

    static bool IsPressed(const std::string& action);
    static const Binding* GetBinding(const std::string& action);

    static std::vector<Binding> GetBindingsByCategory(const std::string& category);
    static std::vector<std::string> GetCategories();

    static void SetHandler(const std::string& action, std::function<void()> handler);

private:
    static std::vector<Binding> bindings_;
    static std::unordered_map<std::string, int> binding_index_;
    static int GetActionIndex(const std::string& action);
};
