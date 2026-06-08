#!/usr/bin/env python3
"""Bootstrap Phase 6: secondary feature seeds (family, pet, fish, siege, shop)."""

import sqlite3
import subprocess
import sys
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
DATA = BASE / "assets" / "data"
MAP_DB = DATA / "luna_map.db"
MIGRATE = BASE / "tools" / "asset_pipeline" / "migrate_schema_sqlite.py"

FISH_SEED = [
    (21000001, "Common Carp", 0, 1.0),
    (21000002, "Silver Trout", 1, 1.5),
    (21000003, "Golden Koi", 2, 2.0),
]

TERRITORY_SEED = [
    (1, "Alker Harbor", 0, "", 10, 0, 5),
    (2, "Red Orc Outpost", 0, "", 12, 0, 7),
    (3, "Siege Castle", 0, "", 15, 1, 10),
]

TOURNAMENT_SEED = [
    (1, "Weekly Arena", 0, 8, 5000, 0, 0, 1, 6, 0, 0),
    (2, "Guild Championship", 0, 16, 15000, 0, 0, 1, 6, 0, 0),
]

SHOP_SEED = [
    (1001, "Health Potion", 50, "Consumables", "Restore 200 HP", 0, 1, 99, 0, 0),
    (1002, "Mana Potion", 50, "Consumables", "Restore 150 MP", 0, 1, 99, 0, 0),
    (201, "Pet Summon Scroll", 200, "Pets", "Summon a pet companion", 0, 1, 5, 0, 0),
    (301, "Costume Box", 150, "Cosmetics", "Random cosmetic costume", 1, 1, 3, 0, 0),
    (401, "Fishing Bait x10", 30, "Misc", "Ten fishing baits", 0, 10, 99, 0, 0),
]

HOUSE_TEMPLATE_SEED = [
    (0, "Cozy Cottage", 10000, 16, 51),
    (1, "Town House", 25000, 24, 13),
    (2, "Grand Villa", 50000, 32, 20),
]

FURNITURE_CATALOG_SEED = [
    (9001, "Wooden Table", "Tables"),
    (9002, "Comfy Chair", "Seating"),
    (9003, "Lamp", "Lighting"),
    (9004, "Rug", "Decor"),
    (9005, "Bookshelf", "Storage"),
    (9006, "Bed", "Bedroom"),
    (9007, "Desk", "Tables"),
    (9008, "Cabinet", "Storage"),
]


def run_migration() -> None:
    subprocess.run([sys.executable, str(MIGRATE)], check=True)


def seed_phase6(cur: sqlite3.Cursor) -> None:
    for row in FISH_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_fish_types (item_id, name, rarity, weight) "
            "VALUES (?, ?, ?, ?)",
            row,
        )
    for row in TERRITORY_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_territories "
            "(territory_id, name, owner_guild_id, owner_guild_name, tax_rate, is_castle, defense_bonus) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)",
            row,
        )
    import time
    now = int(time.time())
    for row in TOURNAMENT_SEED:
        reg_end = now + (3600 if row[0] == 1 else 7200)
        cur.execute(
            "INSERT OR REPLACE INTO phase6_tournaments "
            "(tournament_id, name, state, max_teams, prize_gold, registration_end, "
            "current_round, min_team_size, max_team_size, winner_guild_id, prize_claimed) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            (row[0], row[1], row[2], row[3], row[4], reg_end,
             row[5], row[6], row[7], row[8], row[9]),
        )
    for row in SHOP_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_shop_items "
            "(item_id, name, price, category, description, currency_type, stack_count, "
            "max_purchase, on_sale, sale_price) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            row,
        )
    cur.execute(
        "INSERT OR IGNORE INTO player_cash_shop "
        "(character_id, luna_points, battle_pass_level, battle_pass_xp, battle_pass_active) "
        "VALUES (1, 500, 1, 0, 0)"
    )
    for row in HOUSE_TEMPLATE_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_house_templates "
            "(template_id, name, price, max_furniture, map_id) VALUES (?, ?, ?, ?, ?)",
            row,
        )
    for row in FURNITURE_CATALOG_SEED:
        cur.execute(
            "INSERT OR REPLACE INTO phase6_furniture_catalog "
            "(item_id, name, category) VALUES (?, ?, ?)",
            row,
        )

    cur.execute(
        "INSERT OR IGNORE INTO TB_PET "
        "(PetIdx, CharacterIdx, PetDBIdx, PetName, Level, HP, Satiation) "
        "VALUES (1, 1, 1, 'Fluffy', 1, 100, 100)"
    )
    cur.execute(
        "INSERT OR REPLACE INTO player_pet "
        "(character_id, pet_id, template_id, name, level, hp, max_hp, satiation, summoned, evolution, exp) "
        "VALUES (1, 1, 1, 'Fluffy', 1, 100, 100, 100, 0, 1, 0)"
    )
    cur.execute(
        "INSERT OR IGNORE INTO TB_HOUSE "
        "(HouseIdx, CharacterIdx, MapIdx, PosX, PosY, HouseType, FurnitureCount) "
        "VALUES (1, 1, 51, 10.0, 0.0, 0, 0)"
    )


def main() -> int:
    print("Phase 6 bootstrap:")
    run_migration()
    DATA.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(MAP_DB)
    seed_phase6(conn.cursor())
    fish = conn.execute("SELECT COUNT(*) FROM phase6_fish_types").fetchone()[0]
    shop = conn.execute("SELECT COUNT(*) FROM phase6_shop_items").fetchone()[0]
    houses = conn.execute("SELECT COUNT(*) FROM phase6_house_templates").fetchone()[0]
    tours = conn.execute("SELECT COUNT(*) FROM phase6_tournaments").fetchone()[0]
    conn.commit()
    conn.close()
    print(f"  phase6_fish_types: {fish}")
    print(f"  phase6_shop_items: {shop}")
    print(f"  phase6_house_templates: {houses}")
    print(f"  phase6_tournaments: {tours}")
    print("Phase 6 bootstrap OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
