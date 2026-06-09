#include "test_harness.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
void RunFormulaTests(); void RunSystemsTests(); void RunNetworkTests(); void RunDatabaseTests(); void RunPerformanceTests(); void RunVisualTests();
int main() {
    spdlog::set_default_logger(spdlog::stdout_color_mt("test_suite"));
    spdlog::set_level(spdlog::level::info);
    spdlog::info(""); spdlog::info("╔══════════════════════════════════════════════╗");
    spdlog::info("║    LUNA+ Reborn — Automated Test Suite      ║");
    spdlog::info("╚══════════════════════════════════════════════╝"); spdlog::info("");
    int ip = g_tests_passed, ifa = g_tests_failed;
    RunFormulaTests(); RunSystemsTests(); RunNetworkTests(); RunDatabaseTests(); RunPerformanceTests(); RunVisualTests();
    int total = (g_tests_passed - ip) + (g_tests_failed - ifa);
    spdlog::info(""); spdlog::info("╔══════════════════════════════════════════════╗");
    spdlog::info("║           FINAL TEST RESULTS                ║");
    spdlog::info("╠══════════════════════════════════════════════╣");
    spdlog::info("║  Total tests:  {:>3}                        ║", total);
    spdlog::info("║  Passed:       {:>3}                        ║", g_tests_passed);
    spdlog::info("║  Failed:       {:>3}                        ║", g_tests_failed);
    if (g_tests_failed == 0) spdlog::info("║  STATUS: ✅ ALL TESTS PASSED               ║");
    else spdlog::info("║  STATUS: ❌ {} TEST(S) FAILED              ║", g_tests_failed);
    spdlog::info("╚══════════════════════════════════════════════╝"); spdlog::info("");
    return g_tests_failed > 0 ? 1 : 0;
}
