# Agent-02 — Missing Dialogs: SystemMsg, ShoutDlg, Billing, HousingWeb, ConsignmentGuide

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Dari verifikasi, 5 dialog .bin yang masih benar-benar MISSING (tidak ada implementasi sama sekali). Buat dialog C++ minimal untuk masing-masing.

## Aturan Ketat

1. CEK dulu apakah file dialog sudah ada:
   `ls client/ui/dialogs/SystemMsgDialog.* 2>/dev/null`
   Jika ✅ SUDAH ADA → skip. Jangan buat ulang.
2. CEK di GameScreen.cpp: `rg "SystemMsg\|ShoutDlg\|BillingDlg\|HousingWeb\|ConsignmentGuide" game/`
   Jika sudah terdaftar → skip.
3. Untuk yang benar-benar MISSING: buat .hpp + .cpp mengikuti pattern dialog termudah (seperti WeatherDialog.cpp yang sederhana).
4. JANGAN buat dialog untuk .bin yang sudah ada implementasinya.
5. Build verify setelah selesai.

## Pattern Dialog Minimal

```cpp
// SystemMsgDialog.hpp
#pragma once
#include <string>
class GameState;
class WindowManager;
class Window;
class SystemMsgDialog {
public:
    void Open(WindowManager* wm, const std::string& message);
    void Close();
private:
    Window* window_ = nullptr;
};

// SystemMsgDialog.cpp
#include "SystemMsgDialog.hpp"
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
void SystemMsgDialog::Open(WindowManager* wm, const std::string& msg) {
    if (window_) return;
    window_ = wm->CreateWindow("System Message", 200, 200, 400, 150);
    window_->AddWidget<Label>(msg, 20, 30, 360, 60);
    auto* btn = window_->AddWidget<Button>("OK", 160, 90, 80, 28);
    btn->OnEvent([this](const UIEvent& e) { if (e.type == UIEvent::Click) Close(); });
}
void SystemMsgDialog::Close() { if (window_) { window_->Close(); window_ = nullptr; } }
```

## Daftar yang harus dicek

1. SystemMsg.bin → client/ui/dialogs/SystemMsgDialog.*
2. ShoutDlg.bin → client/ui/dialogs/ShoutDialog.*
3. BillingDlg.bin → client/ui/dialogs/BillingDialog.*
4. HousingWebDlg.bin → client/ui/dialogs/HousingWebDialog.*
5. Consignment_Guide.bin → client/ui/dialogs/ (atau di ConsignmentDialog.cpp inline)

## Output

✅ Kembalikan: "Agent-02 done: created [list dialog yang benar-benar dibuat]"
