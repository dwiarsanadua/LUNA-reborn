#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BASELINE="${1:-$ROOT/screenshots/baseline}"
CURRENT="${2:-$ROOT/screenshots/current}"
THRESHOLD="${3:-0.08}"

if [[ ! -d "$BASELINE" ]]; then
  echo "Missing baseline: $BASELINE"
  echo "Run: $ROOT/tools/ui_regression/run_baseline_setup.sh"
  exit 1
fi

if [[ ! -d "$CURRENT" ]]; then
  echo "Missing current captures: $CURRENT"
  echo "Run: $ROOT/tools/ui_regression/run_capture.sh"
  exit 1
fi

python3 "$ROOT/tools/ui_regression/compare_screenshots.py" \
  --baseline "$BASELINE" \
  --current "$CURRENT" \
  --threshold "$THRESHOLD" \
  --report "$ROOT/screenshots/regression_report.json"
