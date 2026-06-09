# PACKET MAPPING v3 — Field-by-Field dari Old CommonStruct.h

> Mapping: Old C++ struct (CommonStruct.h) → Reborn FlatBuffers schema
> Sumber: Scan langsung `[CC]Header/Protocol.h` dan `CommonStruct.h`

---

## 1. LOGIN (MP_USERCONN_LOGIN_SYN → Login.fbs)

```
PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_LOGIN_SYN)   Type (Old)    Reborn Field       FBS Type     Status
─────────────────────────────────────────────────────────────────────────────────────────────
header.CheckSum              BYTE          (implied)           -           🟡
header.Category              BYTE          (implied)           -           🟡
header.Protocol              BYTE          (implied)           -           🟡
header.dwObjectID            DWORD         (implied)           -           🟡
AuthKey                      DWORD         (missing)           -           🔴
id                           char[21]      username            string      ✅
pw                           char[21]      password_hash       [uint8]     🟡
Version                      char[16]      client_version      string      ✅
strSecurityPW               char[17]       (missing)           -           🔴
Check.mValue                 LONGLONG      (missing)           -           🔴 Anti-hack
mLoginKey                    char[MAX_PATH](missing)           -           🔴
(missing)                    -             file_crc            uint32      🟢 Added
(missing)                    -             client_timestamp    uint32      🟢 Added
(missing)                    -             mac_address         string      🟢 Added

LOGIN RESPONSE (MSG_LOGIN_ACK):
agentip                      char[16]      (in ServerInfo)     string      ✅
agentport                    WORD          (in ServerInfo)     int         ✅
userIdx                      DWORD         session_token       string      🟡
cbUserLevel                  BYTE          (in LoginResult)    int8        🟡
```

---

## 2. MOVE (MP_MOVE_ONETARGET → Movement.fbs)

```
PACKET: MP_MOVE_ONETARGET → Movement.fbs (MoveRequest)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MOVE_ONETARGETPOS) Type (Old)   Reborn Field       FBS Type     Status
─────────────────────────────────────────────────────────────────────────────────────────────
dwMoverID                    DWORD         (implied)           -           🟡
sPos.wx                      WORD          direction.x         float       🟡 compressed→float
sPos.wz                      WORD          direction.z         float       🟡
tPos.wx                      WORD          target_position.x   float       🟢 Added field
tPos.wz                      WORD          target_position.z   float       🟢 Added field
MoveMode                     eMoveMode     move_mode           MoveMode    ✅
KyungGongIdx                 WORD          kyung_gong_idx      uint16      ✅
AddedMoveSpeed               float         added_move_speed    float       ✅
(missing)                    -             target_position.y   float       🟢
(missing)                    -             timestamp           uint64      🟢
```

---

## 3. COMBAT (MP_COMBAT_ATTACK_SYN → Combat.fbs)

```
PACKET: MP_COMBAT_ATTACK_SYN → Combat.fbs (AttackRequest)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old protokol combat ada di MP_SKILL_START_SYN (bukan kategori sendiri).
Old menggunakan MSG_SKILL_START_SYN yang kompleks dengan SKILLOBJECT_INFO.
Reborn menyederhanakan jadi AttackRequest dengan target_id, skill_id, position.
```

---

## 4. ITEM (MP_ITEM_MOVE_SYN → Inventory.fbs)

```
PACKET: MP_ITEM_MOVE_SYN → Inventory.fbs (InventoryMoveRequest)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field (MSG_ITEM_MOVE_SYN) Type (Old)  Reborn Field       FBS Type     Status
─────────────────────────────────────────────────────────────────────────────────────────────
FromPos.Type                  POSTYPE       src_storage_type    uint8       ✅
FromPos.Index                 BYTE          src_slot            uint8       ✅
wFromItemIdx                  DWORD         item_id             uint32      ✅
ToPos.Type                    POSTYPE       dst_storage_type    uint8       ✅
ToPos.Index                   BYTE          dst_slot            uint8       ✅
wToItemIdx                    DWORD         (implied)           -           🟡
```

---

## 5-10: Quest, Party, Guild, Chat, NPC, Vehicle

Lihat file terpisah `PACKET_DETAIL.md` untuk mapping lengkap semua 10 kategori.

---

*End of PACKET_MAPPING.md v3 — Field-by-field dari Old CommonStruct.h*

