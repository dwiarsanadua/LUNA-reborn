-- ============================================================
-- LUNA Plus — Log Database Schema (PostgreSQL)
-- Database: LUNA_LOGDB
-- Auto-generated from source code reverse engineering (P3.1)
-- ============================================================

BEGIN;

-- ─── Hacking Log ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_LogHacking (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    CheatKind       INTEGER DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_hacking_char ON TB_LogHacking(CharacterIdx);
CREATE INDEX idx_hacking_date ON TB_LogHacking(LogDate);

-- ─── Item Drop Option Log ───────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM_DROP_OPTION_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    LogType         SMALLINT DEFAULT 0,
    ItemDBIdx       INTEGER NOT NULL,
    Key1            INTEGER,
    Value1          REAL,
    Key2            INTEGER,
    Value2          REAL,
    Key3            INTEGER,
    Value3          REAL,
    Key4            INTEGER,
    Value4          REAL,
    Key5            INTEGER,
    Value5          REAL,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Item Shop Log ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_ITEM_SHOP_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    LogType         SMALLINT DEFAULT 0,
    UserIdx         INTEGER NOT NULL,
    UserID          VARCHAR(32),
    ItemIdx         INTEGER NOT NULL,
    ItemDBIdx       INTEGER DEFAULT 0,
    ItemSize        INTEGER DEFAULT 1,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_shop_user ON TB_ITEM_SHOP_LOG(UserIdx);

-- ─── Pet Log ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TB_PET_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    LogType         SMALLINT DEFAULT 0,
    UserIdx         INTEGER NOT NULL,
    PetIdx          INTEGER NOT NULL,
    Experience      INTEGER DEFAULT 0,
    Friendship      INTEGER DEFAULT 0,
    Grade           SMALLINT DEFAULT 0,
    Level           SMALLINT DEFAULT 0,
    Kind            SMALLINT DEFAULT 0,
    State           SMALLINT DEFAULT 0,
    SkillSlot       INTEGER DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Chat Log ───────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_CHAT_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ChatType        SMALLINT DEFAULT 0,
    TargetIdx       INTEGER DEFAULT 0,
    Message         TEXT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_chat_char ON TP_CHAT_LOG(CharacterIdx);
CREATE INDEX idx_chat_date ON TP_CHAT_LOG(LogDate);

-- ─── Family Log ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_FAMILY_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    FamilyIdx       INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Family Point Log ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_FAMILY_POINT_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    FamilyIdx       INTEGER NOT NULL,
    CharacterIdx    INTEGER NOT NULL,
    PointChange     INTEGER DEFAULT 0,
    Reason          VARCHAR(64),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Farm Log ───────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_FARM_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    FarmIdx         INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TP_FARM_CROP_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    FarmIdx         INTEGER NOT NULL,
    CropDBIdx       INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TP_FARM_ANIMAL_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    FarmIdx         INTEGER NOT NULL,
    AnimalDBIdx     INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Guild Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_GUILD_SCORE_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL,
    ScoreChange     INTEGER DEFAULT 0,
    Reason          VARCHAR(64),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TP_GUILD_WAREHOUSE_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    ItemIdx         INTEGER,
    MoneyChange     BIGINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS UP_GUILDLOG (
    LogIdx          SERIAL PRIMARY KEY,
    GuildIdx        INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Housing Log ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_HOUSING_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    HouseIdx        INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Item Option Log ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_ITEM_OPTION_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    ItemIdx         INTEGER NOT NULL,
    OptType         SMALLINT,
    OldValue        REAL,
    NewValue        REAL,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Job Log ────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_JOB_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    OldJob          SMALLINT,
    NewJob          SMALLINT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Quest Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_QUEST_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    QuestID         INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Script Hack Log ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_SCRIPT_HACK_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ScriptData      TEXT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Siege Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_SIEGE_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    WarfareIdx      INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    GuildIdx        INTEGER,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Skill Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_SKILL_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    SkillID         INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Cooking Log ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_COOKING_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS TP_COOKRECIPE_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    RecipeID        INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Consignment Log ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_CONSIGNMENT_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    ConsignmentIdx  INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    CharacterIdx    INTEGER,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Character Log ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_CHARACTERLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    LogData         TEXT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_char_log ON UP_CHARACTERLOG(CharacterIdx);

-- ─── EXP/Point Log ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_EXPPOINTLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ExpChange       BIGINT DEFAULT 0,
    PointType       SMALLINT DEFAULT 0,
    Reason          VARCHAR(64),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Money Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_MONEYLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    MoneyChange     BIGINT DEFAULT 0,
    MoneyType       SMALLINT DEFAULT 0,
    Reason          VARCHAR(64),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS UP_WRONGMONEYLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    WrongMoney      BIGINT DEFAULT 0,
    CorrectMoney    BIGINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS UP_ITEMMONEYLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ItemIdx         INTEGER,
    MoneyChange     BIGINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Character Rename Log ───────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_CHARRENAME_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    OldName         VARCHAR(32),
    NewName         VARCHAR(32),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Cheat Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_CHEATE_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    CheatType       SMALLINT DEFAULT 0,
    LogData         TEXT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Fishing Log ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_FISHING_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Auto Note Log ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_AUTONOTELOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── GM Tool Log ────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_TOOLLOG (
    LogIdx          SERIAL PRIMARY KEY,
    OperatorID      VARCHAR(32),
    ActionType      SMALLINT DEFAULT 0,
    TargetIdx       INTEGER,
    LogData         TEXT,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS UP_GMTOOLUSELOG (
    LogIdx          SERIAL PRIMARY KEY,
    OperatorID      VARCHAR(32),
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── nProtect Block Log ─────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_NPROTECTBLOCKLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    BlockType       SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Speed Hack Check Log ──────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_SPEEDHACKCHECK (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    SpeedValue      REAL,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── MunGong Exp Log ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_MUNGONGEXPLOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    ExpChange       BIGINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Game Room Point Log ───────────────────────────────────
CREATE TABLE IF NOT EXISTS TP_GAMEROOM_POINT_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    PointChange     INTEGER DEFAULT 0,
    Reason          VARCHAR(64),
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Event Log ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS UP_EVENT_LOG (
    LogIdx          SERIAL PRIMARY KEY,
    CharacterIdx    INTEGER NOT NULL,
    EventID         INTEGER,
    ActionType      SMALLINT DEFAULT 0,
    LogDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

COMMIT;
