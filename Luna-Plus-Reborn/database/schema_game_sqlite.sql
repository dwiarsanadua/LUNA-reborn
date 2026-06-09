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
    InventoryExpansion INTEGER DEFAULT 0,
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
    Attribute       INTEGER DEFAULT 0,
    BattleStyle     INTEGER DEFAULT 0
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
    EmblemData      BLOB,
    EmblemLen       INTEGER DEFAULT 0,
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
    Memo            TEXT,
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
    GardenGrade     INTEGER DEFAULT 0,
    HouseGrade      INTEGER DEFAULT 0,
    WarehouseGrade  INTEGER DEFAULT 0,
    AnimalCageGrade INTEGER DEFAULT 0,
    FenceGrade      INTEGER DEFAULT 0,
    TaxArrearageFreq INTEGER DEFAULT 0,
    TaxPayPlayerName TEXT,
    CreateDate      TEXT DEFAULT (datetime('now')),
    UNIQUE(CharacterIdx)
);

CREATE TABLE IF NOT EXISTS TB_FARM_CROP (
    CropIdx         INTEGER PRIMARY KEY AUTOINCREMENT,
    FarmIdx         INTEGER NOT NULL REFERENCES TB_FARM(FarmIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    CropOwner       INTEGER DEFAULT 0,
    CropStep        INTEGER DEFAULT 0,
    CropLife        INTEGER DEFAULT 100,
    CropNextStepTime INTEGER DEFAULT 0,
    CropSeedGrade   INTEGER DEFAULT 0,
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
    AnimalOwner     INTEGER DEFAULT 0,
    AnimalStep      INTEGER DEFAULT 0,
    AnimalLife      INTEGER DEFAULT 100,
    AnimalNextStepTime INTEGER DEFAULT 0,
    Contentment     INTEGER DEFAULT 100,
    Interest        INTEGER DEFAULT 100,
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
    State           INTEGER DEFAULT 0,
    HouseName       TEXT,
    HousePoint      INTEGER DEFAULT 0,
    ExteriorKind    INTEGER DEFAULT 0,
    DecoratePoint   INTEGER DEFAULT 0,
    VisitCount      INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_house_char ON TB_HOUSE(CharacterIdx);
CREATE INDEX IF NOT EXISTS idx_house_point ON TB_HOUSE(HousePoint DESC);

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
    MaterialIdx     INTEGER DEFAULT 0,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RotY            REAL DEFAULT 0,
    Category        INTEGER DEFAULT 0,
    Slot            INTEGER DEFAULT 0,
    State           INTEGER DEFAULT 0,
    NotDelete       INTEGER DEFAULT 0,
    RemainTime      INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now'))
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

-- ─── Auction House ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_AUCTION (
    AuctionIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    ItemIdx         INTEGER NOT NULL REFERENCES TB_ITEM(ItemIdx),
    Price           INTEGER NOT NULL,
    BuyNowPrice     INTEGER DEFAULT 0,
    DurationHours   INTEGER DEFAULT 24,
    RegDate         TEXT DEFAULT (datetime('now')),
    ExpireDate      TEXT,
    IsSold          INTEGER DEFAULT 0,
    SoldDate        TEXT,
    IsCanceled      INTEGER DEFAULT 0,
    BidderIdx       INTEGER REFERENCES TB_CHARACTER(CharacterIdx),
    BidPrice        INTEGER DEFAULT 0,
    BidDate         TEXT
);

CREATE INDEX IF NOT EXISTS idx_auction_char ON TB_AUCTION(CharacterIdx);
CREATE INDEX IF NOT EXISTS idx_auction_item ON TB_AUCTION(ItemIdx);
CREATE INDEX IF NOT EXISTS idx_auction_active ON TB_AUCTION(IsSold, IsCanceled);
CREATE INDEX IF NOT EXISTS idx_auction_expire ON TB_AUCTION(ExpireDate);

-- ─── Billing ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_BILLING (
    BillingIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    AccountID       TEXT NOT NULL,
    CharacterIdx    INTEGER REFERENCES TB_CHARACTER(CharacterIdx),
    Amount          INTEGER NOT NULL,
    Currency        TEXT DEFAULT 'KRW',
    PaymentMethod   TEXT,
    ProductID       TEXT,
    ProductName     TEXT,
    RegDate         TEXT DEFAULT (datetime('now')),
    CompleteDate    TEXT,
    State           INTEGER DEFAULT 0,
    TransactionID   TEXT
);

CREATE INDEX IF NOT EXISTS idx_billing_account ON TB_BILLING(AccountID);
CREATE INDEX IF NOT EXISTS idx_billing_char ON TB_BILLING(CharacterIdx);
CREATE INDEX IF NOT EXISTS idx_billing_state ON TB_BILLING(State);

-- ─── Ranking ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_RANKING (
    RankingIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    RankingType     INTEGER NOT NULL,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Score           INTEGER DEFAULT 0,
    Rank            INTEGER DEFAULT 0,
    RegDate         TEXT DEFAULT (datetime('now')),
    UpdateDate      TEXT,
    UNIQUE(RankingType, CharacterIdx)
);

CREATE INDEX IF NOT EXISTS idx_ranking_type ON TB_RANKING(RankingType);
CREATE INDEX IF NOT EXISTS idx_ranking_score ON TB_RANKING(RankingType, Score DESC);

CREATE TABLE IF NOT EXISTS TB_RANKING_HISTORY (
    HistoryIdx      INTEGER PRIMARY KEY AUTOINCREMENT,
    RankingType     INTEGER NOT NULL,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Rank            INTEGER DEFAULT 0,
    Score           INTEGER DEFAULT 0,
    RecordDate      TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_ranking_history ON TB_RANKING_HISTORY(RankingType, RecordDate);

-- ─── Guild War ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD_WAR (
    WarIdx          INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildIdx1       INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    GuildIdx2       INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    Money           INTEGER DEFAULT 0,
    WarDate         TEXT DEFAULT (datetime('now')),
    State           INTEGER DEFAULT 0,
    WinnerIdx       INTEGER REFERENCES TB_GUILD(GuildIdx)
);

CREATE INDEX IF NOT EXISTS idx_guildwar_guild1 ON TB_GUILD_WAR(GuildIdx1);
CREATE INDEX IF NOT EXISTS idx_guildwar_guild2 ON TB_GUILD_WAR(GuildIdx2);

CREATE TABLE IF NOT EXISTS TB_GUILD_WAR_RECORD (
    RecordIdx       INTEGER PRIMARY KEY AUTOINCREMENT,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    Victory         INTEGER DEFAULT 0,
    Draw            INTEGER DEFAULT 0,
    Loose           INTEGER DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_guildwar_record ON TB_GUILD_WAR_RECORD(GuildIdx);

-- ─── Farm Time Delay ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FARM_TIMEDELAY (
    DelayIdx        INTEGER PRIMARY KEY AUTOINCREMENT,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Kind            INTEGER NOT NULL,
    RemainSecond    INTEGER DEFAULT 0,
    UNIQUE(CharacterIdx, Kind)
);

-- ─── House Rank ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_HOUSE_RANK (
    HouseIdx        INTEGER PRIMARY KEY REFERENCES TB_HOUSE(HouseIdx) ON DELETE CASCADE,
    Rank            INTEGER DEFAULT 0,
    HousePoint      INTEGER DEFAULT 0,
    CalcDate        TEXT
);

-- ============================================================
-- SQL QUERY TEMPLATES (migrated from MSSQL stored procedures)
-- SQLite does not support stored procedures; these are
-- parameterized query templates used by the C++ DB layer.
-- ============================================================

-- ─── USP_HOUSE_FURNITURE_LOAD ──────────────────────────────
-- Old: EXEC dbo.MP_HOUSE_FURNITURE_LOAD %d, %d, %d, %d, %d, %d, %d, %d
-- Reborn: SELECT * FROM TB_HOUSE_FURNITURE WHERE HouseIdx = ?

-- ─── USP_HOUSE_FURNITURE_UPDATE ────────────────────────────
-- Old: EXEC dbo.MP_HOUSE_FURNITURE_UPDATE %d, %d, %d, %d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d
-- Reborn INSERT/UPDATE:
--   INSERT INTO TB_HOUSE_FURNITURE (HouseIdx, ItemDBIdx, MaterialIdx, PosX, PosY, PosZ, RotY, Category, Slot, State, NotDelete, RemainTime)
--   VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
--   ON CONFLICT(FurnitureIdx) DO UPDATE SET PosX=excluded.PosX, PosY=excluded.PosY, PosZ=excluded.PosZ, RotY=excluded.Roty, State=excluded.State, RemainTime=excluded.RemainTime

-- ─── USP_HOUSE_DELETE ───────────────────────────────────────
-- Old: EXEC dbo.MP_HOUSEDELETE %d, %d, %d, '%s'
-- Reborn: DELETE FROM TB_HOUSE WHERE CharacterIdx = (SELECT CharacterIdx FROM TB_CHARACTER WHERE CharName = ?)

-- ─── USP_HOUSE_RANK_LOAD ────────────────────────────────────
-- Old: EXEC dbo.MP_HOUSERANK_LOAD
-- Reborn: SELECT h.HouseIdx, h.HouseName, h.HousePoint, h.ExteriorKind, h.CharacterIdx, c.CharName
--         FROM TB_HOUSE h JOIN TB_CHARACTER c ON h.CharacterIdx = c.CharacterIdx
--         ORDER BY h.HousePoint DESC LIMIT 3

-- ─── USP_HOUSE_RANK_UPDATE ──────────────────────────────────
-- Old: EXEC dbo.MP_HOUSERANK
-- Reborn: INSERT INTO TB_HOUSE_RANK (HouseIdx, Rank, HousePoint, CalcDate)
--         SELECT HouseIdx,
--                ROW_NUMBER() OVER (ORDER BY HousePoint DESC) as Rank,
--                HousePoint, datetime('now')
--         FROM TB_HOUSE ORDER BY HousePoint DESC LIMIT 3
--         ON CONFLICT(HouseIdx) DO UPDATE SET Rank=excluded.Rank, HousePoint=excluded.HousePoint

-- ─── USP_HOUSE_CREATE ───────────────────────────────────────
-- Old: EXEC dbo.MP_HOUSECREATE %d, %d, %d, %f, %f, %d
-- Reborn: INSERT INTO TB_HOUSE (CharacterIdx, MapIdx, PosX, PosY, HouseType) VALUES (?, ?, ?, ?, ?)

-- ─── USP_HOUSE_DATA_LOAD ────────────────────────────────────
-- Old: EXEC dbo.MP_HOUSEDATA_LOAD %d
-- Reborn: SELECT * FROM TB_HOUSE WHERE CharacterIdx = ?

-- ─── USP_FARM_SET_FARMSTATE ─────────────────────────────────
-- Old: EXEC dbo.MP_FARM_SETFARMSTATE %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d
-- Reborn: UPDATE TB_FARM SET State = ?, GardenGrade = ?, HouseGrade = ?, WarehouseGrade = ?,
--         AnimalCageGrade = ?, FenceGrade = ?, TaxArrearageFreq = ?, TaxPayPlayerName = ?
--         WHERE MapIdx = ? AND FarmIdx = ?

-- ─── USP_FARM_SET_CROPINFO ──────────────────────────────────
-- Old: EXEC dbo.MP_FARM_SETCROPINFO %d, %d, %d, %d, %d, %d, %d, %d
-- Reborn: INSERT INTO TB_FARM_CROP (FarmIdx, ItemDBIdx, CropOwner, CropStep, CropLife, CropNextStepTime, CropSeedGrade)
--         VALUES (?, ?, ?, ?, ?, ?, ?)
--         ON CONFLICT(CropIdx) DO UPDATE SET CropStep=excluded.CropStep, CropLife=excluded.CropLife,
--         CropNextStepTime=excluded.CropNextStepTime, CropSeedGrade=excluded.CropSeedGrade

-- ─── USP_FARM_SET_ANIMALINFO ────────────────────────────────
-- Old: EXEC dbo.MP_FARM_SETANIMALINFO %d, %d, %d, %d, %d, %d, %d, %d, %d
-- Reborn: INSERT INTO TB_FARM_ANIMAL (FarmIdx, ItemDBIdx, AnimalOwner, AnimalStep, AnimalLife, AnimalNextStepTime, Contentment, Interest)
--         VALUES (?, ?, ?, ?, ?, ?, ?, ?)
--         ON CONFLICT(AnimalIdx) DO UPDATE SET AnimalStep=excluded.AnimalStep, AnimalLife=excluded.AnimalLife,
--         AnimalNextStepTime=excluded.AnimalNextStepTime, Contentment=excluded.Contentment, Interest=excluded.Interest

-- ─── USP_FARM_SET_TAXINFO ───────────────────────────────────
-- Old: EXEC dbo.MP_FARM_SETTAXINFO %d, %d, %d, %d
-- Reborn: UPDATE TB_FARM SET TaxArrearageFreq = ?, TaxPayPlayerName = (SELECT CharName FROM TB_CHARACTER WHERE CharacterIdx = ?)
--         WHERE MapIdx = ? AND FarmIdx = ?

-- ─── USP_FARM_SET_TIMEDELAY ─────────────────────────────────
-- Old: EXEC dbo.MP_FARM_SETTIMEDELAY %d, %d, %d
-- Reborn: INSERT INTO TB_FARM_TIMEDELAY (CharacterIdx, Kind, RemainSecond) VALUES (?, ?, ?)
--         ON CONFLICT(CharacterIdx, Kind) DO UPDATE SET RemainSecond = excluded.RemainSecond

-- ─── USP_FARM_LOAD_TIMEDELAY ────────────────────────────────
-- Old: EXEC dbo.MP_FARM_LOADTIMEDELAY %d
-- Reborn: SELECT * FROM TB_FARM_TIMEDELAY WHERE CharacterIdx = ?

-- ─── USP_FARM_LOAD_FARMSTATE ────────────────────────────────
-- Old: EXEC dbo.MP_FARM_LOADFARMSTATE %d, %d
-- Reborn: SELECT * FROM TB_FARM WHERE MapIdx = ? AND FarmIdx = ?

-- ─── USP_SIEGE_RECALL_INSERT ────────────────────────────────
-- Old: EXEC dbo.MP_SIEGERECALL_INSERT %d, %d, %f, %f, %f
-- Reborn: INSERT INTO TB_SIEGE_RECALL (CharacterIdx, MapIdx, PosX, PosY, PosZ) VALUES (?, ?, ?, ?, ?)

-- ─── USP_SIEGE_RECALL_LOAD ──────────────────────────────────
-- Old: EXEC dbo.MP_SIEGERECALL_LOAD %d
-- Reborn: SELECT * FROM TB_SIEGE_RECALL WHERE CharacterIdx = ?

-- ─── USP_SIEGE_WARFARE_INFO_LOAD ────────────────────────────
-- Old: EXEC dbo.MP_SIEGEWARFARE_INFO_LOAD
-- Reborn: SELECT MapIdx as MapType, GuildIdx as CastleGuildIdx, State FROM TB_SIEGE_WARFARE

-- ─── USP_SIEGE_WARFARE_INFO_UPDATE ──────────────────────────
-- Old: EXEC dbo.MP_SIEGEWARFARE_INFO_UPDATE %d, %d, %d
-- Reborn: UPDATE TB_SIEGE_WARFARE SET GuildIdx = ?, State = ? WHERE MapIdx = ?

-- ─── USP_SIEGE_WAR_WATERSEED_COMPLETE ───────────────────────
-- Old: EXEC dbo.MP_SIEGEWAR_WATERSEED_COMPLETE %d, %d, %d, %d
-- Reborn: UPDATE TB_SIEGE_WARFARE SET WaterSeed = 1 WHERE MapIdx = ? AND GuildIdx = ?

-- ─── USP_GUILD_WAR_LOAD ─────────────────────────────────────
-- Old: EXEC dbo.MP_GUILDFIELDWAR_LOAD %d
-- Reborn: SELECT * FROM TB_GUILD_WAR WHERE WarIdx > ? ORDER BY WarIdx LIMIT 100

-- ─── USP_GUILD_WAR_INSERT ───────────────────────────────────
-- Old: EXEC dbo.MP_GUILDFIELDWAR_INSERT %d, %d, %u
-- Reborn: INSERT INTO TB_GUILD_WAR (GuildIdx1, GuildIdx2, Money) VALUES (?, ?, ?)

-- ─── USP_GUILD_WAR_DELETE ───────────────────────────────────
-- Old: EXEC dbo.MP_GUILDFIELDWAR_DELETE %d, %d
-- Reborn: DELETE FROM TB_GUILD_WAR WHERE GuildIdx1 = ? AND GuildIdx2 = ?

-- ─── USP_GUILD_WAR_RECORD_UPDATE ────────────────────────────
-- Old: EXEC dbo.MP_GUILDFIELDWAR_RECORD %d, %d, %d, %d
-- Reborn: INSERT INTO TB_GUILD_WAR_RECORD (GuildIdx, Victory, Draw, Loose) VALUES (?, ?, ?, ?)
--         ON CONFLICT(RecordIdx) DO UPDATE SET Victory=excluded.Victory, Draw=excluded.Draw, Loose=excluded.Loose

-- ─── USP_GUILD_WAR_RECORD_DELETE ────────────────────────────
-- Old: EXEC dbo.MP_GUILDWARRECORD_DELETE %d
-- Reborn: DELETE FROM TB_GUILD_WAR_RECORD WHERE GuildIdx = ?

-- ─── USP_GUILD_WAR_RECORD_LOAD ──────────────────────────────
-- Old: EXEC dbo.MP_GUILDWARRECORD_LOAD %d
-- Reborn: SELECT * FROM TB_GUILD_WAR_RECORD WHERE GuildIdx > ? ORDER BY GuildIdx LIMIT 100

-- ─── USP_GUILD_UNION_LOAD ───────────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_LOAD %d
-- Reborn: SELECT u.*, m.GuildIdx as MemberGuildIdx FROM TB_GUILD_UNION u
--         LEFT JOIN TB_GUILD_UNION_MEMBER m ON u.UnionIdx = m.UnionIdx
--         WHERE u.UnionIdx > ? ORDER BY u.UnionIdx LIMIT 100

-- ─── USP_GUILD_UNION_LOADMARK ───────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_LOADMARK %d
-- Reborn: SELECT UnionIdx, MarkLen, MarkData FROM TB_GUILD_UNION WHERE UnionIdx > ? ORDER BY UnionIdx LIMIT 100

-- ─── USP_GUILD_UNION_CREATE ─────────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_CREATE %d, '%s'
-- Reborn: INSERT INTO TB_GUILD_UNION (UnionName, MasterGuildIdx) VALUES (?, ?)

-- ─── USP_GUILD_UNION_DESTROY ────────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_DESTROY %d, %d, %d, %d
-- Reborn: DELETE FROM TB_GUILD_UNION WHERE UnionIdx = ?

-- ─── USP_GUILD_UNION_ADDGUILD ───────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_ADDGUILD %d, %d, %d
-- Reborn: INSERT INTO TB_GUILD_UNION_MEMBER (UnionIdx, GuildIdx) VALUES (?, ?)

-- ─── USP_GUILD_UNION_REMOVEGUILD ────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_REMOVEGUILD %d, %d, %d
-- Reborn: DELETE FROM TB_GUILD_UNION_MEMBER WHERE UnionIdx = ? AND GuildIdx = ?

-- ─── USP_GUILD_UNION_SECEDEGUILD ────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_SECEDEGUILD %d, %d, %d, %d
-- Reborn: DELETE FROM TB_GUILD_UNION_MEMBER WHERE UnionIdx = ? AND GuildIdx = ?

-- ─── USP_GUILD_UNION_MARKREGIST ─────────────────────────────
-- Old: EXEC dbo.MP_GUILD_UNION_MARKREGIST %d, %d, %d, 0x...
-- Reborn: UPDATE TB_GUILD_UNION SET MarkData = ?, MarkLen = ? WHERE UnionIdx = ?

-- ─── USP_AUCTION_REGISTER ───────────────────────────────────
-- Reborn: INSERT INTO TB_AUCTION (CharacterIdx, ItemIdx, Price, BuyNowPrice, DurationHours, ExpireDate)
--         VALUES (?, ?, ?, ?, ?, datetime('now', '+' || ? || ' hours'))

-- ─── USP_AUCTION_CANCEL ─────────────────────────────────────
-- Reborn: UPDATE TB_AUCTION SET IsCanceled = 1 WHERE AuctionIdx = ? AND CharacterIdx = ?

-- ─── USP_AUCTION_BUY ────────────────────────────────────────
-- Reborn: UPDATE TB_AUCTION SET IsSold = 1, SoldDate = datetime('now'), BidderIdx = ?, BidPrice = ?
--         WHERE AuctionIdx = ? AND IsSold = 0 AND IsCanceled = 0

-- ─── USP_AUCTION_BID ────────────────────────────────────────
-- Reborn: UPDATE TB_AUCTION SET BidderIdx = ?, BidPrice = ?, BidDate = datetime('now')
--         WHERE AuctionIdx = ? AND IsSold = 0 AND IsCanceled = 0 AND BidPrice < ?

-- ─── USP_AUCTION_LIST ───────────────────────────────────────
-- Reborn: SELECT a.*, c.CharName FROM TB_AUCTION a
--         JOIN TB_CHARACTER c ON a.CharacterIdx = c.CharacterIdx
--         WHERE a.IsSold = 0 AND a.IsCanceled = 0 AND a.ExpireDate > datetime('now')
--         ORDER BY a.RegDate DESC LIMIT ?

-- ─── USP_AUCTION_MY_LIST ────────────────────────────────────
-- Reborn: SELECT a.*, c.CharName FROM TB_AUCTION a
--         JOIN TB_CHARACTER c ON a.CharacterIdx = c.CharacterIdx
--         WHERE a.CharacterIdx = ? ORDER BY a.RegDate DESC

-- ─── USP_BILLING_CHARGE ─────────────────────────────────────
-- Reborn: INSERT INTO TB_BILLING (AccountID, CharacterIdx, Amount, Currency, PaymentMethod, ProductID, ProductName, State)
--         VALUES (?, ?, ?, ?, ?, ?, ?, 0)

-- ─── USP_BILLING_COMPLETE ───────────────────────────────────
-- Reborn: UPDATE TB_BILLING SET State = 1, CompleteDate = datetime('now'), TransactionID = ?
--         WHERE BillingIdx = ?

-- ─── USP_BILLING_HISTORY ────────────────────────────────────
-- Reborn: SELECT * FROM TB_BILLING WHERE AccountID = ? ORDER BY RegDate DESC LIMIT ?

-- ─── USP_EVENT_LOAD ─────────────────────────────────────────
-- Reborn: SELECT * FROM TB_EVENT WHERE CharacterIdx = ? AND EventID = ?

-- ─── USP_EVENT_SAVE ─────────────────────────────────────────
-- Reborn: INSERT INTO TB_EVENT (CharacterIdx, EventID, EventData) VALUES (?, ?, ?)
--         ON CONFLICT(CharacterIdx, EventID) DO UPDATE SET EventData = excluded.EventData

-- ─── USP_EVENT_CHECK ────────────────────────────────────────
-- Old: EXEC dbo.MP_QUEST_EVENTCHECK %d, %d, %d, %d, '%s'
-- Reborn: SELECT COUNT(*) FROM TB_EVENT WHERE CharacterIdx = ? AND EventID = ?

-- ─── USP_RANKING_UPDATE ─────────────────────────────────────
-- Reborn: INSERT INTO TB_RANKING (RankingType, CharacterIdx, Score, Rank, UpdateDate)
--         VALUES (?, ?, ?, ?, datetime('now'))
--         ON CONFLICT(RankingType, CharacterIdx) DO UPDATE SET Score = excluded.Score, Rank = excluded.Rank, UpdateDate = excluded.UpdateDate

-- ─── USP_RANKING_GET ────────────────────────────────────────
-- Reborn: SELECT r.*, c.CharName FROM TB_RANKING r
--         JOIN TB_CHARACTER c ON r.CharacterIdx = c.CharacterIdx
--         WHERE r.RankingType = ? ORDER BY r.Score DESC LIMIT ?

-- ─── USP_RANKING_SAVE_HISTORY ───────────────────────────────
-- Reborn: INSERT INTO TB_RANKING_HISTORY (RankingType, CharacterIdx, Rank, Score, RecordDate)
--         SELECT RankingType, CharacterIdx, Rank, Score, datetime('now') FROM TB_RANKING WHERE RankingType = ?

-- ─── USP_GUILD_MARK_UPDATE ──────────────────────────────────
-- Old: EXEC dbo.MP_GUILD_MARKUPDATE %d, %d, 0x...
-- Reborn: UPDATE TB_GUILD SET MarkData = ?, MarkLen = ? WHERE GuildIdx = ?

-- ─── USP_GUILD_MARK_LOAD ────────────────────────────────────
-- Old: EXEC dbo.MP_GUILD_LOADMARK %d
-- Reborn: SELECT MarkData, MarkLen FROM TB_GUILD WHERE GuildIdx = ?

-- ─── Resident Registration ─────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_RESIDENTREGIST (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    NickName        TEXT,
    Sex             INTEGER DEFAULT 0,
    Age             INTEGER DEFAULT 0,
    Location        INTEGER DEFAULT 0,
    Favor           INTEGER DEFAULT 0,
    PropensityLike01   INTEGER DEFAULT 0,
    PropensityLike02   INTEGER DEFAULT 0,
    PropensityLike03   INTEGER DEFAULT 0,
    PropensityDisLike01 INTEGER DEFAULT 0,
    PropensityDisLike02 INTEGER DEFAULT 0,
    Introduction    TEXT
);

-- ─── USP_RESIDENTREGIST_LOAD ────────────────────────────────
-- Old: EXEC dbo.MP_RESIDENTREGIST_LOADINFO %d
-- Reborn: SELECT * FROM TB_RESIDENTREGIST WHERE CharacterIdx = ?

-- ─── USP_RESIDENTREGIST_SAVE ────────────────────────────────
-- Old: EXEC dbo.MP_RESIDENTREGIST_SAVEINFO %d, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d
-- Reborn: INSERT INTO TB_RESIDENTREGIST (CharacterIdx, NickName, Sex, Age, Location, Favor,
--         PropensityLike01, PropensityLike02, PropensityLike03, PropensityDisLike01, PropensityDisLike02)
--         VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
--         ON CONFLICT(CharacterIdx) DO UPDATE SET Sex=excluded.Sex, Age=excluded.Age, Location=excluded.Location,
--         Favor=excluded.Favor, PropensityLike01=excluded.PropensityLike01, PropensityLike02=excluded.PropensityLike02,
--         PropensityLike03=excluded.PropensityLike03, PropensityDisLike01=excluded.PropensityDisLike01,
--         PropensityDisLike02=excluded.PropensityDisLike02

-- ─── USP_RESIDENTREGIST_UPDATE_INTRODUCTION ─────────────────
-- Old: EXEC dbo.MP_RESIDENTREGIST_UPDATEINTRODUCTION %d, '%s'
-- Reborn: UPDATE TB_RESIDENTREGIST SET Introduction = ? WHERE CharacterIdx = ?

-- ─── USP_RESIDENTREGIST_RESET ───────────────────────────────
-- Old: EXEC dbo.MP_RESIDENTREGIST_RESET %d
-- Reborn: DELETE FROM TB_RESIDENTREGIST WHERE CharacterIdx = ?

-- ─── USP_INVENTORY_EXPANSION ────────────────────────────────
-- Old: EXEC dbo.MP_INCREASE_CHARACTER_INVENTORY %d
-- Reborn: UPDATE TB_CHARACTER SET InventoryExpansion = InventoryExpansion + 1 WHERE CharacterIdx = ?

-- ─── USP_INVENTORY_RESET ────────────────────────────────────
-- Old: EXEC dbo.MP_RESET_CHARACTER_INVENTORY %d
-- Reborn: UPDATE TB_CHARACTER SET InventoryExpansion = 0 WHERE CharacterIdx = ?

COMMIT;
