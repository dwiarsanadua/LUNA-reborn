#!/usr/bin/env python3
"""
LUNA Plus Reborn — Texture Converter
Converts .dds/.tif/.tga textures to .png
"""

import os
import sys
import shutil
import subprocess
import argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

def find_tool(name):
    return shutil.which(name) or os.environ.get(f'{name.upper()}_PATH', f'/opt/homebrew/bin/{name}')

def convert_dds(src: Path, dst: Path) -> bool:
    dst.parent.mkdir(parents=True, exist_ok=True)
    ffmpeg = find_tool('ffmpeg')
    cmd = [ffmpeg, '-y', '-i', str(src), '-frames:v', '1', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def convert_tif_tga(src: Path, dst: Path) -> bool:
    dst.parent.mkdir(parents=True, exist_ok=True)
    sips = find_tool('sips')
    cmd = [sips, '-s', 'format', 'png', str(src), '--out', str(dst)]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def convert_file(src_path: str, src_base: Path, dst_base: Path) -> tuple:
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
        rel = src.relative_to(src_base)
    except ValueError:
        return (src_path, 'fail', 'outside src_base')
    dst = dst_base / rel.with_suffix(f'.{new_ext}')
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
    parser.add_argument('--input', type=str, help='Input directory')
    parser.add_argument('--output', type=str, help='Output directory')
    parser.add_argument('--workers', type=int, default=4, help='Parallel workers')
    parser.add_argument('--file', type=str, help='Single file to convert')
    args = parser.parse_args()

    default_src = os.environ.get('LUNA_LEGACY_SRC', str(Path(__file__).parent.parent.parent.parent / 'Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets'))
    default_dst = os.environ.get('LUNA_REBORN_ROOT', str(Path(__file__).parent.parent.parent)) + '/assets/textures'
    src_base = Path(args.input) if args.input else Path(default_src)
    dst_base = Path(args.output) if args.output else Path(default_dst)

    if args.file:
        path, status, err = convert_file(args.file, src_base, dst_base)
        print(f"{status}: {path}" + (f" ({err})" if err else ""))
        return 0 if status == 'ok' else 1

    if not src_base.exists():
        print(f"Source directory not found: {src_base}")
        return 1

    files = []
    for ext in ['.dds', '.tif', '.TIF', '.tga', '.TGA']:
        files.extend(src_base.rglob(f'*{ext}'))
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
    print(f"Output: {dst_base}")
    print()

    if args.workers > 1 and not args.file:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(convert_file, str(f), src_base, dst_base): f for f in files}
            for future in as_completed(futures):
                path, status, err = future.result()
                rel = os.path.relpath(path, str(src_base))
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
            path, status, err = convert_file(str(f), src_base, dst_base)
            rel = os.path.relpath(path, str(src_base))
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
