#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CLIENT="$ROOT/build/bin/LunaPlusClient"
OUT="${1:-$ROOT/screenshots/current}"

if [[ ! -x "$CLIENT" ]]; then
  echo "Build client first: cmake --build $ROOT/build --target LunaPlusClient"
  exit 1
fi

mkdir -p "$OUT"
cd "$ROOT"
"$CLIENT" --ui-capture "$OUT"
python3 "$ROOT/tools/ui_regression/convert_tga_screenshots.py" "$OUT" 2>/dev/null || true
echo "Screenshots saved to $OUT"
