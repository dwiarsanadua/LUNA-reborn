#!/usr/bin/env python3
"""
Generate sandbox-compatible quest hook scripts for MapScriptRuntime.

These scripts avoid require() because LuaEngine runs in a restricted sandbox.
Output: assets/scripts/quests/quest_XXXX.lua
"""

import json
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
INPUT = BASE / "assets" / "data" / "quests_full.json"
OUTPUT_DIR = BASE / "assets" / "scripts" / "quests"


def esc(s: str) -> str:
    return s.replace("\\", "\\\\").replace('"', '\\"')


def generate_runtime_script(quest: dict) -> str:
    qid = quest["id"]
    name = quest.get("name", f"Quest {qid}")
    lines = [
        f'-- Quest {qid}: {name}',
        "-- MapScriptRuntime sandbox hook (no require)",
        "if QUEST_ID == 0 then QUEST_ID = %d end" % qid,
        'print("Quest hook: id=" .. tostring(QUEST_ID) .. " player=" .. tostring(PLAYER_ID))',
    ]
    kills = [c for c in quest.get("conditions", []) if c.get("type") == "kill"]
    if kills:
        parts = []
        for c in kills:
            parts.append("%d x%d" % (c.get("target_id", 0), c.get("count", 1)))
        lines.append('print("Objectives: kill %s")' % esc(", ".join(parts)))
    return "\n".join(lines) + "\n"


def main() -> None:
    if not INPUT.is_file():
        raise SystemExit(f"Missing {INPUT} — run parse_quests.py first")

    with open(INPUT, "r", encoding="utf-8") as f:
        data = json.load(f)

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    count = 0
    for quest in data.get("quests", []):
        qid = quest.get("id")
        if not qid:
            continue
        path = OUTPUT_DIR / f"quest_{qid:04d}.lua"
        path.write_text(generate_runtime_script(quest), encoding="utf-8")
        count += 1

    print(f"[generate_quest_runtime_lua] Wrote {count} scripts to {OUTPUT_DIR}", flush=True)


if __name__ == "__main__":
    main()
