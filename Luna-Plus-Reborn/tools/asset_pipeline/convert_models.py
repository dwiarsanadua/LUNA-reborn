#!/usr/bin/env python3
"""
LUNA Plus Reborn — Model Converter
Converts .mod model files to .obj format
"""

import os
import sys
import struct
import argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
import traceback

OT_MATERIAL = 0x00F00000
OT_MESH     = 0xF4000000
OT_BONE     = 0xF5000000
BASE_SZ     = 324
FG_HDR_SZ   = 24

SRC_BASE_DEFAULT = os.environ.get('LUNA_LEGACY_SRC', str(Path(__file__).parent.parent.parent.parent / 'Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked'))
DST_BASE_DEFAULT = os.environ.get('LUNA_REBORN_ROOT', str(Path(__file__).parent.parent.parent)) + '/assets/models'

def convert_mod(input_path: str, output_dir: str) -> tuple:
    name = os.path.splitext(os.path.basename(input_path))[0]
    cat = os.path.basename(os.path.dirname(input_path))
    
    dst_dir = os.path.join(output_dir, cat)
    os.makedirs(dst_dir, exist_ok=True)
    
    try:
        with open(input_path, 'rb') as f:
            data = f.read()
    except Exception as e:
        return (input_path, 'fail', f"read error: {e}")
    
    if len(data) < 28:
        return (input_path, 'skip', 'too small')
    
    try:
        ver, obj_num, mtl_num = struct.unpack_from('<III', data, 0)[:3]
    except:
        return (input_path, 'fail', 'invalid header')
    
    offset = 28
    for i in range(mtl_num):
        try:
            mtype, msize = struct.unpack_from('<II', data, offset)
            offset += 8 + msize
        except:
            break
    
    all_verts = []
    all_uvs = []
    all_faces = []
    
    try:
        for obj_idx in range(obj_num):
            if offset + 8 > len(data):
                break
            otype, osize = struct.unpack_from('<II', data, offset)
            offset += 8
            obj_end = offset + osize
            
            if otype == OT_MESH:
                child_num = struct.unpack_from('<I', data, offset + 188)[0]
                offset += BASE_SZ + child_num * 4
                
                if offset + 7*4 + 24 > len(data):
                    offset = obj_end
                    continue
                
                mhdr = struct.unpack_from('<7I', data, offset)
                max_vn, vn, orig_vn, ext_vn, tex_vn, mtl_idx, fg_num = mhdr
                
                if vn == 0 or vn > 50000 or fg_num == 0:
                    offset = obj_end
                    continue
                
                offset += 7*4
                offset += 4 + 4 + 12 + 4
                
                base_v = len(all_verts)
                
                for i in range(vn):
                    x, y, z = struct.unpack_from('<fff', data, offset)
                    all_verts.append((x, y, z))
                    offset += 12
                
                base_uv = len(all_uvs)
                for i in range(tex_vn):
                    u, v = struct.unpack_from('<ff', data, offset)
                    all_uvs.append((u, 1.0 - v))
                    offset += 8
                while len(all_uvs) < len(all_verts):
                    all_uvs.append((0, 0))
                
                offset += ext_vn * 4
                
                for fg in range(fg_num):
                    if offset + FG_HDR_SZ > len(data):
                        break
                    h = struct.unpack_from('<6I', data, offset)
                    fg_mtl, fg_idx, fg_faces, fg_max, fg_vi, fg_luv = h
                    
                    if fg_faces == 0 or fg_faces > 50000:
                        break
                    
                    idx_off = offset + FG_HDR_SZ
                    for f in range(fg_faces):
                        if idx_off + f*6 + 6 > len(data):
                            break
                        a, b, c = struct.unpack_from('<HHH', data, idx_off + f*6)
                        if a == b or b == c or a == c:
                            continue
                        if a < vn and b < vn and c < vn:
                            all_faces.append((base_v + a, base_v + b, base_v + c, fg_mtl))
                    
                    offs = FG_HDR_SZ + fg_faces * 6
                    if fg_luv > 0:
                        offs += fg_luv * 8
                    offset += offs
            
            offset = max(offset, obj_end)
    except Exception as e:
        return (input_path, 'fail', f"parse error: {e}")
    
    if not all_verts or not all_faces:
        return (input_path, 'skip', 'no geometry')
    
    dst = os.path.join(dst_dir, f"{name}.obj")
    try:
        with open(dst, 'w') as f:
            f.write(f"# MOD converted: {os.path.basename(input_path)}\n")
            f.write(f"# {len(all_verts)} verts, {len(all_faces)} faces\n\n")
            for v in all_verts:
                f.write(f"v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n")
            f.write("\n")
            for uv in all_uvs[:len(all_verts)]:
                f.write(f"vt {uv[0]:.6f} {uv[1]:.6f}\n")
            f.write("\n")
            for fa in all_faces:
                f.write(f"f {fa[0]+1}/{fa[0]+1} {fa[1]+1}/{fa[1]+1} {fa[2]+1}/{fa[2]+1}\n")
    except Exception as e:
        return (input_path, 'fail', f"write error: {e}")
    
    return (input_path, 'ok', f"{len(all_verts)}v {len(all_faces)}t")

def main():
    parser = argparse.ArgumentParser(description='Convert MOD models to OBJ')
    parser.add_argument('--all', action='store_true', help='Process all model categories')
    parser.add_argument('--category', type=str, help='Specific category (character, monster, npc, etc.)')
    parser.add_argument('--workers', type=int, default=4, help='Parallel workers')
    parser.add_argument('--file', type=str, help='Single file to convert')
    parser.add_argument('--src', default=SRC_BASE_DEFAULT, help='Source base directory')
    parser.add_argument('--dst', default=DST_BASE_DEFAULT, help='Output base directory')
    args = parser.parse_args()

    src_base = Path(args.src)
    dst_base = Path(args.dst)

    SOURCE_DIRS_LOCAL = {
        'character': src_base / 'character',
        'monster': src_base / 'monster',
        'npc': src_base / 'npc',
        'effect': src_base / 'effect',
        'farm': src_base / 'farm',
        'housing': src_base / 'housing',
        'map': src_base / 'map',
    }

    if args.file:
        path, status, msg = convert_mod(args.file, str(dst_base))
        print(f"{status}: {os.path.basename(path)} ({msg})")
        return 0 if status == 'ok' else 1

    categories = [args.category] if args.category else list(SOURCE_DIRS_LOCAL.keys())

    all_mods = []
    for cat in categories:
        src_dir = SOURCE_DIRS_LOCAL.get(cat)
        if not src_dir or not src_dir.exists():
            print(f"Source directory not found: {src_dir}")
            continue
        mods = sorted([str(f) for f in src_dir.glob('*.mod')])
        print(f"{cat}: {len(mods)} .mod files")
        all_mods.extend(mods)

    if not all_mods:
        print("No .mod files found.")
        return 0

    total = len(all_mods)
    success = 0
    failed = 0
    skipped = 0
    failed_list = []

    print(f"\nTotal: {total} models to convert")
    print(f"Output: {dst_base}")
    print()

    if args.workers > 1:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(convert_mod, f, str(dst_base)): f for f in all_mods}
            for future in as_completed(futures):
                path, status, msg = future.result()
                name = os.path.basename(path)
                if status == 'ok':
                    success += 1
                    print(f"  OK  {name} ({msg})")
                elif status == 'skip':
                    skipped += 1
                    if 'too small' not in msg:
                        print(f"  SKIP {name} ({msg})")
                else:
                    failed += 1
                    failed_list.append(path)
                    print(f"  FAIL {name} ({msg})")
    else:
        for f in all_mods:
            path, status, msg = convert_mod(f, str(dst_base))
            name = os.path.basename(path)
            if status == 'ok':
                success += 1
                print(f"  OK  {name} ({msg})")
            elif status == 'skip':
                skipped += 1
            else:
                failed += 1
                failed_list.append(path)
                print(f"  FAIL {name} ({msg})")

    print()
    print(f"=== Model Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")
    if failed_list:
        with open(os.path.join(os.path.dirname(__file__), 'model_errors.log'), 'w') as f:
            for p in failed_list:
                f.write(p + '\n')

    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
