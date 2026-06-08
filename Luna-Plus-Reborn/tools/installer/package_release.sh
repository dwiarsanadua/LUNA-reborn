#!/usr/bin/env bash
# Package Luna Plus Reborn release tree for installer / zip distribution.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD="${ROOT}/build"
DIST="${ROOT}/dist"
STAGE="${DIST}/release"
WIN="${DIST}/windows"

echo "=== Luna Plus Reborn release packaging ==="
cd "${ROOT}"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(sysctl -n hw.ncpu 2>/dev/null || nproc)"
python3 tools/asset_pipeline/bootstrap_phase6.py
python3 tools/asset_pipeline/generate_parity_checklist.py

rm -rf "${STAGE}"
mkdir -p "${STAGE}/bin" "${STAGE}/assets" "${STAGE}/tools"

cp build/bin/LunaPlusClient "${STAGE}/bin/" 2>/dev/null || cp build/bin/LunaPlusClient.exe "${STAGE}/bin/" 2>/dev/null || true
cp build/bin/MapServer "${STAGE}/bin/" 2>/dev/null || true
cp build/bin/AgentServer "${STAGE}/bin/" 2>/dev/null || true
cp build/bin/DistributeServer "${STAGE}/bin/" 2>/dev/null || true
cp -R assets/* "${STAGE}/assets/"
cp tools/run_local_stack.sh "${STAGE}/tools/" 2>/dev/null || true
cp docs/PARITY_CHECKLIST.md "${STAGE}/" 2>/dev/null || true

(
  cd "${DIST}"
  rm -f luna-plus-reborn-release.zip
  zip -r luna-plus-reborn-release.zip release >/dev/null
)

rm -rf "${WIN}"
mkdir -p "${WIN}"
cp -R "${STAGE}/"* "${WIN}/"
echo "  staged: ${STAGE}"
echo "  zip:    ${DIST}/luna-plus-reborn-release.zip"
echo "  windows installer input: ${WIN}"
echo "Run on Windows: makensis tools/installer/windows/LunaPlusReborn.nsi"
echo "=== packaging OK ==="
