#!/usr/bin/env bash
# Start Agent + Distribute + MapServer(s) for local E2E testing.
# Usage:
#   ./run_local_stack.sh          # maps 51, 13, 20
#   ./run_local_stack.sh 51       # single map
#   ./run_local_stack.sh 51 13    # selected maps
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
BIN="${BUILD}/bin"
LOG_DIR="${ROOT}/logs"
mkdir -p "$LOG_DIR"

if [[ ! -x "${BIN}/AgentServer" ]]; then
  echo "Building servers..."
  cmake --build "$BUILD" --target AgentServer DistributeServer MapServer -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
fi

cleanup() {
  jobs -p | xargs -r kill 2>/dev/null || true
}
trap cleanup EXIT INT TERM

echo "Starting AgentServer :8100"
"${BIN}/AgentServer" >"${LOG_DIR}/agent.log" 2>&1 &
sleep 0.5

echo "Starting DistributeServer :8300"
"${BIN}/DistributeServer" 8300 >"${LOG_DIR}/distribute.log" 2>&1 &
sleep 0.5

if [[ $# -eq 0 ]]; then
  MAP_LIST=(51 13 20)
else
  MAP_LIST=("$@")
fi

echo ""
echo "Stack running:"
echo "  Agent      -> 127.0.0.1:8100"
echo "  Distribute -> 127.0.0.1:8300"
for MAP_ID in "${MAP_LIST[@]}"; do
  MAP_PORT=$((8200 + MAP_ID))
  echo "Starting MapServer map ${MAP_ID} :${MAP_PORT}"
  "${BIN}/MapServer" "${MAP_ID}" "${MAP_PORT}" >"${LOG_DIR}/map_${MAP_ID}.log" 2>&1 &
  sleep 0.3
  echo "  Map ${MAP_ID}     -> 127.0.0.1:${MAP_PORT}"
done
echo "Logs: ${LOG_DIR}"
echo "Press Ctrl+C to stop."
wait
