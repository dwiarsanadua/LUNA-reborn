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
    tax_rate INTEGER DEFAULT 10,
    siege_time INTEGER DEFAULT 0,
    attacker_guild_id INTEGER DEFAULT 0,
    attacker_guild_name TEXT DEFAULT '',
    is_castle INTEGER DEFAULT 0,
    defense_bonus INTEGER DEFAULT 0,
    tax_accumulated INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS phase6_shop_items (
    item_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    price INTEGER DEFAULT 0,
    category TEXT DEFAULT 'Misc',
    description TEXT DEFAULT '',
    currency_type INTEGER DEFAULT 0,
    stack_count INTEGER DEFAULT 1,
    max_purchase INTEGER DEFAULT 99,
    on_sale INTEGER DEFAULT 0,
    sale_price INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS player_cash_shop (
    character_id INTEGER PRIMARY KEY,
    luna_points INTEGER DEFAULT 500,
    battle_pass_level INTEGER DEFAULT 1,
    battle_pass_xp INTEGER DEFAULT 0,
    battle_pass_active INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS player_cashshop_purchases (
    character_id INTEGER NOT NULL,
    shop_item_id INTEGER NOT NULL,
    purchase_count INTEGER DEFAULT 0,
    PRIMARY KEY (character_id, shop_item_id)
);

CREATE TABLE IF NOT EXISTS phase6_house_templates (
    template_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    price INTEGER DEFAULT 10000,
    max_furniture INTEGER DEFAULT 16,
    map_id INTEGER DEFAULT 51
);

CREATE TABLE IF NOT EXISTS phase6_furniture_catalog (
    item_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    category TEXT DEFAULT 'Misc'
);

CREATE TABLE IF NOT EXISTS phase6_tournaments (
    tournament_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    state INTEGER DEFAULT 0,
    max_teams INTEGER DEFAULT 8,
    prize_gold INTEGER DEFAULT 5000,
    registration_end INTEGER DEFAULT 0,
    current_round INTEGER DEFAULT 0,
    min_team_size INTEGER DEFAULT 1,
    max_team_size INTEGER DEFAULT 6,
    winner_guild_id INTEGER DEFAULT 0,
    prize_claimed INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS phase6_tournament_registrations (
    tournament_id INTEGER NOT NULL,
    guild_id INTEGER NOT NULL,
    guild_name TEXT DEFAULT '',
    team_leader_id INTEGER DEFAULT 0,
    seed INTEGER DEFAULT 0,
    eliminated INTEGER DEFAULT 0,
    PRIMARY KEY (tournament_id, guild_id)
);

CREATE TABLE IF NOT EXISTS phase6_tournament_matches (
    tournament_id INTEGER NOT NULL,
    round_num INTEGER NOT NULL,
    match_index INTEGER NOT NULL,
    team1_guild_id INTEGER DEFAULT 0,
    team2_guild_id INTEGER DEFAULT 0,
    winner_guild_id INTEGER DEFAULT 0,
    completed INTEGER DEFAULT 0,
    PRIMARY KEY (tournament_id, round_num, match_index)
);

-- Phase 6 Slice 2: per-character persistence (MapServer runtime)

CREATE TABLE IF NOT EXISTS player_family (
    character_id INTEGER PRIMARY KEY,
    name TEXT DEFAULT '',
    relation INTEGER DEFAULT 1,
    partner_id INTEGER DEFAULT 0,
    partner_name TEXT DEFAULT '',
    family_id INTEGER DEFAULT 0,
    family_name TEXT DEFAULT '',
    married_date INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS player_pet (
    character_id INTEGER PRIMARY KEY,
    pet_id INTEGER DEFAULT 1,
    template_id INTEGER DEFAULT 1,
    name TEXT DEFAULT 'Fluffy',
    level INTEGER DEFAULT 1,
    hp INTEGER DEFAULT 100,
    max_hp INTEGER DEFAULT 100,
    satiation INTEGER DEFAULT 100,
    summoned INTEGER DEFAULT 0,
    evolution INTEGER DEFAULT 1,
    exp INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS player_farm_plot (
    character_id INTEGER NOT NULL,
    plot_id INTEGER NOT NULL,
    seed_id INTEGER DEFAULT 0,
    plant_name TEXT DEFAULT '',
    growth_stage INTEGER DEFAULT 0,
    max_stages INTEGER DEFAULT 4,
    growth_timer REAL DEFAULT 0,
    growth_time REAL DEFAULT 60,
    watered INTEGER DEFAULT 0,
    harvested INTEGER DEFAULT 0,
    PRIMARY KEY (character_id, plot_id)
);

CREATE INDEX IF NOT EXISTS idx_player_farm_char ON player_farm_plot(character_id);
