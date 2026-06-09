# OMEGA-01 — Fix 8 Missing UI Texture References

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Dari audit image_path.bin.txt, ditemukan 8 file texture yang dirujuk tapi tidak ada di assets Reborn. Buat placeholder PNG kecil (1x1 pixel transparan) agar referensi tidak broken, lalu update image_path.bin.txt untuk titik ke path yang benar.

## 8 File Missing

| Old Path (.tif) | Reborn Path (.png) | Ukuran |
|-----------------|-------------------|--------|
| ./data/interface/2dimage/image/login_bar01_01.tif | assets/textures/ui/login_bar01_01.png | 2×128 |
| ./data/interface/2dimage/image/login_bar01_02.tif | assets/textures/ui/login_bar01_02.png | 256×128 |
| ./data/interface/2dimage/image/ITEM_MATERIAL_WOOD_01.TIF | assets/textures/ui/ITEM_MATERIAL_WOOD_01.png | 64×64 |
| ./data/interface/2dimage/image/ITEM_MATERIAL_ANIMAL_01.TIF | assets/textures/ui/ITEM_MATERIAL_ANIMAL_01.png | 64×64 |
| ./data/interface/2dimage/image/ITEM_MATERIAL_ANIFEED_01.TIF | assets/textures/ui/ITEM_MATERIAL_ANIFEED_01.png | 64×64 |
| ./data/interface/2dimage/image/Localization_US.tif | assets/textures/ui/Localization_US.png | 32×32 |
| ./data/interface/2dimage/image/Localization_PH.tif | assets/textures/ui/Localization_PH.png | 32×32 |
| ./data/interface/2dimage/image/Keyboard.tif | assets/textures/ui/Keyboard.png | 512×256 |

## Cara Fix

### Opsi A: Buat placeholder (paling cepat)
```python
from PIL import Image
files = {
    "login_bar01_01.png": (2, 128),
    "login_bar01_02.png": (256, 128),
    "ITEM_MATERIAL_WOOD_01.png": (64, 64),
    "ITEM_MATERIAL_ANIMAL_01.png": (64, 64),
    "ITEM_MATERIAL_ANIFEED_01.png": (64, 64),
    "Localization_US.png": (32, 32),
    "Localization_PH.png": (32, 32),
    "Keyboard.png": (512, 256),
}
import os
path = "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/textures/ui/"
for name, (w, h) in files.items():
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    img.save(os.path.join(path, name))
    print(f"Created {name} ({w}x{h})")
```

### Opsi B: Jika PIL tidak tersedia — buat file minimal pakai Python stdlib
```python
import struct, zlib
def create_png(path, w, h):
    def chunk(ctype, data):
        c = ctype + data
        return struct.pack('>I', len(data)) + c + struct.pack('>I', zlib.crc32(c) & 0xffffffff)
    raw = b''
    for y in range(h):
        raw += b'\x00' + b'\x00\x00\x00\x00' * w
    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n')
        f.write(chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)))
        f.write(chunk(b'IDAT', zlib.compress(raw)))
        f.write(chunk(b'IEND', b''))
```

## Verifikasi

Setelah file dibuat, jalankan ulang audit:
```bash
find assets/textures/ui/ -name "login_bar01*" -o -name "ITEM_MATERIAL*" -o -name "Localization_*" -o -name "Keyboard.png"
```

## ✅ Kembalikan: "OMEGA-01 done: 8 missing textures created — image_path.bin.txt references resolved"
