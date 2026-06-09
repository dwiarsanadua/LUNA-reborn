# Prompt Analisa Luna Old vs Reborn — Spesifikasi Adaptasi Detail

## Instruksi Global

Anda adalah AI arsitek yang bertugas melakukan **analisa gap menyeluruh** antara dua codebase: **Luna-Plus-Old** (codebase legacy/production) dan **Luna-Plus-Reborn** (rewrite modern). Tujuan akhir: menghasilkan **spesifikasi adaptasi** yang memetakan SETIAP komponen Old ke implementasi Reborn, dengan prioritas dan detail teknis.

Anda harus:
1. Scan SELURUH direktori kedua codebase secara paralel
2. Cocokkan file-by-file, class-by-class, function-by-function
3. Output tabel gap per layer SERINCI MUNGKIN
4. Beri rekomendasi teknologi modern yang COCOK (bukan sekedar trendy)

---

## A. Lokasi Codebase

```
OLD:  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old
REBORN: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
```

---

## B. Layer yang Harus Dianalisa (WAJIB semua)

### B1. Player Flow (Alur Utama)
Old punya alur: Launcher → Login → Char Select → Game World → Combat/Social
Untuk SETIAP screen:
- File Old apa yang mengimplementasikan?
- File Reborn apa yang mengimplementasikan?
- Class/method mana yang handle render, input, network packet?
- Apa yang MISSING di Reborn?
- Apa yang sudah ada tapi belum benar?

### B2. UI System (Window/Dialog)
Old punya ~81+ dialog window dijalankan via `cWindowManager` dengan script `.bin`.
Untuk SETIAP dialog:
- Nama file .bin di Old
- Apakah .bin.txt sudah di-parse di Reborn? (cek UiScriptParser)
- Apakah C++ dialog class sudah dibuat? (cek client/ui/dialogs/*.cpp)
- Fungsi apa yang MISSING? (render, click handler, drag, hotkey, tooltip)
- Mapping: Old .bin field → Reborn widget

### B3. Gameplay Constants & Hero System
Old punya Hero.cpp (~2.300 lines) dengan:
- Combat formula (damage, critical, block, miss, element advantage)
- Battle delay / attack speed
- Aggro system
- Buff/debuff stacking
- Movement + pathfinding
- PK mode
- Combo system

Untuk SETIAP fungsi di Hero.cpp Old:
- Apakah ada di game/ecs/systems/CombatSystem.cpp?
- Apakah constant numeric (damage multiplier, delay, range) SAMA?
- Apa yang berbeda?
- Apa yang MISSING?

### B4. Network Protocol
Old punya 94 MP_CATEGORY packet types.
Reborn punya ~27 FlatBuffers schema.

Untuk SETIAP packet category (contoh: MP_USERCONN_*, MP_MOVE_*, MP_COMBAT_*):
- Nama packet di Old (dari server/ atau client/ network headers — cari enum MP_* di .h files)
- Nama schema .fbs di Reborn (dari game/network/protocol/)
- Apakah handler sudah di-wire? (cek client + server)
- **Field-by-field**: Baca struct Old (dari protocol headers) dan schema Reborn (.fbs) secara SIDE-BY-SIDE.
  Output tabel:
  ```
  PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Old Field          Type (Old)       Reborn Field       Type (FBS)     Match?
  ─────────────────────────────────────────────────────────────────────────────
  dwUserID           DWORD[48]        username           string          ✅
  szPassword         CHAR[32]         password_hash      [uint8]         🟡 Hash vs plain
  dwVersion          DWORD            version            int             ✅
  dwCRC              DWORD            (missing)           -              🔴 Missing
  bUseNProtect       BYTE             (missing)           -              🔴 Missing
  dwClientTime       DWORD            (missing)           -              🔴 Missing
  ```
- Catat field yang ada di Old tapi MISSING di Reborn
- Catat field yang ada di Reborn tapi TIDAK di Old (redundan)

### B5. Asset & Data Pipeline
Old punya format proprietary:
- `.mod` (3D model) → Reborn: `.glb` / `.obj` ✅
- `.anm` (animation) → Reborn: `.anm.json` ✅
- `.dds` / `.tif` (texture) → Reborn: `.png` ✅
- `.hfl` (heightmap) → Reborn: `.hgt` ✅
- `.chx` (character def) → Reborn: `.json` ✅
- `.map` (scene) → Reborn: `scene.json` ✅

Untuk asset: verifikasi bahwa asset Reborn mencakup SEMUA yang Old miliki.
Cari asset yang ada di Old tapi TIDAK di Reborn.

### B6. Server Systems
Old server (baca dari code/server/ di Old):
- AgentServer (auth)
- MapServer (world simulation)
- DistributeServer (load balancing)

Untuk SETIAP server system:
- Old path & file → Reborn path & file
- Logic apa yang sudah di-port?
- Logic apa yang MISSING?

### B7. Database Schema (MSSQL vs SQLite/PostgreSQL)
Old: MSSQL dengan ~50+ stored procedures di `database/schema_game.sql`
Reborn: SQLite (dev) + PostgreSQL (prod) di `database/` folder

Untuk SETIAP stored procedure / query:
- Nama stored procedure di Old
- Apakah ada equivalent di Reborn? (SQL query langsung atau function)
- **Field-by-field**: bandingkan kolom dan tipe data antara Old dan Reborn
- Output tabel:
```
STORED PROCEDURE: USP_CHARACTER_LOAD
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (MSSQL)                    Reborn (SQLite)                    Match?
─────────────────────────────────────────────────────────────────────────────
SELECT c.char_id,               SELECT id, name, level,            🟡 Partial
       c.char_name,              str, dex, con, int, wis,
       c.level,                  hp, mp, exp, map_id,
       c.str, c.dex, c.con,     pos_x, pos_y, pos_z,
       c.int, c.wis,             gold
       c.hp, c.mp, c.exp,
       c.map_id, c.pos_x,
       c.pos_y, c.pos_z,
       c.gold, c.battle_style   (missing)                          🔴 Missing
FROM TB_CHARACTER c
WHERE c.char_id = @char_id
```
- Catat kolom/query yang ada di Old tapi MISSING di Reborn
- Catat stored procedure yang belum ada equivalent-nya

### B8. Build System & Project Structure
Old: Visual Studio 2003 (.sln + .dsp project files)
Reborn: CMake 3.30+ (CMakeLists.txt + CMakePresets.json)

Bandingkan:
- Struktur project Old (.sln) → CMakeLists.txt Reborn (target, dependencies, link libraries)
- Target apa yang ada di Old tapi tidak di Reborn? (contoh: MapEditor, Launcher.exe terpisah)
- Dependency apa yang Old pakai (DirectX 9 SDK, MFC, MSSQL) vs Reborn (bgfx, glfw, asio, sqlite3)
- Build step: Old pakai Visual Studio build → Reborn pakai Ninja/Make
- Apakah semua library dependency Old sudah diganti dengan equivalent modern?

### B9. Error Handling & Edge Cases
Untuk SETIAP layer, cek:
- Apakah Reborn handle error cases yang sama dengan Old?
  Contoh: koneksi putus, packet corrupt, loading gagal, file tidak ditemukan
- Apakah Old punya retry logic, fallback, atau error dialog yang tidak di-port?
- Output:
```
ERROR HANDLING: Network Disconnect
  Old:     Reconnect dialog dengan countdown 10s, lalu balik ke login
  Reborn:  Log "disconnected", user harus restart client
  Gap:     🔴 Missing — tidak ada reconnection flow
```

### B10. Performance Comparison
Untuk SETIAP layer yang punya benchmark potensial:
- Render: FPS Old vs Reborn (Old: DX9 ~60fps di 1024x768)
- Network: Packet throughput, latency
- Loading: Waktu startup, map load, asset preload
- Memory: RAM usage, texture memory
- Output:
```
PERFORMANCE: Map Loading (map 51)
  Old:     ~1.2s (DirectX 9, direct .mod load)
  Reborn:  ~0.3s (bgfx Metal, GLB format)
  Assessment: ✅ 4x lebih cepat — GLB lebih efisien dari .mod binary
```

### B11. Security & Anti-Cheat
Old: HackShield + nProtect (anti-cheat), packet encryption (XOR/RC4), periodic integrity check
Reborn: PacketCrypto (AES-GCM), server-side validation (no anti-cheat client)

Bandingkan:
- **Authentication flow**: Old vs Reborn — apakah token-based? session expiry? brute-force protection?
- **Packet encryption**: Old pake XOR/RC4 sederhana → Reborn AES-GCM. Apakah sudah equivalent?
- **Anti-cheat**: Old punya HackShield + nProtect di client. Reborn tidak ada. Apa mitigasi pengganti? (server-side validation? rate limiting?)
- **SQL injection protection**: Old concatenate query (raw), Reborn perlu parameterized query. Cek apakah sudah aman.
- **Memory protection / CRC check**: Old berkala cek integritas memory. Reborn??
- Output tabel:
```
SECURITY: Authentication
  Old:     Plaintext password + XOR token + session 24 jam
  Reborn:  Plaintext password + AES-GCM token + session 24 jam
  Status:  🟡 Partial — password masih plaintext, token encryption more secure

SECURITY: Anti-Cheat
  Old:     HackShield + nProtect (kernel-level)
  Reborn:  (none — server-side validation only)
  Status:  🔴 Missing — perlu rate limiting + anomaly detection di server

SECURITY: SQL Injection
  Old:     String concatenation: "SELECT ... WHERE id='" + username + "'"
  Reborn:  Parameterized query (SQLite prepared statement)
  Status:  ✅ Lebih aman — prepared statement prevent injection
```

### B12. Concurrency & Threading Model
Old: Win32 threads + IOCP (I/O Completion Ports) untuk network, critical section untuk shared data
Reborn: asio (asio::io_context) untuk network, std::mutex/std::atomic untuk shared data

Bandingkan:
- **Network I/O model**: Old IOCP (Windows-only) vs Reborn asio (cross-platform). Apakah asio config sudah optimal?
- **Thread pool**: Old punya thread pool untuk DB/job. Reborn? (cek server/ agent/map thread config)
- **Shared state protection**: Old pake critical section + mutex. Reborn pake std::mutex + std::atomic. Coverage sama?
- **Deadlock risk**: Apakah ada potensi deadlock di locking order?
- **Render thread**: Old single thread (Win32 message loop). Reborn single thread (GLFW loop). Sama.
- Output:
```
CONCURRENCY: Network I/O
  Old:     IOCP (I/O Completion Ports) — 4 worker threads
  Reborn:  asio::io_context — 4 worker threads
  Status:  ✅ Cross-platform equivalent of IOCP. Performance should be comparable.

CONCURRENCY: Database
  Old:     Separate DB thread pool (3-5 threads) with MSSQL
  Reborn:  SQLite with WAL mode, single writer
  Status:  🟡 SQLite single-writer bisa jadi bottleneck dibanding MSSQL thread pool.
           Saran: tetap pakai SQLite untuk dev, PostgreSQL untuk prod (multi-writer).
```

### B13. Localization (i18n)
Old: Multi-language support (Korean, English, Japanese, Chinese) via string tables + .bin UI scripts per language
Reborn: `Localization.cpp` + `UiStringTable` parser (`InterfaceMsg.bin.txt`)

Verifikasi:
- **String coverage**: Old punya ribuan string ID di `InterfaceMsg.bin`. Reborn sudah load 1624 strings (dari log). Berapa total di Old?
- **Language switching**: Old bisa ganti bahasa runtime. Reborn? (cek Localization::SetLanguage)
- **Font support**: Old pakai per-language font. Reborn pakai 2002_EYA.ttf. Support CJK characters?
- **UI script per language**: Old punya .bin terpisah per bahasa. Reborn??
- Output:
```
LOCALIZATION: String Table
  Old:     InterfaceMsg.bin — ~2000+ string IDs (KR, EN, JP, CN)
  Reborn:  UiStringTable — loaded 1624 strings
  Status:  🟡 Partial — coverage ~80%, perlu verifikasi string yang missing

LOCALIZATION: Font
  Old:     Per-language font files (korean.ttf, english.ttf, etc.)
  Reborn:  2002_EYA.ttf (single font)
  Status:  🔴 CJK characters might not render correctly with single font.
           Saran: bundle font untuk EN + KR minimal.
```
Old: Miles Sound System
Reborn: miniaudio

Verifikasi:
- Apakah semua format audio (.wav, .mp3, .ogg) di-support?
- Apakah BGM crossfade?
- Apakah SFX positional (3D)?
- Apakah daftar sound Old (dari SoundList.bin.txt) sudah di-load?

### B15. Audio System
Old: Miles Sound System
Reborn: miniaudio

Verifikasi:
- Apakah semua format audio (.wav, .mp3, .ogg) di-support?
- Apakah BGM crossfade?
- Apakah SFX positional (3D)?
- Apakah daftar sound Old (dari SoundList.bin.txt) sudah di-load?
- Output tabel dengan severity + effort + dependencies

### B16. Physics & Collision
Old: collision system proprietary
Reborn: Jolt Physics (optional/stub)

Untuk SETIAP fungsi collision di Old:
- Apakah ada di Reborn?
- Apakah stub atau implementasi nyata?

---

## C. Format Output WAJIB

### C1. Tabel Gap Per Layer (WAJIB sertakan Severity, Effort, Dependencies)

Setiap baris/entri WAJIB memiliki kolom Severity, Effort, dan Dependencies.

Contoh format:

```
LAYER: UI / Login Dialog
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old File / Func              Reborn File             Status    Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
interface/                   client/screens/         🟡       H    2 days   Skin textures
WebLauncherIDPass.bin        LoginScreen.cpp          Partial
  ├─ Window frame            UiSkinManager::         🔴       H    4 hrs    close.png,
  │                          DrawWindow()             Missing           min.png
  ├─ ID EditBox              InputField struct        ✅       -    -        -
  ├─ PW EditBox (masked)     InputField.              ✅       -    -        -
  │                          masked=true
  ├─ OK Button               UiSkinManager::          🔴       M    1 hr     close.png
  │                          DrawButton()             Missing
  ├─ Cancel Button           (not implemented)        🔴       M    1 hr     OK button
  └─ Save ID Checkbox        (not implemented)        🔴       L    2 hrs    CheckBox widget

LAYER: GAMEPLAY / Hero Combat
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Function                 Reborn Equivalent       Status    Sev  Effort   Dependencies
────────────────────────────────────────────────────────────────────────────────────────────
Hero::CalcDamage()           CombatSystem::          🟡       C    3 days   CombatSystem
                             CalculateDamage()        Different         refactor
  ├─ Base damage             atk * skill_power        ✅       -    -        -
  ├─ Crit chance =           stat.crit_rate / 100     ❌       M    4 hrs    -
  │  DEX/1000                                         Berbeda
  ├─ Element advantage       element_multiplier       🔴       H    2 hrs    Damage calc
  │  1.3x                                            Missing
  ├─ Level gap penalty       level_scaling            🟡       L    2 hrs    -
  └─ Block chance =          (not implemented)        🔴       M    4 hrs    CombatSystem
    CON/2000                                          Missing
```

Severity: C = Critical (blocking), H = High (major feature), M = Medium (nice to have), L = Low (cosmetic)
Effort: hours/days/weeks
Dependencies: apa yang harus selesai SEBELUM item ini bisa dikerjakan

### C2. Summary per Layer

Setelah tabel detail, beri ringkasan. WAJIB sertakan total effort estimate.

```
SUMMARY - UI SYSTEM
  Total Old dialogs:          81
  Reborn C++ classes:         37 (45%)
  Fully functional:           12 (15%)
  Partial/stub:               25 (30%)
  Missing:                    44 (55%)
  Total estimated effort:     ~6 weeks (1 FTE)
  
  Priority fix (top 5, with effort):
  1. Skin textures (close.png, min.png) — 2 days — blocker untuk semua window
  2. OK/Cancel button wiring — 1 day — setelah skin textures
  3. Drag window by title bar — 2 days — setelah WindowManager refactor
  4. Tooltip on hover — 3 days — setelah widget mouse events
  5. Keyboard navigation (Tab, Enter, Esc) — 2 days — input system
```

### C3. Rekomendasi Teknologi

Untuk SETIAP layer, beri rekomendasi:
- Teknologi yang dipakai Old (contoh: Win32 EditBox)
- Teknologi yang dipakai Reborn saat ini (contoh: UIRenderer batch)
- Apakah teknologi Reborn SUDAH COCOK? Jika belum, rekomendasi alternatif

Contoh:
```
LAYER: UI Rendering
  Old:     Windows GDI + DirectX 9 (D3DXSprite)
  Reborn:  bgfx + UIRenderer (batch system)
  Assessment: ✅ COCOK — batch system lebih efisien dari GDI sprite.
              Saran: tambah scissor test untuk clipping (sudah diimplementasikan).
```

```
LAYER: Input
  Old:     Windows message loop (WM_KEYDOWN, WM_CHAR)
  Reborn:  GLFW callback + InputSystem
  Assessment: ✅ COCOK — GLFW cross-platform, lebih modern dari Win32 message loop.
              Saran: tambah gamepad/joystick support via GLFW gamepad API.
```

### C4. Field-by-Field Packet Mapping (WAJIB)

Untuk 10 packet category paling penting (Login, Move, Combat, Skill, Inventory, Quest, Party, Guild, Chat, NPC), output tabel:

```
PACKET: MP_USERCONN_LOGIN_SYN → Login.fbs (LoginRequest)    [Total effort: 2 days]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old Field             Type (Old)    Reborn Field   FBS Type   Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────────
dwUserID[48]          DWORD[]       username       string     ✅     -   -       -
szPassword[32]        CHAR[]        password_hash  [uint8]    🟡     M   4 hrs   -
dwVersion             DWORD         version        int        ✅     -   -       -
dwCRC                 DWORD         (missing)      -          🔴     L   1 hr    -
bUseNProtect          BYTE          (missing)      -          🔴     L   -       NProtect removed
dwClientTime          DWORD         (missing)      -          🔴     L   30 min  -
```

### C5. Database Query Comparison

```
STORED PROC: USP_CHARACTER_LOAD               [Total effort: 1 day]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Old (MSSQL)             Reborn (SQLite)        Status   Sev  Effort  Deps
─────────────────────────────────────────────────────────────────────────────────────────
char_id                 id                     ✅       -    -       -
char_name               name                   ✅       -    -       -
level                   level                  ✅       -    -       -
str, dex, con, ...      str, dex, con, ...     ✅       -    -       -
hp, mp, exp             hp, mp, exp            ✅       -    -       -
map_id                  map_id                 ✅       -    -       -
pos_x, pos_y, pos_z,    pos_x, pos_y,          ✅       -    -       -
gold                    pos_z, gold
battle_style            (missing)              🔴       L    2 hrs   Schema update
---------------------------------------------------------------
Total match:            14/15 columns (93%)
```

### C6. Build System Comparison

```
BUILD TARGET: LunaPlusClient (Reborn) → Game.exe (Old)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Aspek                Old (VS2003)       Reborn (CMake)     Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────
Compiler             MSVC 7.1           AppleClang 16       ✅    -   -      -
Renderer             DirectX 9 SDK      bgfx (Metal)        ✅    -   -      -
Window               Win32 + MFC        GLFW                ✅    -   -      -
Audio                Miles Sound Sys    miniaudio           ✅    -   -      -
Physics              Proprietary        Jolt (optional)     🟡    L   1 day  -
Script               Lua (custom host)  sol2/luajit         ✅    -   -      -
Database             MSSQL + ADO        SQLite+PostgreSQL   ✅    -   -      -
UI Framework         cWindowManager     WindowManager+UIR   ✅    -   -      -
Anti-cheat           HackShield+nProt   (not ported)        🔴    H   2 wks  -
```

### C7. Error Handling Comparison

```
ERROR HANDLING                            Status   Sev  Effort Deps
─────────────────────────────────────────────────────────────────────
Network Disconnect:
  Old:   Reconnect dialog 10s → retry     🔴       H    3 days  NetworkSystem
  Reborn: Log + user must restart client  Missing
Asset Load Failure:
  Old:   Fallback texture + log           🟡       M    1 day   TextureManager
  Reborn: Log + BGFX_INVALID_HANDLE       Partial
```

### C8. Performance Comparison

```
PERFORMANCE                            Measurement              Status Sev Effort Deps
─────────────────────────────────────────────────────────────────────────────────────────
Map Load (map 51):
  Old:  ~1.2s (DX9, .mod from PAK)    1.2s vs 0.3s             ✅     -   -      -
  Reborn: ~0.3s (bgfx Metal, GLB)                              4x faster
Login Screen FPS:
  Old:  60 FPS (capped)               60 vs 1800                🟡     L   2 hrs  -
  Reborn: 1800 FPS (no cap)                                   Need frame cap
```

---

## D. Prioritas Analisa

Kerjakan dalam urutan ini:
1. **Player Flow** (paling penting — menentukan arsitektur)
2. **UI System** (paling banyak gap — 81 dialog)
3. **Gameplay Constants** (Hero, combat, aggro)
4. **Network Protocol + Field-by-field Packet** → WAJIB field-by-field untuk 10 packet category paling penting (Login, Move, Combat, Skill, Inventory, Quest, Party, Guild, Chat, NPC)
5. **Server Systems** (agent, map, distribute)
6. **Database Schema** (MSSQL stored procedures vs SQLite)
7. **Build System** (VS2003 .sln vs CMake)
8. **Security & Anti-Cheat** (auth, encryption, anti-tamper)
9. **Asset Pipeline** (verifikasi coverage)
10. **Error Handling & Edge Cases**
11. **Concurrency & Threading** (IOCP vs asio, thread pool, deadlock)
12. **Localization (i18n)** (string coverage, font, language switching)
13. **Performance Comparison** (jika ada data)
14. **Audio & Physics** (opsional, gap kecil)

---

## E. Output Files

Buat 4 file output di `/Users/macbookair/PRIBADI/luna-plus-master/`:

1. **GAP_ANALYSIS.md** — Tabel gap lengkap semua layer (Player Flow, UI, Gameplay, Network, Server, Database, Build, Security, Concurrency, Localization, Asset, Error Handling, Performance, Audio, Physics)
2. **ADAPTASI_SPEC.md** — Spesifikasi teknis adaptasi per layer (apa yang harus dibuat/diubah/dihapus, dengan pseudo-code)
3. **PACKET_MAPPING.md** — Field-by-field mapping untuk 10 packet category paling penting (Login, Move, Combat, Skill, Inventory, Quest, Party, Guild, Chat, NPC) — tabel side-by-side Old struct vs Reborn .fbs, dengan severity + effort + dependencies
4. **DB_QUERY_MAPPING.md** — Mapping stored procedure Old vs SQL query Reborn, per kolom, dengan severity + effort + dependencies

---

## F. Constraints & Rules

1. **JANGAN menyarankan hal yang tidak ada di Old**. Setiap fitur yang direkomendasikan harus punya referensi di Old.
2. **JANGAN merekomendasikan teknologi yang tidak cocok** untuk use case MMORPG desktop. Contoh: jangan sarankan web framework untuk render 3D.
3. **Prioritaskan reuse** asset dan data yang sudah di-convert. Jangan suruh convert ulang.
4. **Setiap gap harus punya**:
   - Severity: Critical / High / Medium / Low
   - Effort estimate: hours/days/weeks
   - Dependencies: apa yang harus selesai sebelum ini
5. **Untuk fungsi kompleks** (Hero combat, protocol handler), beri pseudo-code atau referensi implementasi.
6. **Field-by-field packet mapping WAJIB** untuk minimal 10 packet category. Baca langsung file header Old (.h) dan file schema Reborn (.fbs). Output harus menunjukkan field lama, field baru, dan status tiap field.
7. **Database query mapping WAJIB** untuk semua stored procedure yang critical (character load, save, item, quest, skill). Bandingkan kolom per kolom.
8. **Build system comparison**: baca .sln/.dsp files di Old vs CMakeLists.txt di Reborn. Catat target, dependencies, link flags yang berbeda.
9. **Error handling**: cek tiap fungsi di Old yang punya error path (return false, throw, dialog error), cari equivalent di Reborn. Jika tidak ada, catat sebagai gap.
10. **Security**: jangan hanya bilang "tidak ada anti-cheat". Beri rekomendasi mitigasi praktis (rate limiting, server validation, anomaly detection).
11. **Concurrency**: bandingkan thread count, lock contention potensial, dan deadlock risk. Jangan hanya "IOCP vs asio".
12. **Localization**: cek file-file .bin.txt di `assets/interface/` — apakah ada versi per bahasa? Berapa banyak string di Old vs Reborn?
13. **Severity + Effort + Dependencies WAJIB ada di SETIAP entri tabel**. Tidak boleh ada satupun entri tanpa ketiganya.

---

Mulai analisa sekarang. Scan kedua direktori secara paralel.
