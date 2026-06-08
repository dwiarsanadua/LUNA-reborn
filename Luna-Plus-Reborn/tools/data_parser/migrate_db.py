#!/usr/bin/env python3
"""
LUNA Plus Reborn — Database Migration Tool (Phase 2.0)
Migrates game content from legacy DB to luna_game.db.

Data Flow:
  game_data.db  (raw col_NNNN, 234 tables)
       │
       ▼  create_legacy_db() — normalizes col_NNNN → named columns
  game_data_legacy.db  (16 normalized content tables)
       │
       ▼  migrate() — copies content + adds indexes
  luna_game.db  (runtime player TB_* + game content tables)

Usage:
  python3 tools/data_parser/migrate_db.py

Environment Variables:
  SOURCE_MODE=legacy   Use game_data_legacy.db (default)
  SOURCE_MODE=raw      Parse directly from game_data.db (col_NNNN)
"""

import sqlite3
import sys
import os
import json
import time
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
ASSETS_DATA = BASE / "assets" / "data"

LEGACY_SRC = ASSETS_DATA / "game_data_legacy.db"
RAW_SRC = ASSETS_DATA / "game_data.db"
LUNA_DB_DST = ASSETS_DATA / "luna_game.db"
LEGACY_OLD = BASE / "Luna-Plus-Old" / "LunaPlus" / "Data" / "game_data.db"

DEPRECATED_TABLE_PATTERNS = [
    "game_vs_",            # shader tables
    "game_bodylist_",      # avatar customization (legacy format)
    "game_facelist_",
    "game_hairlist_",
    "game_modlist_",
    "game_monster_63_",    # per-difficulty monster overrides
    "game_monster_64_",
    "game_monster_65_",
    "game_monster_limit_",
    "game_emoticon_back",
    "game_emoticon_front",
    "game_challengezonemonster",
    "game_vs_",
    "game_camerafilter",
    "game_weathereffect",
    "game_weatherevent",
    "game_filterword",
    "game_gamedesc",
    "game_soundlist",
    "game_motionlist",
    "hackshield",
    "nprotect",
    "game_boss_arach",
    "game_boss_dragonian",
    "game_boss_kierra",
    "game_boss_leostein",
    "game_boss_tarintus",
    "game_bossmonsterfilelist",
    "game_bossreward",
    "game_fieldbossdropitemlist",
    "game_fieldbosslist",
    "game_pet_costume",
    "game_pethpmpinfo",
    "game_petmotion",
    "game_petstatusinfo",
    "game_petsysteminfo",
]

RELEVANT_CONTENT_TABLES = [
    "item_templates",
    "monster_templates",
    "monster_drops",
    "npc_templates",
    "npc_positions",
    "npc_shop_entries",
    "quest_templates",
    "quest_conditions",
    "quest_strings",
    "skill_data",
    "buff_skills",
    "skill_trees",
    "map_warps",
    "map_boundaries",
    "map_data",
    "monster_spawns",
]

CONTENT_INDEXES = {
    "item_templates": [
        "CREATE INDEX IF NOT EXISTS idx_item_templates_type ON item_templates(item_type)",
        "CREATE INDEX IF NOT EXISTS idx_item_templates_subtype ON item_templates(item_subtype)",
        "CREATE INDEX IF NOT EXISTS idx_item_templates_level ON item_templates(level_required)",
        "CREATE INDEX IF NOT EXISTS idx_item_templates_name ON item_templates(name)",
    ],
    "monster_templates": [
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_level ON monster_templates(level)",
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_zone ON monster_templates(monster_type)",
        "CREATE INDEX IF NOT EXISTS idx_monster_templates_element ON monster_templates(element_type)",
    ],
    "monster_drops": [
        "CREATE INDEX IF NOT EXISTS idx_monster_drops_monster ON monster_drops(monster_id)",
        "CREATE INDEX IF NOT EXISTS idx_monster_drops_item ON monster_drops(item_id)",
    ],
    "npc_templates": [
        "CREATE INDEX IF NOT EXISTS idx_npc_templates_type ON npc_templates(npc_type)",
    ],
    "npc_positions": [
        "CREATE INDEX IF NOT EXISTS idx_npc_positions_map ON npc_positions(map_id)",
        "CREATE INDEX IF NOT EXISTS idx_npc_positions_npc ON npc_positions(npc_id)",
    ],
    "npc_shop_entries": [
        "CREATE INDEX IF NOT EXISTS idx_npc_shop_npc ON npc_shop_entries(npc_id)",
        "CREATE INDEX IF NOT EXISTS idx_npc_shop_item ON npc_shop_entries(item_id)",
    ],
    "quest_templates": [
        "CREATE INDEX IF NOT EXISTS idx_quest_templates_level ON quest_templates(level_required)",
        "CREATE INDEX IF NOT EXISTS idx_quest_templates_giver ON quest_templates(giver_npc_id)",
    ],
    "quest_conditions": [
        "CREATE INDEX IF NOT EXISTS idx_quest_conditions_quest ON quest_conditions(quest_id)",
    ],
    "quest_strings": [
        "CREATE INDEX IF NOT EXISTS idx_quest_strings_quest ON quest_strings(quest_id)",
    ],
    "skill_data": [
        "CREATE INDEX IF NOT EXISTS idx_skill_data_class ON skill_data(class_id)",
        "CREATE INDEX IF NOT EXISTS idx_skill_data_type ON skill_data(skill_type)",
        "CREATE INDEX IF NOT EXISTS idx_skill_data_level ON skill_data(level_required)",
    ],
    "buff_skills": [
        "CREATE INDEX IF NOT EXISTS idx_buff_skills_ref ON buff_skills(skill_ref_id)",
        "CREATE INDEX IF NOT EXISTS idx_buff_skills_type ON buff_skills(buff_type)",
    ],
    "skill_trees": [
        "CREATE INDEX IF NOT EXISTS idx_skill_trees_class ON skill_trees(class_id)",
        "CREATE INDEX IF NOT EXISTS idx_skill_trees_skill ON skill_trees(skill_id)",
    ],
    "map_warps": [
        "CREATE INDEX IF NOT EXISTS idx_map_warps_from ON map_warps(map_from)",
        "CREATE INDEX IF NOT EXISTS idx_map_warps_to ON map_warps(map_to)",
    ],
    "map_boundaries": [
        "CREATE INDEX IF NOT EXISTS idx_map_boundaries_a ON map_boundaries(map_a)",
        "CREATE INDEX IF NOT EXISTS idx_map_boundaries_b ON map_boundaries(map_b)",
    ],
    "monster_spawns": [
        "CREATE INDEX IF NOT EXISTS idx_monster_spawns_map ON monster_spawns(map_id)",
        "CREATE INDEX IF NOT EXISTS idx_monster_spawns_monster ON monster_spawns(monster_id)",
    ],
    "map_data": [
        "CREATE INDEX IF NOT EXISTS idx_map_data_name ON map_data(name)",
    ],
}

RAW_TABLE_MAP = {
    "item_templates": "game_itemlist",
    "monster_templates": "game_monsterlist",
    "monster_drops": None,
    "npc_templates": "game_npclist",
    "npc_positions": "game_staticnpc",
    "npc_shop_entries": "game_dealitem",
    "quest_templates": "game_questnpclist",
    "quest_conditions": None,
    "quest_strings": "game_queststring",
    "skill_data": "game_skilllist",
    "buff_skills": "game_skill_buff_list",
    "skill_trees": "game_skilltreelist",
    "map_warps": "game_mapmovelist",
    "map_boundaries": "game_mapchange",
    "map_data": None,
    "monster_spawns": None,
}


def col(r, i, default=0):
    v = r[i] if i < len(r) else None
    if v is None or (isinstance(v, str) and v.strip() == ""):
        return default
    return v


def is_deprecated(table: str) -> bool:
    for pat in DEPRECATED_TABLE_PATTERNS:
        if table.lower().startswith(pat.lower()):
            return True
    return False


def get_table_schema(db: sqlite3.Connection, table: str) -> list:
    return db.execute(f"PRAGMA table_info([{table}])").fetchall()


def copy_table(src: sqlite3.Connection, dst: sqlite3.Connection, table: str) -> int:
    schema = get_table_schema(src, table)
    if not schema:
        return 0
    col_names = [row[1] for row in schema]
    col_defs = ", ".join(
        f'"{row[1]}" {row[2]}'
        + (" PRIMARY KEY" if row[5] else "")
        + (" AUTOINCREMENT" if row[2].upper().startswith("INTEGER") and row[5] else "")
        + (f" DEFAULT {row[4]}" if row[4] is not None else "")
        + (" NOT NULL" if row[3] else "")
        for row in schema
    )
    dst.execute(f"DROP TABLE IF EXISTS [{table}]")
    dst.execute(f"CREATE TABLE [{table}] ({col_defs})")
    rows = src.execute(f"SELECT * FROM [{table}]").fetchall()
    placeholders = ", ".join("?" for _ in col_names)
    col_list = ", ".join(f'"{c}"' for c in col_names)
    dst.executemany(
        f"INSERT INTO [{table}] ({col_list}) VALUES ({placeholders})", rows
    )
    return len(rows)


def create_indexes(dst: sqlite3.Connection, table: str):
    for idx_sql in CONTENT_INDEXES.get(table, []):
        try:
            dst.execute(idx_sql)
        except sqlite3.Error as e:
            print(f"    [WARN] Index failed on {table}: {e}")


def count_table_rows(db: sqlite3.Connection, table: str) -> int:
    try:
        return db.execute(f"SELECT COUNT(*) FROM [{table}]").fetchone()[0]
    except sqlite3.Error:
        return 0


def add_migration_version(dst: sqlite3.Connection, version: str, description: str):
    dst.execute("""
        CREATE TABLE IF NOT EXISTS _migration_version (
            version TEXT PRIMARY KEY,
            applied_at TEXT NOT NULL DEFAULT (datetime('now')),
            description TEXT
        )
    """)
    dst.execute(
        "INSERT OR REPLACE INTO _migration_version (version, description) VALUES (?, ?)",
        (version, description),
    )


def extract_legacy_from_raw(raw_path: Path, dst: sqlite3.Connection) -> list:
    """
    Build game_data_legacy tables directly from raw game_data.db.
    Used when game_data_legacy.db doesn't exist but game_data.db does.
    Returns list of (table_name, row_count) tuples.
    """
    print("\n  [EXTRACT] Building normalized tables from raw game_data.db...")
    src = sqlite3.connect(str(raw_path))
    src.row_factory = sqlite3.Row

    results = []

    monster_rows = src.execute("SELECT * FROM game_monsterlist").fetchall() if RAW_TABLE_MAP["monster_templates"] else []
    item_rows = src.execute("SELECT * FROM game_itemlist").fetchall() if RAW_TABLE_MAP["item_templates"] else []
    npc_rows = src.execute("SELECT * FROM game_npclist").fetchall() if RAW_TABLE_MAP["npc_templates"] else []
    snpc_rows = src.execute("SELECT * FROM game_staticnpc").fetchall() if RAW_TABLE_MAP["npc_positions"] else []
    deal_rows = src.execute("SELECT * FROM game_dealitem").fetchall() if RAW_TABLE_MAP["npc_shop_entries"] else []
    quest_rows = src.execute("SELECT * FROM game_questnpclist").fetchall() if RAW_TABLE_MAP["quest_templates"] else []
    skill_rows = src.execute("SELECT * FROM game_skilllist").fetchall() if RAW_TABLE_MAP["skill_data"] else []
    buff_rows = src.execute("SELECT * FROM game_skill_buff_list").fetchall() if RAW_TABLE_MAP["buff_skills"] else []
    tree_rows = src.execute("SELECT * FROM game_skilltreelist").fetchall() if RAW_TABLE_MAP["skill_trees"] else []
    warp_rows = src.execute("SELECT * FROM game_mapmovelist").fetchall() if RAW_TABLE_MAP["map_warps"] else []
    change_rows = src.execute("SELECT * FROM game_mapchange").fetchall() if RAW_TABLE_MAP["map_boundaries"] else []
    qstr_rows = src.execute("SELECT * FROM game_queststring").fetchall() if RAW_TABLE_MAP["quest_strings"] else []

    # item_templates
    dst.execute("DROP TABLE IF EXISTS item_templates")
    dst.execute("""
        CREATE TABLE item_templates (
            id INTEGER PRIMARY KEY, name TEXT, item_type INTEGER DEFAULT 0,
            item_subtype INTEGER DEFAULT 0, level_required INTEGER DEFAULT 0,
            attack INTEGER DEFAULT 0, defense INTEGER DEFAULT 0,
            magic_attack INTEGER DEFAULT 0, magic_defense INTEGER DEFAULT 0,
            price_buy INTEGER DEFAULT 0, price_sell INTEGER DEFAULT 0,
            max_stack INTEGER DEFAULT 1, rarity INTEGER DEFAULT 0,
            resource_id INTEGER DEFAULT 0
        )
    """)
    seen_items = set()
    item_count = 0
    for r in item_rows:
        iid = col(r, 0)
        if iid in seen_items:
            continue
        seen_items.add(iid)
        try:
            dst.execute("""
                INSERT INTO item_templates (id,name,item_type,item_subtype,level_required,attack,defense,magic_attack,magic_defense,price_buy,price_sell,rarity,resource_id)
                VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)
            """, (iid, col(r, 1), col(r, 3, 0), col(r, 4, 0), col(r, 28, 0),
                  col(r, 10, 0), col(r, 12, 0), col(r, 11, 0), 0,
                  col(r, 19, 0), col(r, 20, 0), col(r, 4, 1), col(r, 5, 0)))
            item_count += 1
        except sqlite3.IntegrityError:
            pass
    results.append(("item_templates", item_count))

    # monster_templates
    dst.execute("DROP TABLE IF EXISTS monster_templates")
    dst.execute("""
        CREATE TABLE monster_templates (
            id INTEGER PRIMARY KEY, name TEXT, model_file TEXT,
            level INTEGER, hp INTEGER, mp INTEGER DEFAULT 0,
            attack INTEGER, defense INTEGER, speed REAL DEFAULT 1.0,
            exp_reward INTEGER DEFAULT 0, gold_min INTEGER DEFAULT 0,
            gold_max INTEGER DEFAULT 0, element_type INTEGER DEFAULT 0,
            ai_type INTEGER DEFAULT 0, aggro_range INTEGER DEFAULT 0,
            size_scale REAL DEFAULT 1.0, monster_type INTEGER DEFAULT 0
        )
    """)
    for r in monster_rows:
        dst.execute("""
            INSERT INTO monster_templates (id,name,model_file,level,hp,attack,defense,speed,exp_reward,gold_min,gold_max,element_type,ai_type,aggro_range,size_scale,monster_type)
            VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
        """, (col(r, 0), col(r, 1), col(r, 3), col(r, 11), col(r, 8),
              col(r, 12), col(r, 13), float(col(r, 17, 1.0)),
              int(float(col(r, 14, 0))), col(r, 20), col(r, 21),
              col(r, 10), col(r, 18), col(r, 19),
              float(col(r, 16, 1.0)), col(r, 18)))
    results.append(("monster_templates", len(monster_rows)))

    # monster_drops
    dst.execute("DROP TABLE IF EXISTS monster_drops")
    dst.execute("""
        CREATE TABLE monster_drops (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            monster_id INTEGER, item_id INTEGER, item_name TEXT,
            min_count INTEGER DEFAULT 1, max_count INTEGER DEFAULT 1,
            probability REAL DEFAULT 0.0, drop_table_id INTEGER DEFAULT 0
        )
    """)
    drop_count = 0
    for r in monster_rows:
        mid = col(r, 0)
        for off in range(62, 125, 7):
            if off + 6 >= len(r):
                break
            iid = col(r, off + 4, 0)
            if iid == 0:
                continue
            qty = col(r, off + 5, 1)
            prob = col(r, off + 6, 0)
            pv = (prob / 1000000.0) if isinstance(prob, (int, float)) and prob > 0 else 0.01
            dst.execute("INSERT INTO monster_drops (monster_id,item_id,item_name,min_count,max_count,probability) VALUES (?,?,?,1,?,?)",
                        (mid, iid, col(r, off + 3, ""), qty, pv))
            drop_count += 1
    results.append(("monster_drops", drop_count))

    # npc_templates
    dst.execute("DROP TABLE IF EXISTS npc_templates")
    dst.execute("""
        CREATE TABLE npc_templates (
            id INTEGER PRIMARY KEY, name TEXT,
            npc_type INTEGER DEFAULT 0, shop_type INTEGER DEFAULT 0,
            dialog_text TEXT DEFAULT ''
        )
    """)
    for r in npc_rows:
        dst.execute("INSERT INTO npc_templates (id,name,npc_type,shop_type) VALUES (?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 6, 1)))
    results.append(("npc_templates", len(npc_rows)))

    # npc_positions
    dst.execute("DROP TABLE IF EXISTS npc_positions")
    dst.execute("""
        CREATE TABLE npc_positions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER, npc_id INTEGER, name TEXT,
            npc_type INTEGER DEFAULT 0, pos_x REAL, pos_y REAL,
            pos_z REAL, rotation REAL DEFAULT 0
        )
    """)
    for r in snpc_rows:
        dst.execute("INSERT INTO npc_positions (map_id,npc_id,name,npc_type,pos_x,pos_y,pos_z,rotation) VALUES (?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 3), col(r, 4), 0, col(r, 5), col(r, 6)))
    results.append(("npc_positions", len(snpc_rows)))

    # npc_shop_entries
    dst.execute("DROP TABLE IF EXISTS npc_shop_entries")
    dst.execute("""
        CREATE TABLE npc_shop_entries (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            npc_id INTEGER, item_id INTEGER, price INTEGER DEFAULT 0,
            stock INTEGER DEFAULT -1, map_id INTEGER DEFAULT 0
        )
    """)
    shop_count = 0
    for r in deal_rows:
        npc_id = col(r, 4)
        if not isinstance(npc_id, int) or npc_id == 0:
            npc_id = col(r, 0) * 100 + 1
        for off in range(8, 80, 2):
            if off + 1 >= len(r):
                break
            iid = col(r, off + 1, 0)
            if iid == 0:
                continue
            price = col(r, off + 2, 0) if off + 2 < len(r) else 0
            dst.execute("INSERT INTO npc_shop_entries (npc_id,item_id,price,stock,map_id) VALUES (?,?,?,-1,?)",
                        (npc_id, iid, price, col(r, 0)))
            shop_count += 1
    results.append(("npc_shop_entries", shop_count))

    # quest_templates
    dst.execute("DROP TABLE IF EXISTS quest_templates")
    dst.execute("""
        CREATE TABLE quest_templates (
            id INTEGER PRIMARY KEY, title TEXT DEFAULT '',
            description TEXT DEFAULT '', level_required INTEGER DEFAULT 0,
            giver_npc_id INTEGER DEFAULT 0, completer_npc_id INTEGER DEFAULT 0,
            reward_exp INTEGER DEFAULT 0, reward_gold INTEGER DEFAULT 0,
            reward_item_id INTEGER DEFAULT 0, reward_item_count INTEGER DEFAULT 1
        )
    """)
    seen_q = set()
    for r in quest_rows:
        qid = col(r, 0)
        if qid in seen_q:
            continue
        seen_q.add(qid)
        dst.execute("INSERT INTO quest_templates (id,giver_npc_id,completer_npc_id) VALUES (?,?,?)",
                    (qid, col(r, 1), col(r, 1)))
    results.append(("quest_templates", len(seen_q)))

    # quest_conditions
    dst.execute("DROP TABLE IF EXISTS quest_conditions")
    dst.execute("""
        CREATE TABLE quest_conditions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER, condition_type INTEGER DEFAULT 0,
            target_id INTEGER DEFAULT 0, target_count INTEGER DEFAULT 1,
            map_id INTEGER DEFAULT 0, pos_x REAL DEFAULT 0,
            pos_y REAL DEFAULT 0, radius REAL DEFAULT 10
        )
    """)
    cond_count = 0
    for r in quest_rows:
        qid = col(r, 0)
        tid = col(r, 3, 0)
        if tid:
            dst.execute("INSERT INTO quest_conditions (quest_id,condition_type,target_id,target_count,pos_x,pos_y) VALUES (?,0,?,1,?,?)",
                        (qid, tid, col(r, 4, 0), col(r, 5, 0)))
            cond_count += 1
    results.append(("quest_conditions", cond_count))

    # quest_strings
    dst.execute("DROP TABLE IF EXISTS quest_strings")
    dst.execute("""
        CREATE TABLE quest_strings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER, language TEXT DEFAULT 'EN',
            title TEXT DEFAULT '', description TEXT DEFAULT ''
        )
    """)
    str_count = 0
    for r in qstr_rows:
        text = str(col(r, 0, ""))
        if "$SUBQUESTSTR" in text:
            parts = text.split()
            if len(parts) >= 2:
                try:
                    qid = int(parts[1])
                    dst.execute("INSERT INTO quest_strings (quest_id,title,description) VALUES (?,?,?)",
                                (qid, str(parts[0]), str(col(r, 5, ""))[:200]))
                    str_count += 1
                except ValueError:
                    pass
    results.append(("quest_strings", str_count))

    # skill_data
    dst.execute("DROP TABLE IF EXISTS skill_data")
    dst.execute("""
        CREATE TABLE skill_data (
            id INTEGER PRIMARY KEY, name TEXT, class_id INTEGER DEFAULT 0,
            skill_type INTEGER DEFAULT 0, level_required INTEGER DEFAULT 0,
            target_type INTEGER DEFAULT 0, range REAL DEFAULT 0,
            cost_hp INTEGER DEFAULT 0, cost_mp INTEGER DEFAULT 0,
            cooldown_ms INTEGER DEFAULT 0, damage_mult REAL DEFAULT 1.0,
            damage_fixed INTEGER DEFAULT 0, weapon_type INTEGER DEFAULT 0,
            sp_cost INTEGER DEFAULT 0
        )
    """)
    seen_skills = set()
    skill_count = 0
    for r in skill_rows:
        sid = col(r, 0)
        if sid in seen_skills:
            continue
        seen_skills.add(sid)
        try:
            dst.execute("INSERT INTO skill_data (id,name,class_id,skill_type,level_required,cost_mp,cooldown_ms,weapon_type,sp_cost) VALUES (?,?,?,?,?,?,?,?,?)",
                        (sid, col(r, 1), col(r, 2, 0), col(r, 5, 0), col(r, 19, 0),
                         col(r, 8, 0), col(r, 21, 0), col(r, 22, 0), col(r, 20, 0)))
            skill_count += 1
        except sqlite3.IntegrityError:
            pass
    results.append(("skill_data", skill_count))

    # buff_skills
    dst.execute("DROP TABLE IF EXISTS buff_skills")
    dst.execute("""
        CREATE TABLE buff_skills (
            id INTEGER PRIMARY KEY, name TEXT, buff_level INTEGER DEFAULT 1,
            skill_ref_id INTEGER DEFAULT 0, duration_ms INTEGER DEFAULT 0,
            buff_type INTEGER DEFAULT 0, buff_value INTEGER DEFAULT 0,
            buff_chance INTEGER DEFAULT 100, icon_id INTEGER DEFAULT 0
        )
    """)
    seen_buffs = set()
    buff_count = 0
    for r in buff_rows:
        bid = col(r, 0)
        if bid in seen_buffs:
            continue
        seen_buffs.add(bid)
        try:
            dst.execute("INSERT INTO buff_skills (id,name,buff_level,skill_ref_id,duration_ms,buff_type,buff_value,buff_chance,icon_id) VALUES (?,?,?,?,?,?,?,?,?)",
                        (bid, col(r, 1), col(r, 2, 1), col(r, 4, 0), col(r, 7, 0),
                         col(r, 6, 0), col(r, 13, 0), col(r, 11, 100), col(r, 5, 0)))
            buff_count += 1
        except sqlite3.IntegrityError:
            pass
    results.append(("buff_skills", buff_count))

    # skill_trees
    dst.execute("DROP TABLE IF EXISTS skill_trees")
    dst.execute("""
        CREATE TABLE skill_trees (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            class_id INTEGER, tree_level INTEGER,
            slot_index INTEGER, skill_id INTEGER
        )
    """)
    for r in tree_rows:
        dst.execute("INSERT INTO skill_trees (class_id,tree_level,slot_index,skill_id) VALUES (?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 3)))
    results.append(("skill_trees", len(tree_rows)))

    # map_warps
    dst.execute("DROP TABLE IF EXISTS map_warps")
    dst.execute("""
        CREATE TABLE map_warps (
            id INTEGER PRIMARY KEY, map_from INTEGER, map_to INTEGER,
            from_x REAL, from_z REAL, to_x REAL, to_z REAL,
            name TEXT, dest_name TEXT, fee INTEGER DEFAULT 0
        )
    """)
    for r in warp_rows:
        dst.execute("INSERT INTO map_warps (id,map_from,map_to,from_x,from_z,to_x,to_z,name,dest_name,fee) VALUES (?,?,?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 3), col(r, 4), col(r, 5), col(r, 6), col(r, 7), col(r, 8), col(r, 1), col(r, 2), col(r, 9)))
    results.append(("map_warps", len(warp_rows)))

    # map_boundaries
    dst.execute("DROP TABLE IF EXISTS map_boundaries")
    dst.execute("""
        CREATE TABLE map_boundaries (
            id INTEGER PRIMARY KEY, map_a INTEGER, map_b INTEGER,
            boundary_x_a REAL, boundary_z_a REAL,
            boundary_x_b REAL, boundary_z_b REAL,
            name_a TEXT, name_b TEXT, level_required INTEGER DEFAULT 0
        )
    """)
    for r in change_rows:
        dst.execute("INSERT INTO map_boundaries (id,map_a,map_b,boundary_x_a,boundary_z_a,boundary_x_b,boundary_z_b,name_a,name_b,level_required) VALUES (?,?,?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 3), col(r, 4), col(r, 5), col(r, 6), col(r, 7), col(r, 8), col(r, 1), col(r, 2), col(r, 9)))
    results.append(("map_boundaries", len(change_rows)))

    # map_data
    dst.execute("DROP TABLE IF EXISTS map_data")
    dst.execute("""
        CREATE TABLE map_data (
            id INTEGER PRIMARY KEY, name TEXT, file_path TEXT,
            hgt_file TEXT, box_min_x REAL, box_min_y REAL,
            box_min_z REAL, box_max_x REAL, box_max_y REAL, box_max_z REAL
        )
    """)
    map_ids = set()
    for r in warp_rows:
        map_ids.add(col(r, 3))
        map_ids.add(col(r, 4))
    for mid in sorted(map_ids):
        dst.execute("INSERT INTO map_data (id,name,hgt_file) VALUES (?,?,?)",
                    (mid, f"Map_{mid}", f"{mid}.hgt"))
    results.append(("map_data", len(map_ids)))

    # monster_spawns
    dst.execute("DROP TABLE IF EXISTS monster_spawns")
    dst.execute("""
        CREATE TABLE monster_spawns (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER, monster_id INTEGER, count INTEGER DEFAULT 1,
            respawn_time INTEGER DEFAULT 30, spawn_radius REAL DEFAULT 10.0
        )
    """)
    spawn_count = 0
    for r in monster_rows:
        mid = col(r, 0)
        zone = col(r, 8, 0)
        if zone:
            dst.execute("INSERT INTO monster_spawns (map_id,monster_id,count,respawn_time) VALUES (?,?,1,30)",
                        (zone, mid))
            spawn_count += 1
    results.append(("monster_spawns", spawn_count))

    src.close()
    dst.commit()
    return results


def export_to_json(dst: sqlite3.Connection, output_dir: Path):
    """Export game content tables to JSON files."""
    print("  [EXPORT] Exporting game content to JSON...")
    dst.row_factory = sqlite3.Row
    exports = {}

    tables_json = {
        "item_templates": "items.json",
        "monster_templates": "monsters.json",
        "monster_drops": "monster_drops.json",
        "npc_templates": "npcs.json",
        "npc_positions": "npc_positions.json",
        "npc_shop_entries": "npc_shops.json",
        "quest_templates": "quests.json",
        "quest_conditions": "quest_conditions.json",
        "quest_strings": "quest_strings.json",
        "skill_data": "skills.json",
        "buff_skills": "buffs.json",
        "skill_trees": "skill_trees.json",
        "map_warps": "map_warps.json",
        "map_boundaries": "map_boundaries.json",
        "map_data": "map_data.json",
        "monster_spawns": "monster_spawns.json",
    }

    for table, filename in tables_json.items():
        try:
            rows = dst.execute(f"SELECT * FROM [{table}]").fetchall()
            data = [dict(r) for r in rows]
            filepath = output_dir / filename
            with open(filepath, "w", encoding="utf-8") as f:
                json.dump(data, f, indent=2, ensure_ascii=False)
            exports[filename] = len(data)
            print(f"    {filename}: {len(data)} records")
        except sqlite3.Error as e:
            print(f"    [ERR] {filename}: {e}")

    return exports


def main():
    print("=" * 64)
    print("  LUNA Plus Reborn — Database Migration Tool v2.0")
    print("=" * 64)

    source_mode = os.environ.get("SOURCE_MODE", "legacy").lower()

    # Validate sources
    raw_available = RAW_SRC.exists()
    legacy_available = LEGACY_SRC.exists()
    old_available = LEGACY_OLD.exists() and LEGACY_OLD.stat().st_size > 0

    print(f"\n  Sources available:")
    print(f"    {LEGACY_SRC.name}: {'YES' if legacy_available else 'NO'}")
    print(f"    {RAW_SRC.name}:    {'YES' if raw_available else 'NO'}")
    print(f"    legacy-old/:   {'YES' if old_available else 'NO'}")
    print(f"  Source mode: {source_mode}")

    # Determine source
    src_db = None
    used_raw_extract = False

    if source_mode == "legacy" and legacy_available:
        print(f"\n  Using {LEGACY_SRC.name} as source")
        src_db = sqlite3.connect(str(LEGACY_SRC))
        src_db.row_factory = sqlite3.Row
    elif raw_available:
        print(f"\n  Using {RAW_SRC.name} as source (building normalized tables)")
        src_db = sqlite3.connect(":memory:")
        used_raw_extract = True
        extract_legacy_from_raw(RAW_SRC, src_db)
    else:
        print("\n  [ERROR] No valid source database found!")
        print("  Run data_parser.py first to generate game data databases.")
        sys.exit(1)

    src_db.execute("PRAGMA synchronous = OFF")

    # Connect destination
    dst = sqlite3.connect(str(LUNA_DB_DST))
    dst.execute("PRAGMA synchronous = OFF")
    dst.execute("PRAGMA journal_mode = MEMORY")
    dst.execute("PRAGMA foreign_keys = OFF")

    # Discover tables
    legacy_tables = {
        row[0]
        for row in src_db.execute(
            "SELECT name FROM sqlite_master WHERE type='table'"
        ).fetchall()
    }
    print(f"\n  Source tables found: {len(legacy_tables)}")

    # Scan raw DB for deprecated tables
    deprecated_found = []
    hackshield_found = []
    nprotect_found = []
    if raw_available:
        gd = sqlite3.connect(str(RAW_SRC))
        gd_tables = {
            row[0]
            for row in gd.execute(
                "SELECT name FROM sqlite_master WHERE type='table'"
            ).fetchall()
        }
        for t in sorted(gd_tables):
            if t.lower().startswith("hackshield"):
                hackshield_found.append(t)
            elif t.lower().startswith("nprotect"):
                nprotect_found.append(t)
            elif is_deprecated(t):
                deprecated_found.append(t)
        gd.close()

    # Also check the destination for existing TB_ tables we should preserve
    existing_dst_tables = {
        row[0]
        for row in dst.execute(
            "SELECT name FROM sqlite_master WHERE type='table'"
        ).fetchall()
        if row[0].startswith("TB_")
    }
    if existing_dst_tables:
        print(f"  Existing runtime tables preserved: {len(existing_dst_tables)}")

    # Migrate content tables
    tables_copied = []
    tables_skipped = []
    tables_already_exist = []
    total_rows = 0

    for table in RELEVANT_CONTENT_TABLES:
        if table not in legacy_tables:
            tables_skipped.append((table, "not found in source"))
            print(f"  [SKIP] {table:30s} not found in source")
            continue

        try:
            rows = copy_table(src_db, dst, table)
            create_indexes(dst, table)
            tables_copied.append((table, rows))
            total_rows += rows
            print(f"  [OK]   {table:30s} → {rows:>6d} rows")
        except sqlite3.Error as e:
            tables_skipped.append((table, str(e)))
            print(f"  [ERR]  {table:30s} → {e}")

    # Record migration version
    add_migration_version(dst, "2.0.0", "Full migration from game data sources to luna_game.db")

    dst.commit()
    dst.execute("VACUUM")

    # Report
    dst_size = LUNA_DB_DST.stat().st_size

    print(f"\n{'=' * 64}")
    print(f"  MIGRATION REPORT")
    print(f"{'=' * 64}")
    print(f"  Tables copied:    {len(tables_copied)}")
    for name, count in tables_copied:
        print(f"    {name:30s} {count:>6d} rows")
    print(f"  Total rows migrated: {total_rows:,}")

    if tables_skipped:
        print(f"\n  Tables skipped:   {len(tables_skipped)}")
        for name, reason in tables_skipped:
            print(f"    {name:30s} ({reason})")

    if deprecated_found:
        print(f"\n  Deprecated tables identified: {len(deprecated_found)}")
        for t in sorted(deprecated_found):
            print(f"    {t}")

    if hackshield_found:
        print(f"\n  HackShield tables found (deprecated): {len(hackshield_found)}")
        for t in sorted(hackshield_found):
            print(f"    {t}")

    if nprotect_found:
        print(f"\n  NProtect tables found (deprecated): {len(nprotect_found)}")
        for t in sorted(nprotect_found):
            print(f"    {t}")

    print(f"\n  Database size:    {dst_size / 1024:.1f} KB")
    print(f"  Migration version: 2.0.0")
    print(f"{'=' * 64}")

    # JSON export
    print(f"\n{'=' * 64}")
    print(f"  JSON EXPORT")
    print(f"{'=' * 64}")
    try:
        exports = export_to_json(dst, ASSETS_DATA)
        print(f"  Exported {len(exports)} files")
    except Exception as e:
        print(f"  [ERR] JSON export failed: {e}")

    src_db.close()
    dst.close()

    print(f"\n  Migration complete.")


if __name__ == "__main__":
    main()
