#!/usr/bin/env python3
"""Bootstrap Phase 5: schema migration, quest/trigger seeds, Lua stdlib copy."""

import json
import shutil
import sqlite3
import subprocess
import sys
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
DATA = BASE / "assets" / "data"
MAP_DB = DATA / "luna_map.db"
TRIGGER_SEED = DATA / "map_triggers_seed.json"
QUEST_SEED = DATA / "quest_templates_seed.json"
QUESTS_FULL = DATA / "quests_full.json"
FSM_ENGINE_SRC = BASE / "engine" / "scripting" / "stdlib" / "fsm_engine.lua"
FSM_ENGINE_DST = BASE / "assets" / "scripts" / "fsm_engine.lua"
MIGRATE = BASE / "tools" / "asset_pipeline" / "migrate_schema_sqlite.py"
BUILD_QUESTS = BASE / "tools" / "asset_pipeline" / "build_quest_scripts.py"

COND_TYPE = {"kill": 0, "collect": 1, "talk": 2}


def run_migration() -> None:
    subprocess.run([sys.executable, str(MIGRATE)], check=True)


def maybe_build_quests() -> None:
    if QUESTS_FULL.is_file():
        return
    if BUILD_QUESTS.is_file():
        subprocess.run([sys.executable, str(BUILD_QUESTS)], check=False)


def copy_fsm_engine() -> None:
    FSM_ENGINE_DST.parent.mkdir(parents=True, exist_ok=True)
    if FSM_ENGINE_SRC.is_file():
        shutil.copy2(FSM_ENGINE_SRC, FSM_ENGINE_DST)


def seed_triggers(cur: sqlite3.Cursor) -> int:
    if not TRIGGER_SEED.is_file():
        return 0
    blob = json.loads(TRIGGER_SEED.read_text())
    for t in blob.get("triggers", []):
        cur.execute(
            "INSERT OR REPLACE INTO map_triggers "
            "(trigger_id,map_id,trigger_type,pos_x,pos_z,radius,param0,param1,script_path,repeatable) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)",
            (
                t.get("trigger_id"),
                t.get("map_id", 51),
                t.get("type", 0),
                t.get("x", 0),
                t.get("z", 0),
                t.get("radius", 8),
                t.get("param0", 0),
                t.get("param1", 0),
                t.get("script", ""),
                1 if t.get("repeatable") else 0,
            ),
        )
    return cur.execute("SELECT COUNT(*) FROM map_triggers").fetchone()[0]


def import_quests_full(cur: sqlite3.Cursor) -> tuple[int, int]:
    if not QUESTS_FULL.is_file():
        return 0, 0

    blob = json.loads(QUESTS_FULL.read_text())
    cur.execute("DELETE FROM quest_conditions")
    cur.execute("DELETE FROM quest_strings")
    cur.execute("DELETE FROM quest_templates")

    for q in blob.get("quests", []):
        qid = q.get("id")
        if not qid:
            continue
        rewards = q.get("rewards") or {}
        items = rewards.get("items") or []
        reward_item = items[0]["item_id"] if items else 0
        reward_count = items[0]["count"] if items else 1
        cur.execute(
            "INSERT OR REPLACE INTO quest_templates "
            "(id,title,description,level_required,giver_npc_id,completer_npc_id,"
            "reward_exp,reward_gold,reward_item_id,reward_item_count) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)",
            (
                qid,
                q.get("name", "Quest"),
                q.get("description", ""),
                q.get("level_required", 1),
                q.get("npc_start_id", 0),
                q.get("npc_complete_id", q.get("npc_start_id", 0)),
                rewards.get("exp", 0),
                rewards.get("gold", 0),
                reward_item,
                reward_count,
            ),
        )
        if q.get("name") or q.get("description"):
            cur.execute(
                "INSERT INTO quest_strings (quest_id,language,title,description) VALUES (?,?,?,?)",
                (qid, "EN", q.get("name", ""), q.get("description", "")),
            )
        for cond in q.get("conditions", []):
            ctype = COND_TYPE.get(cond.get("type", ""), 0)
            target = cond.get("target_id") or cond.get("item_id") or cond.get("npc_id") or 0
            count = cond.get("count", 1)
            cur.execute(
                "INSERT INTO quest_conditions (quest_id,condition_type,target_id,target_count) "
                "VALUES (?,?,?,?)",
                (qid, ctype, target, count),
            )

    quest_count = cur.execute("SELECT COUNT(*) FROM quest_templates").fetchone()[0]
    cond_count = cur.execute("SELECT COUNT(*) FROM quest_conditions").fetchone()[0]
    return quest_count, cond_count


def seed_legacy_templates(cur: sqlite3.Cursor) -> int:
    if not QUEST_SEED.is_file() or cur.execute(
        "SELECT COUNT(*) FROM quest_templates"
    ).fetchone()[0]:
        return cur.execute("SELECT COUNT(*) FROM quest_templates").fetchone()[0]

    blob = json.loads(QUEST_SEED.read_text())
    for q in blob.get("quests", []):
        cur.execute(
            "INSERT OR REPLACE INTO quest_templates "
            "(id,title,description,level_required,giver_npc_id,completer_npc_id,"
            "reward_exp,reward_gold,reward_item_id,reward_item_count) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)",
            (
                q.get("quest_id"),
                q.get("name", "Quest"),
                "",
                q.get("min_level", 1),
                q.get("giver_npc_id", 0),
                q.get("completer_npc_id", q.get("giver_npc_id", 0)),
                q.get("reward_exp", 0),
                q.get("reward_gold", 0),
                q.get("reward_item_id", 0),
                q.get("reward_item_count", 0),
            ),
        )
    return cur.execute("SELECT COUNT(*) FROM quest_templates").fetchone()[0]


def main() -> int:
    run_migration()
    maybe_build_quests()
    copy_fsm_engine()

    conn = sqlite3.connect(MAP_DB)
    cur = conn.cursor()
    trigger_count = seed_triggers(cur)
    quest_count, cond_count = import_quests_full(cur)
    if quest_count == 0:
        quest_count = seed_legacy_templates(cur)
    conn.commit()
    conn.close()

    print(
        f"Phase 5 bootstrap OK: {trigger_count} triggers, "
        f"{quest_count} quest templates, {cond_count} conditions in {MAP_DB}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
