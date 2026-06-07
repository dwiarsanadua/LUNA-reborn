#!/usr/bin/env python3
"""
LUNA Plus Reborn — Asset Converter (Phase 0.6)
Converts game assets to modern formats:
  .dds → .png  (via ffmpeg)
  .tif → .png  (via sips)
  .tga → .png  (via sips)
"""

import os
import sys
import subprocess
from pathlib import Path

SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets")
DST = Path("/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets_converted")

TOOLS = {
    'sips': '/usr/bin/sips',
    'ffmpeg': '/opt/homebrew/bin/ffmpeg',
}

def convert_dds(src: Path, dst: Path) -> bool:
    """Convert DDS to PNG using ffmpeg."""
    dst.parent.mkdir(parents=True, exist_ok=True)
    cmd = [TOOLS['ffmpeg'], '-y', '-i', str(src), '-frames:v', '1', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def convert_tif_tga(src: Path, dst: Path) -> bool:
    """Convert TIF/TGA to PNG using sips."""
    dst.parent.mkdir(parents=True, exist_ok=True)
    cmd = [TOOLS['sips'], '-s', 'format', 'png', str(src), '--out', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0


def main():
    extensions = {
        '.dds': ('png', convert_dds),
        '.tif': ('png', convert_tif_tga),
        '.TIF': ('png', convert_tif_tga),
        '.tga': ('png', convert_tif_tga),
        '.TGA': ('png', convert_tif_tga),
    }

    total = 0
    success = 0
    failed = 0
    skipped = 0

    print(f"Scanning {SRC} for assets to convert...")
    files = []
    for ext in extensions:
        files.extend(SRC.rglob(f'*{ext}'))
    files = list(set(files))  # deduplicate

    print(f"Found {len(files)} files to convert")
    print()

    for src in files:
        ext = src.suffix
        if ext == '.TIF': ext = '.tif'
        if ext == '.TGA': ext = '.tga'
        new_ext, converter = extensions[ext]
        rel = src.relative_to(SRC)
        dst = DST / rel.with_suffix(f'.{new_ext}')

        if dst.exists():
            skipped += 1
            continue

        total += 1
        try:
            if converter(src, dst):
                success += 1
                print(f"  ✅ {rel}")
            else:
                failed += 1
                print(f"  ❌ {rel}")
        except Exception as e:
            failed += 1
            print(f"  ❌ {rel} ({e})")

    print()
    print(f"=== Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")
    print(f"  Output:    {DST}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
