#!/usr/bin/env python3
"""
LUNA Plus Reborn — Data Parser (Phase 0.4)
Parses 282 game data files (.bin.txt) → SQLite database.
Also generates game_data_legacy.db with normalized schemas.

Input:  NEW_LUNA/data/*.bin.txt (TSV, EUC-KR, ^s sub-separator)
Output: assets/data/game_data.db        (raw col_NNNN schema)
        assets/data/game_data_legacy.db  (normalized named schema)
"""

import os
import sys
import sqlite3
import csv
import re
import json
from pathlib import Path

DATA_SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/NEW_LUNA/data")
DB_OUT = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets/data/game_data.db")
LEGACY_DB_OUT = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets/data/game_data_legacy.db")

SCHEMA_DOC = """
╔══════════════════════════════════════════════════════════════╗
║  game_data.db — Raw Column Schema                          ║
║  Tables use col_NNNN naming; see create_legacy_db.py       ║
║  for the named mapping.                                    ║
╚══════════════════════════════════════════════════════════════╝

Key tables and their column meanings:

game_monsterlist (125 cols):
  col_0000 = monster_id (INTEGER)
  col_0001 = name_en (TEXT)
  col_0003 = model_file (TEXT)
  col_0008 = zone/map_id (INTEGER)
  col_0010 = element_type (INTEGER)
  col_0011 = hp (INTEGER)
  col_0012 = attack (INTEGER)
  col_0013 = defense (INTEGER)
  col_0014 = exp_scaled (REAL)
  col_0016 = size_scale (REAL)
  col_0017 = speed (REAL)
  col_0018 = ai_type (INTEGER)
  col_0019 = aggro_range (INTEGER)
  col_0020 = gold_min (INTEGER)
  col_0021 = gold_max (INTEGER)
  col_0062+ = inline drop slots (7-col groups: table_id, ?, ?, name, item_id, qty, prob)

game_itemlist (64+ cols):
  col_0000 = item_id (INTEGER)
  col_0001 = name (TEXT)
  col_0003 = item_type (INTEGER)  — 1-30=weapons, 501+=armor, 1494+=consumables
  col_0004 = item_subtype/rarity (INTEGER)
  col_0010 = attack (INTEGER)
  col_0011 = magic_attack (INTEGER)
  col_0012 = defense (INTEGER)
  col_0019 = price_buy (INTEGER)
  col_0020 = price_sell (INTEGER)
  col_0028 = level_required (INTEGER)

game_skilllist (50+ cols):
  col_0000 = skill_id (INTEGER)
  col_0001 = name (TEXT)
  col_0002 = class_id (INTEGER)
  col_0005 = skill_type (INTEGER)
  col_0008 = cost_mp (INTEGER)
  col_0019 = level_required (INTEGER)
  col_0020 = sp_cost (INTEGER)
  col_0021 = cooldown (INTEGER)
  col_0022 = weapon_type (INTEGER)

game_skill_buff_list (30+ cols):
  col_0000 = buff_id (INTEGER)
  col_0001 = name (TEXT)
  col_0002 = buff_level (INTEGER)
  col_0004 = skill_ref_id (INTEGER)
  col_0005 = icon_id (INTEGER)
  col_0006 = buff_type (INTEGER)
  col_0007 = duration_ms (INTEGER)
  col_0011 = buff_chance (INTEGER)
  col_0013 = buff_value (INTEGER)

game_skilltreelist (4 cols):
  col_0000 = class_id
  col_0001 = tree_level
  col_0002 = slot_index
  col_0003 = skill_id

game_npclist (15 cols):
  col_0000 = npc_id
  col_0001 = name
  col_0002 = npc_type/job

game_staticnpc (8 cols):
  col_0000 = map_id
  col_0001 = npc_index_in_map
  col_0002 = name
  col_0003 = npc_type
  col_0004 = pos_x
  col_0005 = pos_z
  col_0006 = direction

game_dealitem (80 cols, repeating item pairs):
  col_0000 = map_id
  col_0001 = map_name
  col_0002 = npc_type
  col_0003 = npc_name
  col_0004 = npc_sub_index
  col_0005-6 = pos_x, pos_z (or unknown)
  col_0007 = tab/page
  col_0008+ = (item_name, item_id, price?) repeating 3-col groups

game_mapmovelist (10 cols):
  col_0000 = warp_id
  col_0001 = source_map_name
  col_0002 = dest_map_name
  col_0003 = source_map_id
  col_0004 = dest_map_id
  col_0005 = source_x
  col_0006 = source_z
  col_0007 = dest_x
  col_0008 = dest_z
  col_0009 = fee

game_mapchange (10 cols):
  col_0000 = change_id
  col_0001-2 = map_a_name, map_b_name
  col_0003-4 = map_a_id, map_b_id
  col_0005-8 = boundary positions
  col_0009 = level_required

game_questnpclist (7 cols):
  col_0000 = quest_id
  col_0001 = npc_id / step
  col_0002 = objective_name
  col_0003 = objective_item_id
  col_0004-5 = pos_x, pos_z
  col_0006 = rotation

game_queststring:
  Contains $SUBQUESTSTR blocks with #TITLE, #DESC fields

game_monsterdropitemlist (7 cols):
  col_0000 = monster_id
  col_0001 = item_name
  col_0002 = item_id
  col_0003 = drop_rate/quantity
"""


def decode_euckr(bytes_data: bytes) -> str:
    try:
        return bytes_data.decode("euc-kr")
    except UnicodeDecodeError:
        return bytes_data.decode("euc-kr", errors="replace")


def parse_fields(line: str) -> list[str]:
    stripped = line.strip("\r\n")
    cells = stripped.split("\t")
    return [cell.replace("^s", "\x1f") for cell in cells]


def table_name_from_file(filename: str) -> str:
    name = filename.removesuffix(".bin.txt")
    name = re.sub(r"^(\d+)_", "", name)
    name = re.sub(r"_\d+$", "", name)
    name = re.sub(r"[^a-zA-Z0-9_]", "_", name).lower()
    name = re.sub(r"_+", "_", name).strip("_")
    if not name:
        name = f"table_{hash(filename) & 0xFFFFFFF}"
    return f"game_{name}"


def process_file(filepath: Path, db: sqlite3.Connection) -> tuple[str, int, int]:
    """Process a single .bin.txt file. Returns (table_name, row_count, col_count)."""
    filename = filepath.name
    table = table_name_from_file(filename)

    with open(filepath, "rb") as f:
        raw = f.read()

    text = decode_euckr(raw)
    lines = text.strip().split("\n")
    rows = [parse_fields(l) for l in lines if l.strip()]

    if not rows:
        return table, 0, 0

    col_count = max(len(r) for r in rows)

    # Normalize all rows to same column count
    normalized = []
    for row in rows:
        while len(row) < col_count:
            row.append("")
        normalized.append(row)

    # Escape column names
    cols = [f"col_{i:04d}" for i in range(col_count)]

    # Try to use first data row to detect types (all numeric → INTEGER)
    col_types = []
    for i in range(col_count):
        vals = [r[i] for r in normalized[:5] if r[i]]
        is_int = all(v.lstrip("-").isdigit() for v in vals if v)
        col_types.append("INTEGER" if is_int else "TEXT")

    col_defs = ", ".join(f'"{c}" {t}' for c, t in zip(cols, col_types))
    placeholders = ", ".join("?" for _ in cols)

    # Drop and recreate table
    db.execute(f"DROP TABLE IF EXISTS [{table}]")
    db.execute(f"CREATE TABLE [{table}] ({col_defs})")

    # Insert rows
    insert_sql = f"INSERT INTO [{table}] ({', '.join(f'\"{c}\"' for c in cols)}) VALUES ({placeholders})"
    db.executemany(insert_sql, normalized)

    # Create index on first column (usually ID)
    try:
        db.execute(f"CREATE INDEX IF NOT EXISTS idx_{table}_col0 ON [{table}]({cols[0]})")
    except sqlite3.Error:
        pass

    return table, len(normalized), col_count


def create_legacy_db(src_path: Path, dst_path: Path):
    """Create normalized legacy database from game_data.db."""
    if not src_path.exists():
        print("  [LEGACY] Source DB not found, skipping")
        return

    src = sqlite3.connect(str(src_path))
    src.row_factory = sqlite3.Row

    if dst_path.exists():
        dst_path.unlink()
    dst = sqlite3.connect(str(dst_path))
    dst.execute("PRAGMA synchronous = OFF")
    dst.execute("PRAGMA journal_mode = MEMORY")

    def col(r, i, default=0):
        v = r[i] if i < len(r) else None
        if v is None or (isinstance(v, str) and v.strip() == ""):
            return default
        return v

    # monster_templates
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
    rows = src.execute("SELECT * FROM game_monsterlist").fetchall()
    for r in rows:
        dst.execute("""INSERT INTO monster_templates
            (id,name,model_file,level,hp,attack,defense,speed,exp_reward,
             gold_min,gold_max,element_type,ai_type,aggro_range,size_scale,monster_type)
            VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)""",
            (col(r,0), col(r,1), col(r,3), col(r,11), col(r,8),
             col(r,12), col(r,13), float(col(r,17,1.0)),
             int(float(col(r,14,0))), col(r,20), col(r,21),
             col(r,10), col(r,18), col(r,19),
             float(col(r,16,1.0)), col(r,18)))
    print(f"  [LEGACY] monster_templates: {len(rows)} rows")

    # monster_drops (inline from monsterlist col 62-124)
    dst.execute("""
        CREATE TABLE monster_drops (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            monster_id INTEGER, item_id INTEGER, item_name TEXT,
            min_count INTEGER DEFAULT 1, max_count INTEGER DEFAULT 1,
            probability REAL DEFAULT 0.0, drop_table_id INTEGER DEFAULT 0
        )
    """)
    drop_count = 0
    for r in rows:
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
    print(f"  [LEGACY] monster_drops: {drop_count} rows")

    # npc_templates
    dst.execute("""
        CREATE TABLE npc_templates (
            id INTEGER PRIMARY KEY, name TEXT,
            npc_type INTEGER DEFAULT 0, shop_type INTEGER DEFAULT 0
        )
    """)
    npc_rows = src.execute("SELECT * FROM game_npclist").fetchall()
    for r in npc_rows:
        dst.execute("INSERT INTO npc_templates (id,name,npc_type,shop_type) VALUES (?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 6, 1)))
    print(f"  [LEGACY] npc_templates: {len(npc_rows)} rows")

    # npc_positions
    dst.execute("""
        CREATE TABLE npc_positions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER, npc_id INTEGER, name TEXT,
            npc_type INTEGER DEFAULT 0, pos_x REAL, pos_y REAL,
            pos_z REAL, rotation REAL DEFAULT 0
        )
    """)
    snpc_rows = src.execute("SELECT * FROM game_staticnpc").fetchall()
    for r in snpc_rows:
        dst.execute("INSERT INTO npc_positions (map_id,npc_id,name,npc_type,pos_x,pos_y,pos_z,rotation) VALUES (?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 3), col(r, 4), 0, col(r, 5), col(r, 6)))
    print(f"  [LEGACY] npc_positions: {len(snpc_rows)} rows")

    # npc_shop_entries
    dst.execute("""
        CREATE TABLE npc_shop_entries (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            npc_id INTEGER, item_id INTEGER, price INTEGER DEFAULT 0,
            stock INTEGER DEFAULT -1, map_id INTEGER DEFAULT 0
        )
    """)
    deal_rows = src.execute("SELECT * FROM game_dealitem").fetchall()
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
    print(f"  [LEGACY] npc_shop_entries: {shop_count} rows")

    # quest_templates
    dst.execute("""
        CREATE TABLE quest_templates (
            id INTEGER PRIMARY KEY, title TEXT DEFAULT '',
            description TEXT DEFAULT '', level_required INTEGER DEFAULT 0,
            giver_npc_id INTEGER DEFAULT 0, completer_npc_id INTEGER DEFAULT 0,
            reward_exp INTEGER DEFAULT 0, reward_gold INTEGER DEFAULT 0,
            reward_item_id INTEGER DEFAULT 0, reward_item_count INTEGER DEFAULT 1
        )
    """)
    quest_rows = src.execute("SELECT * FROM game_questnpclist").fetchall()
    seen = {}
    for r in quest_rows:
        qid = col(r, 0)
        if qid in seen:
            continue
        seen[qid] = True
        dst.execute("INSERT INTO quest_templates (id,giver_npc_id,completer_npc_id) VALUES (?,?,?)",
                    (qid, col(r, 1), col(r, 1)))
    print(f"  [LEGACY] quest_templates: {len(seen)} rows")

    # quest_conditions
    dst.execute("""
        CREATE TABLE quest_conditions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER, condition_type INTEGER DEFAULT 0,
            target_id INTEGER DEFAULT 0, target_count INTEGER DEFAULT 1,
            map_id INTEGER DEFAULT 0, pos_x REAL DEFAULT 0,
            pos_y REAL DEFAULT 0, radius REAL DEFAULT 10
        )
    """)
    for r in quest_rows:
        qid = col(r, 0)
        tid = col(r, 3, 0)
        if tid:
            dst.execute("INSERT INTO quest_conditions (quest_id,condition_type,target_id,target_count,pos_x,pos_y) VALUES (?,0,?,1,?,?)",
                        (qid, tid, col(r, 4, 0), col(r, 5, 0)))
    print(f"  [LEGACY] quest_conditions: inserted")

    # quest_strings
    dst.execute("""
        CREATE TABLE quest_strings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER, language TEXT DEFAULT 'EN',
            title TEXT DEFAULT '', description TEXT DEFAULT ''
        )
    """)
    qstr_rows = src.execute("SELECT * FROM game_queststring").fetchall()
    for r in qstr_rows:
        text = str(col(r, 0, ""))
        if "$SUBQUESTSTR" in text:
            parts = text.split()
            if len(parts) >= 2:
                try:
                    qid = int(parts[1])
                    dst.execute("INSERT INTO quest_strings (quest_id,title,description) VALUES (?,?,?)",
                                (qid, str(parts[0]), str(col(r, 5, ""))[:200]))
                except ValueError:
                    pass
    print(f"  [LEGACY] quest_strings: inserted")

    # skill_data
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
    skill_rows = src.execute("SELECT * FROM game_skilllist").fetchall()
    for r in skill_rows:
        sid = col(r, 0)
        if sid in seen_skills:
            continue
        seen_skills.add(sid)
        try:
            dst.execute("INSERT INTO skill_data (id,name,class_id,skill_type,level_required,cost_mp,cooldown_ms,weapon_type,sp_cost) VALUES (?,?,?,?,?,?,?,?,?)",
                        (sid, col(r, 1), col(r, 2, 0), col(r, 5, 0), col(r, 19, 0), col(r, 8, 0), col(r, 21, 0), col(r, 22, 0), col(r, 20, 0)))
            skill_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"  [LEGACY] skill_data: {skill_count} rows")

    # buff_skills
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
    buff_rows = src.execute("SELECT * FROM game_skill_buff_list").fetchall()
    for r in buff_rows:
        bid = col(r, 0)
        if bid in seen_buffs:
            continue
        seen_buffs.add(bid)
        try:
            dst.execute("INSERT INTO buff_skills (id,name,buff_level,skill_ref_id,duration_ms,buff_type,buff_value,buff_chance,icon_id) VALUES (?,?,?,?,?,?,?,?,?)",
                        (bid, col(r, 1), col(r, 2, 1), col(r, 4, 0), col(r, 7, 0), col(r, 6, 0), col(r, 13, 0), col(r, 11, 100), col(r, 5, 0)))
            buff_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"  [LEGACY] buff_skills: {buff_count} rows")

    # skill_trees
    dst.execute("""
        CREATE TABLE skill_trees (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            class_id INTEGER, tree_level INTEGER,
            slot_index INTEGER, skill_id INTEGER
        )
    """)
    tree_rows = src.execute("SELECT * FROM game_skilltreelist").fetchall()
    for r in tree_rows:
        dst.execute("INSERT INTO skill_trees (class_id,tree_level,slot_index,skill_id) VALUES (?,?,?,?)",
                    (col(r, 0), col(r, 1), col(r, 2), col(r, 3)))
    print(f"  [LEGACY] skill_trees: {len(tree_rows)} rows")

    # item_templates
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
    item_rows = src.execute("SELECT * FROM game_itemlist").fetchall()
    for r in item_rows:
        iid = col(r, 0)
        if iid in seen_items:
            continue
        seen_items.add(iid)
        try:
            dst.execute("INSERT INTO item_templates (id,name,item_type,item_subtype,level_required,attack,defense,magic_attack,magic_defense,price_buy,price_sell,rarity,resource_id) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)",
                        (iid, col(r, 1), col(r, 3, 0), col(r, 4, 0), col(r, 28, 0), col(r, 10, 0), col(r, 12, 0), col(r, 11, 0), 0, col(r, 19, 0), col(r, 20, 0), col(r, 4, 1), col(r, 5, 0)))
            item_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"  [LEGACY] item_templates: {item_count} rows")

    # map_warps
    dst.execute("""
        CREATE TABLE map_warps (
            id INTEGER PRIMARY KEY, map_from INTEGER, map_to INTEGER,
            from_x REAL, from_z REAL, to_x REAL, to_z REAL,
            name TEXT, dest_name TEXT, fee INTEGER DEFAULT 0
        )
    """)
    warp_rows = src.execute("SELECT * FROM game_mapmovelist").fetchall()
    for r in warp_rows:
        dst.execute("INSERT INTO map_warps (id,map_from,map_to,from_x,from_z,to_x,to_z,name,dest_name,fee) VALUES (?,?,?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 3), col(r, 4), col(r, 5), col(r, 6), col(r, 7), col(r, 8), col(r, 1), col(r, 2), col(r, 9)))
    print(f"  [LEGACY] map_warps: {len(warp_rows)} rows")

    # map_boundaries
    dst.execute("""
        CREATE TABLE map_boundaries (
            id INTEGER PRIMARY KEY, map_a INTEGER, map_b INTEGER,
            boundary_x_a REAL, boundary_z_a REAL,
            boundary_x_b REAL, boundary_z_b REAL,
            name_a TEXT, name_b TEXT, level_required INTEGER DEFAULT 0
        )
    """)
    change_rows = src.execute("SELECT * FROM game_mapchange").fetchall()
    for r in change_rows:
        dst.execute("INSERT INTO map_boundaries (id,map_a,map_b,boundary_x_a,boundary_z_a,boundary_x_b,boundary_z_b,name_a,name_b,level_required) VALUES (?,?,?,?,?,?,?,?,?,?)",
                    (col(r, 0), col(r, 3), col(r, 4), col(r, 5), col(r, 6), col(r, 7), col(r, 8), col(r, 1), col(r, 2), col(r, 9)))
    print(f"  [LEGACY] map_boundaries: {len(change_rows)} rows")

    # map_data (register maps from warps)
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
    print(f"  [LEGACY] map_data: {len(map_ids)} rows")

    # monster_spawns
    dst.execute("""
        CREATE TABLE monster_spawns (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER, monster_id INTEGER, count INTEGER DEFAULT 1,
            respawn_time INTEGER DEFAULT 30, spawn_radius REAL DEFAULT 10.0
        )
    """)
    for r in rows:
        mid = col(r, 0)
        zone = col(r, 8, 0)
        if zone:
            dst.execute("INSERT INTO monster_spawns (map_id,monster_id,count,respawn_time) VALUES (?,?,1,30)",
                        (zone, mid))
    print(f"  [LEGACY] monster_spawns: inserted")

    dst.commit()
    dst.execute("VACUUM")
    dst.close()
    src.close()

    size = dst_path.stat().st_size
    print(f"  [LEGACY] Created {dst_path} ({size / 1024:.1f} KB)")


# ═══════════════════════════════════════════════════════════════════════
#  Export functions — legacy DB → JSON for ECS consumption
# ═══════════════════════════════════════════════════════════════════════

OUTPUT_DIR = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets/data")


def export_items(db_path: str, output_path: str) -> int:
    """Export all item templates to JSON"""
    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row
    rows = db.execute("SELECT * FROM item_templates").fetchall()
    items = []
    for r in rows:
        items.append({
            "id": r["id"],
            "name": r["name"],
            "item_type": r["item_type"],
            "item_subtype": r["item_subtype"],
            "rarity": r["rarity"],
            "level_required": r["level_required"],
            "attack": r["attack"],
            "defense": r["defense"],
            "magic_attack": r["magic_attack"],
            "magic_defense": r["magic_defense"],
            "price_buy": r["price_buy"],
            "price_sell": r["price_sell"],
            "max_stack": r["max_stack"],
            "resource_id": r["resource_id"]
        })
    db.close()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(items, f, indent=2, ensure_ascii=False)
    print(f"  [EXPORT] items: {len(items)} records -> {output_path}")
    return len(items)


def export_monsters(db_path: str, output_path: str) -> int:
    """Export monster stats + loot tables to JSON"""
    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row
    mrows = db.execute("SELECT * FROM monster_templates").fetchall()
    drows = db.execute("SELECT * FROM monster_drops").fetchall()
    srows = db.execute("SELECT * FROM monster_spawns").fetchall()

    drops_by_monster: dict[int, list[dict]] = {}
    for d in drows:
        mid = d["monster_id"]
        drops_by_monster.setdefault(mid, []).append({
            "item_id": d["item_id"],
            "item_name": d["item_name"],
            "min_count": d["min_count"],
            "max_count": d["max_count"],
            "probability": d["probability"]
        })

    spawns_by_monster: dict[int, list[dict]] = {}
    for s in srows:
        mid = s["monster_id"]
        spawns_by_monster.setdefault(mid, []).append({
            "map_id": s["map_id"],
            "count": s["count"],
            "respawn_time": s["respawn_time"],
            "spawn_radius": s["spawn_radius"]
        })

    monsters = []
    for r in mrows:
        mid = r["id"]
        monsters.append({
            "id": mid,
            "name": r["name"],
            "level": r["level"],
            "hp": r["hp"],
            "mp": r["mp"],
            "attack": r["attack"],
            "defense": r["defense"],
            "magic_attack": 0,
            "magic_defense": 0,
            "speed": r["speed"],
            "exp_reward": r["exp_reward"],
            "gold_min": r["gold_min"],
            "gold_max": r["gold_max"],
            "element_type": r["element_type"],
            "ai_type": r["ai_type"],
            "aggro_range": r["aggro_range"],
            "size_scale": r["size_scale"],
            "drops": drops_by_monster.get(mid, []),
            "spawns": spawns_by_monster.get(mid, [])
        })
    db.close()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(monsters, f, indent=2, ensure_ascii=False)
    print(f"  [EXPORT] monsters: {len(monsters)} records -> {output_path}")
    return len(monsters)


def export_skills(db_path: str, output_path: str) -> int:
    """Export skill definitions to JSON"""
    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row
    srows = db.execute("SELECT * FROM skill_data").fetchall()
    brows = db.execute("SELECT * FROM buff_skills").fetchall()
    trows = db.execute("SELECT * FROM skill_trees").fetchall()

    buffs_by_skill: dict[int, list[dict]] = {}
    for b in brows:
        ref = b["skill_ref_id"]
        buffs_by_skill.setdefault(ref, []).append({
            "buff_id": b["id"],
            "name": b["name"],
            "buff_level": b["buff_level"],
            "duration_ms": b["duration_ms"],
            "buff_type": b["buff_type"],
            "buff_value": b["buff_value"],
            "buff_chance": b["buff_chance"],
            "icon_id": b["icon_id"]
        })

    trees_by_skill: dict[int, list[dict]] = {}
    for t in trows:
        sid = t["skill_id"]
        trees_by_skill.setdefault(sid, []).append({
            "class_id": t["class_id"],
            "tree_level": t["tree_level"],
            "slot_index": t["slot_index"]
        })

    skills = []
    for r in srows:
        sid = r["id"]
        skills.append({
            "id": sid,
            "name": r["name"],
            "class_id": r["class_id"],
            "skill_type": r["skill_type"],
            "level_required": r["level_required"],
            "target_type": r["target_type"],
            "range": r["range"],
            "cost_hp": r["cost_hp"],
            "cost_mp": r["cost_mp"],
            "cooldown_ms": r["cooldown_ms"],
            "damage_mult": r["damage_mult"],
            "damage_fixed": r["damage_fixed"],
            "weapon_type": r["weapon_type"],
            "sp_cost": r["sp_cost"],
            "buffs": buffs_by_skill.get(sid, []),
            "tree_entries": trees_by_skill.get(sid, [])
        })
    db.close()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(skills, f, indent=2, ensure_ascii=False)
    print(f"  [EXPORT] skills: {len(skills)} records -> {output_path}")
    return len(skills)


def export_quests(db_path: str, output_path: str) -> int:
    """Export quest chains + conditions + rewards to JSON"""
    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row
    qrows = db.execute("SELECT * FROM quest_templates").fetchall()
    crows = db.execute("SELECT * FROM quest_conditions").fetchall()
    srows = db.execute("SELECT * FROM quest_strings").fetchall()

    conds_by_quest: dict[int, list[dict]] = {}
    for c in crows:
        qid = c["quest_id"]
        conds_by_quest.setdefault(qid, []).append({
            "condition_type": c["condition_type"],
            "target_id": c["target_id"],
            "target_count": c["target_count"],
            "map_id": c["map_id"],
            "pos_x": c["pos_x"],
            "pos_y": c["pos_y"],
            "radius": c["radius"]
        })

    strings_by_quest: dict[int, dict] = {}
    for s in srows:
        qid = s["quest_id"]
        strings_by_quest[qid] = {
            "title": s["title"],
            "description": s["description"]
        }

    quests = []
    for r in qrows:
        qid = r["id"]
        st = strings_by_quest.get(qid, {})
        quests.append({
            "id": qid,
            "title": r["title"] or st.get("title", ""),
            "description": r["description"] or st.get("description", ""),
            "level_required": r["level_required"],
            "giver_npc_id": r["giver_npc_id"],
            "completer_npc_id": r["completer_npc_id"],
            "reward_exp": r["reward_exp"],
            "reward_gold": r["reward_gold"],
            "reward_item_id": r["reward_item_id"],
            "reward_item_count": r["reward_item_count"],
            "conditions": conds_by_quest.get(qid, []),
            "dialog_start": "",
            "dialog_progress": "",
            "dialog_complete": ""
        })
    db.close()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(quests, f, indent=2, ensure_ascii=False)
    print(f"  [EXPORT] quests: {len(quests)} records -> {output_path}")
    return len(quests)


def export_npcs(db_path: str, output_path: str) -> int:
    """Export NPC data + dialogs + shops to JSON"""
    db = sqlite3.connect(db_path)
    db.row_factory = sqlite3.Row
    nrows = db.execute("SELECT * FROM npc_templates").fetchall()
    prows = db.execute("SELECT * FROM npc_positions").fetchall()
    srows = db.execute("SELECT * FROM npc_shop_entries").fetchall()

    positions: dict[int, list[dict]] = {}
    for p in prows:
        nid = p["npc_id"]
        positions.setdefault(nid, []).append({
            "map_id": p["map_id"],
            "pos_x": p["pos_x"],
            "pos_y": p["pos_y"],
            "pos_z": p["pos_z"],
            "rotation": p["rotation"]
        })

    shop_items: dict[int, list[dict]] = {}
    for s in srows:
        nid = s["npc_id"]
        shop_items.setdefault(nid, []).append({
            "item_id": s["item_id"],
            "price": s["price"],
            "stock": s["stock"]
        })

    npcs = []
    for r in nrows:
        nid = r["id"]
        npcs.append({
            "id": nid,
            "name": r["name"],
            "npc_type": r["npc_type"],
            "shop_type": r["shop_type"],
            "dialog_text": r["dialog_text"] or "",
            "positions": positions.get(nid, []),
            "shop_items": shop_items.get(nid, [])
        })
    db.close()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(npcs, f, indent=2, ensure_ascii=False)
    print(f"  [EXPORT] npcs: {len(npcs)} records -> {output_path}")
    return len(npcs)


def main():
    if not DATA_SRC.is_dir():
        print(f"ERROR: Data directory not found: {DATA_SRC}")
        sys.exit(1)

    files = sorted(DATA_SRC.glob("*.bin.txt"))
    print(f"Found {len(files)} game data files in {DATA_SRC}")

    DB_OUT.parent.mkdir(parents=True, exist_ok=True)
    if DB_OUT.exists():
        DB_OUT.unlink()

    db = sqlite3.connect(str(DB_OUT))
    db.execute("PRAGMA synchronous = OFF")
    db.execute("PRAGMA journal_mode = MEMORY")
    db.execute("PRAGMA cache_size = -64000")

    total_rows = 0
    parsed = 0
    skipped = 0
    for f in files:
        try:
            table, rows, cols = process_file(f, db)
            total_rows += rows
            parsed += 1
            status = "OK" if rows > 0 else "EMPTY"
            print(f"  {status:5} | {table:40s} | {rows:5d} rows | {cols} cols | {f.name}")
            db.commit()
        except Exception as e:
            print(f"  ERROR  | {f.name:47s} | {e}")
            skipped += 1

    # Create metadata table
    db.execute("""
        CREATE TABLE IF NOT EXISTS _metadata (
            file TEXT PRIMARY KEY,
            table_name TEXT,
            row_count INTEGER,
            col_count INTEGER
        )
    """)
    for f in files:
        table = table_name_from_file(f.name)
        try:
            row_count = db.execute(f"SELECT COUNT(*) FROM [{table}]").fetchone()[0]
            col_count = len(db.execute(f"PRAGMA table_info([{table}])").fetchall())
            db.execute(
                "INSERT INTO _metadata VALUES (?, ?, ?, ?)",
                (f.name, table, row_count, col_count),
            )
        except sqlite3.Error:
            pass
    db.commit()
    db.close()

    print(f"\n{'='*60}")
    print(f"  Parsed: {parsed} files")
    print(f"  Skipped: {skipped} files")
    print(f"  Total rows: {total_rows:,}")
    print(f"  Database: {DB_OUT}")
    if DB_OUT.exists():
        print(f"  Size: {DB_OUT.stat().st_size / 1024:.1f} KB")
    print(f"{'='*60}\n")

    # Generate legacy DB
    print("Generating game_data_legacy.db with normalized schema...")
    create_legacy_db(DB_OUT, LEGACY_DB_OUT)
    print(f"\n{'='*60}")
    print(f"  Legacy DB: {LEGACY_DB_OUT}")
    if LEGACY_DB_OUT.exists():
        print(f"  Size: {LEGACY_DB_OUT.stat().st_size / 1024:.1f} KB")
    print(f"{'='*60}")

    print("\n" + SCHEMA_DOC)

    # Export to JSON
    print("\nExporting legacy data to JSON for ECS consumption...")
    legacy_path = str(LEGACY_DB_OUT)
    output_dir = str(OUTPUT_DIR)
    try:
        n_items = export_items(legacy_path, os.path.join(output_dir, "items.json"))
        n_monsters = export_monsters(legacy_path, os.path.join(output_dir, "monsters.json"))
        n_skills = export_skills(legacy_path, os.path.join(output_dir, "skills.json"))
        n_quests = export_quests(legacy_path, os.path.join(output_dir, "quests.json"))
        n_npcs = export_npcs(legacy_path, os.path.join(output_dir, "npcs.json"))
        print(f"\n{'='*60}")
        print(f"  Export complete:")
        print(f"    Items:    {n_items}")
        print(f"    Monsters: {n_monsters}")
        print(f"    Skills:   {n_skills}")
        print(f"    Quests:   {n_quests}")
        print(f"    NPCs:     {n_npcs}")
        print(f"{'='*60}")
    except Exception as e:
        print(f"  [EXPORT ERROR] {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
