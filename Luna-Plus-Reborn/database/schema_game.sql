-- ============================================================
-- LUNA Plus — Game Database Schema (PostgreSQL)
-- Database: LUNA_GAMEDB
-- Auto-generated from source code reverse engineering (P3.1)
-- ============================================================

BEGIN;

-- ─── Character ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CHARACTER (
    CharacterIdx    SERIAL PRIMARY KEY,
    AccountID       VARCHAR(32) NOT NULL,
    CharName        VARCHAR(32) UNIQUE NOT NULL,
    Level           INTEGER DEFAULT 1,
    Exp             BIGINT DEFAULT 0,
    Money           BIGINT DEFAULT 0,
    MapIdx          INTEGER DEFAULT 0,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    HP              INTEGER DEFAULT 100,
    MP              INTEGER DEFAULT 100,
    SP              INTEGER DEFAULT 100,
    Str             SMALLINT DEFAULT 10,
    Dex             SMALLINT DEFAULT 10,
    Int             SMALLINT DEFAULT 10,
    Con             SMALLINT DEFAULT 10,
    Wis             SMALLINT DEFAULT 10,
    Luck            SMALLINT DEFAULT 10,
    Job             SMALLINT DEFAULT 0,
    Face            SMALLINT DEFAULT 0,
    Hair            SMALLINT DEFAULT 0,
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    DeleteDate      TIMESTAMP,
    LastLogin       TIMESTAMP,
    LastLogout      TIMESTAMP,
    LoginTime       BIGINT DEFAULT 0,
    PlayTime        BIGINT DEFAULT 0,
    PvpKillCount    INTEGER DEFAULT 0,
    PvpDeathCount   INTEGER DEFAULT 0,
    PvpPoint        INTEGER DEFAULT 0,
    Fame            INTEGER DEFAULT 0,
    BadFame         INTEGER DEFAULT 0,
    StatPoint       INTEGER DEFAULT 0,
    SkillPoint      INTEGER DEFAULT 0,
    HonorPoint      INTEGER DEFAULT 0,
    RestExp         BIGINT DEFAULT 0,
    ServerGroup     SMALLINT DEFAULT 0,
    ChannelIdx      SMALLINT DEFAULT 0,
    Attribute       SMALLINT DEFAULT 0
);

CREATE INDEX idx_char_account ON TB_CHARACTER(AccountID);
CREATE INDEX idx_char_name ON TB_CHARACTER(CharName);
CREATE INDEX idx_char_level ON TB_CHARACTER(Level);
CREATE INDEX idx_char_map ON TB_CHARACTER(MapIdx);

-- ─── Character Buffs ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CHARACTER_BUFF (
    BuffIdx         SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SkillIdx        INTEGER NOT NULL,
    RemainTime      INTEGER DEFAULT 0,
    Count           INTEGER DEFAULT 0,
    IsEndDate       BOOLEAN DEFAULT FALSE,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_buff_char ON TB_CHARACTER_BUFF(CharacterIdx);

-- ─── Item ───────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM (
    ItemIdx         SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER REFERENCES TB_CHARACTER(CharacterIdx),
    ItemDBIdx       INTEGER NOT NULL,
    Count           INTEGER DEFAULT 1,
    Slot            SMALLINT DEFAULT 0,
    StorageSlot     SMALLINT DEFAULT -1,
    StorageType     SMALLINT DEFAULT 0,  -- 0=inven, 1=storage, 2=pet, 3=munpa
    Grade           SMALLINT DEFAULT 0,
    Durability      SMALLINT DEFAULT 100,
    MaxDurability   SMALLINT DEFAULT 100,
    IsSealed        BOOLEAN DEFAULT FALSE,
    SealRemainTime  INTEGER DEFAULT 0,
    SealGameTime    INTEGER DEFAULT 0,
    Bless           SMALLINT DEFAULT 0,
    Socket          SMALLINT DEFAULT 0,
    Element         SMALLINT DEFAULT 0,
    ElementValue    SMALLINT DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_item_char ON TB_ITEM(CharacterIdx);
CREATE INDEX idx_item_db ON TB_ITEM(ItemDBIdx);
CREATE INDEX idx_item_slot ON TB_ITEM(CharacterIdx, Slot);

-- ─── Item Options ───────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM_OPTION (
    OptionIdx       SERIAL PRIMARY KEY,
    ItemIdx         INTEGER NOT NULL REFERENCES TB_ITEM(ItemIdx) ON DELETE CASCADE,
    OptType         SMALLINT NOT NULL,
    OptValue        REAL NOT NULL
);

CREATE INDEX idx_option_item ON TB_ITEM_OPTION(ItemIdx);

-- ─── Skill ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_SKILL (
    SkillIdx        SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SkillID         INTEGER NOT NULL,
    SkillLevel      SMALLINT DEFAULT 1,
    IsPassive       BOOLEAN DEFAULT FALSE,
    Slot            SMALLINT DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_skill_char ON TB_SKILL(CharacterIdx);

-- ─── Quick Slot ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_QUICKSLOT (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SlotData        TEXT NOT NULL DEFAULT ''
);

-- ─── Quest ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_QUEST (
    QuestIdx        SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    QuestID         INTEGER NOT NULL,
    QuestState      SMALLINT DEFAULT 0,  -- 0=progress, 1=complete, 2=fail
    Progress        TEXT,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CompleteDate    TIMESTAMP,
    UNIQUE(CharacterIdx, QuestID)
);

CREATE INDEX idx_quest_char ON TB_QUEST(CharacterIdx);

-- ─── Main Quest ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_MAINQUEST (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    MainQuestID     INTEGER DEFAULT 0,
    SubQuestID      INTEGER DEFAULT 0,
    QuestStep       INTEGER DEFAULT 0
);

-- ─── Party ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PARTY (
    PartyIdx        SERIAL PRIMARY KEY,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    IsAutoMatch     BOOLEAN DEFAULT FALSE,
    MapServerIdx    INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_PARTY_MEMBER (
    PartyIdx        INTEGER NOT NULL REFERENCES TB_PARTY(PartyIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    JoinDate        TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (PartyIdx, CharacterIdx)
);

-- ─── Guild ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD (
    GuildIdx        SERIAL PRIMARY KEY,
    GuildName       VARCHAR(32) UNIQUE NOT NULL,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Level           SMALLINT DEFAULT 1,
    Exp             BIGINT DEFAULT 0,
    Score           INTEGER DEFAULT 0,
    Money           BIGINT DEFAULT 0,
    MaxMember       SMALLINT DEFAULT 20,
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    DeleteDate      TIMESTAMP,
    MarkData        BYTEA,
    MarkLen         INTEGER DEFAULT 0,
    Notice          VARCHAR(256)
);

CREATE INDEX idx_guild_name ON TB_GUILD(GuildName);
CREATE INDEX idx_guild_master ON TB_GUILD(MasterIdx);

CREATE TABLE IF NOT EXISTS TB_GUILD_MEMBER (
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Rank            SMALLINT DEFAULT 1,
    JoinDate        TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    Contribution    INTEGER DEFAULT 0,
    PRIMARY KEY (GuildIdx, CharacterIdx)
);

CREATE TABLE IF NOT EXISTS TB_GUILD_SKILL (
    SkillIdx        SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    SKILL_IDX       INTEGER NOT NULL,
    SKILL_LEVEL     SMALLINT DEFAULT 1
);

CREATE INDEX idx_guildskill_guild ON TB_GUILD_SKILL(GuildIdx);

CREATE TABLE IF NOT EXISTS TB_GUILD_WAREHOUSE (
    GuildIdx        INTEGER PRIMARY KEY REFERENCES TB_GUILD(GuildIdx) ON DELETE CASCADE,
    Money           BIGINT DEFAULT 0,
    SlotCount       SMALLINT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_GUILD_UNION (
    UnionIdx        SERIAL PRIMARY KEY,
    UnionName       VARCHAR(32) UNIQUE NOT NULL,
    MasterGuildIdx  INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    MarkData        BYTEA,
    MarkLen         INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_GUILD_UNION_MEMBER (
    UnionIdx        INTEGER NOT NULL REFERENCES TB_GUILD_UNION(UnionIdx) ON DELETE CASCADE,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    JoinDate        TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (UnionIdx, GuildIdx)
);

-- ─── Friend ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FRIEND (
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FriendIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FriendGroup     SMALLINT DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (CharacterIdx, FriendIdx)
);

-- ─── Note / Mail ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_NOTE (
    NoteIdx         SERIAL PRIMARY KEY,
    SenderIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    ReceiverIdx     INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Title           VARCHAR(64),
    Content         TEXT,
    SendDate        TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    IsRead          BOOLEAN DEFAULT FALSE,
    ReadDate        TIMESTAMP,
    HasPackage      BOOLEAN DEFAULT FALSE,
    PackageMoney    BIGINT DEFAULT 0,
    PackageItemIdx  INTEGER REFERENCES TB_ITEM(ItemIdx)
);

CREATE INDEX idx_note_receiver ON TB_NOTE(ReceiverIdx);
CREATE INDEX idx_note_sender ON TB_NOTE(SenderIdx);
CREATE INDEX idx_note_unread ON TB_NOTE(ReceiverIdx, IsRead);

-- ─── Family ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FAMILY (
    FamilyIdx       SERIAL PRIMARY KEY,
    FamilyName      VARCHAR(32) UNIQUE NOT NULL,
    MasterIdx       INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    EmblemData      BYTEA,
    EmblemLen       INTEGER DEFAULT 0,
    Point           INTEGER DEFAULT 0,
    Level           SMALLINT DEFAULT 1
);

CREATE TABLE IF NOT EXISTS TB_FAMILY_MEMBER (
    FamilyIdx       INTEGER NOT NULL REFERENCES TB_FAMILY(FamilyIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    JoinDate        TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    Role            SMALLINT DEFAULT 0,
    Contribution    INTEGER DEFAULT 0,
    PRIMARY KEY (FamilyIdx, CharacterIdx)
);

-- ─── Farm ───────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FARM (
    FarmIdx         SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    FarmName        VARCHAR(32),
    MapIdx          INTEGER DEFAULT 0,
    Size            INTEGER DEFAULT 1,
    TaxRate         REAL DEFAULT 0,
    State           SMALLINT DEFAULT 0,
    DelayTime       INTEGER DEFAULT 0,
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(CharacterIdx)
);

CREATE TABLE IF NOT EXISTS TB_FARM_CROP (
    CropIdx         SERIAL PRIMARY KEY,
    FarmIdx         INTEGER NOT NULL REFERENCES TB_FARM(FarmIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            INTEGER DEFAULT 0,
    PosY            INTEGER DEFAULT 0,
    State           SMALLINT DEFAULT 0,
    RemainTime      INTEGER DEFAULT 0,
    PlantDate       TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TB_FARM_ANIMAL (
    AnimalIdx       SERIAL PRIMARY KEY,
    FarmIdx         INTEGER NOT NULL REFERENCES TB_FARM(FarmIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            INTEGER DEFAULT 0,
    PosY            INTEGER DEFAULT 0,
    State           SMALLINT DEFAULT 0,
    RemainTime      INTEGER DEFAULT 0
);

-- ─── Housing ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_HOUSE (
    HouseIdx        SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER DEFAULT 0,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    HouseType       SMALLINT DEFAULT 0,
    FurnitureCount  INTEGER DEFAULT 0,
    CreateDate      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    LastRepairDate  TIMESTAMP,
    State           SMALLINT DEFAULT 0
);

CREATE INDEX idx_house_char ON TB_HOUSE(CharacterIdx);

CREATE TABLE IF NOT EXISTS TB_HOUSE_FURNITURE (
    FurnitureIdx    SERIAL PRIMARY KEY,
    HouseIdx        INTEGER NOT NULL REFERENCES TB_HOUSE(HouseIdx) ON DELETE CASCADE,
    ItemDBIdx       INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RotY            REAL DEFAULT 0
);

-- ─── Pet ────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PET (
    PetIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    PetDBIdx        INTEGER NOT NULL,
    PetName         VARCHAR(32),
    Level           SMALLINT DEFAULT 1,
    Experience      BIGINT DEFAULT 0,
    Friendship      INTEGER DEFAULT 0,
    Grade           SMALLINT DEFAULT 0,
    Kind            SMALLINT DEFAULT 0,
    State           SMALLINT DEFAULT 0,
    SkillSlot       INTEGER DEFAULT 0,
    HP              INTEGER DEFAULT 100,
    MP              INTEGER DEFAULT 100,
    Satiation       SMALLINT DEFAULT 100,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_pet_char ON TB_PET(CharacterIdx);

-- ─── Vehicle ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_VEHICLE (
    VehicleIdx      SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    VehicleDBIdx    INTEGER NOT NULL,
    HP              INTEGER DEFAULT 100,
    State           SMALLINT DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TB_VEHICLE_PASSENGER (
    VehicleIdx      INTEGER NOT NULL REFERENCES TB_VEHICLE(VehicleIdx) ON DELETE CASCADE,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    SeatIdx         SMALLINT DEFAULT 0,
    PRIMARY KEY (VehicleIdx, CharacterIdx)
);

-- ─── Fishing ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_FISHING (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SkillLevel      SMALLINT DEFAULT 0,
    Experience      INTEGER DEFAULT 0,
    CatchCount      INTEGER DEFAULT 0,
    MaxLength       REAL DEFAULT 0
);

-- ─── Cooking ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_COOKING (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SkillLevel      SMALLINT DEFAULT 0,
    Experience      INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_COOKRECIPE (
    RecipeIdx       SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    RecipeID        INTEGER NOT NULL,
    IsLearned       BOOLEAN DEFAULT FALSE,
    UNIQUE(CharacterIdx, RecipeID)
);

-- ─── Storage ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_STORAGE (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    SlotCount       SMALLINT DEFAULT 0,
    Money           BIGINT DEFAULT 0
);

-- ─── Consignment ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_CONSIGNMENT (
    ConsignmentIdx  SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    ItemIdx         INTEGER NOT NULL REFERENCES TB_ITEM(ItemIdx),
    Price           BIGINT NOT NULL,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ExpireDate      TIMESTAMP,
    IsSold          BOOLEAN DEFAULT FALSE,
    SoldDate        TIMESTAMP,
    IsCanceled      BOOLEAN DEFAULT FALSE
);

CREATE INDEX idx_consignment_char ON TB_CONSIGNMENT(CharacterIdx);
CREATE INDEX idx_consignment_item ON TB_CONSIGNMENT(ItemIdx);
CREATE INDEX idx_consignment_active ON TB_CONSIGNMENT(IsSold, IsCanceled);

-- ─── Siege / Recall ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_SIEGE_RECALL (
    RecallIdx       SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    KillCount       INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS TB_SIEGE_WARFARE (
    WarfareIdx      SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    MapIdx          INTEGER NOT NULL,
    StartTime       TIMESTAMP,
    EndTime         TIMESTAMP,
    State           SMALLINT DEFAULT 0,
    WaterSeed       BOOLEAN DEFAULT FALSE
);

-- ─── Guild Tournament ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_GUILD_TOURNAMENT (
    TournamentIdx   SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL REFERENCES TB_GUILD(GuildIdx),
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    State           SMALLINT DEFAULT 0,
    Round           SMALLINT DEFAULT 0,
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
    DungeonIdx      SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    EnterDate       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ClearDate       TIMESTAMP,
    State           SMALLINT DEFAULT 0
);

-- ─── Trigger ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_TRIGGER (
    TriggerIdx      SERIAL PRIMARY KEY,
    MapIdx          INTEGER NOT NULL,
    TriggerType     SMALLINT DEFAULT 0,
    TriggerData     TEXT,
    TriggerCount    INTEGER DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdateDate      TIMESTAMP
);

-- ─── Punish List ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PUNISHLIST (
    PunishIdx       SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    PunishType      SMALLINT DEFAULT 0,
    Reason          VARCHAR(256),
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ExpireDate      TIMESTAMP,
    OperatorID      VARCHAR(32),
    PunishCount     INTEGER DEFAULT 0
);

CREATE INDEX idx_punish_char ON TB_PUNISHLIST(CharacterIdx);

-- ─── Auto Note ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_AUTONOTELIST (
    AutoNoteIdx     SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    AutoName        VARCHAR(32),
    AutoCharIdx     INTEGER,
    AutoUserIdx     INTEGER,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── PCRoom ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PCROOM (
    PCRoomIdx       SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    Point           INTEGER DEFAULT 0,
    PlayTime        BIGINT DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Bad Fame ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_BADFAME (
    BadFameIdx      SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    BadFameValue    INTEGER DEFAULT 0,
    Reason          VARCHAR(256),
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Event ──────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_EVENT (
    EventIdx        SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    EventID         INTEGER NOT NULL,
    EventData       TEXT,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(CharacterIdx, EventID)
);

-- ─── Job ────────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_JOB (
    CharacterIdx    INTEGER PRIMARY KEY REFERENCES TB_CHARACTER(CharacterIdx),
    JobID           SMALLINT DEFAULT 0,
    JobLevel        SMALLINT DEFAULT 0,
    JobExp          INTEGER DEFAULT 0
);

-- ─── MoveManager / Recall ──────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_MOVE_RECALL (
    RecallIdx       SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    MapIdx          INTEGER NOT NULL,
    PosX            REAL DEFAULT 0,
    PosY            REAL DEFAULT 0,
    PosZ            REAL DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── NPC Recall ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_NPC_RECALL (
    NpcRecallIdx    SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL REFERENCES TB_CHARACTER(CharacterIdx),
    NpcID           INTEGER NOT NULL,
    RemainTime      INTEGER DEFAULT 0,
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================
-- STORED PROCEDURES (PostgreSQL Functions)
-- These replace the original MSSQL dbo.MP_* stored procedures.
-- ============================================================

-- ─── Character SPs ──────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_CHARACTER_SELECTBYUSERIDX(p_AccountID VARCHAR(32))
RETURNS SETOF TB_CHARACTER AS $$
    SELECT * FROM TB_CHARACTER WHERE AccountID = p_AccountID AND DeleteDate IS NULL;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_SELECTBYCHARACTERIDX(p_CharacterIdx INTEGER)
RETURNS SETOF TB_CHARACTER AS $$
    SELECT * FROM TB_CHARACTER WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_NAMECHECK(p_CharName VARCHAR(32))
RETURNS BOOLEAN AS $$
    SELECT EXISTS(SELECT 1 FROM TB_CHARACTER WHERE CharName = p_CharName AND DeleteDate IS NULL);
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_CREATECHARACTER(
    p_AccountID VARCHAR(32), p_CharName VARCHAR(32),
    p_Str SMALLINT, p_Dex SMALLINT, p_Int SMALLINT,
    p_Con SMALLINT, p_Wis SMALLINT, p_Luck SMALLINT,
    p_Face SMALLINT, p_Hair SMALLINT, p_Job SMALLINT
) RETURNS INTEGER AS $$
DECLARE
    v_CharIdx INTEGER;
BEGIN
    INSERT INTO TB_CHARACTER (AccountID, CharName, Str, Dex, Int, Con, Wis, Luck, Face, Hair, Job)
    VALUES (p_AccountID, p_CharName, p_Str, p_Dex, p_Int, p_Con, p_Wis, p_Luck, p_Face, p_Hair, p_Job)
    RETURNING CharacterIdx INTO v_CharIdx;
    RETURN v_CharIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_CHARACTER_DELETECHARACTER(p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    UPDATE TB_CHARACTER SET DeleteDate = CURRENT_TIMESTAMP WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_LOGOUTPOINTUPDATE(
    p_CharacterIdx INTEGER, p_MapIdx INTEGER,
    p_PosX REAL, p_PosY REAL, p_PosZ REAL
) RETURNS VOID AS $$
    UPDATE TB_CHARACTER SET MapIdx = p_MapIdx, PosX = p_PosX, PosY = p_PosY, PosZ = p_PosZ,
        LastLogout = CURRENT_TIMESTAMP
    WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_UPDATE_ON_MAPOUT(
    p_CharacterIdx INTEGER, p_HP INTEGER, p_MP INTEGER, p_SP INTEGER,
    p_MapIdx INTEGER, p_PosX REAL, p_PosY REAL, p_PosZ REAL,
    p_Exp BIGINT, p_Money BIGINT, p_Level INTEGER, p_PlayTime BIGINT
) RETURNS VOID AS $$
    UPDATE TB_CHARACTER SET
        HP = p_HP, MP = p_MP, SP = p_SP,
        MapIdx = p_MapIdx, PosX = p_PosX, PosY = p_PosY, PosZ = p_PosZ,
        Exp = p_Exp, Money = p_Money, Level = p_Level, PlayTime = p_PlayTime,
        LastLogout = CURRENT_TIMESTAMP
    WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_JOB_UPDATE(p_CharacterIdx INTEGER, p_Job SMALLINT)
RETURNS VOID AS $$
    UPDATE TB_CHARACTER SET Job = p_Job WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_BUFF_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_CHARACTER_BUFF AS $$
    SELECT * FROM TB_CHARACTER_BUFF WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_BUFF_ADD(
    p_CharacterIdx INTEGER, p_SkillIdx INTEGER, p_RemainTime INTEGER,
    p_Count INTEGER, p_IsEndDate BOOLEAN
) RETURNS INTEGER AS $$
DECLARE
    v_BuffIdx INTEGER;
BEGIN
    INSERT INTO TB_CHARACTER_BUFF (CharacterIdx, SkillIdx, RemainTime, Count, IsEndDate)
    VALUES (p_CharacterIdx, p_SkillIdx, p_RemainTime, p_Count, p_IsEndDate)
    RETURNING BuffIdx INTO v_BuffIdx;
    RETURN v_BuffIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_CHARACTER_BUFF_UPDATE(
    p_CharacterIdx INTEGER, p_SkillIdx INTEGER, p_RemainTime INTEGER,
    p_Count INTEGER, p_IsEndDate BOOLEAN
) RETURNS VOID AS $$
    UPDATE TB_CHARACTER_BUFF SET
        RemainTime = p_RemainTime, Count = p_Count, IsEndDate = p_IsEndDate
    WHERE CharacterIdx = p_CharacterIdx AND SkillIdx = p_SkillIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHARACTER_BUFF_REMOVE(p_CharacterIdx INTEGER, p_SkillIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_CHARACTER_BUFF WHERE CharacterIdx = p_CharacterIdx AND SkillIdx = p_SkillIdx;
$$ LANGUAGE SQL;

-- ─── Item SPs ───────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_ITEM_INSERT_INVEN(
    p_CharacterIdx INTEGER, p_ItemDBIdx INTEGER, p_Count INTEGER,
    p_Slot SMALLINT, p_Grade SMALLINT, p_Durability SMALLINT
) RETURNS INTEGER AS $$
DECLARE
    v_ItemIdx INTEGER;
BEGIN
    INSERT INTO TB_ITEM (CharacterIdx, ItemDBIdx, Count, Slot, Grade, Durability)
    VALUES (p_CharacterIdx, p_ItemDBIdx, p_Count, p_Slot, p_Grade, p_Durability)
    RETURNING ItemIdx INTO v_ItemIdx;
    RETURN v_ItemIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_ITEM_UPDATE(
    p_ItemIdx INTEGER, p_Count INTEGER, p_Slot SMALLINT,
    p_Grade SMALLINT, p_Durability SMALLINT
) RETURNS VOID AS $$
    UPDATE TB_ITEM SET Count = p_Count, Slot = p_Slot, Grade = p_Grade, Durability = p_Durability
    WHERE ItemIdx = p_ItemIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_ITEM_DELETE(p_ItemIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_ITEM WHERE ItemIdx = p_ItemIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_ITEM_MOVEUPDATE(
    p_ItemIdx INTEGER, p_Slot SMALLINT
) RETURNS VOID AS $$
    UPDATE TB_ITEM SET Slot = p_Slot, StorageType = 0, StorageSlot = -1
    WHERE ItemIdx = p_ItemIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_ITEM_MOVEUPDATESTORAGE(
    p_ItemIdx INTEGER, p_StorageSlot SMALLINT
) RETURNS VOID AS $$
    UPDATE TB_ITEM SET StorageSlot = p_StorageSlot, StorageType = 1
    WHERE ItemIdx = p_ItemIdx;
$$ LANGUAGE SQL;

-- ─── Guild SPs ──────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_GUILD_CREATE2(
    p_GuildName VARCHAR(32), p_MasterIdx INTEGER
) RETURNS INTEGER AS $$
DECLARE
    v_GuildIdx INTEGER;
BEGIN
    INSERT INTO TB_GUILD (GuildName, MasterIdx) VALUES (p_GuildName, p_MasterIdx)
    RETURNING GuildIdx INTO v_GuildIdx;
    INSERT INTO TB_GUILD_MEMBER (GuildIdx, CharacterIdx, Rank)
    VALUES (v_GuildIdx, p_MasterIdx, 0);
    RETURN v_GuildIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_GUILD_LOADGUILD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_GUILD AS $$
    SELECT g.* FROM TB_GUILD g
    JOIN TB_GUILD_MEMBER m ON g.GuildIdx = m.GuildIdx
    WHERE m.CharacterIdx = p_CharacterIdx AND g.DeleteDate IS NULL;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_GUILD_ADDMEMBER(p_GuildIdx INTEGER, p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    INSERT INTO TB_GUILD_MEMBER (GuildIdx, CharacterIdx) VALUES (p_GuildIdx, p_CharacterIdx);
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_GUILD_DELETEMEMBER(p_GuildIdx INTEGER, p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_GUILD_MEMBER WHERE GuildIdx = p_GuildIdx AND CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_GUILD_BREAKUP(p_GuildIdx INTEGER)
RETURNS VOID AS $$
    UPDATE TB_GUILD SET DeleteDate = CURRENT_TIMESTAMP WHERE GuildIdx = p_GuildIdx;
$$ LANGUAGE SQL;

-- ─── Quest SPs ──────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_QUEST_TOTALINFO(p_CharacterIdx INTEGER)
RETURNS SETOF TB_QUEST AS $$
    SELECT * FROM TB_QUEST WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_QUEST_UPDATE(
    p_CharacterIdx INTEGER, p_QuestID INTEGER, p_QuestState SMALLINT, p_Progress TEXT
) RETURNS VOID AS $$
    INSERT INTO TB_QUEST (CharacterIdx, QuestID, QuestState, Progress)
    VALUES (p_CharacterIdx, p_QuestID, p_QuestState, p_Progress)
    ON CONFLICT (CharacterIdx, QuestID)
    DO UPDATE SET QuestState = EXCLUDED.QuestState, Progress = EXCLUDED.Progress;
$$ LANGUAGE SQL;

-- ─── Friend SPs ─────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_FRIEND_ADDFRIEND(
    p_CharacterIdx INTEGER, p_FriendIdx INTEGER, p_Group SMALLINT
) RETURNS VOID AS $$
    INSERT INTO TB_FRIEND (CharacterIdx, FriendIdx, FriendGroup)
    VALUES (p_CharacterIdx, p_FriendIdx, p_Group);
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_FRIEND_DELFRIEND(p_CharacterIdx INTEGER, p_FriendIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_FRIEND WHERE CharacterIdx = p_CharacterIdx AND FriendIdx = p_FriendIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_FRIEND_GETFRIENDLIST(p_CharacterIdx INTEGER)
RETURNS SETOF TB_FRIEND AS $$
    SELECT * FROM TB_FRIEND WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Note SPs ───────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_NOTE_GETNOTELIST(p_CharacterIdx INTEGER)
RETURNS SETOF TB_NOTE AS $$
    SELECT * FROM TB_NOTE WHERE ReceiverIdx = p_CharacterIdx ORDER BY SendDate DESC;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_NOTE_SENDNOTE(
    p_SenderIdx INTEGER, p_ReceiverIdx INTEGER,
    p_Title VARCHAR(64), p_Content TEXT
) RETURNS INTEGER AS $$
DECLARE
    v_NoteIdx INTEGER;
BEGIN
    INSERT INTO TB_NOTE (SenderIdx, ReceiverIdx, Title, Content)
    VALUES (p_SenderIdx, p_ReceiverIdx, p_Title, p_Content)
    RETURNING NoteIdx INTO v_NoteIdx;
    RETURN v_NoteIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_NOTE_READNOTE(p_NoteIdx INTEGER)
RETURNS SETOF TB_NOTE AS $$
    UPDATE TB_NOTE SET IsRead = TRUE, ReadDate = CURRENT_TIMESTAMP
    WHERE NoteIdx = p_NoteIdx;
    SELECT * FROM TB_NOTE WHERE NoteIdx = p_NoteIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_NOTE_DELNOTE(p_NoteIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_NOTE WHERE NoteIdx = p_NoteIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_NOTE_ISNEWNOTE(p_CharacterIdx INTEGER)
RETURNS BOOLEAN AS $$
    SELECT EXISTS(SELECT 1 FROM TB_NOTE WHERE ReceiverIdx = p_CharacterIdx AND IsRead = FALSE);
$$ LANGUAGE SQL;

-- ─── Party SPs ──────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_PARTY_CREATE(p_MasterIdx INTEGER)
RETURNS INTEGER AS $$
DECLARE
    v_PartyIdx INTEGER;
BEGIN
    INSERT INTO TB_PARTY (MasterIdx) VALUES (p_MasterIdx) RETURNING PartyIdx INTO v_PartyIdx;
    INSERT INTO TB_PARTY_MEMBER (PartyIdx, CharacterIdx) VALUES (v_PartyIdx, p_MasterIdx);
    RETURN v_PartyIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_PARTY_BREAK(p_PartyIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_PARTY_MEMBER WHERE PartyIdx = p_PartyIdx;
    DELETE FROM TB_PARTY WHERE PartyIdx = p_PartyIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_PARTY_ADDMEMBER(p_PartyIdx INTEGER, p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    INSERT INTO TB_PARTY_MEMBER (PartyIdx, CharacterIdx) VALUES (p_PartyIdx, p_CharacterIdx);
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_PARTY_DELMEMBER(p_PartyIdx INTEGER, p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_PARTY_MEMBER WHERE PartyIdx = p_PartyIdx AND CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Family SPs ─────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_FAMILY_SAVEINFO(
    p_FamilyName VARCHAR(32), p_MasterIdx INTEGER
) RETURNS INTEGER AS $$
DECLARE
    v_FamilyIdx INTEGER;
BEGIN
    INSERT INTO TB_FAMILY (FamilyName, MasterIdx) VALUES (p_FamilyName, p_MasterIdx)
    RETURNING FamilyIdx INTO v_FamilyIdx;
    INSERT INTO TB_FAMILY_MEMBER (FamilyIdx, CharacterIdx, Role)
    VALUES (v_FamilyIdx, p_MasterIdx, 0);
    RETURN v_FamilyIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_FAMILY_LOADINFO(p_CharacterIdx INTEGER)
RETURNS SETOF TB_FAMILY AS $$
    SELECT f.* FROM TB_FAMILY f
    JOIN TB_FAMILY_MEMBER m ON f.FamilyIdx = m.FamilyIdx
    WHERE m.CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_FAMILY_CHECKNAME(p_FamilyName VARCHAR(32))
RETURNS BOOLEAN AS $$
    SELECT EXISTS(SELECT 1 FROM TB_FAMILY WHERE FamilyName = p_FamilyName);
$$ LANGUAGE SQL;

-- ─── Pet SPs ────────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_PET_INFO_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_PET AS $$
    SELECT * FROM TB_PET WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_PET_INFO_ADD(
    p_CharacterIdx INTEGER, p_PetDBIdx INTEGER, p_PetName VARCHAR(32)
) RETURNS INTEGER AS $$
DECLARE
    v_PetIdx INTEGER;
BEGIN
    INSERT INTO TB_PET (CharacterIdx, PetDBIdx, PetName)
    VALUES (p_CharacterIdx, p_PetDBIdx, p_PetName)
    RETURNING PetIdx INTO v_PetIdx;
    RETURN v_PetIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_PET_INFO_REMOVE(p_PetIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_PET WHERE PetIdx = p_PetIdx;
$$ LANGUAGE SQL;

-- ─── Consignment SPs ────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_CONSIGNMENT_INSERT(
    p_CharacterIdx INTEGER, p_ItemIdx INTEGER, p_Price BIGINT, p_ExpireDays INTEGER DEFAULT 7
) RETURNS INTEGER AS $$
DECLARE
    v_ConsignmentIdx INTEGER;
BEGIN
    INSERT INTO TB_CONSIGNMENT (CharacterIdx, ItemIdx, Price, ExpireDate)
    VALUES (p_CharacterIdx, p_ItemIdx, p_Price, CURRENT_TIMESTAMP + (p_ExpireDays || ' days')::INTERVAL)
    RETURNING ConsignmentIdx INTO v_ConsignmentIdx;
    RETURN v_ConsignmentIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_CONSIGNMENT_GETLIST()
RETURNS SETOF TB_CONSIGNMENT AS $$
    SELECT * FROM TB_CONSIGNMENT
    WHERE IsSold = FALSE AND IsCanceled = FALSE AND ExpireDate > CURRENT_TIMESTAMP;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CONSIGNMENT_CHECKDATE()
RETURNS VOID AS $$
    UPDATE TB_CONSIGNMENT SET IsCanceled = TRUE
    WHERE ExpireDate <= CURRENT_TIMESTAMP AND IsSold = FALSE AND IsCanceled = FALSE;
$$ LANGUAGE SQL;

-- ─── Housing SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_HOUSECREATE(
    p_CharacterIdx INTEGER, p_MapIdx INTEGER,
    p_PosX REAL, p_PosY REAL, p_HouseType SMALLINT
) RETURNS INTEGER AS $$
DECLARE
    v_HouseIdx INTEGER;
BEGIN
    INSERT INTO TB_HOUSE (CharacterIdx, MapIdx, PosX, PosY, HouseType)
    VALUES (p_CharacterIdx, p_MapIdx, p_PosX, p_PosY, p_HouseType)
    RETURNING HouseIdx INTO v_HouseIdx;
    RETURN v_HouseIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_HOUSEDATA_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_HOUSE AS $$
    SELECT * FROM TB_HOUSE WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Siege Recall SPs ───────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_SIEGERECALL_INSERT(
    p_CharacterIdx INTEGER, p_MapIdx INTEGER,
    p_PosX REAL, p_PosY REAL, p_PosZ REAL
) RETURNS INTEGER AS $$
DECLARE
    v_RecallIdx INTEGER;
BEGIN
    INSERT INTO TB_SIEGE_RECALL (CharacterIdx, MapIdx, PosX, PosY, PosZ)
    VALUES (p_CharacterIdx, p_MapIdx, p_PosX, p_PosY, p_PosZ)
    RETURNING RecallIdx INTO v_RecallIdx;
    RETURN v_RecallIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_SIEGERECALL_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_SIEGE_RECALL AS $$
    SELECT * FROM TB_SIEGE_RECALL WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Vehicle SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_VEHICLE_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_VEHICLE AS $$
    SELECT * FROM TB_VEHICLE WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Quick Slot SPs ─────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_QUICK_INFO(p_CharacterIdx INTEGER)
RETURNS TB_QUICKSLOT AS $$
    SELECT * FROM TB_QUICKSLOT WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_QUICK_UPDATE(p_CharacterIdx INTEGER, p_SlotData TEXT)
RETURNS VOID AS $$
    INSERT INTO TB_QUICKSLOT (CharacterIdx, SlotData)
    VALUES (p_CharacterIdx, p_SlotData)
    ON CONFLICT (CharacterIdx) DO UPDATE SET SlotData = EXCLUDED.SlotData;
$$ LANGUAGE SQL;

-- ─── Dungeon SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_DUNGEONENTRANCE(
    p_CharacterIdx INTEGER, p_MapIdx INTEGER
) RETURNS INTEGER AS $$
DECLARE
    v_DungeonIdx INTEGER;
BEGIN
    INSERT INTO TB_DUNGEON (CharacterIdx, MapIdx)
    VALUES (p_CharacterIdx, p_MapIdx)
    RETURNING DungeonIdx INTO v_DungeonIdx;
    RETURN v_DungeonIdx;
END;
$$ LANGUAGE plpgsql;

-- ─── Fishing SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_FISHINGDATA_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_FISHING AS $$
    SELECT * FROM TB_FISHING WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Cooking SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_COOKING_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_COOKING AS $$
    SELECT * FROM TB_COOKING WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Farm SPs ───────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_FARM_SELECT(p_CharacterIdx INTEGER)
RETURNS SETOF TB_FARM AS $$
    SELECT * FROM TB_FARM WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_FARM_CROP_SELECT(p_FarmIdx INTEGER)
RETURNS SETOF TB_FARM_CROP AS $$
    SELECT * FROM TB_FARM_CROP WHERE FarmIdx = p_FarmIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_FARM_ANIMAL_SELECT(p_FarmIdx INTEGER)
RETURNS SETOF TB_FARM_ANIMAL AS $$
    SELECT * FROM TB_FARM_ANIMAL WHERE FarmIdx = p_FarmIdx;
$$ LANGUAGE SQL;

-- ─── PvP / Bad Fame SPs ────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_BADFAME_CHARACTERUPDATE(
    p_CharacterIdx INTEGER, p_Value INTEGER
) RETURNS VOID AS $$
    UPDATE TB_CHARACTER SET BadFame = BadFame + p_Value WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Storage SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_STORAGE_INFO(p_CharacterIdx INTEGER)
RETURNS SETOF TB_STORAGE AS $$
    SELECT * FROM TB_STORAGE WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

-- ─── Challenge Zone SPs ─────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_CHALLENGEZONE_ENTERFREQ_LOAD_INFO(p_CharacterIdx INTEGER)
RETURNS SETOF TB_CHALLENGEZONE AS $$
    SELECT * FROM TB_CHALLENGEZONE WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION MP_CHALLENGEZONE_SUCCESS_SAVE_INFO(
    p_CharacterIdx INTEGER
) RETURNS VOID AS $$
    INSERT INTO TB_CHALLENGEZONE (CharacterIdx, SuccessCount)
    VALUES (p_CharacterIdx, 1)
    ON CONFLICT (CharacterIdx) DO UPDATE SET SuccessCount = TB_CHALLENGEZONE.SuccessCount + 1;
$$ LANGUAGE SQL;

-- ─── Guild Tournament SPs ──────────────────────────────────
CREATE OR REPLACE FUNCTION MP_GT_CHECK_REGIST(p_GuildIdx INTEGER)
RETURNS BOOLEAN AS $$
    SELECT EXISTS(SELECT 1 FROM TB_GUILD_TOURNAMENT WHERE GuildIdx = p_GuildIdx AND State > 0);
$$ LANGUAGE SQL;

-- ─── Punish List SPs ───────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_PUNISHLIST_ADD(
    p_CharacterIdx INTEGER, p_PunishType SMALLINT, p_Reason VARCHAR(256),
    p_OperatorID VARCHAR(32), p_ExpireDays INTEGER DEFAULT 0
) RETURNS INTEGER AS $$
DECLARE
    v_PunishIdx INTEGER;
BEGIN
    INSERT INTO TB_PUNISHLIST (CharacterIdx, PunishType, Reason, OperatorID, ExpireDate)
    VALUES (p_CharacterIdx, p_PunishType, p_Reason, p_OperatorID,
            CASE WHEN p_ExpireDays > 0 THEN CURRENT_TIMESTAMP + (p_ExpireDays || ' days')::INTERVAL ELSE NULL END)
    RETURNING PunishIdx INTO v_PunishIdx;
    RETURN v_PunishIdx;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION MP_PUNISHLIST_LOAD(p_CharacterIdx INTEGER)
RETURNS SETOF TB_PUNISHLIST AS $$
    SELECT * FROM TB_PUNISHLIST
    WHERE CharacterIdx = p_CharacterIdx AND (ExpireDate IS NULL OR ExpireDate > CURRENT_TIMESTAMP);
$$ LANGUAGE SQL;

-- ─── Trigger SPs ────────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_TRIGGER_SELECT(p_MapIdx INTEGER)
RETURNS SETOF TB_TRIGGER AS $$
    SELECT * FROM TB_TRIGGER WHERE MapIdx = p_MapIdx;
$$ LANGUAGE SQL;

-- ─── Monster Meter ─────────────────────────────────────────
CREATE OR REPLACE FUNCTION MP_MONSTERMETER_INITINFO(p_CharacterIdx INTEGER)
RETURNS VOID AS $$
    DELETE FROM TB_MONSTERMETER WHERE CharacterIdx = p_CharacterIdx;
$$ LANGUAGE SQL;

COMMIT;
