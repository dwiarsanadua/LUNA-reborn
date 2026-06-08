# Luna-Plus-Reborn: Gap Analysis & Technical Fix Plan

## 1. Gejala Masalah (Red Screen Issue)
Berdasarkan screenshot dan audit kode, client saat ini mengalami "Red Screen" dengan statistik `Triangles: 0` dan `Draw Calls: 2`. Ini menunjukkan bahwa:
*   **3D Rendering Skip:** Kode di `main.cpp` sengaja melewati rendering 3D (Terrain, Props, Characters) saat berada di `LoginScreen`.
*   **UI Rendering Failure:** `UIRenderer` gagal merender background 2D, kemungkinan karena shader tidak kompatibel dengan Metal (macOS) atau aset `.png` tidak ditemukan di path yang benar.
*   **Shader Mismatch:** File `.bin` di folder `shaders/` mungkin dikompilasi untuk renderer lain (seperti DX11), sehingga gagal di macOS.

## 2. Gap Analysis (Old vs Reborn)

| Kategori | Luna-Plus-Old (Legacy) | Luna-Plus-Reborn (Current) | Status Gap |
| :--- | :--- | :--- | :--- |
| **Login Flow** | 3D Scene (Map 51) dengan kamera panning. | Static 2D Background (saat ini gagal render). | 🔴 Kritis |
| **Shader System** | Fixed-function / HLSL SM 1.0 (D3D9). | bgfx cross-platform (Metal/DX11/Vulkan). | 🟡 Menengah |
| **Asset Loading** | Packed `.pak` (Virtual File System). | Unpacked files (Manual path search). | 🟡 Menengah |
| **Update Loop** | Frame-independent logic dengan fixed tick. | Variable delta-time (butuh tuning konstanta). | 🟢 Ringan |
| **UI Framework** | Data-driven via `.bin` text scripts. | Hardcoded C++ di `LoginScreen.cpp`. | 🔴 Kritis |

## 3. "PERFECT PLAN" — Langkah Perbaikan & Sinkronisasi

### Tahap 1: Restorasi Rendering & Login Scene
1.  **Enable 3D Login Background:** 
    *   Modifikasi `main.cpp` agar tetap merender `gfx.Render` saat login.
    *   Set kamera ke koordinat spesifik Alker Plains (Map 51) untuk mensimulasikan login screen original.
2.  **Shader Recompilation:**
    *   Gunakan `shaderc` dari bgfx untuk mengompilasi ulang `.sc` ke `.bin` spesifik Metal (macOS) dan DX11 (Windows).
3.  **Path Normalization:**
    *   Implementasi `VFS` atau `PathManager` agar client selalu menemukan folder `assets/` tanpa peduli di mana binary dijalankan.

### Tahap 2: Sinkronisasi Logika (Behavioral Mapping)
1.  **Movement & Physics:**
    *   Tuning `Hero.cpp` agar kecepatan lari, rotasi, dan gravitasi menggunakan konstanta yang diekstrak dari `Hero.cpp` (Old).
2.  **Action Sequence:**
    *   Pastikan urutan: `Input -> FSM Update -> Physics -> Animation Sync -> Render` dilakukan dalam satu frame yang sama untuk menghindari input lag.

### Tahap 3: UI & Interface Recovery
1.  **UI Script Parser:**
    *   Bangun kembali `cScriptManager` versi modern untuk membaca file `.bin.txt` (decrypted) sehingga layout UI bisa 1:1 dengan original.
2.  **Asset Preloading:**
    *   Optimasi `AssetPreloader` untuk memuat semua UI Icons di awal agar tidak ada "pop-in" visual.

### Tahap 4: Final Validation
1.  **Comparison Run:** Jalankan Old Client dan Reborn Client berdampingan.
2.  **Performance Check:** Pastikan FPS stabil di 60+ pada resolusi 1080p dengan modern features (Shadows/Bloom).

---
*Dokumen ini dibuat berdasarkan hasil audit mendalam terhadap Luna-Plus-Old dan Reborn.*
*Update Terakhir: Juni 2026*
