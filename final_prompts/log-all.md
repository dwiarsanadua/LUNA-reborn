# LOG-ALL — Add Detailed Logging to Every System

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Tambah logging `spdlog::info/warn/error` di SETIAP entry point sistem agar saat game di-run, semua aktivitas tercatat. Format: `[System] action: detail (value=X)`.

## Aturan

1. ✅ Jika sudah ada logging — skip
2. 🔧 Jika belum — tambah spdlog::info untuk sukses, spdlog::warn untuk warning, spdlog::error untuk error
3. JANGAN ubah logic — hanya tambah log
4. Build verify

## File yang harus diupdate

### 1. client/main.cpp — Game Startup

```cpp
// Di awal main():
spdlog::info("╔════════════════════════════════════╗");
spdlog::info("║    LUNA+ Reborn — Starting Up      ║");
spdlog::info("╚════════════════════════════════════╝");
spdlog::info("[SYSTEM] Platform: {}", "macOS");
spdlog::info("[SYSTEM] Renderer: bgfx Metal");
spdlog::info("[SYSTEM] Assets: {} textures, {} models, {} animations", 
             FindFiles("assets/textures/*.png").size(), 
             FindFiles("assets/models/*.glb").size(),
             FindFiles("assets/animations/*.anm.json").size());

// Sebelum GLFW init:
spdlog::info("[GLFW] Initializing...");
// Setelah GLFW init:
spdlog::info("[GLFW] Window created: {}x{}", width, height);

// Sebelum bgfx init:
spdlog::info("[BGFX] Initializing renderer...");
// Setelah bgfx init:
spdlog::info("[BGFX] Renderer: {}", bgfx::getRendererName(bgfx::getRendererType()));

// Loading screen:
spdlog::info("[ASSET] Loading UI skin...");
spdlog::info("[ASSET] Loading string table...");
spdlog::info("[ASSET] Loading game data...");

// Game loop start:
spdlog::info("[GAME] Entering main loop");
```

### 2. client/ui/screens/LoginScreen.cpp — Login Flow

```cpp
// Di OnEnter():
spdlog::info("[LOGIN] Screen entered");

// Saat login button ditekan:
spdlog::info("[LOGIN] Attempt login: user='{}'", username);
spdlog::info("[LOGIN] Sending LoginRequest (version={}, mac={})", version, mac);

// Saat login response diterima:
spdlog::info("[LOGIN] Response received: result={}", static_cast<int>(result));
if (success) spdlog::info("[LOGIN] Login SUCCESS — session_token={}", token);
else spdlog::warn("[LOGIN] Login FAILED: {}", reason);
```

### 3. client/ui/screens/GameScreen.cpp — Game World

```cpp
// Di Enter():
spdlog::info("[GAME] Entering game screen — map={}", map_id);

// Hero spawn:
spdlog::info("[GAME] Hero spawned at ({:.1f}, {:.1f}, {:.1f})", x, y, z);

// Combat:
spdlog::info("[COMBAT] Attack: {} → {} (skill={}, dmg={})", 
             attacker_id, target_id, skill_id, damage);
spdlog::info("[COMBAT] Monster {} HP: {}/{}", monster_id, hp, max_hp);
spdlog::info("[COMBAT] Monster {} died — exp_gain={}", monster_id, exp);
spdlog::info("[COMBAT] Player level up: {} → {}", old_level, new_level);

// Packet handler:
spdlog::info("[NET] Packet received: type={} ({} bytes)", packet_type, size);
```

### 4. client/engine/EngineMap.cpp — Map Loading

```cpp
// Load map:
spdlog::info("[MAP] Loading map {}...", map_id);

// Terrain:
spdlog::info("[MAP] Loading heightmap: {}.hgt ({}x{})", map_id, w, h);

// Scene objects:
spdlog::info("[MAP] Loading scene: {} objects", scene.objects.size());
for (auto& obj : scene.objects) {
    spdlog::info("[MAP]   Object: {} at ({:.1f}, {:.1f}, {:.1f})", 
                 obj.model_name, obj.position.x, obj.position.y, obj.position.z);
}

// Collision:
spdlog::info("[MAP] Collision bounds: ({:.0f},{:.0f}) → ({:.0f},{:.0f})",
             box_min.x, box_min.z, box_max.x, box_max.z);
```

### 5. server/map/MapServer.cpp — Server Tick

```cpp
// Tick:
spdlog::info("[SERVER] Tick: {} entities, {} ms", 
             registry.size(), tick_duration_ms);

// Player connect:
spdlog::info("[SERVER] Player connected: id={}, ip={}", player_id, ip);

// Player disconnect:
spdlog::info("[SERVER] Player disconnected: id={}", player_id);

// Movement:
spdlog::info("[MOVE] Player {} move: ({:.1f},{:.1f}) → ({:.1f},{:.1f})", 
             player_id, from.x, from.z, to.x, to.z);

// Combat:
spdlog::info("[COMBAT] Player {} attacks monster {}: dmg={}, crit={}, block={}",
             player_id, monster_id, damage, is_critical, is_blocked);

// Chat:
spdlog::info("[CHAT] [{}] {}: {}", channel, sender, message);
```

### 6. game/ecs/systems/CombatSystem.cpp — Combat Detail

```cpp
// CalculateDamage:
spdlog::debug("[DMG] CalculateDamage: atk={:.1f} def={:.1f} skill={:.1f}",
              atk, def, skill_add_damage);
spdlog::debug("[DMG]   Level penalty: diff={} mult={:.2f}", level_diff, penalty);
spdlog::debug("[DMG]   Block: rate={:.4f} blocked={}", block_chance, result.is_blocked);
spdlog::debug("[DMG]   Crit: rate={:.4f} dmg_mult={:.2f} critted={}", 
              crit_rate, crit_dmg, result.is_critical);
spdlog::debug("[DMG]   Final: {} (miss={})", result.damage, result.is_miss);

// HandleAttack:
spdlog::info("[COMBAT] {} → {}: dmg={}, skill={}", 
             entity_id(attacker), entity_id(target), result.damage, skill_id);

// ApplyDamage:
spdlog::debug("[COMBAT]   HP: {} → {} (dmg={})", hp_before, hp_after, damage);

// Death:
spdlog::info("[COMBAT] {} died — killer={}", entity_id(target), entity_id(attacker));
```

### 7. game/ecs/systems/PartySystem.cpp — Party

```cpp
spdlog::info("[PARTY] Created: leader={}, party_id={}", leader_id, party_id);
spdlog::info("[PARTY] Invite: {} → {}", inviter_id, invitee_id);
spdlog::info("[PARTY] Joined: {} → party_id={}", player_id, party_id);
spdlog::info("[PARTY] Left: {} → party_id={}", player_id, party_id);
spdlog::info("[PARTY] EXP distributed: {} total, {} each across {} members",
             total_exp, share, member_count);
```

### 8. game/ecs/systems/GuildSystem.cpp — Guild

```cpp
spdlog::info("[GUILD] Created: name={}, founder={}", name, founder_id);
spdlog::info("[GUILD] Invite: {} → {}", inviter_id, invitee_id);
spdlog::info("[GUILD] Joined: {} → guild_id={}", player_id, guild_id);
spdlog::info("[GUILD] Gold deposit: {} (+{} gold)", player_id, amount);
spdlog::info("[GUILD] Notice updated: guild_id={}", guild_id);
```

### 9. server/agent/AgentServer.cpp — Auth

```cpp
spdlog::info("[AUTH] Login attempt: user='{}' from {}", username, ip);
spdlog::info("[AUTH] Login success: user='{}' → session={}", username, token);
spdlog::warn("[AUTH] Login failed: user='{}' — invalid password", username);
spdlog::warn("[AUTH] Login failed: user='{}' — banned", username);
spdlog::info("[AUTH] Character list: user='{}' → {} chars", username, char_count);
spdlog::info("[AUTH] Character created: user='{}' → name='{}'", username, char_name);
spdlog::info("[AUTH] Character deleted: user='{}' → name='{}'", username, char_name);
```

### 10. lib/soundlib/SoundLib.cpp — Audio

```cpp
spdlog::info("[AUDIO] Init: {} Hz, {} channels", frequency, channels);
spdlog::info("[AUDIO] BGM loaded: {} (id={})", path, id);
spdlog::info("[AUDIO] BGM play: id={}, loop={}", id, loop);
spdlog::info("[AUDIO] SFX loaded: {} (id={})", path, id);
spdlog::info("[AUDIO] SFX play: id={}, vol={:.1f}, pan={:.1f}", id, volume, pan);
spdlog::error("[AUDIO] Failed to load: {}", path);
```

### 11. client/rendering/UIRenderer.cpp — UI Debug

```cpp
// Di DrawWindow/Button/Gauge — debug log window creation:
// (jangan log setiap frame — cukup sekali saat konstruksi)

// Di Window::Render():
// log window open/close saja:
spdlog::info("[UI] Window opened: {}", title_);
spdlog::info("[UI] Window closed: {}", title_);
```

### 12. SceneLoader (OMEGA-SCENE) — Scene Loading

```cpp
spdlog::info("[SCENE] Loading: {} ({} objects, {} lights)", 
             path, scene.objects.size(), scene.lights.size());
for (auto& obj : scene.objects) {
    spdlog::info("[SCENE]   Object: {} at ({:.1f},{:.1f},{:.1f})", 
                 obj.model_name, obj.position.x, obj.position.y, obj.position.z);
}
spdlog::info("[SCENE] Instantiated {}/{} objects", loaded, total);
spdlog::warn("[SCENE] Model not found: {}", model_name);
```

---

## Log Level Convention

| Level | Penggunaan | Contoh |
|-------|-----------|--------|
| `spdlog::info` | Normal operation | Login, combat, party, guild |
| `spdlog::warn` | Minor issue, fallback | File not found → fallback, rate limit |
| `spdlog::error` | Critical failure | DB error, load failure, crash |
| `spdlog::debug` | Detail per-frame | Damage calculation steps |

## ✅ Kembalikan: "LOG-ALL done: logging added to X systems across Y files"
