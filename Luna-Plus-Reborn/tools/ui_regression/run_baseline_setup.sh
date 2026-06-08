#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BASELINE="$ROOT/screenshots/baseline"
CURRENT="$ROOT/screenshots/current"

mkdir -p "$BASELINE"

if [[ -d "$CURRENT" ]] && compgen -G "$CURRENT/*.png" > /dev/null; then
  cp -f "$CURRENT"/*.png "$BASELINE/" 2>/dev/null || true
  echo "Copied PNG screenshots from current -> baseline"
elif [[ -x "$ROOT/build/bin/LunaPlusClient" ]]; then
  "$ROOT/tools/ui_regression/run_capture.sh" "$BASELINE"
  echo "Captured baseline screenshots to $BASELINE"
else
  touch "$BASELINE/.gitkeep"
  echo "Created empty baseline dir (build client and run run_capture.sh to populate)"
fi
