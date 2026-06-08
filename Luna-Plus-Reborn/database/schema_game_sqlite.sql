-- ============================================================
-- LUNA Plus — Game Database Schema (SQLite)
-- SQLite-compatible version of schema_game.sql
-- ============================================================

BEGIN;

-- ─── Character ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CHARACTER (
    CharacterIdx    INTEGER PRIMARY KEY AUTOINCREMENT,
    AccountID       TEXT NOT NULL,
    CharName        TEXT UNIQUE NOT NULL,
    Level           INTEGER DEFAULT 1,
    Exp             INTEGER DEFAULT 0,
    Money           INTEGER DEFAULT 0,
    MapIdx          INTEGER DEFAULT 0,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    HP              INTEGER DEFAULT 100,
    MP              INTEGER DEFAULT 100,
    SP              INTEGER DEFAULT 100,
    Str             INTEGER DEFAULT 10,
    Dex             INTEGER DEFAULT 10,
    Int             INTEGER DEFAULT 10,
    Con             INTEGER DEFAULT 10,
    Wis             INTEGER DEFAULT 10,
    Luck            INTEGER DEFAULT 10,
    Job             INTEGER DEFAULT 0,
    Face            INTEGER DEFAULT 0,
    Hair            INTEGER DEFAULT 0,
    CreateDate      TEXT DEFAULT (datetime('now')),
    DeleteDate      TEXT,
    LastLogin       TEXT,
    LastLogout      TEXT,
    LoginTime       INTEGER DEFAULT 0,
    PlayTime        INTEGER DEFAULT 0,
    PvpKillCount    INTEGER DEFAULT 0,
    PvpDeathCount   INTEGER DEFAULT 0,
    PvpPoint        INTEGER DEFAULT 0,
    Fame            INTEGER DEFAULT 0,
    BadFame         INTEGER DEFAULT 0,
    StatPoint       INTEGER DEFAULT 0,
    SkillPoint      INTEGER DEFAULT 0,
    HonorPoint      INTEGER DEFAULT 0,
    RestExp         INTEGER DEFAULT 0,
    ServerGroup     INTEGER DEFAULT 0,
    ChannelIdx      INTEGER DEFAULT 0,
    Attribute       INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_char_account ON TB_CHARACTER(AccountID);
CREATE INDEX IF NOT EXISTS idx_char_name ON TB_CHARACTER(CharName);
CREATE INDEX IF NOT EXISTS idx_char_level ON TB_CHARACTER(Level);
CREATE INDEX IF NOT EXISTS idx_char_map ON TB_CHARACTER(MapIdx);

-- ─── Character Buffs ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CHARACTER_BUFF (
    BuffIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SkillIdx        INTEGER NOT NULL,
    RemainTime      INTEGER DEFAULT 0,
    Count           INTEGER DEFAULT 0,
    IsEndDate       INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_buff_char ON TB_CHARACTER_BUFF(CharacterIdx);

-- ─── Item ───────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM (
    ItemIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER REFERENCES TB_CHARACTER(CharacterIdx),
    ItemDBIdx       INTEGER NOT NULL,
    Count           INTEGER DEFAULT 1,
    Slot            INTEGER DEFAULT 0,
    StorageSlot     INTEGER DEFAULT -1,
    StorageType     INTEGER DEFAULT 0,
    Grade           INTEGER DEFAULT 0,
    Durability      INTEGER DEFAULT 100,
    MaxDurability   INTEGER DEFAULT 100,
    IsSealed        INTEGER DEFAULT 0,
    SealRemainTime  INTEGER DEFAULT 0,
    SealGameTime    INTEGER DEFAULT 0,
    Bless           INTEGER DEFAULT 0,
    Socket          INTEGER DEFAULT 0,
    Element         INTEGER DEFAULT 0,
    ElementValue    INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_item_char ON TB_ITEM(CharacterIdx);
CREATE INDEX IF NOT EXISTS idx_item_db ON TB_ITEM(ItemDBIdx);
CREATE INDEX IF NOT EXISTS idx_item_slot ON TB_ITEM(CharacterIdx, Slot);

-- ─── Item Options ───────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM_OPTION (
    OptionIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    ItemIdx         INTEGER NOT NULL REFERENCES TB_ITEM(ItemIdx) ON DELETE CASCADE,
    OptType         INTEGER NOT NULL,
    OptValue        REAL NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_option_item ON TB_ITEM_OPTION(ItemIdx);

-- ─── Skill ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_SKILL (
    SkillIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SkillID         INTEGER NOT NULL,
    SkillLevel      INTEGER DEFAULT 1,
    IsPassive       INTEGER DEFAULT 0,
    Slot            INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_skill_char ON TB_SKILL(CharacterIdx);

-- ─── Quick Slot ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_QUICKSLOT (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SlotData        TEXT NOT NULL DEFAULT ''
);

-- ─── Quest ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_QUEST (
    QuestIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    QuestID         INTEGER NOT NULL,
    QuestState      INTEGER DEFAULT 0,
    Progress        TEXT,
    RegDate         TEXT DEFAULT (datetime('now')),
    CompleteDate    TEXT,
    UNIQUE(CharacterIdx, QuestID)
);

CREATE INDEX IF NOT EXISTS idx_quest_char ON TB_QUEST(CharacterIdx);

-- ─── Main Quest ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_MAINQUEST (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    MainQuestID     INTEGER DEFAULT 0,
    SubQuestID      INTEGER DEFAULT 0,
    QuestStep       INTEGER DEFAULT 0
);

-- ─── Party ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PARTY (
    PartyIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    CreateDate      TEXT DEFAULT (datetime('now')),
    IsAutoMatch     INTEGER DEFAULT 0,
    MapServerIdx    INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_PARTY_MEMBER (
    PartyIdx        INTEGER NOT NULL REFERENCES TB_PARTY(PartyIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    JoinDate        TEXT DEFAULT (datetime('now')),
    PRIMARY KEY (PartyIdx, CharacterIdx)
);

-- ─── Guild ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD (
    GuildIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildName       TEXT UNIQUE NOT NULL,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Level           INTEGER DEFAULT 1,
    Exp             INTEGER DEFAULT 0,
    Score           INTEGER DEFAULT 0,
    Money           INTEGER DEFAULT 0,
    MaxMember       INTEGER DEFAULT 20,
    CreateDate      TEXT DEFAULT (datetime('now')),
    DeleteDate      TEXT,
    MarkData        BLOB,
    MarkLen         INTEGER DEFAULT 0,
    Notice          TEXT
);

CREATE INDEX IF NOT EXISTS idx_guild_name ON TB_GUILD(GuildName);
CREATE INDEX IF NOT EXISTS idx_guild_master ON TB_GUILD(MasterIdx);

CREATE TABLE IF NOT EXISTS TB_GUILD_MEMBER (
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Rank            INTEGER DEFAULT 1,
    JoinDate        TEXT DEFAULT (datetime('now')),
    Contribution    INTEGER DEFAULT 0,
    PRIMARY KEY (GuildIdx, CharacterIdx)
);

CREATE TABLE IF NOT EXISTS TB_GUILD_SKILL (
    SkillIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    SKILL_IDX       INTEGER NOT NULL,
    SKILL_LEVEL     INTEGER DEFAULT 1
);

CREATE INDEX IF NOT EXISTS idx_guildskill_guild ON TB_GUILD_SKILL(GuildIdx);

CREATE TABLE IF NOT EXISTS TB_GUILD_WAREHOUSE (
    GuildIdx        INTEGER PRIMARY KEY REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    Money           INTEGER DEFAULT 0,
    SlotCount       INTEGER DEFAULT 0
);

-- ─── Friend ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FRIEND (
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FriendIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FriendGroup     INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now')),
    PRIMARY KEY (CharacterIdx, FriendIdx)
);

-- ─── Note / Mail ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_NOTE (
    NoteIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    SenderIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    ReceiverIdx     INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Title           TEXT,
    Content         TEXT,
    SendDate        TEXT DEFAULT (datetime('now')),
    IsRead          INTEGER DEFAULT 0,
    ReadDate        TEXT,
    HasPackage      INTEGER DEFAULT 0,
    PackageMoney    INTEGER DEFAULT 0,
    PackageItemIdx  INTEGER REFERENCES TB_ITEM(ItemIdx)
);

CREATE INDEX IF NOT EXISTS idx_note_receiver ON TB_NOTE(ReceiverIdx);
CREATE INDEX IF NOT EXISTS idx_note_sender ON TB_NOTE(SenderIdx);
CREATE INDEX IF NOT EXISTS idx_note_unread ON TB_NOTE(ReceiverIdx, IsRead);

-- ─── Family ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FAMILY (
    FamilyIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    FamilyName      TEXT UNIQUE NOT NULL,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    CreateDate      TEXT DEFAULT (datetime('now')),
    EmblemData      BLOB,
    EmblemLen       INTEGER DEFAULT 0,
    Point           INTEGER DEFAULT 0,
    Level           INTEGER DEFAULT 1
);

CREATE TABLE IF NOT EXISTS TB_FAMILY_MEMBER (
    FamilyIdx       INTEGER NOT NULL REFERENCES TB_FAMILY(FamilyIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    JoinDate        TEXT DEFAULT (datetime('now')),
    Role            INTEGER DEFAULT 0,
    Contribution    INTEGER DEFAULT 0,
    PRIMARY KEY (FamilyIdx, CharacterIdx)
);

-- ─── Farm ───────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FARM (
    FarmIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FarmName        TEXT,
    MapIdx          INTEGER DEFAULT 0,
    Size            INTEGER DEFAULT 1,
    TaxRate         REAL DEFAULT 0,
    State           INTEGER DEFAULT 0,
    DelayTime       INTEGER DEFAULT 0,
    CreateDate      TEXT DEFAULT (datetime('now')),
    UNIQUE(CharacterIdx)
);

CREATE TABLE IF NOT EXISTS TB_FARM_CROP (
    CropIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    FarmIdx         INTEGER NOT NULL REFERENCES TB_FARM(FarmIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            INTEGER DEFAULT 0,
    PosY            INTEGER DEFAULT 0,
    State           INTEGER DEFAULT 0,
    RemainTime      INTEGER DEFAULT 0,
    PlantDate       TEXT DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS TB_FARM_ANIMAL (
    AnimalIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    FarmIdx         INTEGER NOT NULL REFERENCES TB_FARM(FarmIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            INTEGER DEFAULT 0,
    PosY            INTEGER DEFAULT 0,
    State           INTEGER DEFAULT 0,
    RemainTime      INTEGER DEFAULT 0
);

-- ─── Housing ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_HOUSE (
    HouseIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER DEFAULT 0,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    HouseType       INTEGER DEFAULT 0,
    FurnitureCount  INTEGER DEFAULT 0,
    CreateDate      TEXT DEFAULT (datetime('now')),
    LastRepairDate  TEXT,
    State           INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_house_char ON TB_HOUSE(CharacterIdx);

-- ─── Pet ────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PET (
    PetIdx          INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    PetDBIdx        INTEGER NOT NULL,
    PetName         TEXT,
    Level           INTEGER DEFAULT 1,
    Experience      INTEGER DEFAULT 0,
    Friendship      INTEGER DEFAULT 0,
    Grade           INTEGER DEFAULT 0,
    Kind            INTEGER DEFAULT 0,
    State           INTEGER DEFAULT 0,
    SkillSlot       INTEGER DEFAULT 0,
    HP              INTEGER DEFAULT 100,
    MP              INTEGER DEFAULT 100,
    Satiation       INTEGER DEFAULT 100,
    RegDate         TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_pet_char ON TB_PET(CharacterIdx);

-- ─── Vehicle ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_VEHICLE (
    VehicleIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    VehicleDBIdx    INTEGER NOT NULL,
    HP              INTEGER DEFAULT 100,
    State           INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── Fishing ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FISHING (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SkillLevel      INTEGER DEFAULT 0,
    Experience      INTEGER DEFAULT 0,
    CatchCount      INTEGER DEFAULT 0,
    MaxLength       REAL DEFAULT 0
);

-- ─── Cooking ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_COOKING (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SkillLevel      INTEGER DEFAULT 0,
    Experience      INTEGER DEFAULT 0
);

-- ─── Storage ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_STORAGE (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SlotCount       INTEGER DEFAULT 0,
    Money           INTEGER DEFAULT 0
);

-- ─── Consignment ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CONSIGNMENT (
    ConsignmentIdx  INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    ItemIdx         INTEGER NOT NULL REFERENCES TB_ITEM(ItemIdx),
    Price           INTEGER NOT NULL,
    RegDate         TEXT DEFAULT (datetime('now')),
    ExpireDate      TEXT,
    IsSold          INTEGER DEFAULT 0,
    SoldDate        TEXT,
    IsCanceled      INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_consignment_char ON TB_CONSIGNMENT(CharacterIdx);
CREATE INDEX IF NOT EXISTS idx_consignment_item ON TB_CONSIGNMENT(ItemIdx);
CREATE INDEX IF NOT EXISTS idx_consignment_active ON TB_CONSIGNMENT(IsSold, IsCanceled);

-- ─── Siege / Recall ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_SIEGE_RECALL (
    RecallIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now')),
    KillCount       INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_SIEGE_WARFARE (
    WarfareIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    MapIdx          INTEGER NOT NULL,
    StartTime       TEXT,
    EndTime         TEXT,
    State           INTEGER DEFAULT 0,
    WaterSeed       INTEGER DEFAULT 0
);

-- ─── Punish List ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PUNISHLIST (
    PunishIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    PunishType      INTEGER DEFAULT 0,
    Reason          TEXT,
    RegDate         TEXT DEFAULT (datetime('now')),
    ExpireDate      TEXT,
    OperatorID      TEXT,
    PunishCount     INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_punish_char ON TB_PUNISHLIST(CharacterIdx);

-- ─── Mob / NPC Recall ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_NPC_RECALL (
    NpcRecallIdx    INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    NpcID           INTEGER NOT NULL,
    RemainTime      INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── Guild Union ───────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD_UNION (
    UnionIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    UnionName       TEXT UNIQUE NOT NULL,
    MasterGuildIdx  INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    CreateDate      TEXT DEFAULT (datetime('now')),
    MarkData        BLOB,
    MarkLen         INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_GUILD_UNION_MEMBER (
    UnionIdx        INTEGER NOT NULL REFERENCES TB_GUILD_UNION(UnionIdx) ON DELETE CASCADE,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    JoinDate        TEXT DEFAULT (datetime('now')),
    PRIMARY KEY (UnionIdx, GuildIdx)
);

-- ─── Housing Furniture ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_HOUSE_FURNITURE (
    FurnitureIdx    INTEGER PRIMARY KEY AUTOINCREMENT,
    HouseIdx        INTEGER NOT NULL REFERENCES TB_HOUSE(HouseIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RotY            REAL DEFAULT 0
);

-- ─── Vehicle Passenger ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_VEHICLE_PASSENGER (
    VehicleIdx      INTEGER NOT NULL REFERENCES TB_VEHICLE(VehicleIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SeatIdx         INTEGER DEFAULT 0,
    PRIMARY KEY (VehicleIdx, CharacterIdx)
);

-- ─── Cook Recipe ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_COOKRECIPE (
    RecipeIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    RecipeID        INTEGER NOT NULL,
    IsLearned       INTEGER DEFAULT 0,
    UNIQUE(CharacterIdx, RecipeID)
);

-- ─── Guild Tournament ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD_TOURNAMENT (
    TournamentIdx   INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    RegDate         TEXT DEFAULT (datetime('now')),
    State           INTEGER DEFAULT 0,
    Round           INTEGER DEFAULT 0,
    Score           INTEGER DEFAULT 0,
    Reward          INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_GUILD_TOURNAMENT_PLAYER (
    TournamentIdx   INTEGER NOT NULL REFERENCES TB_GUILD_TOURNAMENT(TournamentIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    KillCount       INTEGER DEFAULT 0,
    DeathCount      INTEGER DEFAULT 0,
    Score           INTEGER DEFAULT 0,
    PRIMARY KEY (TournamentIdx, CharacterIdx)
);

-- ─── Challenge Zone ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CHALLENGEZONE (
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    EnterFreq       INTEGER DEFAULT 0,
    EnterBonusFreq  INTEGER DEFAULT 0,
    SuccessCount    INTEGER DEFAULT 0,
    ExpRate         REAL DEFAULT 1.0,
    PRIMARY KEY (CharacterIdx)
);

-- ─── Monster Meter ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_MONSTERMETER (
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MonsterDBIdx    INTEGER NOT NULL,
    KillCount       INTEGER DEFAULT 0,
    MaxDamage       INTEGER DEFAULT 0,
    PRIMARY KEY (CharacterIdx, MonsterDBIdx)
);

-- ─── Dungeon ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_DUNGEON (
    DungeonIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    EnterDate       TEXT DEFAULT (datetime('now')),
    ClearDate       TEXT,
    State           INTEGER DEFAULT 0
);

-- ─── Trigger ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_TRIGGER (
    TriggerIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    MapIdx          INTEGER NOT NULL,
    TriggerType     INTEGER DEFAULT 0,
    TriggerData     TEXT,
    TriggerCount    INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now')),
    UpdateDate      TEXT
);

-- ─── Auto Note ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_AUTONOTELIST (
    AutoNoteIdx     INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    AutoName        TEXT,
    AutoCharIdx     INTEGER,
    AutoUserIdx     INTEGER,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── PC Room ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PCROOM (
    PCRoomIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Point           INTEGER DEFAULT 0,
    PlayTime        INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── Bad Fame ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_BADFAME (
    BadFameIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    BadFameValue    INTEGER DEFAULT 0,
    Reason          TEXT,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── Event ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_EVENT (
    EventIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    EventID         INTEGER NOT NULL,
    EventData       TEXT,
    RegDate         TEXT DEFAULT (datetime('now')),
    UNIQUE(CharacterIdx, EventID)
);

-- ─── Job ────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_JOB (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    JobID           INTEGER DEFAULT 0,
    JobLevel        INTEGER DEFAULT 0,
    JobExp          INTEGER DEFAULT 0
);

-- ─── Move Recall ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_MOVE_RECALL (
    RecallIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
);

-- ─── Migration Version Tracking ────────────────────────────
CREATE TABLE IF NOT EXISTS _migration_version (
    version         TEXT PRIMARY KEY,
    applied_at      TEXT NOT NULL DEFAULT (datetime('now')),
    description     TEXT
);

-- ============================================================
-- GAME CONTENT TABLES (migrated from game_data_legacy.db)
-- These are read-only reference tables for game content.
-- ============================================================

-- ─── Item Templates ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS item_templates (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    item_type       INTEGER DEFAULT 0,
    item_subtype    INTEGER DEFAULT 0,
    level_required  INTEGER DEFAULT 0,
    attack          INTEGER DEFAULT 0,
    defense         INTEGER DEFAULT 0,
    magic_attack    INTEGER DEFAULT 0,
    magic_defense   INTEGER DEFAULT 0,
    price_buy       INTEGER DEFAULT 0,
    price_sell      INTEGER DEFAULT 0,
    max_stack       INTEGER DEFAULT 1,
    rarity          INTEGER DEFAULT 0,
    resource_id     INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_item_templates_type ON item_templates(item_type);
CREATE INDEX IF NOT EXISTS idx_item_templates_subtype ON item_templates(item_subtype);
CREATE INDEX IF NOT EXISTS idx_item_templates_level ON item_templates(level_required);
CREATE INDEX IF NOT EXISTS idx_item_templates_name ON item_templates(name);

-- ─── Monster Templates ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS monster_templates (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    model_file      TEXT,
    level           INTEGER,
    hp              INTEGER,
    mp              INTEGER DEFAULT 0,
    attack          INTEGER,
    defense         INTEGER,
    speed           REAL DEFAULT 1.0,
    exp_reward      INTEGER DEFAULT 0,
    gold_min        INTEGER DEFAULT 0,
    gold_max        INTEGER DEFAULT 0,
    element_type    INTEGER DEFAULT 0,
    ai_type         INTEGER DEFAULT 0,
    aggro_range     INTEGER DEFAULT 0,
    size_scale      REAL DEFAULT 1.0,
    monster_type    INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_monster_templates_level ON monster_templates(level);
CREATE INDEX IF NOT EXISTS idx_monster_templates_zone ON monster_templates(monster_type);
CREATE INDEX IF NOT EXISTS idx_monster_templates_element ON monster_templates(element_type);

-- ─── Monster Drops ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS monster_drops (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    monster_id      INTEGER,
    item_id         INTEGER,
    item_name       TEXT,
    min_count       INTEGER DEFAULT 1,
    max_count       INTEGER DEFAULT 1,
    probability     REAL DEFAULT 0.0,
    drop_table_id   INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_monster_drops_monster ON monster_drops(monster_id);
CREATE INDEX IF NOT EXISTS idx_monster_drops_item ON monster_drops(item_id);

-- ─── Monster Spawns ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS monster_spawns (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    map_id          INTEGER,
    monster_id      INTEGER,
    count           INTEGER DEFAULT 1,
    respawn_time    INTEGER DEFAULT 30,
    spawn_radius    REAL DEFAULT 10.0
);

CREATE INDEX IF NOT EXISTS idx_monster_spawns_map ON monster_spawns(map_id);
CREATE INDEX IF NOT EXISTS idx_monster_spawns_monster ON monster_spawns(monster_id);

-- ─── NPC Templates ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS npc_templates (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    npc_type        INTEGER DEFAULT 0,
    shop_type       INTEGER DEFAULT 0,
    dialog_text     TEXT DEFAULT ''
);

CREATE INDEX IF NOT EXISTS idx_npc_templates_type ON npc_templates(npc_type);

-- ─── NPC Positions ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS npc_positions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    map_id          INTEGER,
    npc_id          INTEGER,
    name            TEXT,
    npc_type        INTEGER DEFAULT 0,
    pos_x           REAL,
    pos_y           REAL,
    pos_z           REAL,
    rotation        REAL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_npc_positions_map ON npc_positions(map_id);
CREATE INDEX IF NOT EXISTS idx_npc_positions_npc ON npc_positions(npc_id);

-- ─── NPC Shop Entries ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS npc_shop_entries (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    npc_id          INTEGER,
    item_id         INTEGER,
    price           INTEGER DEFAULT 0,
    stock           INTEGER DEFAULT -1,
    map_id          INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_npc_shop_npc ON npc_shop_entries(npc_id);
CREATE INDEX IF NOT EXISTS idx_npc_shop_item ON npc_shop_entries(item_id);

-- ─── Quest Templates ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS quest_templates (
    id                INTEGER PRIMARY KEY,
    title             TEXT DEFAULT '',
    description       TEXT DEFAULT '',
    level_required    INTEGER DEFAULT 0,
    giver_npc_id      INTEGER DEFAULT 0,
    completer_npc_id  INTEGER DEFAULT 0,
    reward_exp        INTEGER DEFAULT 0,
    reward_gold       INTEGER DEFAULT 0,
    reward_item_id    INTEGER DEFAULT 0,
    reward_item_count INTEGER DEFAULT 1
);

CREATE INDEX IF NOT EXISTS idx_quest_templates_level ON quest_templates(level_required);
CREATE INDEX IF NOT EXISTS idx_quest_templates_giver ON quest_templates(giver_npc_id);

-- ─── Quest Conditions ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS quest_conditions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    quest_id        INTEGER,
    condition_type  INTEGER DEFAULT 0,
    target_id       INTEGER DEFAULT 0,
    target_count    INTEGER DEFAULT 1,
    map_id          INTEGER DEFAULT 0,
    pos_x           REAL DEFAULT 0,
    pos_y           REAL DEFAULT 0,
    radius          REAL DEFAULT 10
);

CREATE INDEX IF NOT EXISTS idx_quest_conditions_quest ON quest_conditions(quest_id);

-- ─── Quest Strings ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS quest_strings (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    quest_id        INTEGER,
    language        TEXT DEFAULT 'EN',
    title           TEXT DEFAULT '',
    description     TEXT DEFAULT ''
);

CREATE INDEX IF NOT EXISTS idx_quest_strings_quest ON quest_strings(quest_id);

-- ─── Skill Data ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS skill_data (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    class_id        INTEGER DEFAULT 0,
    skill_type      INTEGER DEFAULT 0,
    level_required  INTEGER DEFAULT 0,
    target_type     INTEGER DEFAULT 0,
    range           REAL DEFAULT 0,
    cost_hp         INTEGER DEFAULT 0,
    cost_mp         INTEGER DEFAULT 0,
    cooldown_ms     INTEGER DEFAULT 0,
    damage_mult     REAL DEFAULT 1.0,
    damage_fixed    INTEGER DEFAULT 0,
    weapon_type     INTEGER DEFAULT 0,
    sp_cost         INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_skill_data_class ON skill_data(class_id);
CREATE INDEX IF NOT EXISTS idx_skill_data_type ON skill_data(skill_type);
CREATE INDEX IF NOT EXISTS idx_skill_data_level ON skill_data(level_required);

-- ─── Buff Skills ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS buff_skills (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    buff_level      INTEGER DEFAULT 1,
    skill_ref_id    INTEGER DEFAULT 0,
    duration_ms     INTEGER DEFAULT 0,
    buff_type       INTEGER DEFAULT 0,
    buff_value      INTEGER DEFAULT 0,
    buff_chance     INTEGER DEFAULT 100,
    icon_id         INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_buff_skills_ref ON buff_skills(skill_ref_id);
CREATE INDEX IF NOT EXISTS idx_buff_skills_type ON buff_skills(buff_type);

-- ─── Skill Trees ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS skill_trees (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    class_id        INTEGER,
    tree_level      INTEGER,
    slot_index      INTEGER,
    skill_id        INTEGER
);

CREATE INDEX IF NOT EXISTS idx_skill_trees_class ON skill_trees(class_id);
CREATE INDEX IF NOT EXISTS idx_skill_trees_skill ON skill_trees(skill_id);

-- ─── Map Warps ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS map_warps (
    id              INTEGER PRIMARY KEY,
    map_from        INTEGER,
    map_to          INTEGER,
    from_x          REAL,
    from_z          REAL,
    to_x            REAL,
    to_z            REAL,
    name            TEXT,
    dest_name       TEXT,
    fee             INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_map_warps_from ON map_warps(map_from);
CREATE INDEX IF NOT EXISTS idx_map_warps_to ON map_warps(map_to);

-- ─── Map Boundaries ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS map_boundaries (
    id              INTEGER PRIMARY KEY,
    map_a           INTEGER,
    map_b           INTEGER,
    boundary_x_a    REAL,
    boundary_z_a    REAL,
    boundary_x_b    REAL,
    boundary_z_b    REAL,
    name_a          TEXT,
    name_b          TEXT,
    level_required  INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_map_boundaries_a ON map_boundaries(map_a);
CREATE INDEX IF NOT EXISTS idx_map_boundaries_b ON map_boundaries(map_b);

-- ─── Map Data ───────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS map_data (
    id              INTEGER PRIMARY KEY,
    name            TEXT,
    file_path       TEXT,
    hgt_file        TEXT,
    box_min_x       REAL,
    box_min_y       REAL,
    box_min_z       REAL,
    box_max_x       REAL,
    box_max_y       REAL,
    box_max_z       REAL
);

CREATE INDEX IF NOT EXISTS idx_map_data_name ON map_data(name);

COMMIT;
