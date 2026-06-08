#!/usr/bin/env python3
"""Bootstrap Phase 5 server tables and seed data into luna_map.db."""

import sqlite3
import json
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
DATA = BASE / "assets" / "data"
MAP_DB = DATA / "luna_map.db"
TRIGGER_SEED = DATA / "map_triggers_seed.json"
QUEST_SEED = DATA / "quest_templates_seed.json"


def main() -> int:
    DATA.mkdir(parents=True, exist_ok=True)
    (BASE / "assets" / "scripts").mkdir(parents=True, exist_ok=True)

    conn = sqlite3.connect(MAP_DB)
    cur = conn.cursor()
    cur.executescript("""
    CREATE TABLE IF NOT EXISTS map_triggers (
        trigger_id INTEGER PRIMARY KEY,
        map_id INTEGER NOT NULL,
        trigger_type INTEGER DEFAULT 0,
        pos_x REAL, pos_z REAL, radius REAL DEFAULT 8,
        param0 INTEGER DEFAULT 0, param1 INTEGER DEFAULT 0,
        script_path TEXT, repeatable INTEGER DEFAULT 0
    );
    CREATE TABLE IF NOT EXISTS player_quests (
        character_id INTEGER NOT NULL,
        quest_id INTEGER NOT NULL,
        state INTEGER DEFAULT 0,
        progress TEXT,
        updated_at TEXT DEFAULT (datetime('now')),
        PRIMARY KEY (character_id, quest_id)
    );
    CREATE TABLE IF NOT EXISTS quest_templates (
        quest_id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        min_level INTEGER DEFAULT 1,
        giver_npc_id INTEGER DEFAULT 0,
        completer_npc_id INTEGER DEFAULT 0,
        kill_monster_id INTEGER DEFAULT 0,
        kill_count INTEGER DEFAULT 0,
        collect_item_id INTEGER DEFAULT 0,
        collect_count INTEGER DEFAULT 0,
        reward_exp INTEGER DEFAULT 0,
        reward_gold INTEGER DEFAULT 0,
        reward_item_id INTEGER DEFAULT 0,
        reward_item_count INTEGER DEFAULT 0,
        repeatable INTEGER DEFAULT 0
    );
    """)

    if TRIGGER_SEED.is_file():
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

    if QUEST_SEED.is_file():
        blob = json.loads(QUEST_SEED.read_text())
        for q in blob.get("quests", []):
            cur.execute(
                "INSERT OR REPLACE INTO quest_templates "
                "(quest_id,name,min_level,giver_npc_id,completer_npc_id,"
                "kill_monster_id,kill_count,collect_item_id,collect_count,"
                "reward_exp,reward_gold,reward_item_id,reward_item_count,repeatable) "
                "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
                (
                    q.get("quest_id"),
                    q.get("name", "Quest"),
                    q.get("min_level", 1),
                    q.get("giver_npc_id", 0),
                    q.get("completer_npc_id", q.get("giver_npc_id", 0)),
                    q.get("kill_monster_id", 0),
                    q.get("kill_count", 0),
                    q.get("collect_item_id", 0),
                    q.get("collect_count", 0),
                    q.get("reward_exp", 0),
                    q.get("reward_gold", 0),
                    q.get("reward_item_id", 0),
                    q.get("reward_item_count", 0),
                    q.get("repeatable", 0),
                ),
            )

    conn.commit()
    trigger_count = cur.execute("SELECT COUNT(*) FROM map_triggers").fetchone()[0]
    quest_count = cur.execute("SELECT COUNT(*) FROM quest_templates").fetchone()[0]
    conn.close()
    print(f"Phase 5 bootstrap OK: {trigger_count} triggers, {quest_count} quest templates in {MAP_DB}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
