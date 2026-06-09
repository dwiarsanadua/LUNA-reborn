# PACKET MAPPING — Field-by-Field (10 Packet Categories)

> Old struct → Reborn FlatBuffers schema
> Format: Old Field | Type (Old) | Reborn Field | FBS Type | Status | Sev | Effort | Deps

---

## 1. LOGIN (MP_USERCONN_LOGIN_SYN → Login.fbs)

```
PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)         [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwUserID[48]                DWORD[48]     username          string       ✅      -   -      -
szPassword[32]              CHAR[32]      password_hash     [uint8]      🟡      M   4 hrs  Crypto
dwVersion                   DWORD         client_version    string       🟡      L   1 hr   -
dwCRC                       DWORD         (missing)         -            🔴      L   1 hr   -
bUseNProtect                BYTE          (missing)         -            🔴      L   -      Removed
dwClientTime                DWORD         (missing)         -            🔴      L   30 min -
(missing)                   -             mac_address       string       🟢      -   -      Added field
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 6, Reborn 4. Match: 2/6 (33%). Added: 1 new (mac_address).
```

**Struct Old:**
```cpp
// From Protocol.h / server struct
struct LOGIN_SYN {
    MSGROOT header;        // Category=MP_USERCONN, Protocol=MP_USERCONN_LOGIN_SYN
    char dwUserID[48];     // Username
    char szPassword[32];   // Plaintext password
    DWORD dwVersion;       // Client version
    DWORD dwCRC;           // File CRC check
    BYTE bUseNProtect;     // NProtect flag
    DWORD dwClientTime;    // Client timestamp
};
```

**Schema Reborn:**
```fbs
// Login.fbs
table LoginRequest {
    username: string (required);
    password_hash: [uint8] (required);  // SHA-256 of password
    client_version: string;
    mac_address: string;
}
```

---

## 2. MOVE (MP_MOVE_WALK → Movement.fbs)

```
PACKET: MP_MOVE_WALK → Movement.fbs (MoveRequest)                [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
header.dwObjectID           DWORD         (implied)        -            🟡      -   -      -
CurPos.wx/wz                WORD x 2        direction       Vec3         🟡      M   4 hrs  -
MoveMode                    eMoveMode       move_mode       MoveMode     🟡      M   2 hrs  -
KyungGongIdx                WORD            (missing)       -            🔴      M   1 day  Skill
AddedMoveSpeed              float           velocity        Vec3         🟡      L   2 hrs  -
(missing)                   -               target_position Vec3         🟢      -   -      Added
(missing)                   -               timestamp       uint64       🟢      -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 4, Reborn 5. Match: 2/4 (50%). Added: 2 new.
```

**Struct Old:**
```cpp
struct SEND_MOVEINFO {
    COMPRESSEDPOS CurPos;     // wx, wz compressed position
    eMoveMode MoveMode;       // Walk=0, Run=1
    WORD KyungGongIdx;        // Dash skill index (0 = none)
    float AddedMoveSpeed;     // Speed modifier
};
```

**Schema Reborn:**
```fbs
table MoveRequest {
    direction: Vec3;
    target_position: Vec3;
    move_mode: MoveMode;
}
```

---

## 3. COMBAT (MP_COMBAT_ATTACK_SYN → Combat.fbs)

```
PACKET: MP_COMBAT_ATTACK_SYN → Combat.fbs (AttackRequest)         [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwObjectID (attacker)       DWORD         (implied)        -            🟡      -   -      -
dwTargetObjectID            DWORD         target_id        uint32       ✅      -   -      -
dwSkillID (0=auto)          DWORD         skill_id         uint16       🟡      M   2 hrs  Skill
(missing)                   -             position         Vec3         🟢      -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 3, Reborn 3. Match: 2/3 (66%). Added: position.

AttackResult:
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
damage                      int            damage           int32        ✅      -   -      -
isCritical                  BOOL           is_critical      bool         ✅      -   -      -
isBlocked                   BOOL           (missing)        -            🔴      M   4 hrs  -
isMiss                      BOOL           is_miss          bool         ✅      -   -      -
damageType                  BYTE           damage_type      DamageType   🟡      M   2 hrs  -
(missing)                   -              target_hp_       int32        🟢      -   -      Added
                                            remaining
(missing)                   -              effects          [DamageEffect] 🟢   -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 5, Reborn 6. Match: 4/5 (80%). Added: 2 new. Missing: 1 (isBlocked).
```

---

## 4. SKILL (MP_SKILL_CAST_SYN → Skill.fbs)

```
PACKET: MP_SKILL_CAST_SYN → Skill.fbs (SkillCastRequest)          [Total effort: 4 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwCasterID                  DWORD         caster_id        uint32       ✅      -   -      -
wSkillIdx                   WORD          skill_id         uint16       ✅      -   -      -
dwTargetID                  DWORD         target_id        uint32       ✅      -   -      -
(missing)                   -             position         Vec3         🟢      -   -      Added
(missing)                   -             hits             uint8        🟢      -   -      Added
(missing)                   -             results          [AttackResult] 🟢   -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 3, Reborn 5. Match: 3/3 (100%). Added: 3 new (enhanced result data).
```

---

## 5. INVENTORY (MP_ITEM_* → Inventory.fbs)

```
PACKET: MP_ITEM_MOVE_SYN → Inventory.fbs (InventoryUpdate)        [Total effort: 5 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwObjectID                  DWORD         (implied)        -            🟡      -   -      -
bySrcSlot                   BYTE          slot_index       uint8        ✅      -   -      -
byDstSlot                   BYTE          (missing)        -            🔴      M   2 hrs  -
wSrcStorageType             WORD          (missing)        -            🔴      M   2 hrs  -
wDstStorageType             WORD          (missing)        -            🔴      M   2 hrs  -
dwItemDBIdx                 DWORD         item_id          uint32       ✅      -   -      -
byCount                     BYTE          count            uint16       🟡      L   1 hr   -

MP_ITEM_USE_SYN:
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
bySlot                      BYTE          slot_index       uint8        ✅      -   -      -
dwTargetID                  DWORD         target_id        uint32       🟡      M   2 hrs  -

InventorySlot (full data):
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
bySlot                      BYTE          slot_index       uint8        ✅      -   -      -
dwItemDBIdx                 DWORD         item_id          uint32       ✅      -   -      -
byCount                     BYTE          count            uint16       ✅      -   -      -
wDurability                 WORD          (missing)        -            🔴      M   2 hrs  -
bEnchantLevel               BYTE          enchant          uint8        ✅      -   -      -
(banyak field option)       (varies)      (missing)        -            🔴      H   1 week -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 7+ per item. Reborn: 4. Critical gaps: durability, options, storage type.
```

---

## 6. QUEST (MP_QUEST_* → Quest.fbs)

```
PACKET: MP_QUEST_LIST_ACK → Quest.fbs (QuestListResponse)         [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwQuestID                   DWORD         quest_id         uint32       ✅      -   -      -
szQuestName[64]             CHAR[64]      name             string       ✅      -   -      -
cState                      BYTE          is_completed     bool         🟡      L   1 hr   -
cProgress[10]               BYTE[10]      objectives       [QuestOb-    🟡      M   1 day  -
                                           jectiveState]
(missing)                   -             is_reward_taken  bool         🟢      -   -      Added

QuestStartRequest:
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwNPCID                     DWORD         npc_id           uint32       ✅      -   -      -
dwQuestID                   DWORD         quest_id         uint32       ✅      -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 4, Reborn 5+ per quest. Good coverage.
```

---

## 7. PARTY (MP_PARTY_* → Party.fbs)

```
PACKET: MP_PARTY_CREATE_SYN → Party.fbs (PartyCreateRequest)      [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwObjectID                  DWORD         character_id     uint32       ✅      -   -      -

PartyMemberInfo:
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwCharID                    DWORD         character_id     uint32       ✅      -   -      -
szCharName[32]              CHAR[32]      name             string       ✅      -   -      -
wLevel                      WORD          level            uint16       ✅      -   -      -
dwHP / dwMaxHP              DWORD x 2     hp / max_hp      int32 x 2    ✅      -   -      -
wMapID                      WORD          map_id           uint16       ✅      -   -      -
bLeader                     BOOL          is_leader        bool         ✅      -   -      -
bMemberCount                BYTE          (implied)        -            🟡      -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 7, Reborn 7. Excellent coverage. ✅
```

---

## 8. GUILD (MP_GUILD_* → Guild.fbs)

```
PACKET: MP_GUILD_CREATE_SYN → Guild.fbs (GuildCreateRequest)      [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwMasterID                  DWORD         character_id     uint32       ✅      -   -      -
szGuildName[32]             CHAR[32]      guild_name       string       ✅      -   -      -

GuildInfo:
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwGuildID                   DWORD         guild_id         uint32       ✅      -   -      -
szGuildName[32]             CHAR[32]      name             string       ✅      -   -      -
wLevel                      WORD          level            uint8        🟡      L   1 hr   -
dwGP                        DWORD         gp               uint32       ✅      -   -      -
dwMasterID                  DWORD         master_id        uint32       ✅      -   -      -
dwMemberCount               DWORD         (implied)        -            🟡      -   -      -
szGuildMark[256]            CHAR[256]     (missing)        -            🔴      M   2 days Image
dwGuildPoint                DWORD         (missing)        -            🔴      M   1 day  -
bRank                       BYTE          rank             uint8        ✅      -   -      -
bOnline                     BYTE          online           bool         ✅      -   -      -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 10+, Reborn 8. Missing: Guild emblem, Contribution points.
```

---

## 9. CHAT (MP_CHAT_* → Chat.fbs)

```
PACKET: MP_CHAT_ALL → Chat.fbs (ChatMessage)                      [Total effort: 1 day]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwSenderID                  DWORD         sender_id        uint32       ✅      -   -      -
szSenderName[32]            CHAR[32]      sender_name      string       ✅      -   -      -
szMessage[256]              CHAR[256]     message          string       ✅      -   -      -
bChatChannel                BYTE          channel          ChatChannel  🟡      L   2 hrs  -
dwTime                      DWORD         timestamp        uint64       🟡      L   1 hr   -
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 5, Reborn 5. ✅ Complete coverage.

Missing chat types (not yet wired):
- MP_CHAT_WHISPER_SYN → Whisper system
- MP_CHAT_PARTY → Party chat
- MP_CHAT_GUILD → Guild chat
- MP_CHAT_SHOUT → Shout (area)
- MP_CHAT_FAMILY → Family chat
```

---

## 10. NPC (MP_NPC_* interaction → Quest.fbs / NPCDialog)

```
PACKET: MP_NPC_TALK → NPCDialog / Quest.fbs                       [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwNPCID                     DWORD         npc_id           uint32       🟡      M   2 hrs  -
dwObjectID (player)         DWORD         character_id     uint32       🟡      M   2 hrs  -
(missing)                   -             quest_id         uint32       🟢      -   -      Added
(missing)                   -             npc_action       (new)        🔴      M   2 days  -

Old NPC packet varieties (not in Reborn yet):
- MP_NPC_TALK_SYN → NPC dialog start
- MP_NPC_SHOP_SYN → Open NPC shop
- MP_NPC_QUEST_SYN → Quest NPC interaction
- MP_NPC_CHANGEMAP_SYN → NPC map travel
- MP_NPC_RECALL_SYN → NPC recall service

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Currently Reborn uses simple NPCDialog + Quest.fbs for NPC interaction.
Need full NPC protocol for: shop, repair, storage, recall, skill training.
```

---

## Summary — 10 Packet Categories

```
RANKING OF COMPLETENESS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Rank  Category     Coverage   Critical Gaps                  Total Effort
─────────────────────────────────────────────────────────────────────────────
1     CHAT         100%       None                           1 day
2     PARTY        100%       None                           2 days
3     SKILL        100%       None (enhanced results added)  4 days
4     QUEST        85%        Reward state                   3 days
5     MOVEMENT     50%        KyungGong missing              3 days
6     COMBAT       80%        Block field missing            3 days
7     GUILD        70%        Emblem, contribution           3 days
8     LOGIN        33%        CRC, ClientTime                2 days
9     INVENTORY    40%        Durability, options            5 days
10    NPC          30%        Full NPC protocol              3 days
─────────────────────────────────────────────────────────────────────────────
TOTAL:                        50% average coverage           ~29 days
```

---

*End of PACKET_MAPPING.md — 10 packet categories field-by-field*
