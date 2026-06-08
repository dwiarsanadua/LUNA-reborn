#!/usr/bin/env python3
"""Build quest data and Lua hooks from legacy QuestScript sources."""

import subprocess
import sys
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
PARSER = BASE / "tools" / "data_parser" / "parse_quests.py"
FSM_GEN = BASE / "tools" / "data_parser" / "generate_quest_fsm.py"
RUNTIME_GEN = BASE / "tools" / "data_parser" / "generate_quest_runtime_lua.py"
OLD_SCRIPT = BASE.parent / "Luna-Plus-Old" / "NEW_LUNA" / "data" / "QuestScript.bin.txt"


def main() -> int:
    if not OLD_SCRIPT.is_file():
        print(f"Legacy quest source missing: {OLD_SCRIPT}")
        print("Skipping quest pipeline (seed quests still available).")
        return 0

    steps = [PARSER, FSM_GEN, RUNTIME_GEN]
    for script in steps:
        if not script.is_file():
            print(f"Missing tool: {script}")
            return 1
        print(f"Running {script.name}...", flush=True)
        subprocess.run([sys.executable, str(script)], check=True)

    quests_json = BASE / "assets" / "data" / "quests_full.json"
    runtime_dir = BASE / "assets" / "scripts" / "quests"
    fsm_dir = BASE / "assets" / "data" / "quest_fsms"
    print(
        f"Quest pipeline OK: {quests_json.name}, "
        f"{len(list(runtime_dir.glob('*.lua')))} runtime scripts, "
        f"{len(list(fsm_dir.glob('*.lua')))} FSM scripts"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
