-- ============================================================
-- LUNA Plus — Member Database Schema (PostgreSQL)
-- Database: LUNA_MEMBERDB
-- Auto-generated from source code reverse engineering (P3.1)
-- ============================================================

BEGIN;

-- ─── Account ─────────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS AccountInfo (
    AccountID       VARCHAR(32) PRIMARY KEY,
    Password        VARCHAR(64) NOT NULL,
    Email           VARCHAR(128),
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    LastLogin       TIMESTAMP,
    LastIP          VARCHAR(45),
    LoginCount      INTEGER DEFAULT 0,
    Blocked         BOOLEAN DEFAULT FALSE,
    BlockReason     VARCHAR(256),
    MemberType      SMALLINT DEFAULT 0,
    WebLauncherKey  VARCHAR(64)
);

CREATE INDEX idx_account_email ON AccountInfo(Email);

-- ─── IP Checks ──────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS IPCheck (
    IP              VARCHAR(45) PRIMARY KEY,
    Allowed         BOOLEAN DEFAULT TRUE,
    Country         VARCHAR(4),
    RegDate         TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ─── Login History ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS LoginHistory (
    LogID           SERIAL PRIMARY KEY,
    AccountID       VARCHAR(32) REFERENCES AccountInfo(AccountID),
    LoginTime       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    LogoutTime      TIMESTAMP,
    IP              VARCHAR(45),
    Result          SMALLINT DEFAULT 0
);

CREATE INDEX idx_login_account ON LoginHistory(AccountID);
CREATE INDEX idx_login_time ON LoginHistory(LoginTime);

-- ─── Blocked Accounts ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS BlockedAccount (
    AccountID       VARCHAR(32) PRIMARY KEY REFERENCES AccountInfo(AccountID),
    BlockDate       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    BlockEndDate    TIMESTAMP,
    BlockReason     VARCHAR(256),
    OperatorID      VARCHAR(32)
);

-- ─── Session Tokens ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS SessionToken (
    AccountID       VARCHAR(32) PRIMARY KEY REFERENCES AccountInfo(AccountID),
    Token           VARCHAR(128) NOT NULL,
    CreatedAt       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ExpiresAt       TIMESTAMP,
    IP              VARCHAR(45)
);

CREATE INDEX idx_session_token ON SessionToken(Token);

-- ─── Operator / GM ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS Operator (
    OperatorID      VARCHAR(32) PRIMARY KEY,
    Password        VARCHAR(64) NOT NULL,
    Level           INTEGER DEFAULT 0,
    LastLogin       TIMESTAMP,
    AllowedIP       VARCHAR(45)
);

-- ─── Web Launcher Keys ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS WebLauncherKey (
    AccountID       VARCHAR(32) PRIMARY KEY REFERENCES AccountInfo(AccountID),
    LoginKey        VARCHAR(64) NOT NULL,
    CreatedAt       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ExpiresAt       TIMESTAMP,
    Used            BOOLEAN DEFAULT FALSE
);

CREATE INDEX idx_launcher_key ON WebLauncherKey(LoginKey);

COMMIT;
