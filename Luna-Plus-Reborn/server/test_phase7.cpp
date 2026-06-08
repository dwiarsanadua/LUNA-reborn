// Phase 7 integration test — distribution gates
#include <cstdio>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

static int Run(const char* cmd) {
    printf("  > %s\n", cmd);
    return std::system(cmd);
}

int main() {
    printf("=== Phase 7 Distribution Test ===\n");

    if (Run("python3 tools/asset_pipeline/bootstrap_phase7.py") != 0) {
        printf("FAIL: bootstrap_phase7.py\n");
        return 1;
    }
    printf("OK: bootstrap_phase7\n");

    if (Run("python3 tools/asset_pipeline/validate_assets.py --phase7") != 0) {
        printf("FAIL: validate_assets --phase7\n");
        return 1;
    }
    printf("OK: validate_assets --phase7\n");

    if (Run("python3 tools/asset_pipeline/test_bspatch.py") != 0) {
        printf("FAIL: test_bspatch.py\n");
        return 1;
    }
    printf("OK: test_bspatch\n");

    if (!fs::exists("docs/PARITY_CHECKLIST.md")) {
        printf("FAIL: missing docs/PARITY_CHECKLIST.md\n");
        return 1;
    }
    if (!fs::exists("third_party/bsdiff/bspatch.c")) {
        printf("FAIL: missing third_party/bsdiff/bspatch.c\n");
        return 1;
    }
    if (!fs::exists("tools/installer/windows/LunaPlusReborn.nsi")) {
        printf("FAIL: missing Windows NSIS installer script\n");
        return 1;
    }

    printf("=== Phase 7 test: PASS ===\n");
    return 0;
}
