#pragma once
#include <string>

class BcryptUtils {
public:
    static std::string Hash(const std::string& password, int cost = 10);
    static bool Verify(const std::string& password, const std::string& hash);
};
