#!/usr/bin/env python3
"""
LUNA Plus Reborn — Heightmap Converter
Converts .hfl heightfield files to .hgt text format
"""

import os
import sys
import struct
import argparse
from pathlib import Path

SRC = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map")
DST = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets_converted/heightmaps")

def convert_hfl(input_path: str, output_dir: str) -> tuple:
    name = os.path.splitext(os.path.basename(input_path))[0]
    try:
        with open(input_path, 'rb') as f:
            data = f.read()
    except Exception as e:
        return (input_path, 'fail', f"read error: {e}")

    sz = len(data)
    found = False
    result = None

    for hdr in [64, 128, 256, 32, 16, 512]:
        if hdr >= sz:
            continue
        raw = data[hdr:]
        n = len(raw) // 4
        if n == 0:
            continue
        for w in [32, 64, 128, 256, 512, 1024, 2048]:
            h = n // w
            if n > 0 and w * h == n and h >= 8:
                heights = []
                hmin, hmax = 1e9, -1e9
                valid = True
                for y in range(h):
                    row = []
                    for x in range(w):
                        idx = (y * w + x) * 4
                        if idx + 4 > len(raw):
                            valid = False
                            break
                        v = struct.unpack_from('<f', raw, idx)[0]
                        if abs(v) > 100000:
                            v = 0
                        row.append(v)
                        hmin = min(hmin, v)
                        hmax = max(hmax, v)
                    if not valid:
                        break
                    heights.append(row)

                if heights and len(heights) > 5 and valid:
                    os.makedirs(output_dir, exist_ok=True)
                    txt = os.path.join(output_dir, f"{name}.hgt")
                    with open(txt, 'w') as f:
                        f.write(f"{w} {len(heights)}\n")
                        for row in heights:
                            f.write(' '.join(f'{v:.2f}' for v in row) + '\n')
                    result = (input_path, 'ok', f"{w}x{len(heights)} hdr={hdr} range=[{hmin:.1f},{hmax:.1f}]")
                    found = True
                    break
        if found:
            break

    if not found:
        return (input_path, 'skip', f"no grid pattern found (size={sz})")
    return result

def main():
    parser = argparse.ArgumentParser(description='Convert HFL heightmaps to text')
    parser.add_argument('--all', action='store_true', help='Process all heightmaps')
    parser.add_argument('--file', type=str, help='Single file to convert')
    args = parser.parse_args()

    if args.file:
        path, status, msg = convert_hfl(args.file, str(DST))
        print(f"{status}: {os.path.basename(path)} ({msg})")
        return 0 if status == 'ok' else 1

    if not SRC.exists():
        print(f"Source directory not found: {SRC}")
        return 1

    files = sorted([str(f) for f in SRC.glob('*.hfl')])

    if not files:
        print("No .hfl files found.")
        return 0

    total = len(files)
    success = 0
    failed = 0
    skipped = 0

    print(f"Found {total} heightmap files")
    print(f"Output: {DST}")
    print()

    for f in files:
        path, status, msg = convert_hfl(f, str(DST))
        name = os.path.basename(path)
        if status == 'ok':
            success += 1
            print(f"  OK  {name} ({msg})")
        elif status == 'skip':
            skipped += 1
            print(f"  SKIP {name} ({msg})")
        else:
            failed += 1
            print(f"  FAIL {name} ({msg})")

    print()
    print(f"=== Heightmap Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")

    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
