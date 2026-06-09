# FEEL-07 — PK System & PVP Feel Alignment

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Old punya PK system kompleks di `[Server]Map/AttackManager.cpp` + `[CC]BattleSystem/`: IsEnemy() memeriksa party, guild, guild union, siege mode, PK mode, guild war. Reborn PKManagerDlg basic. PVP feel tidak sama.

## Aturan Ketat

1. BACA Old: `/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[CC]BattleSystem/Battle.h` — cari IsEnemy()
2. BACA Reborn: `client/ui/dialogs/PKManagerDlg.cpp`
3. ✅ Jika PVP logic sudah sama — skip
4. 🔧 Jika beda — update

## Old IsEnemy() Logic:

```
IsEnemy_NormalTimes:
  1. Jika same party → false (teman)
  2. Jika PK mode ON → true (musuh)
  3. Jika guild war → true
  4. Jika guild union → false (teman)
  5. Default → false

IsEnemy_SiegeWarning:
  Sama + cek guild + guild union untuk siege
```

## Reborn check:

```bash
rg "IsEnemy\|PKMode\|pk_mode\|PK_FLAG\|pk_flag" game/ecs/systems/ --type cpp -l
rg "PKMODE\|pkMode\|pk_mode" client/ui/dialogs/PKManagerDlg.*
```

## Implementasi (jika perlu):

```cpp
// Tambah di CombatSystem atau sistem PK:
bool CombatSystem::IsEnemy(const CharacterStats& a, const CharacterStats& b, CombatContext ctx) {
    if (ctx == CombatContext::PvP) return true; // PK mode ON
    if (a.party_id == b.party_id && a.party_id > 0) return false;
    if (a.guild_id == b.guild_id && a.guild_id > 0) return false;
    if (a.guild_war_id == b.guild_id) return true;
    return false;
}
```

## Output

✅ Kembalikan: "FEEL-07 done: PK system aligned — X rules updated"
