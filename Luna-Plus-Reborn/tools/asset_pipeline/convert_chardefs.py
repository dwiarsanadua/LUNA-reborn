#!/usr/bin/env python3
"""
LUNA Plus Reborn — Character Definition Converter
Processes .chx character definition files
Converts them to JSON format for the modern engine
"""

import os
import sys
import json
import struct
import argparse
from pathlib import Path

SRC_BASE = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked")
DST_BASE = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/characters")

SOURCE_CATEGORIES = ['character', 'monster', 'npc', 'effect']

def parse_chx(input_path: str) -> dict:
    """Parse a .chx character definition file into a dict."""
    with open(input_path, 'rb') as f:
        data = f.read()

    result = {
        'file': os.path.basename(input_path),
        'size': len(data),
        'sections': [],
    }

    offset = 0
    section_id = 0
    while offset < len(data):
        if offset + 8 > len(data):
            break
        stype, ssize = struct.unpack_from('<II', data, offset)
        offset += 8
        end = min(offset + ssize, len(data))

        section_info = {
            'type': f"0x{stype:08X}",
            'size': ssize,
        }

        if stype == 0xFFFFFFFF:
            section_info['name'] = 'terminator'
            result['sections'].append(section_info)
            break

        if stype == 0x00F00000:
            section_info['name'] = 'material'
        elif stype == 0xF4000000:
            section_info['name'] = 'mesh'
        elif stype == 0xF5000000:
            section_info['name'] = 'bone'
        else:
            section_info['name'] = 'unknown'

        if section_info['name'] == 'mesh' and end - offset >= 28:
            raw = data[offset:end]
            if len(raw) >= 28:
                vals = struct.unpack_from('<7I', raw, 0)
                section_info['max_vertices'] = vals[0]
                section_info['vertices'] = vals[1]
                section_info['orig_vertices'] = vals[2]
                section_info['ext_vertices'] = vals[3]
                section_info['tex_vertices'] = vals[4]
                section_info['material_index'] = vals[5]
                section_info['face_groups'] = vals[6]

        result['sections'].append(section_info)
        offset = end
        section_id += 1

    return result

def convert_chx(input_path: str, output_dir: str) -> tuple:
    name = os.path.splitext(os.path.basename(input_path))[0]
    cat = os.path.basename(os.path.dirname(os.path.dirname(input_path)))

    try:
        parsed = parse_chx(input_path)
    except Exception as e:
        return (input_path, 'fail', f"parse error: {e}")

    dst_dir = os.path.join(output_dir, cat)
    os.makedirs(dst_dir, exist_ok=True)

    dst = os.path.join(dst_dir, f"{name}.json")
    try:
        with open(dst, 'w') as f:
            json.dump(parsed, f, indent=2)
    except Exception as e:
        return (input_path, 'fail', f"write error: {e}")

    return (input_path, 'ok', f"{parsed['size']}b, {len(parsed['sections'])} sections")

def main():
    parser = argparse.ArgumentParser(description='Convert CHX character defs to JSON')
    parser.add_argument('--all', action='store_true', help='Process all character defs')
    parser.add_argument('--category', type=str, help='Specific category')
    parser.add_argument('--file', type=str, help='Single file to convert')
    args = parser.parse_args()

    if args.file:
        path, status, msg = convert_chx(args.file, str(DST_BASE))
        print(f"{status}: {os.path.basename(path)} ({msg})")
        return 0 if status == 'ok' else 1

    categories = [args.category] if args.category else SOURCE_CATEGORIES

    all_chx = []
    for cat in categories:
        src_dir = SRC_BASE / cat
        if not src_dir.exists():
            print(f"Source directory not found: {src_dir}")
            continue
        chx_files = sorted([str(f) for f in src_dir.glob('*.chx')])
        print(f"{cat}: {len(chx_files)} .chx files")
        all_chx.extend(chx_files)

    if not all_chx:
        print("No .chx files found.")
        return 0

    total = len(all_chx)
    success = 0
    failed = 0
    skipped = 0

    print(f"\nTotal: {total} character defs to convert")
    print(f"Output: {DST_BASE}")
    print()

    for f in all_chx:
        path, status, msg = convert_chx(f, str(DST_BASE))
        name = os.path.basename(path)
        if status == 'ok':
            success += 1
            print(f"  OK  {name} ({msg})")
        elif status == 'skip':
            skipped += 1
        else:
            failed += 1
            print(f"  FAIL {name} ({msg})")

    print()
    print(f"=== Character Definition Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")

    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
