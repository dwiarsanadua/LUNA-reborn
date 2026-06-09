#!/usr/bin/env python3
"""
Extract missing UI skin textures from legacy PAK files.
Looks for C_launcher.png, close.png, min.png, and other skin textures.
"""
import os, sys, struct, glob
from pathlib import Path

# Cari legacy assets
LEGACY_DIRS = [
    "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked",
    "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS",
]
DST = "/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/textures/ui/"

# Texture names that the UI skin system needs
NEEDED_TEXTURES = [
    "close", "close_f", "min", "min_f",
    "C_launcher", "Launcher_01_01",
    "login_bar00", "login_bar01",
    "btn_normal", "btn_hover", "btn_pressed",
    "title_left", "title_center", "title_right",
    "border_left", "border_right", "border_top", "border_bottom",
    "corner_tl", "corner_tr", "corner_bl", "corner_br",
    "body_bg",
    "scroll_track", "scroll_thumb",
    "tab_active", "tab_inactive", "tab_hover",
    "slot_normal", "slot_hover", "slot_highlight",
]

def convert_to_png(src, dst):
    """Convert image to PNG using PIL or sips."""
    try:
        from PIL import Image
        img = Image.open(src)
        img.save(dst, "PNG")
        return True
    except ImportError:
        # Fallback to sips (macOS)
        ret = os.system(f'sips -s format png "{src}" --out "{dst}" 2>/dev/null')
        return ret == 0

def create_placeholder(dst):
    """Create a 32x32 solid color placeholder PNG."""
    try:
        from PIL import Image
        img = Image.new('RGBA', (32, 32), (100, 100, 150, 200))
        img.save(dst)
        return True
    except ImportError:
        # Fallback with raw PNG bytes
        import struct, zlib
        width, height = 32, 32
        raw_data = b''
        for y in range(height):
            raw_data += b'\x00'  # filter byte
            for x in range(width):
                raw_data += struct.pack('BBBB', 100, 100, 150, 200)
        def chunk(chunk_type, data):
            c = chunk_type + data
            return struct.pack('>I', len(data)) + c + struct.pack('>I', zlib.crc32(c) & 0xffffffff)
        ihdr = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)  # 8-bit RGBA
        png = b'\x89PNG\r\n\x1a\n'
        png += chunk(b'IHDR', ihdr)
        png += chunk(b'IDAT', zlib.compress(raw_data))
        png += chunk(b'IEND', b'')
        with open(dst, 'wb') as f:
            f.write(png)
        return True

def main():
    os.makedirs(DST, exist_ok=True)
    found = 0
    missing = []

    # Search through all legacy directories recursively for image files
    for base in LEGACY_DIRS:
        if not os.path.isdir(base):
            continue
        for root, dirs, files in os.walk(base):
            for f in files:
                name_lower = f.lower()
                for needed in NEEDED_TEXTURES:
                    if needed.lower() in name_lower and name_lower.endswith(('.png', '.dds', '.tga', '.tif')):
                        src = os.path.join(root, f)
                        dst = os.path.join(DST, needed + ".png")
                        if not os.path.exists(dst):
                            if f.lower().endswith('.png'):
                                import shutil
                                shutil.copy2(src, dst)
                                print(f"  FOUND: {needed} -> {dst}")
                                found += 1
                            else:
                                if convert_to_png(src, dst):
                                    print(f"  FOUND (converted): {needed} -> {dst}")
                                    found += 1
                        else:
                            print(f"  SKIP (already exists): {needed}")
                            found += 1
                        break

    print(f"\nFound {found} textures")

    # Generate placeholder for truly missing textures
    for needed in NEEDED_TEXTURES:
        dst = os.path.join(DST, needed + ".png")
        if not os.path.exists(dst):
            missing.append(needed)
            create_placeholder(dst)
            print(f"  PLACEHOLDER: {needed} -> {dst}")

    if missing:
        print(f"Missing (placeholders created): {', '.join(missing)}")
    else:
        print("All textures found, no placeholders needed")

if __name__ == "__main__":
    main()
