# ADAPTASI SPEC — Spesifikasi Teknis Adaptasi Luna-Plus-Old → Reborn

> Target: Memetakan SETIAP komponen Old ke implementasi Reborn dengan pseudo-code untuk fungsi kompleks

---

## 1. Player Flow — Adaptasi

### 1.1 Launcher → PatchSystem + LauncherScreen

**Status**: Partial
**Pseudo-code**:
```
// Old: MainDialog.cpp — FTP download + progress bar + HTML notice
class LauncherScreen {
    void OnEnter() {
        patchSystem.checkForUpdates()
        launcher.loadHtmlNotice(Paths::getNoticeHtml())
        audioManager.playBgm("launcher_bgm.mp3")
    }
    
    void OnUpdate(dt) {
        if (patchSystem.hasUpdate()) {
            patchSystem.startDownload() // FTP → HTTP download
            progressBar.setValue(patchSystem.getProgress())
        } else {
            screenManager.switchTo("LoginScreen")
        }
    }
}
```

**Perubahan**:
- FTP client → HTTP download via libcurl/ASIO
- HTML notice → WebView or plain text
- MFC dialog → GLFW + UIRenderer

### 1.2 Login Flow → LoginScreen + NetworkClient

**Status**: Partial
**Pseudo-code**:
```
// Old: WebLauncherIDPass.bin + AgentNetworkMsgParser
// Reborn: LoginScreen.cpp + Login.fbs (LoginRequest)

void LoginScreen::onLoginPressed() {
    string username = inputFieldUsername.getText()
    string password = inputFieldPassword.getText()
    
    // Convert password to SHA-256 hash (Old: plaintext + XOR)
    vector<uint8_t> passwordHash = crypto::sha256(password)
    
    auto request = LoginRequest(
        username,
        passwordHash,
        CLIENT_VERSION_STRING,
        getMacAddress()
    )
    
    networkClient.send(request, PacketType.MP_USERCONN_LOGIN_SYN)
}

void LoginScreen::onLoginResponse(LoginResponse response) {
    switch (response.result) {
        case LoginResult.Success:
            sessionToken = response.session_token
            serverList = response.server_list
            screenManager.switchTo("CharSelectScreen")
            break
        case LoginResult.InvalidCredentials:
            showError("Invalid username or password")
            break
        case LoginResult.Banned:
            showError("Account is banned")
            break
        case LoginResult.Maintenance:
            showError("Server under maintenance")
            break
    }
}
```

**Perubahan**:
- Plaintext password → SHA-256 hash
- XOR token → AES-GCM token (already done in PacketCrypto)
- Add missing fields: dwCRC, bUseNProtect, dwClientTime (deprecated, can be removed)

### 1.3 Character Select → CharSelectScreen

**Status**: Partial
**Pseudo-code**:
```
// Old: CharSelect.bin + MP_USERCONN_CHARACTERLIST_SYN
// Reborn: CharSelectScreen.cpp + Character.fbs

void CharSelectScreen::loadCharacterList() {
    networkClient.send(CharacterListRequest(sessionToken))
}

void CharSelectScreen::onCharacterList(CharacterListResponse response) {
    for each (charInfo in response.characters) {
        auto slot = new CharacterSlot()
        slot.setName(charInfo.name)
        slot.setLevel(charInfo.level)
        slot.setClass(charInfo.job)
        slot.setPreviewModel(charInfo.appearance) // 3D preview
        addWidget(slot)
    }
}

// MISSING: Character delete functionality
void CharSelectScreen::onDeleteCharacter(uint32_t charId) {
    // IMPLEMENT: MP_USERCONN_CHARACTER_REMOVE_SYN
    auto dialog = new ConfirmDialog("Delete character? This cannot be undone.")
    dialog.onConfirm = [this, charId]() {
        networkClient.send(CharacterDeleteRequest(charId))
    }
}
```

---

## 2. UI System — Adaptasi

### 2.1 UiScriptParser — Complete .bin.txt Parsing

**Status**: Partial (needs full coverage)
**Pseudo-code**:
```
// Old: cWindowManager loads .bin files
// Reborn: UiScriptParser parses .bin.txt

class UiScriptParser {
    // Current: Partial implementation
    // NEED: Parse ALL control types from Old .bin format
    
    struct BinControl {
        string type;     // "STATIC", "BUTTON", "EDIT", "LISTBOX", etc.
        Rect bounds;     // x, y, width, height
        uint32_t id;
        string text;     // String ID reference
        uint32_t style;  // WS_VISIBLE | WS_DISABLED | etc.
        string font;
        string image;    // Background image
        Color color;
    }
    
    vector<BinControl> parse(const string& filename) {
        // Read .bin.txt file (converted from .bin)
        // Parse line-by-line
        // Create Widget tree from controls
    }
    
    Widget* createWidget(const BinControl& ctrl) {
        switch (ctrl.type) {
            case "STATIC":     return new Label(ctrl.bounds, ctrl.text)
            case "BUTTON":     return new Button(ctrl.bounds, ctrl.text)
            case "EDIT":       return new InputField(ctrl.bounds)
            case "LISTBOX":    return new ListBox(ctrl.bounds)
            case "COMBOBOX":   return new ComboBox(ctrl.bounds)
            case "SCROLLBAR":  return new ScrollBar(ctrl.bounds)
            case "IMAGE":      return new ScriptSprite(ctrl.bounds, ctrl.image)
            case "GAUGE":      return new GaugeBar(ctrl.bounds)
            default:           return new Widget(ctrl.bounds)
        }
    }
}
```

### 2.2 Dialog Classes — Mapping per Window

For each .bin file → create C++ dialog class:

```
Template:
class XxxDialog : public Window {
public:
    XxxDialog() : Window("XxxDialog") {
        // UiScriptParser::parse("Xxx.bin.txt") → Widget tree
    }
    
    void onShow() override { /* load data from server */ }
    void onHide() override { /* cleanup */ }
    
    // Event handlers
    void onButtonClick(uint32_t id) override { /* handle */ }
    void onListBoxSelect(uint32_t id, int idx) override { /* handle */ }
    
    // Network handlers
    void onNetworkMessage(const Packet& pkt) { /* handle response */ }
};
```

**Prioritas dialog yang harus dibuat** (top 15):
1. NPCShop — NPC shop buy/sell
2. TargetWindow — Target info display
3. ChatRoom — Chat room management
4. GuildWarehouse — Guild storage
5. StallBuy/Sell — Street stall
6. MixDialog — Item mixing
7. ComposeDialog — Item composition
8. PartySet — Party settings
9. Revival — Revive options
10. QuestQuickView — Quick quest view
11. FamilyMark — Family emblem
12. ConsignmentCategory — Auction category
13. WeatherDlg — Weather display
14. SiegeWarFlagDlg — Siege war flag
15. ChangeClass — Class advancement

---

## 3. Gameplay Constants — Adaptasi

### 3.1 Combat Formula Fix

**Critical numeric differences**:

```
// OLD formula (Hero.cpp):
CritRate = DEX / 1000           // 0.1% per DEX point
BlockRate = CON / 2000          // 0.05% per CON point
MissRate = 0.05f                // Fixed 5%
BaseDamage = ATK * skill_power  // skill_power varies
ElementAdv = 1.3f / 0.7f       // 7-element cycle
LevelMod = ±5% per level, cap 50%

// REBORN formula (CombatSystem.cpp):
CritRate = min(50%, 5% + DEX/100)  // 1% per DEX point — 10x HIGHER
BlockRate = block_rate stat         // Different system
MissRate = max(1%, 5% - DEX/500)    // Inverse relation to DEX
BaseDamage = (ATK * 2) - DEF        // Very different
ElementAdv = 1.3f / 0.7f           // Same ✓
LevelMod = ±5% per level, cap 50%  // Same ✓
```

**Recommended fix**: Align Reborn formulas with Old:
```
// ADAPTED formula for Reborn:
float getCritRate(CharacterStats stats) {
    return stats.dexterity / 1000.0f;  // 0.1% per DEX (Old behavior)
}

float getBlockRate(CharacterStats stats) {
    return stats.constitution / 2000.0f;  // Old formula
}

float getMissRate(CharacterStats attacker, CharacterStats defender) {
    return 0.05f;  // 5% fixed (Old behavior)
}

float calcBaseDamage(CharacterStats attacker, CharacterStats defender) {
    // Old: ATK * skill_power
    float skillPower = 1.0f;  // default auto-attack
    return max(1.0f, attacker.physic_attack * skillPower - defender.physic_defense * 0.5f);
}
```

### 3.2 Aggro System — Pseudo-code

```
// MISSING in Reborn. Need implementation based on Old hate/threat system.

class AggroSystem {
    struct ThreatEntry {
        uint32_t entityId;
        int32_t hateAmount;
    };
    
    unordered_map<uint32_t, vector<ThreatEntry>> aggroTables;
    
    void addThreat(uint32_t monsterId, uint32_t attackerId, int32_t amount) {
        auto& table = aggroTables[monsterId];
        auto it = find_if(table.begin(), table.end(), 
            [attackerId](const ThreatEntry& e) { return e.entityId == attackerId; });
        
        if (it != table.end()) {
            it->hateAmount += amount;
        } else {
            table.push_back({attackerId, amount});
        }
    }
    
    uint32_t getTopThreat(uint32_t monsterId) {
        auto& table = aggroTables[monsterId];
        if (table.empty()) return 0;
        
        return max_element(table.begin(), table.end(),
            [](const ThreatEntry& a, const ThreatEntry& b) { 
                return a.hateAmount < b.hateAmount; 
            })->entityId;
    }
    
    void onDamage(uint32_t monsterId, uint32_t attackerId, int32_t damage) {
        addThreat(monsterId, attackerId, damage);  // 1:1 threat from damage
    }
    
    void onHeal(uint32_t monsterId, uint32_t healerId, int32_t healAmount) {
        addThreat(monsterId, healerId, healAmount / 2);  // 50% threat from healing
    }
    
    void resetOnDeath(uint32_t monsterId) {
        aggroTables.erase(monsterId);
    }
}
```

---

## 4. Network Protocol — Adaptasi

### 4.1 Packet Handler Wiring

For each packet, wire handler:
```
// Old: switch/case in AgentNetworkMsgParser / MapNetworkMsgParser
// Reborn: PacketRouter with FlatBuffers

class PacketRouter {
    unordered_map<PacketType, function<void(const Packet&)>> handlers;
    
    void registerHandler(PacketType type, function<void(const Packet&)> handler) {
        handlers[type] = handler;
    }
    
    void route(const Packet& packet) {
        auto it = handlers.find(packet.type);
        if (it != handlers.end()) {
            it->second(packet);
        } else {
            log.warn("Unhandled packet type: {}", packet.type);
        }
    }
};
```

**Priority packets to wire**:
1. MP_USERCONN_LOGIN_SYN/ACK/NACK → Login system
2. MP_USERCONN_CHARACTERLIST_ACK → Character list
3. MP_USERCONN_CHARACTERSELECT_ACK → Game enter
4. MP_USERCONN_CHARACTER_MAKE_ACK → Character creation
5. MP_USERCONN_GAMEIN_ACK → World enter
6. MP_MOVE_WALK/RUN/STOP → Movement
7. MP_COMBAT_ATTACK_SYN/ACK → Combat
8. MP_ITEM_* → Inventory
9. MP_CHAT_* → Chat
10. MP_PARTY_* → Party

### 4.2 Packet Encryption

Already done: XOR/RC4 → AES-GCM (PacketCrypto.cpp)
No changes needed.

### 4.3 Checksum Verification

```
// Old: MSGROOT.CheckSum (1 byte XOR of all bytes)
// Reborn: FlatBuffers built-in verification

// Add for backward compat:
bool verifyChecksum(const uint8_t* data, size_t len, uint8_t expected) {
    uint8_t calc = 0;
    for (size_t i = 0; i < len; i++) {
        calc ^= data[i];
    }
    return calc == expected;
}
```

---

## 5. Server Systems — Adaptasi

### 5.1 Agent Server (Authentication)

```
// Old: AgentNetworkMsgParser + AgentDBMsgParser
// Reborn: server/agent/AgentServer.cpp

class AgentServer {
    void handleLogin(LoginRequest request, Connection conn) {
        // 1. Validate credentials
        auto user = db.queryUser(request.username)
        
        if (!user) {
            conn.send(LoginResponse(LoginResult.InvalidCredentials))
            return
        }
        
        // 2. Check password hash
        auto hash = sha256(request.password)
        if (hash != user.passwordHash) {
            conn.send(LoginResponse(LoginResult.InvalidCredentials))
            return
        }
        
        // 3. Check ban status
        if (user.isBanned) {
            conn.send(LoginResponse(LoginResult.Banned))
            return
        }
        
        // 4. Check overlapped login
        if (sessionManager.isLoggedIn(user.id)) {
            sessionManager.disconnectExisting(user.id)
            conn.send(LoginResponse(LoginResult.AlreadyLoggedIn))
            // Old: MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN
        }
        
        // 5. Create session
        auto session = sessionManager.createSession(user.id, conn)
        
        // 6. Send server list
        auto servers = db.queryServerList()
        conn.send(LoginResponse(LoginResult.Success, session.token, servers))
    }
}
```

### 5.2 Map Server (World Simulation)

```
// Old: MapNetworkMsgParser
// Reborn: server/map/MapServer.cpp + ECS systems

class MapServer {
    entt::registry registry;
    
    void onEntityMove(MoveRequest request, entt::entity entity) {
        auto& pos = registry.get<Transform>(entity)
        auto& move = registry.get<Movement>(entity)
        
        // Server-authoritative movement
        Vec3 newPos = pos.position + request.direction * move.speed * deltaTime
        
        // Collision check
        if (!collisionSystem.lineCheck(pos.position, newPos)) {
            pos.position = newPos
            broadcast(EntityMove(entity, pos.position, request.direction, move.speed))
        } else {
            // Correction
            send(MoveCorrection(entity, pos.position))
        }
    }
}
```

---

## 6. Database — Adaptasi

### 6.1 Stored Procedure → Inline Query Migration

```
// Old: MSSQL stored procedure
// USP_CHARACTER_LOAD @char_id
// SELECT ... FROM TB_CHARACTER WHERE char_id = @char_id

// Reborn: SQLite prepared statement
class CharacterRepository {
    Character load(uint32_t charId) {
        auto stmt = db.prepare(
            "SELECT CharacterIdx, AccountID, CharName, Level, Exp, "
            "Money, MapIdx, PosX, PosY, PosZ, HP, MP, SP, "
            "Str, Dex, Int, Con, Wis, Job, Face, Hair "
            "FROM TB_CHARACTER WHERE CharacterIdx = ?"
        )
        stmt.bind(1, charId)
        
        if (stmt.execute()) {
            Character c;
            c.id = stmt.getInt(0)
            c.accountId = stmt.getString(1)
            c.name = stmt.getString(2)
            c.level = stmt.getInt(3)
            // ... map all columns
            return c
        }
        return null
    }
    
    void save(Character c) {
        auto stmt = db.prepare(
            "UPDATE TB_CHARACTER SET Level=?, Exp=?, Money=?, "
            "MapIdx=?, PosX=?, PosY=?, PosZ=?, HP=?, MP=?, SP=?, "
            "Str=?, Dex=?, Int=?, Con=?, Wis=? "
            "WHERE CharacterIdx = ?"
        )
        // bind and execute
    }
}
```

### 6.2 Missing Column: battle_style / PvpKillCount

```
ALTER TABLE TB_CHARACTER ADD COLUMN BattleStyle INTEGER DEFAULT 0;
ALTER TABLE TB_CHARACTER ADD COLUMN PvpKillCount INTEGER DEFAULT 0;
ALTER TABLE TB_CHARACTER ADD COLUMN PvpDeathCount INTEGER DEFAULT 0;
-- Add to PostgreSQL schema for production
```

---

## 7. Build System — Adaptasi

### 7.1 CMake Targets Mapping

```
Old .sln target          → CMake target
─────────────────────────────────────
LunaPlusClient.exe       → add_executable(luna-plus-client ...)
LunaPlusAgent.exe        → add_executable(luna-plus-agent ...)
LunaPlusMap.exe          → add_executable(luna-plus-map ...)
LunaPlusDistribute.exe   → add_executable(luna-plus-distribute ...)
MapEditor.exe            → add_executable(map_editor ...) [exists in tools/]
ModelView.exe            → add_executable(chx_to_gltf ...) [exists]

Missing CMake targets (to add):
- add_executable(packing_tool ...)    # PackingTool
- add_executable(new_packing_tool ...) # NewPackingTool
- add_executable(auto_patch ...)      # AutoPatchTool
```

---

## 8. Security — Rekomendasi

### 8.1 Rate Limiting

```
// Add to server:
class RateLimiter {
    unordered_map<uint32_t, vector<uint64_t>> requestLog; // entityId → timestamps
    uint32_t maxRequestsPerSecond = 100;
    
    bool allow(uint32_t entityId) {
        auto& log = requestLog[entityId];
        auto now = getCurrentTimeMs();
        
        // Remove entries older than 1s
        log.erase(remove_if(log.begin(), log.end(), 
            [now](uint64_t t) { return now - t > 1000; }), log.end());
        
        if (log.size() >= maxRequestsPerSecond) {
            return false;  // Rate limit exceeded
        }
        
        log.push_back(now);
        return true;
    }
}
```

### 8.2 Server-Side Validation

```
// Validate ALL client inputs:
bool validateMovement(MoveRequest request, CharacterStats stats) {
    float maxSpeed = stats.moveSpeed * 1.5f;  // 50% tolerance
    float actualSpeed = request.direction.length() / deltaTime;
    
    if (actualSpeed > maxSpeed) {
        log.warn("Speed hack detected: {} (max: {})", actualSpeed, maxSpeed);
        return false;
    }
    return true;
}

bool validateDamage(DamageReport report, CharacterStats attacker, CharacterStats defender) {
    // Re-calculate expected damage range
    float expectedMin = calcExpectedDamage(attacker, defender) * 0.8f;
    float expectedMax = calcExpectedDamage(attacker, defender) * 1.2f;
    
    if (report.damage < expectedMin || report.damage > expectedMax) {
        log.warn("Damage hack detected: {} (expected: {}-{})", 
                 report.damage, expectedMin, expectedMax);
        return false;
    }
    return true;
}
```

---

## 9. Concurrency — Improvements

### 9.1 Job System

```
// Add job system for CPU-intensive tasks (pathfinding, AI, DB):
class JobSystem {
    asio::thread_pool pool{4};  // 4 worker threads
    
    template<typename F>
    auto enqueue(F&& task) -> std::future<decltype(task())> {
        return asio::post(pool, std::forward<F>(task));
    }
};

// Usage:
jobSystem.enqueue([this]() {
    auto path = pathfindingSystem.findPath(start, end);
    mainThreadDispatcher.post([path]() {
        entity.setPath(path);
    });
});
```

### 9.2 DB Access with Connection Pool

```
class DatabasePool {
    vector<sqlite3*> connections;
    mutex mutex;
    
    sqlite3* getConnection() {
        lock_guard lock(mutex);
        // Round-robin or least-used
        auto conn = connections.back();
        connections.pop_back();
        return conn;
    }
    
    void returnConnection(sqlite3* conn) {
        lock_guard lock(mutex);
        connections.push_back(conn);
    }
};
```

---

## 10. Localization — Font Fix

```
// Add CJK-compatible fonts:
const array<string, 4> FONT_FILES = {
    "2002_EYA.ttf",        // Default (English)
    "NotoSansKR-Regular.ttf",  // Korean
    "NotoSansSC-Regular.ttf",  // Chinese
    "NotoSansJP-Regular.ttf"   // Japanese
};

class FontManager {
    ImFont* loadFontForLanguage(Language lang) {
        string fontFile = FONT_FILES[static_cast<int>(lang)];
        return io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 14.0f, NULL, 
                                            io.Fonts->GetGlyphRangesCyrillic());
    }
    
    void switchLanguage(Language newLang) {
        currentFont = loadFontForLanguage(newLang);
        // Reload all UI elements with new font
    }
};
```

---

## 11. Edge Cases — Error Handlers

```
// Add reconnect logic:
class ReconnectHandler {
    static const int MAX_RETRIES = 3;
    static const int RETRY_DELAY_MS = 10000;  // 10s
    
    int retryCount = 0;
    
    void onDisconnect() {
        if (retryCount < MAX_RETRIES) {
            showReconnectDialog(RETRY_DELAY_MS / 1000)
            
            timer.schedule([this]() {
                if (networkClient.reconnect()) {
                    hideReconnectDialog()
                    retryCount = 0
                } else {
                    retryCount++
                    onDisconnect()  // Retry
                }
            }, RETRY_DELAY_MS)
        } else {
            showError("Connection lost. Returning to login.")
            screenManager.switchTo("LoginScreen")
        }
    }
};

// Add asset fallback:
Texture* TextureManager::loadSafe(const string& path) {
    auto texture = load(path)
    if (texture == null) {
        log.error("Failed to load texture: {}", path)
        return fallbackTexture  // 1x1 magenta texture
    }
    return texture
}
```

---

*End of ADAPTASI_SPEC.md — All technical specifications with pseudo-code*
