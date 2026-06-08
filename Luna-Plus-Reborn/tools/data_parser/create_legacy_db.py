#!/usr/bin/env python3
"""
Create game_data_legacy.db — normalized schema from parsed game_data.db
Maps col_NNNN columns to meaningful field names.
"""

import sqlite3
import sys
from pathlib import Path

SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/game_data.db")
DST = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/game_data_legacy.db")

def col(src, row, i, default=0):
    v = row[i] if i < len(row) else None
    if v is None:
        return default
    if isinstance(v, str) and v.strip() == "":
        return default
    return v

def connect_db(path):
    db = sqlite3.connect(str(path))
    db.row_factory = sqlite3.Row
    return db

def main():
    if not SRC.exists():
        print(f"Source DB not found: {SRC}")
        sys.exit(1)

    src = connect_db(SRC)
    if DST.exists():
        DST.unlink()
    dst = sqlite3.connect(str(DST))
    dst.execute("PRAGMA synchronous = OFF")
    dst.execute("PRAGMA journal_mode = MEMORY")

    # ── monster_templates ──────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE monster_templates (
            id INTEGER PRIMARY KEY,
            name TEXT,
            model_file TEXT,
            level INTEGER,
            hp INTEGER,
            mp INTEGER DEFAULT 0,
            attack INTEGER,
            defense INTEGER,
            speed REAL DEFAULT 1.0,
            exp_reward INTEGER DEFAULT 0,
            gold_min INTEGER DEFAULT 0,
            gold_max INTEGER DEFAULT 0,
            element_type INTEGER DEFAULT 0,
            ai_type INTEGER DEFAULT 0,
            aggro_range INTEGER DEFAULT 0,
            size_scale REAL DEFAULT 1.0,
            monster_type INTEGER DEFAULT 0
        )
    """)
    rows = src.execute("SELECT * FROM game_monsterlist").fetchall()
    for r in rows:
        dst.execute("""
            INSERT INTO monster_templates
            (id, name, model_file, level, hp, mp, attack, defense,
             speed, exp_reward, gold_min, gold_max, element_type,
             ai_type, aggro_range, size_scale, monster_type)
            VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
        """, (
            col(src, r, 0), col(src, r, 1), col(src, r, 3),
            col(src, r, 11), col(src, r, 8), 0,
            col(src, r, 12), col(src, r, 13),
            float(col(src, r, 17, 1.0)),
            int(float(col(src, r, 14, 0))), col(src, r, 20), col(src, r, 21),
            col(src, r, 10), col(src, r, 18),
            col(src, r, 19), float(col(src, r, 16, 1.0)),
            col(src, r, 18)
        ))
    print(f"monster_templates: {len(rows)} rows")

    # ── monster_drops (from monsterlist inline cols 62-124) ───────────
    dst.execute("""
        CREATE TABLE monster_drops (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            monster_id INTEGER,
            item_id INTEGER,
            item_name TEXT,
            min_count INTEGER DEFAULT 1,
            max_count INTEGER DEFAULT 1,
            probability REAL DEFAULT 0.0,
            drop_table_id INTEGER DEFAULT 0
        )
    """)
    drop_count = 0
    for r in rows:
        mid = col(src, r, 0)
        for offset in range(62, 125, 7):
            if offset + 6 >= len(r):
                break
            item_id = col(src, r, offset + 4, 0)
            if item_id == 0:
                continue
            item_name = col(src, r, offset + 3, "")
            qty = col(src, r, offset + 5, 1)
            prob = col(src, r, offset + 6, 0)
            if isinstance(prob, (int, float)) and prob > 0:
                prob_val = prob / 1000000.0
            else:
                prob_val = 0.01
            dst.execute("""
                INSERT INTO monster_drops (monster_id, item_id, item_name, min_count, max_count, probability)
                VALUES (?,?,?,?,?,?)
            """, (mid, item_id, item_name, 1, qty, prob_val))
            drop_count += 1
    print(f"monster_drops: {drop_count} rows")

    # ── npc_templates ─────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE npc_templates (
            id INTEGER PRIMARY KEY,
            name TEXT,
            npc_type INTEGER DEFAULT 0,
            shop_type INTEGER DEFAULT 0,
            dialog_text TEXT
        )
    """)
    npc_rows = src.execute("SELECT * FROM game_npclist").fetchall()
    for r in npc_rows:
        dst.execute("""
            INSERT INTO npc_templates (id, name, npc_type, shop_type)
            VALUES (?,?,?,?)
        """, (col(src, r, 0), col(src, r, 1), col(src, r, 2), col(src, r, 6, 1)))
    print(f"npc_templates: {len(npc_rows)} rows")

    # ── npc_positions ─────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE npc_positions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER,
            npc_id INTEGER,
            name TEXT,
            npc_type INTEGER DEFAULT 0,
            pos_x REAL,
            pos_y REAL,
            pos_z REAL,
            rotation REAL DEFAULT 0
        )
    """)
    snpc_rows = src.execute("SELECT * FROM game_staticnpc").fetchall()
    for r in snpc_rows:
        dst.execute("""
            INSERT INTO npc_positions (map_id, npc_id, name, npc_type, pos_x, pos_y, pos_z, rotation)
            VALUES (?,?,?,?,?,?,?,?)
        """, (
            col(src, r, 0), col(src, r, 1), col(src, r, 2),
            col(src, r, 3), col(src, r, 4), 0, col(src, r, 5), col(src, r, 6)
        ))
    print(f"npc_positions: {len(snpc_rows)} rows")

    # ── npc_shop_entries (from game_dealitem) ─────────────────────────
    dst.execute("""
        CREATE TABLE npc_shop_entries (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            npc_id INTEGER,
            item_id INTEGER,
            price INTEGER DEFAULT 0,
            stock INTEGER DEFAULT -1,
            map_id INTEGER DEFAULT 0
        )
    """)
    deal_rows = src.execute("SELECT * FROM game_dealitem").fetchall()
    shop_count = 0
    for r in deal_rows:
        npc_name = col(src, r, 3, "")
        for item_offset in range(8, 80, 2):
            if item_offset + 1 >= len(r):
                break
            item_id = col(src, r, item_offset + 1, 0)
            if item_id == 0:
                continue
            price = 0
            if item_offset + 2 < len(r):
                price = col(src, r, item_offset + 2, 0)
            npc_id = col(src, r, 4)
            if not isinstance(npc_id, int) or npc_id == 0:
                npc_id = col(src, r, 0) * 100 + 1
            dst.execute("""
                INSERT INTO npc_shop_entries (npc_id, item_id, price, stock, map_id)
                VALUES (?,?,?,-1,?)
            """, (npc_id, item_id, price, col(src, r, 0)))
            shop_count += 1
    print(f"npc_shop_entries: {shop_count} rows")

    # ── quest_templates ───────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE quest_templates (
            id INTEGER PRIMARY KEY,
            title TEXT DEFAULT '',
            description TEXT DEFAULT '',
            level_required INTEGER DEFAULT 0,
            giver_npc_id INTEGER DEFAULT 0,
            completer_npc_id INTEGER DEFAULT 0,
            reward_exp INTEGER DEFAULT 0,
            reward_gold INTEGER DEFAULT 0,
            reward_item_id INTEGER DEFAULT 0,
            reward_item_count INTEGER DEFAULT 1
        )
    """)
    quest_rows = src.execute("SELECT * FROM game_questnpclist").fetchall()
    seen = {}
    for r in quest_rows:
        qid = col(src, r, 0)
        if qid in seen:
            continue
        seen[qid] = True
        dst.execute("""
            INSERT INTO quest_templates (id, giver_npc_id, completer_npc_id)
            VALUES (?,?,?)
        """, (qid, col(src, r, 1), col(src, r, 1)))
    print(f"quest_templates: {len(seen)} rows")

    # ── quest_conditions ───────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE quest_conditions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER,
            condition_type INTEGER DEFAULT 0,
            target_id INTEGER DEFAULT 0,
            target_count INTEGER DEFAULT 1,
            map_id INTEGER DEFAULT 0,
            pos_x REAL DEFAULT 0,
            pos_y REAL DEFAULT 0,
            radius REAL DEFAULT 10
        )
    """)
    for r in quest_rows:
        qid = col(src, r, 0)
        target_id = col(src, r, 3, 0)
        if target_id:
            dst.execute("""
                INSERT INTO quest_conditions (quest_id, condition_type, target_id, target_count, pos_x, pos_y)
                VALUES (?, 0, ?, 1, ?, ?)
            """, (qid, target_id, col(src, r, 4, 0), col(src, r, 5, 0)))
    print(f"quest_conditions: inserted")

    # ── quest_strings (from game_queststring) ─────────────────────────
    dst.execute("""
        CREATE TABLE quest_strings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_id INTEGER,
            language TEXT DEFAULT 'EN',
            title TEXT DEFAULT '',
            description TEXT DEFAULT ''
        )
    """)
    qstr_rows = src.execute("SELECT * FROM game_queststring").fetchall()
    for r in qstr_rows:
        text = col(src, r, 0, "")
        if "$SUBQUESTSTR" in str(text):
            parts = str(text).split()
            if len(parts) >= 2:
                try:
                    qid = int(parts[1])
                    title = col(src, r, 4, "")
                    desc = col(src, r, 5, "")
                    dst.execute("""
                        INSERT INTO quest_strings (quest_id, title, description)
                        VALUES (?,?,?)
                    """, (qid, str(parts[0]), str(desc)[:200]))
                except ValueError:
                    pass
    print(f"quest_strings: inserted")

    # ── skill_data ─────────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE skill_data (
            id INTEGER PRIMARY KEY,
            name TEXT,
            class_id INTEGER DEFAULT 0,
            skill_type INTEGER DEFAULT 0,
            level_required INTEGER DEFAULT 0,
            target_type INTEGER DEFAULT 0,
            range REAL DEFAULT 0,
            cost_hp INTEGER DEFAULT 0,
            cost_mp INTEGER DEFAULT 0,
            cooldown_ms INTEGER DEFAULT 0,
            damage_mult REAL DEFAULT 1.0,
            damage_fixed INTEGER DEFAULT 0,
            weapon_type INTEGER DEFAULT 0,
            sp_cost INTEGER DEFAULT 0
        )
    """)
    skill_rows = src.execute("SELECT * FROM game_skilllist").fetchall()
    seen_skills = set()
    skill_count = 0
    for r in skill_rows:
        sid = col(src, r, 0)
        if sid in seen_skills:
            continue
        seen_skills.add(sid)
        try:
            dst.execute("""
                INSERT INTO skill_data
                (id, name, class_id, skill_type, level_required,
                 cost_mp, cooldown_ms, weapon_type, sp_cost)
                VALUES (?,?,?,?,?,?,?,?,?)
            """, (
                sid, col(src, r, 1), col(src, r, 2, 0),
                col(src, r, 5, 0), col(src, r, 19, 0),
                col(src, r, 8, 0), col(src, r, 21, 0),
                col(src, r, 22, 0), col(src, r, 20, 0)
            ))
            skill_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"skill_data: {skill_count} rows (deduped)")

    # ── buff_skills ────────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE buff_skills (
            id INTEGER PRIMARY KEY,
            name TEXT,
            buff_level INTEGER DEFAULT 1,
            skill_ref_id INTEGER DEFAULT 0,
            duration_ms INTEGER DEFAULT 0,
            buff_type INTEGER DEFAULT 0,
            buff_value INTEGER DEFAULT 0,
            buff_chance INTEGER DEFAULT 100,
            icon_id INTEGER DEFAULT 0
        )
    """)
    seen_buffs = set()
    buff_count = 0
    buff_rows = src.execute("SELECT * FROM game_skill_buff_list").fetchall()
    for r in buff_rows:
        bid = col(src, r, 0)
        if bid in seen_buffs:
            continue
        seen_buffs.add(bid)
        try:
            dst.execute("""
                INSERT INTO buff_skills
                (id, name, buff_level, skill_ref_id, duration_ms,
                 buff_type, buff_value, buff_chance, icon_id)
                VALUES (?,?,?,?,?,?,?,?,?)
            """, (
                bid, col(src, r, 1), col(src, r, 2, 1),
                col(src, r, 4, 0), col(src, r, 7, 0),
                col(src, r, 6, 0), col(src, r, 13, 0),
                col(src, r, 11, 100), col(src, r, 5, 0)
            ))
            buff_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"buff_skills: {buff_count} rows (deduped)")

    # ── skill_trees ────────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE skill_trees (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            class_id INTEGER,
            tree_level INTEGER,
            slot_index INTEGER,
            skill_id INTEGER
        )
    """)
    tree_rows = src.execute("SELECT * FROM game_skilltreelist").fetchall()
    for r in tree_rows:
        dst.execute("""
            INSERT INTO skill_trees (class_id, tree_level, slot_index, skill_id)
            VALUES (?,?,?,?)
        """, (col(src, r, 0), col(src, r, 1), col(src, r, 2), col(src, r, 3)))
    print(f"skill_trees: {len(tree_rows)} rows")

    # ── item_templates ─────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE item_templates (
            id INTEGER PRIMARY KEY,
            name TEXT,
            item_type INTEGER DEFAULT 0,
            item_subtype INTEGER DEFAULT 0,
            level_required INTEGER DEFAULT 0,
            attack INTEGER DEFAULT 0,
            defense INTEGER DEFAULT 0,
            magic_attack INTEGER DEFAULT 0,
            magic_defense INTEGER DEFAULT 0,
            price_buy INTEGER DEFAULT 0,
            price_sell INTEGER DEFAULT 0,
            max_stack INTEGER DEFAULT 1,
            rarity INTEGER DEFAULT 0,
            resource_id INTEGER DEFAULT 0
        )
    """)
    seen_items = set()
    item_count = 0
    item_rows = src.execute("SELECT * FROM game_itemlist").fetchall()
    for r in item_rows:
        iid = col(src, r, 0)
        if iid in seen_items:
            continue
        seen_items.add(iid)
        try:
            dst.execute("""
                INSERT INTO item_templates
                (id, name, item_type, item_subtype, level_required,
                 attack, defense, magic_attack, magic_defense,
                 price_buy, price_sell, rarity, resource_id)
                VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)
            """, (
                iid, col(src, r, 1), col(src, r, 3, 0),
                col(src, r, 4, 0), col(src, r, 28, 0),
                col(src, r, 10, 0), col(src, r, 12, 0),
                col(src, r, 11, 0), 0,
                col(src, r, 19, 0), col(src, r, 20, 0),
                col(src, r, 4, 1), col(src, r, 5, 0)
            ))
            item_count += 1
        except sqlite3.IntegrityError:
            pass
    print(f"item_templates: {item_count} rows (deduped)")

    # ── map_warps (from game_mapmovelist) ──────────────────────────────
    dst.execute("""
        CREATE TABLE map_warps (
            id INTEGER PRIMARY KEY,
            map_from INTEGER,
            map_to INTEGER,
            from_x REAL,
            from_z REAL,
            to_x REAL,
            to_z REAL,
            name TEXT,
            dest_name TEXT,
            fee INTEGER DEFAULT 0
        )
    """)
    warp_rows = src.execute("SELECT * FROM game_mapmovelist").fetchall()
    for r in warp_rows:
        dst.execute("""
            INSERT INTO map_warps (id, map_from, map_to, from_x, from_z, to_x, to_z, name, dest_name, fee)
            VALUES (?,?,?,?,?,?,?,?,?,?)
        """, (
            col(src, r, 0), col(src, r, 3), col(src, r, 4),
            col(src, r, 5), col(src, r, 6),
            col(src, r, 7), col(src, r, 8),
            col(src, r, 1), col(src, r, 2), col(src, r, 9)
        ))
    print(f"map_warps: {len(warp_rows)} rows")

    # ── map_boundaries (from game_mapchange) ──────────────────────────
    dst.execute("""
        CREATE TABLE map_boundaries (
            id INTEGER PRIMARY KEY,
            map_a INTEGER,
            map_b INTEGER,
            boundary_x_a REAL,
            boundary_z_a REAL,
            boundary_x_b REAL,
            boundary_z_b REAL,
            name_a TEXT,
            name_b TEXT,
            level_required INTEGER DEFAULT 0
        )
    """)
    change_rows = src.execute("SELECT * FROM game_mapchange").fetchall()
    for r in change_rows:
        dst.execute("""
            INSERT INTO map_boundaries
            (id, map_a, map_b, boundary_x_a, boundary_z_a, boundary_x_b, boundary_z_b, name_a, name_b, level_required)
            VALUES (?,?,?,?,?,?,?,?,?,?)
        """, (
            col(src, r, 0), col(src, r, 3), col(src, r, 4),
            col(src, r, 5), col(src, r, 6),
            col(src, r, 7), col(src, r, 8),
            col(src, r, 1), col(src, r, 2), col(src, r, 9)
        ))
    print(f"map_boundaries: {len(change_rows)} rows")

    # ── map_data (from scene.json files) ──────────────────────────────
    dst.execute("""
        CREATE TABLE map_data (
            id INTEGER PRIMARY KEY,
            name TEXT,
            file_path TEXT,
            hgt_file TEXT,
            box_min_x REAL,
            box_min_y REAL,
            box_min_z REAL,
            box_max_x REAL,
            box_max_y REAL,
            box_max_z REAL
        )
    """)
    # We'll register maps from the warps table
    map_ids = set()
    for r in warp_rows:
        map_ids.add(col(src, r, 3))
        map_ids.add(col(src, r, 4))
    for mid in sorted(map_ids):
        dst.execute("""
            INSERT INTO map_data (id, name, hgt_file)
            VALUES (?, ?, ?)
        """, (mid, f"Map_{mid}", f"{mid}.hgt"))
    print(f"map_data: {len(map_ids)} rows")

    # ── monster_spawns ─────────────────────────────────────────────────
    dst.execute("""
        CREATE TABLE monster_spawns (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            map_id INTEGER,
            monster_id INTEGER,
            count INTEGER DEFAULT 1,
            respawn_time INTEGER DEFAULT 30,
            spawn_radius REAL DEFAULT 10.0
        )
    """)
    for r in rows:
        mid = col(src, r, 0)
        map_id = col(src, r, 8, 0)
        if map_id:
            dst.execute("""
                INSERT INTO monster_spawns (map_id, monster_id, count, respawn_time)
                VALUES (?,?,1,30)
            """, (map_id, mid))
    print(f"monster_spawns: inserted from {len(rows)} monsters")

    dst.commit()
    dst.execute("VACUUM")
    dst.close()
    src.close()

    size = DST.stat().st_size
    print(f"\nCreated {DST}")
    print(f"Size: {size / 1024:.1f} KB")

if __name__ == "__main__":
    main()
