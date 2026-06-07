#!/usr/bin/env python3
"""
LUNA Plus Reborn — Texture Converter
Converts .dds/.tif/.tga textures to .png
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/textures")
DST = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/textures")

TOOLS = {
    'ffmpeg': '/opt/homebrew/bin/ffmpeg',
    'sips': '/usr/bin/sips',
}

def convert_dds(src: Path, dst: Path) -> bool:
    dst.parent.mkdir(parents=True, exist_ok=True)
    cmd = [TOOLS['ffmpeg'], '-y', '-i', str(src), '-frames:v', '1', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def convert_tif_tga(src: Path, dst: Path) -> bool:
    dst.parent.mkdir(parents=True, exist_ok=True)
    cmd = [TOOLS['sips'], '-s', 'format', 'png', str(src), '--out', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def convert_file(src_path: str) -> tuple:
    src = Path(src_path)
    ext = src.suffix.lower()
    converters = {
        '.dds': ('png', convert_dds),
        '.tif': ('png', convert_tif_tga),
        '.tga': ('png', convert_tif_tga),
    }
    if ext not in converters:
        return (src_path, 'skip', None)
    new_ext, converter = converters[ext]
    try:
        rel = src.relative_to(SRC)
    except ValueError:
        return (src_path, 'fail', 'outside SRC')
    dst = DST / rel.with_suffix(f'.{new_ext}')
    if dst.exists():
        return (src_path, 'skip', 'exists')
    try:
        if converter(src, dst):
            return (src_path, 'ok', None)
        else:
            return (src_path, 'fail', 'converter returned False')
    except Exception as e:
        return (src_path, 'fail', str(e))

def main():
    parser = argparse.ArgumentParser(description='Convert textures to PNG')
    parser.add_argument('--all', action='store_true', help='Process all textures')
    parser.add_argument('--workers', type=int, default=4, help='Parallel workers')
    parser.add_argument('--file', type=str, help='Single file to convert')
    args = parser.parse_args()

    if args.file:
        path, status, err = convert_file(args.file)
        print(f"{status}: {path}" + (f" ({err})" if err else ""))
        return 0 if status == 'ok' else 1

    if not SRC.exists():
        print(f"Source directory not found: {SRC}")
        return 1

    files = []
    for ext in ['.dds', '.tif', '.TIF', '.tga', '.TGA']:
        files.extend(SRC.rglob(f'*{ext}'))
    files = sorted(set(files))

    if not files:
        print("No texture files found.")
        return 0

    total = len(files)
    success = 0
    failed = 0
    skipped = 0
    failed_list = []

    print(f"Found {total} texture files to convert")
    print(f"Output: {DST}")
    print()

    if args.workers > 1 and not args.file:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(convert_file, str(f)): f for f in files}
            for future in as_completed(futures):
                path, status, err = future.result()
                rel = os.path.relpath(path, str(SRC))
                if status == 'ok':
                    success += 1
                    print(f"  OK  {rel}")
                elif status == 'skip':
                    skipped += 1
                else:
                    failed += 1
                    failed_list.append(rel)
                    print(f"  FAIL {rel}" + (f" ({err})" if err else ""))
    else:
        for f in files:
            path, status, err = convert_file(str(f))
            rel = os.path.relpath(path, str(SRC))
            if status == 'ok':
                success += 1
                print(f"  OK  {rel}")
            elif status == 'skip':
                skipped += 1
            else:
                failed += 1
                failed_list.append(rel)
                print(f"  FAIL {rel}" + (f" ({err})" if err else ""))

    print()
    print(f"=== Texture Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")
    if failed_list:
        print(f"  Failed files:")
        for f in failed_list:
            print(f"    - {f}")

    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
