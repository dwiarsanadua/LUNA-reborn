#!/usr/bin/env python3
"""HFL Heightfield Converter"""
import struct, sys, os

def convert_hfl(input_path, output_dir):
    name = os.path.splitext(os.path.basename(input_path))[0]
    with open(input_path, 'rb') as f:
        data = f.read()
    sz = len(data)
    
    # Search for height grid by trying different header sizes
    found = False
    for hdr in [64, 128, 256, 32, 512]:
        raw = data[hdr:]
        n = len(raw) // 4
        for w in [32, 64, 128, 256, 512, 1024]:
            h = n // w
            if n > 0 and w * h == n and h >= 8:
                heights = []
                hmin, hmax = 1e9, -1e9
                for y in range(h):
                    row = []
                    for x in range(w):
                        idx = (y * w + x) * 4
                        if idx + 4 > len(raw): break
                        v = struct.unpack_from('<f', raw, idx)[0]
                        if abs(v) > 100000: v = 0
                        row.append(v)
                        hmin = min(hmin, v)
                        hmax = max(hmax, v)
                    if row: heights.append(row)
                
                if heights and len(heights) > 5:
                    print(f"  {name}: {w}x{len(heights)} hdr={hdr} range=[{hmin:.1f},{hmax:.1f}]")
                    txt = os.path.join(output_dir, f"{name}.hgt")
                    with open(txt, 'w') as f:
                        f.write(f"{w} {len(heights)}\n")
                        for row in heights:
                            f.write(' '.join(f'{v:.2f}' for v in row) + '\n')
                    found = True
                    break
        if found: break
    
    if not found:
        print(f"  {name}: no grid pattern found (size={sz})")
        return False
    return True

def main():
    src = "/Users/macbookair/PRIBADI/luna-plus-master/assets/unpacked/map"
    dst = "/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets_converted/heightmaps"
    os.makedirs(dst, exist_ok=True)
    files = sorted([f for f in os.listdir(src) if f.endswith('.hfl')])
    ok = 0
    for f in files[:10]:
        if convert_hfl(os.path.join(src, f), dst): ok += 1
    print(f"\n{ok}/{len(files[:10])} OK")

if __name__ == "__main__":
    main()
