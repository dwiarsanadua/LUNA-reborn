# QA-07 — Register All QA Tests in CMakeLists.txt

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Update `tools/test_runner/CMakeLists.txt` — tambah semua file test baru ke build target.

## Aturan Ketat

1. ✅ BACA dulu CMakeLists.txt yang sudah ada
2. 🔧 TAMBAH source files baru: `integration.cpp functional.cpp multiplayer.cpp content.cpp performance.cpp security.cpp`
3. JANGAN ubah library links — hanya tambah source files
4. Build verify setelah selesai

## File yang harus diupdate

tools/test_runner/CMakeLists.txt:
```cmake
add_executable(test_runner
    main.cpp
    formulas.cpp
    systems.cpp
    network.cpp
    database.cpp
    integration.cpp      # BARU
    functional.cpp       # BARU
    multiplayer.cpp      # BARU
    content.cpp          # BARU
    performance.cpp      # BARU
    security.cpp         # BARU
)
```

## Output

✅ Kembalikan: "QA-07 done: all QA test files registered, build 0 error"

## Ceklis Final

Setelah ini selesai, jalankan:
```bash
cmake --build build/macos-debug --target test_runner
./build/macos-debug/bin/test_runner
```

Pastikan ALL tests pass sebelum di-commit.
