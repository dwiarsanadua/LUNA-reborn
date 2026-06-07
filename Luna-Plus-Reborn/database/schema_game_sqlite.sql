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

COMMIT;
