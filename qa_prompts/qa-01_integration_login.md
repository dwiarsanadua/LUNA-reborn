# QA-01 — Integration Tests: Login Flow + Packet Round-trip

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat file `tools/test_runner/integration.cpp` — test integrasi untuk Login Flow dan Packet Round-trip.

## Aturan Ketat

1. BACA dulu `test_harness.hpp` — gunakan macro yang sama (TEST, TEST_SUITE, TEST_STEP)
2. BACA dulu `network.cpp` dan `systems.cpp` — pahami pattern yang sudah ada
3. ✅ Jika method SUDAH ADA — jangan buat ulang, panggil langsung
4. 🔧 Hanya tambah test yang benar-benar baru
5. Registrasi di `CMakeLists.txt` (tambah `integration.cpp` ke `add_executable`)
6. Build + run setelah selesai — 0 error

## Test Scenarios

### Test 1: Server Login Flow (tanpa network)
- Buat AgentServer fake object (atau panggil langsung method login)
- Set karakter stats
- Test: LoginRequest diterima → LoginResponse success
- Test: Wrong password → LoginResult_InvalidCredentials

### Test 2: Packet Serialization Round-trip  
- Buat 10 packet dari berbagai tipe (Login, Move, Combat, Chat, Guild, NPC)
- Untuk SETIAP packet:
  1. Buat packet dengan FlatBuffers Builder
  2. Finish → simpan buffer
  3. GetRoot dari buffer yang sama
  4. Verifikasi SEMUA field cocok
- Log field-by-field untuk setiap packet

### Test 3: Entity Spawn/Despawn
- Buat player entity di ECS registry
- Em英雄 Transform + CharacterStats + TagPlayer
- Test: entity valid, component accessible
- Destroy entity → test: component tidak accessible

## Output

✅ Kembalikan: "QA-01 done: integration tests with 0 failures"
