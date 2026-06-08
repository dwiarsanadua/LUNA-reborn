# Luna-Plus-Reborn: Gap Analysis & Technical Fix Plan

## 1. Gejala Masalah (Red & Magenta Screen)
Berdasarkan screenshot terbaru (19:51), client mengalami perubahan gejala dari "Red Screen" (0 triangles) menjadi **"Magenta Screen"** (136 triangles, 72 draw calls).
*   **Analisa Teknis:** Munculnya 136 triangles membuktikan bahwa sistem UI sudah berjalan dan mencoba mengirim data render ke GPU. Namun, warna magenta solid menunjukkan **Shader Mismatch/Failure**.
*   **Akar Masalah:** File `.bin` di folder `shaders/` kemungkinan besar dikompilasi untuk Windows (DirectX) atau OpenGL, sehingga saat dijalankan di macOS (Metal), bgfx gagal menjalankan pipeline shader dan melakukan *fallback* ke warna magenta (warna standar "error" pada shader).

## 2. Gap Analysis (Old vs Reborn)
... (tetap sama) ...

## 3. "PERFECT PLAN" — Langkah Perbaikan & Sinkronisasi

### Tahap 1: Restorasi Rendering (Fokus Metal macOS)
1.  **Recompile Shaders for Metal:**
    *   Wajib mengompilasi ulang `.sc` menjadi `.bin` menggunakan parameter `-p mtl` (Metal). 
    *   File target: `vs_ui.bin`, `fs_ui.bin`, `vs_terrain.bin`, `fs_terrain.bin`.
2.  **Verify UI Program Creation:**
    *   Tambahkan log di `UIRenderer::Init()` untuk memastikan `bgfx::createProgram` tidak mengembalikan handle invalid.
3.  **Enable 3D Login Background:** 
    *   Modifikasi `main.cpp` agar tetap merender `gfx.Render` saat login (Map 51) agar tidak hanya layar 2D statis.

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
