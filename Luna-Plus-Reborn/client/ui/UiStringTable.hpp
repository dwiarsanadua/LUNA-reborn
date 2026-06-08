#pragma once
#include <string>

class UiStringTable {
public:
    static void Init(const std::string& path = "assets/interface/Windows/InterfaceMsg.bin.txt");
    static void Shutdown();

    static const char* Get(int id);
    static const char* GetOrEmpty(int id);

private:
    static bool ready_;
};
