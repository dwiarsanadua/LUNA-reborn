#pragma once
#include <spdlog/spdlog.h>
#include <string>
#include <cstdint>

inline int g_tests_passed = 0;
inline int g_tests_failed = 0;

#define TEST(name, expr) do { \
    g_tests_passed++; \
    if (!(expr)) { \
        spdlog::error("  ❌ FAIL | {} | {}:{}", name, __FILE__, __LINE__); \
        g_tests_passed--; g_tests_failed++; \
    } else { \
        spdlog::info("  ✅ PASS | {}", name); \
    } \
} while(0)

#define TEST_SUITE(name) spdlog::info("\n━━━ {} ━━━", name)
#define TEST_STEP(desc) spdlog::info("  ▶ {}", desc)
