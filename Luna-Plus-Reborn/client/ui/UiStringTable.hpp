#pragma once
#include <string>
#include <vector>
#include <cstdarg>

struct StringEntry {
    int id;
    std::string text;
};

class UiStringTable {
public:
    static void Init(const std::string& path = "assets/interface/Windows/InterfaceMsg.bin.txt");
    static void Shutdown();

    static const char* Get(int id);
    static const char* GetOrEmpty(int id);
    static std::string Format(int id, ...);
    static std::string FormatV(int id, va_list args);
    static bool Has(int id);
    static int Count();
    static std::vector<StringEntry> GetAll();
    static void DumpMissing();

private:
    static bool ready_;
};
