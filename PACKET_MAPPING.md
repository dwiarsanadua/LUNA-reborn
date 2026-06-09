# PACKET MAPPING — Field-by-Field (10 Packet Categories) v2

> Old struct → Reborn FlatBuffers schema
> Status: ✅ **95% field-by-field mapped** (update setelah 50 agent prompt)

---

## 1. LOGIN (MP_USERCONN_LOGIN_SYN → Login.fbs)

```
PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)         [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwUserID[48]                DWORD[48]     username          string       ✅      -   -      -
szPassword[32]              CHAR[32]      password_hash     [uint8]      ✅      -   -      Crypto
dwVersion                   DWORD         client_version    string       ✅      -   -      -
dwCRC                       DWORD         file_crc          uint32       ✅      -   -      Agent #050
bUseNProtect                BYTE          (removed)         -            🔴      L   -      NProtect removed
dwClientTime                DWORD         client_timestamp  uint32       ✅      -   -      Agent #050
(missing)                   -             mac_address       string       🟢      -   -      Added field
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 6, Reborn 6. Match: 5/6 (83%). Added: 1 new (mac_address).
Removed: bUseNProtect (tidak relevan).
```

**Struct Old:**
```cpp
struct LOGIN_SYN {
    MSGROOT header;
    char dwUserID[48];
    char szPassword[32];
    DWORD dwVersion;
    DWORD dwCRC;
    BYTE bUseNProtect;
    DWORD dwClientTime;
};
```

**Schema Reborn:**
```fbs
table LoginRequest {
    username: string (required);
    password_hash: [uint8] (required);
    client_version: string;
    file_crc: uint32;
    client_timestamp: uint32;
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
CurPos.wx/wz                WORD x 2      direction        Vec3         ✅      -   -      -
MoveMode                    eMoveMode     move_mode        MoveMode     ✅      -   -      -
KyungGongIdx                WORD          kyung_gong_idx   uint16       ✅      M   1 day  Agent #001
AddedMoveSpeed              float         added_move_speed float        ✅      L   2 hrs  -
(missing)                   -             target_position  Vec3         🟢      -   -      Added
(missing)                   -             timestamp        uint64       🟢      -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 4, Reborn 6. Match: 4/4 (100%). Added: 2 new (target_position, timestamp).
```

**Struct Old:**
```cpp
struct SEND_MOVEINFO {
    COMPRESSEDPOS CurPos;
    eMoveMode MoveMode;
    WORD KyungGongIdx;
    float AddedMoveSpeed;
};
```

**Schema Reborn:**
```fbs
table MoveRequest {
    direction: Vec3;
    target_position: Vec3;
    move_mode: MoveMode;
    kyung_gong_idx: uint16 = 0;
    added_move_speed: float = 0.0;
}
table EntityMove {
    entity_id: uint32;
    position: Vec3;
    target: Vec3;
    speed: float;
    animation: string;
    timestamp: uint64;
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
dwSkillID (0=auto)          DWORD         skill_id         uint16       ✅      -   -      -
(missing)                   -             position         Vec3         🟢      -   -      Added

AttackResult:
─────────────────────────────────────────────────────────────────────────────────────────────────────
damage                      int            damage           int32        ✅
isCritical                  BOOL           is_critical      bool         ✅
isBlocked                   BOOL           is_blocked       bool         ✅      M   4 hrs  Agent #002
isMiss                      BOOL           is_miss          bool         ✅
damageType                  BYTE           damage_type      DamageType   ✅
(missing)                   -              target_hp_remaining int32     🟢      -   -      Added
(missing)                   -              effects          [DamageEffect] 🟢   -   -      Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 5, Reborn 7. Match: 5/5 (100%). Added: 2 new (target_hp, effects).
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
Total fields: Old 3, Reborn 6. Match: 3/3 (100%). Added: 3 new (enhanced result data).
```

---

## 5. INVENTORY (MP_ITEM_* → Inventory.fbs)

```
PACKET: MP_ITEM_MOVE_SYN → Inventory.fbs (InventoryMoveRequest)   [Total effort: 5 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwObjectID                  DWORD         (implied)        -            🟡      -   -      -
bySrcSlot                   BYTE          src_slot         uint8        ✅      -   -      -
byDstSlot                   BYTE          dst_slot         uint8        ✅      M   2 hrs  Agent #009
wSrcStorageType             WORD          src_storage_type uint8        ✅      M   2 hrs  Agent #009
wDstStorageType             WORD          dst_storage_type uint8        ✅      M   2 hrs  Agent #009
dwItemDBIdx                 DWORD         item_id          uint32       ✅      -   -      -
byCount                     BYTE          count            uint16       ✅      -   -      -

InventorySlot (per item):
─────────────────────────────────────────────────────────────────────────────────────────────────────
bySlot                      BYTE          slot_index       uint8        ✅
dwItemDBIdx                 DWORD         item_id          uint32       ✅
byCount                     BYTE          count            uint16       ✅
wDurability                 WORD          durability       uint16       ✅      M   2 hrs  Agent #009
bEnchantLevel               BYTE          enchant          uint8        ✅
(banyak field option)       (varies)      TB_ITEM_OPTION   (DB table)   ✅
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: 7 per item. Match: 7/7 (100%).
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
cState                      BYTE          is_completed     bool         ✅      -   -      -
cProgress[10]               BYTE[10]      objectives       [QuestObjectiveState] ✅  -
(missing)                   -             is_reward_taken  bool         🟢      -   -      Added

QuestStartRequest:
dwNPCID                     DWORD         npc_id           uint32       ✅
dwQuestID                   DWORD         quest_id         uint32       ✅
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 4, Reborn 5+. Good coverage.
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
dwCharID                    DWORD         character_id     uint32       ✅
szCharName[32]              CHAR[32]      name             string       ✅
wLevel                      WORD          level            uint16       ✅
dwHP / dwMaxHP              DWORD x 2     hp / max_hp      int32 x 2    ✅
wMapID                      WORD          map_id           uint16       ✅
bLeader                     BOOL          is_leader        bool         ✅
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
dwGuildID                   DWORD         guild_id         uint32       ✅
szGuildName[32]             CHAR[32]      name             string       ✅
wLevel                      WORD          level            uint8        ✅
dwGP                        DWORD         gp               uint32       ✅
dwGuildPoint                DWORD         guild_point      uint32       ✅      M   1 day  Agent #011
szGuildMark[256]            CHAR[256]     guild_mark       string       ✅      M   2 days Agent #011
dwMasterID                  DWORD         master_id        uint32       ✅
dwMemberCount               DWORD         (implied)        -            🟡      -   -      -
bRank                       BYTE          rank             uint8        ✅
bOnline                     BYTE          online           bool         ✅
(missing)                   -             members          [GuildMemberInfo] 🟢  -   -    Added
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 9+, Reborn 10+. Complete coverage. ✅
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
bChatChannel                BYTE          channel          ChatChannel  ✅      -   -      -
dwTime                      DWORD         timestamp        uint64       ✅      L   1 hr   Agent #010

Extended chat schemas (BARU → agent #010):
WhisperMessage    → sender_id, sender_name, receiver_id, receiver_name, message, timestamp
PartyChatMessage  → sender_id, sender_name, message, member_ids, timestamp
GuildChatMessage  → sender_id, sender_name, guild_id, message, timestamp
FamilyChatMessage → sender_id, sender_name, family_id, message, timestamp
ShoutMessage      → sender_id, sender_name, message, item_id, item_pos, timestamp
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 5, Reborn 5+ per chat type. ✅ Complete coverage.
```

---

## 10. NPC (MP_NPC_* interaction → NPC.fbs + Quest.fbs)

```
PACKET: MP_NPC_TALK → NPC.fbs (NpcRequest)                        [Total effort: 3 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field                   Type (Old)    Reborn Field     FBS Type     Status  Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────────────
dwNPCID                     DWORD         npc_id           uint32       ✅      -   -      -
dwObjectID (player)         DWORD         character_id     uint32       ✅      -   -      -
(missing)                   -             action           NpcAction     🟢     -   -      Added
(missing)                   -             quest_id         uint32       🟢      -   -      Added

NpcAction enum (BARU → agent #005):
  Talk = 0, Shop = 1, Quest = 2, ChangeMap = 3, Recall = 4

NpcResponse (comprehensive response):
  result, npc_id, action, dialog_text, shop_items, destinations, recall_destinations, quest_ids

Old NPC packet varieties — semua sudah di-cover oleh NPC.fbs:
  MP_NPC_TALK_SYN      → NpcRequest(action=Talk)
  MP_NPC_SHOP_SYN      → NpcRequest(action=Shop) + NpcShopItem[]
  MP_NPC_QUEST_SYN     → NpcRequest(action=Quest) + quest_ids[]
  MP_NPC_CHANGEMAP_SYN → NpcRequest(action=ChangeMap) + NpcChangeMapDestination[]
  MP_NPC_RECALL_SYN    → NpcRequest(action=Recall) + NpcRecallDestination[]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total fields: Old 2, Reborn 4+ per action. ✅ Complete coverage with new NPC.fbs.
```

---

## Summary — 10 Packet Categories

```
RANKING OF COMPLETENESS (Post-Adaptation)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Rank  Category     Coverage   Sebelum          Sesudah           Total Effort
─────────────────────────────────────────────────────────────────────────────
1     CHAT         100%       100%             100%              1 day
2     PARTY        100%       100%             100%              2 days
3     SKILL        100%       100%             100%              4 days
4     NPC          100%       30%              100%  (BARU)     3 days
5     GUILD        100%       70%              100%  (fixed)    3 days
6     MOVEMENT     100%       50%              100%  (fixed)    3 days
7     COMBAT       100%       80%              100%  (fixed)    3 days
8     LOGIN        83%        33%              83%   (fixed)    2 days
9     INVENTORY    100%       40%              100%  (fixed)    5 days
10    QUEST        100%       85%              100%              3 days
─────────────────────────────────────────────────────────────────────────────
TOTAL:                        50% average     98% average      ~29 days
─────────────────────────────────────────────────────────────────────────────
✅ 9/10 kategori 100% complete. Login 83% (bUseNProtect removed).
```

---

## Remaining Protocol Work

Dari total 103 MP_CATEGORY di Old, ~30 kategori game sudah memiliki schema .fbs:

```
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛ 10 kategori game utama → 98% mapped ✅
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛ 20 kategori game sekunder → 50% mapped 🟡
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛ 40 kategori admin/monitoring → 0% (not planned)
⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛ 33 kategori legacy/anti-cheat → 0% (removed)
```

Prioritas ke depan:
1. Wire handler untuk 10 kategori utama (client ↔ server routing)
2. Implement packet handler untuk 20 kategori sekunder
3. Tambah server-side validation untuk anti-cheat mitigation

---

*End of PACKET_MAPPING.md v2 — 10 packet categories verified*

