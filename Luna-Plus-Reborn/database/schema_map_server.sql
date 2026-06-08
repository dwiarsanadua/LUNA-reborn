-- MapServer-specific tables (Phase 5 extensions)

CREATE TABLE IF NOT EXISTS map_triggers (
    trigger_id INTEGER PRIMARY KEY,
    map_id INTEGER NOT NULL,
    trigger_type INTEGER DEFAULT 0,
    pos_x REAL, pos_z REAL, radius REAL DEFAULT 8,
    param0 INTEGER DEFAULT 0, param1 INTEGER DEFAULT 0,
    script_path TEXT, repeatable INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_map_triggers_map ON map_triggers(map_id);

CREATE TABLE IF NOT EXISTS player_quests (
    character_id INTEGER NOT NULL,
    quest_id INTEGER NOT NULL,
    state INTEGER DEFAULT 0,
    progress TEXT,
    updated_at TEXT DEFAULT (datetime('now')),
    PRIMARY KEY (character_id, quest_id)
);

CREATE INDEX IF NOT EXISTS idx_player_quests_char ON player_quests(character_id);

-- Phase 6 secondary feature seeds (MapServer runtime)

CREATE TABLE IF NOT EXISTS phase6_fish_types (
    item_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    rarity INTEGER DEFAULT 0,
    weight REAL DEFAULT 1.0
);

CREATE TABLE IF NOT EXISTS phase6_territories (
    territory_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    owner_guild_id INTEGER DEFAULT 0,
    owner_guild_name TEXT DEFAULT '',
    tax_rate INTEGER DEFAULT 10
);

CREATE TABLE IF NOT EXISTS phase6_shop_items (
    item_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    price INTEGER DEFAULT 0,
    category TEXT DEFAULT 'Misc'
);
