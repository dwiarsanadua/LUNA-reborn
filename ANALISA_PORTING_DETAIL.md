# Laporan Analisa Porting: Luna-Plus-Reborn (macOS/Metal)

**Tanggal:** 10 Juni 2026  
**Status Proyek:** Research & Optimization Phase  
**Objektif:** Mencapai 1:1 Visual Fidelity & Feel antara Luna-Plus-Old (DX9) dan Luna-Plus-Reborn (bgfx/Metal).

---

## 1. Analisa Pipeline Rendering (Shaders & Lighting)

### Temuan Masalah (Gaps):
*   **Visual "Ngide" (Inovasi Tidak Relevan):** Ditemukan implementasi *Rim Light* (cahaya pinggir) dan *Specular* tinggi pada shader `fs_lit.sc` yang membuat karakter terlihat seperti plastik mengkilap. Ini tidak ada di versi original tahun 2008.
*   **Transparency Glitch:** Bagian transparan pada model (rambut, aksesoris) memiliki tepi putih atau tidak tembus pandang karena kurangnya logika *Alpha Testing/Clipping*.
*   **Lighting Contrast:** Pencahayaan standar `dot(N, L)` membuat area bayangan karakter terlalu gelap (*pitch black*), merusak estetika anime yang seharusnya cerah.

### Perbaikan yang Telah Diterapkan:
*   **Half-Lambert Lighting:** Mengubah kalkulasi diffuse menjadi `(N·L * 0.5 + 0.5)` untuk memastikan bayangan tetap terang dan lembut.
*   **Alpha Discard Logic:** Menambahkan `if (color.a < 0.5) discard;` pada fragment shader untuk hasil transparansi yang tajam (1:1 original).
*   **Removal of Modern Effects:** Mematikan *Bloom intensity* (set ke 0.0) dan menghapus *Specular Highlights* pada properti lingkungan.

---

## 2. Analisa Asset Pipeline (Conversion Logic)

### Temuan Masalah (Gaps):
*   **Material Loss:** Script `mod_to_obj.py` sebelumnya melewatkan blok material (0x00F00000) pada file `.mod`, sehingga model hasil konversi kehilangan referensi tekstur dan menjadi putih polos.
*   **UV Coordinate Inversion:** Ditemukan perbedaan penanganan koordinat V antara DirectX dan Metal yang menyebabkan tekstur terlihat terbalik secara vertikal.

### Perbaikan yang Telah Diterapkan:
*   **Automated MTL Generation:** Script sekarang mengekstrak nama tekstur asli dari binary `.mod` dan menghasilkan file `.mtl` secara otomatis.
*   **Engine Integration:** Memperbarui `Model.cpp` dan `CharacterRenderer.cpp` untuk membaca `material_index` dan memuat tekstur yang sesuai dari Atlas atau folder aset.

---

## 3. Analisa Porting UI (Interface & HUD)

### Temuan Masalah (Gaps):
*   **Fallback "DrawRect":** Widget UI (seperti `GaugeBar` untuk HP/MP) masih sering menggunakan *Solid Color Fill* (merah/biru polos) sebagai cadangan jika tekstur atlas tidak ditemukan. Ini menghilangkan detail gradient dan "glossy" dari UI asli.
*   **ImGui Hybrid:** Penggunaan Dear ImGui sebagai wrapper memberikan fleksibilitas namun berisiko membuat UI terlihat seperti aplikasi tools jika skinning asli tidak diterapkan secara menyeluruh.
*   **Font Rendering:** Font pada label sering terlihat *aliased* atau menggunakan font sistem macOS yang tidak sesuai dengan tipografi asli Luna (Gulim/Tahoma).

### Status Implementasi:
*   [x] Parser Script UI (.bin.txt) sudah berfungsi.
*   [ ] Sinkronisasi Atlas Tekstur (Interface.png) masih memerlukan kalibrasi koordinat UV.
*   [ ] Implementasi Nine-Patch Scaling untuk jendela agar tidak terlihat pecah saat di-resize.

---

## 4. Daftar Fitur "Inovasi" yang Dinonaktifkan (Clean-up)

| Fitur | Status | Alasan |
| :--- | :--- | :--- |
| **Bloom Post-Process** | DISABLED | Membuat dunia berkabut, menghilangkan kejernihan warna original. |
| **Specular Highlights** | DISABLED | Membuat objek terlihat seperti plastik/logam modern. |
| **Dynamic Shadow Maps** | SIMPLIFIED | Terlalu tajam; Luna Old lebih cocok dengan soft/blob shadows. |
| **Rim Lighting** | REMOVED | Tidak relevan dengan art style anime tahun 2008. |

---

## 5. Kesimpulan & Langkah Selanjutnya

Porting secara sistem (kode) sudah berjalan stabil di macOS, namun **Visual Fidelity** masih membutuhkan perhatian pada detail terkecil (UI Skinning dan Font). Jiwa dari Luna Plus terletak pada "kecerahan" dan "kejernihan" visualnya, bukan pada fitur rendering modern.

**Rencana Turn Berikutnya:**
1. Kalibrasi ulang `UiAtlasRegistry` untuk memastikan semua icon dan frame UI menggunakan gambar asli.
2. Optimasi font rendering agar setajam versi original.
3. Verifikasi ulang animasi `.anm` untuk memastikan sinkronisasi gerakan tidak terlalu cepat/lambat di CPU ARM (M1/M2).

---
*Dokumen ini dibuat otomatis oleh Gemini CLI sebagai bagian dari Audit Porting Luna-Plus-Reborn.*
