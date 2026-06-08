#!/usr/bin/env python3
"""Convert .hfl heightfield files to .hgt text format"""
import struct, sys, os, glob

def convert_hfl_to_hgt(hfl_path, hgt_path):
    with open(hfl_path, 'rb') as f:
        data = f.read()
    for offset in [0x40, 0x64, 0x80, 0x100]:
        if offset + 8 <= len(data):
            w, h = struct.unpack_from('<II', data, offset)
            if 16 <= w <= 4096 and 16 <= h <= 4096:
                hgt_start = offset + 8
                floats = (len(data) - hgt_start) // 4
                if floats >= w * h:
                    vals = struct.unpack_from(f'<{w*h}f', data, hgt_start)
                    with open(hgt_path, 'w') as out:
                        out.write(f'{w} {h}\n')
                        for i, v in enumerate(vals):
                            out.write(f'{v:.2f}')
                            out.write('\n' if (i+1) % 16 == 0 else ' ')
                    return True
    return False

def main():
    import argparse
    parser = argparse.ArgumentParser(description='Convert HFL heightmaps to HGT')
    parser.add_argument('--all', action='store_true', help='Process all heightmaps')
    parser.add_argument('--src', default=os.environ.get('LUNA_LEGACY_MAP_SRC', str(Path(__file__).parent.parent.parent.parent / 'Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/map/')), help='Source directory')
    parser.add_argument('--dst', default=os.environ.get('LUNA_REBORN_ROOT', str(Path(__file__).parent.parent.parent)) + '/assets/maps', help='Output directory')
    parser.add_argument('--file', type=str, help='Single file to convert')
    args = parser.parse_args()

    src = args.src
    dst = args.dst
    if args.file:
        base = os.path.basename(args.file).replace('.hfl', '.hgt')
        out = os.path.join(dst, base)
        os.makedirs(dst, exist_ok=True)
        if convert_hfl_to_hgt(args.file, out):
            print(f'  OK: {os.path.basename(args.file)}')
        else:
            print(f'  FAIL: {os.path.basename(args.file)}')
            return 1
        return 0
    ok = fail = skip = 0
    for f in glob.glob(os.path.join(src, '*.hfl')):
        base = os.path.basename(f).replace('.hfl', '.hgt')
        out = os.path.join(dst, base)
        if os.path.exists(out):
            skip += 1; continue
        if convert_hfl_to_hgt(f, out):
            ok += 1; print(f'  OK: {os.path.basename(f)}')
        else:
            fail += 1; print(f'  FAIL: {os.path.basename(f)}')
    print(f'\n=== Heightmap Conversion Complete ===\n  Converted: {ok}\n  Failed: {fail}\n  Skipped: {skip}')
    print(f'  Total: {ok+fail+skip}')

if __name__ == '__main__':
    main()
