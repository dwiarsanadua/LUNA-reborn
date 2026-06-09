# Agent G01 — Build Polish & Minor Fixes (Phase 2)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## File yang harus diubah

### client/main.cpp [UPDATE]

**🟡 Frame cap (vsync)** (Severity: L, Effort: <1 day)
```cpp
// Di main loop, setelah glfwSwapBuffers
static double last_frame_time = 0;
double now = glfwGetTime();
double target_dt = 1.0 / 60.0; // 60 FPS cap
double elapsed = now - last_frame_time;
if (elapsed < target_dt) {
    std::this_thread::sleep_for(
        std::chrono::duration<double>(target_dt - elapsed));
}
last_frame_time = now;
```
Atau aktifkan vsync: `glfwSwapInterval(1);`

### client/ui/screens/LoginScreen.cpp [UPDATE]

**🟡 Save ID checkbox** (Severity: L, Effort: 4 hrs)
- CheckBox sudah ada di widget
- Tambah checkbox "Save ID" di LoginScreen
- Simpan ID ke file pakai ConfigManager::SetString
- Load waktu startup

Pseudo-code:
```cpp
// Di LoginScreen::OnEnter()
save_id_checkbox_ = window_->AddWidget<CheckBox>("Save ID", x, y, 100, 20);
saved_id_ = ConfigManager::GetString("login.saved_id", "");
if (!saved_id_.empty()) {
    id_field_.SetText(saved_id_);
    save_id_checkbox_->SetChecked(true);
}

// Saat login sukses
if (save_id_checkbox_->IsChecked()) {
    ConfigManager::SetString("login.saved_id", id_field_.GetText());
} else {
    ConfigManager::SetString("login.saved_id", "");
}
ConfigManager::Save();
```

### server/shared/CMakeLists.txt [UPDATE]

**🟡 Link ValidationSystem** (Severity: L, Effort: 1 hr)
```cmake
target_sources(Database PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ValidationSystem.cpp
)
```

### CMakeLists.txt atau tools/ [UPDATE]

**🟡 Add missing targets (stub)** (Severity: L, Effort: 1 week)
```cmake
# PackingTool — low priority, stub for now
add_executable(packing_tool tools/packing_tool/main.cpp)
target_link_libraries(packing_tool PRIVATE ...)
```

## Aturan

1. Semua fix kecil — kerjakan cepat
2. JANGAN build atau compile
3. ✅ Kembalikan "Agent G01 done: build polish and minor fixes"
