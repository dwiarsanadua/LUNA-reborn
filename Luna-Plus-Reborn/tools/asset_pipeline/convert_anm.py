#!/usr/bin/env python3
"""
LUNA Plus Reborn — Animation Converter
Converts .anm binary animation files to .anm.json format
"""
import struct
import sys
import os
import json
import argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

SRC_DEFAULT = os.environ.get('LUNA_LEGACY_SRC', str(Path(__file__).parent.parent.parent.parent / 'Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked'))
DST_DEFAULT = os.environ.get('LUNA_REBORN_ROOT', str(Path(__file__).parent.parent.parent)) + '/assets/animations'

def read_str(f, length):
    try:
        data = f.read(length)
        return data.decode('euc-kr', errors='replace').split('\x00')[0].strip()
    except:
        return ""

class AnmConverter:
    def __init__(self, file_path):
        self.file_path = file_path
        self.header = {}
        self.motion_objects = []

    def parse(self):
        if not os.path.exists(self.file_path):
            return False

        try:
            with open(self.file_path, 'rb') as f:
                header_data = f.read(160)
                if len(header_data) < 160: return False

                h = struct.unpack('<IIIIIII 4x 128s', header_data)
                self.header = {
                    'version': h[0],
                    'ticks_per_frame': h[1],
                    'first_frame': h[2],
                    'last_frame': h[3],
                    'frame_speed': h[4],
                    'obj_num': h[5],
                    'step': h[6],
                    'name': h[7].decode('euc-kr', errors='replace').split('\x00')[0].strip()
                }

                for i in range(self.header['obj_num']):
                    obj_type_data = f.read(4)
                    if not obj_type_data: break
                    obj_type = struct.unpack('<I', obj_type_data)[0]
                    obj_size = struct.unpack('<I', f.read(4))[0]
                    obj_start = f.tell()

                    mo_hdr_data = f.read(152)
                    if len(mo_hdr_data) < 152: break
                    mo_h = struct.unpack('<IIIII 128s I', mo_hdr_data)

                    mo = {
                        'index': mo_h[0],
                        'rot_keys_num': mo_h[1],
                        'pos_keys_num': mo_h[2],
                        'scale_keys_num': mo_h[3],
                        'morph_keys_num': mo_h[4],
                        'name': mo_h[5].decode('euc-kr', errors='replace').split('\x00')[0].strip(),
                        'flag': mo_h[6],
                        'pos_keys': [],
                        'rot_keys': [],
                        'scale_keys': []
                    }

                    for _ in range(mo['pos_keys_num']):
                        data = f.read(20)
                        if len(data) < 20: break
                        k = struct.unpack('<II fff', data)
                        mo['pos_keys'].append({'ticks': k[0], 'frame': k[1], 'pos': [k[2], k[3], k[4]]})

                    for _ in range(mo['rot_keys_num']):
                        data = f.read(24)
                        if len(data) < 24: break
                        k = struct.unpack('<II ffff', data)
                        mo['rot_keys'].append({'ticks': k[0], 'frame': k[1], 'quat': [k[2], k[3], k[4], k[5]]})

                    for _ in range(mo['scale_keys_num']):
                        data = f.read(36)
                        if len(data) < 36: break
                        k = struct.unpack('<II fff fff f', data)
                        mo['scale_keys'].append({'ticks': k[0], 'frame': k[1], 'scale': [k[2], k[3], k[4]]})

                    f.seek(obj_start + obj_size)
                    self.motion_objects.append(mo)
            return True
        except Exception as e:
            print(f"Error parsing {self.file_path}: {e}")
            return False

    def save_json(self, output_path):
        try:
            data = {
                'header': self.header,
                'objects': self.motion_objects
            }
            with open(output_path, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving JSON {output_path}: {e}")
            return False


def convert_anm_file(input_path, output_dir):
    name = os.path.splitext(os.path.basename(input_path))[0]
    out = os.path.join(output_dir, f"{name}.anm.json")
    if os.path.exists(out):
        return (input_path, 'skip', 'exists')
    conv = AnmConverter(input_path)
    if conv.parse():
        conv.save_json(out)
        return (input_path, 'ok', None)
    return (input_path, 'fail', 'parse failed')


def main():
    parser = argparse.ArgumentParser(description='Convert ANM animations to JSON')
    parser.add_argument('--all', action='store_true', help='Batch convert all .anm files')
    parser.add_argument('--src', default=SRC_DEFAULT, help='Source directory (searched recursively)')
    parser.add_argument('--dst', default=DST_DEFAULT, help='Output directory')
    parser.add_argument('--file', type=str, help='Single file to convert')
    parser.add_argument('--workers', type=int, default=4, help='Parallel workers')
    args = parser.parse_args()

    dst_dir = args.dst
    os.makedirs(dst_dir, exist_ok=True)

    if args.file:
        path, status, err = convert_anm_file(args.file, dst_dir)
        print(f"{status}: {os.path.basename(path)}" + (f" ({err})" if err else ""))
        return 0 if status == 'ok' else 1

    if not args.all and not args.file:
        parser.print_help()
        return 1

    # Find all .anm files recursively
    src_path = Path(args.src)
    if not src_path.exists():
        print(f"Source directory not found: {src_path}")
        return 1

    anm_files = sorted([str(f) for f in src_path.rglob('*.anm')])
    if not anm_files:
        print("No .anm files found.")
        return 0

    total = len(anm_files)
    success = failed = skipped = 0
    print(f"Found {total} animation files")
    print(f"Output: {dst_dir}")
    print()

    if args.workers > 1:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(convert_anm_file, f, dst_dir): f for f in anm_files}
            for future in as_completed(futures):
                path, status, err = future.result()
                name = os.path.basename(path)
                if status == 'ok':
                    success += 1
                    print(f"  OK  {name}")
                elif status == 'skip':
                    skipped += 1
                else:
                    failed += 1
                    print(f"  FAIL {name} ({err})")
    else:
        for f in anm_files:
            path, status, err = convert_anm_file(f, dst_dir)
            name = os.path.basename(path)
            if status == 'ok':
                success += 1
                print(f"  OK  {name}")
            elif status == 'skip':
                skipped += 1
            else:
                failed += 1
                print(f"  FAIL {name} ({err})")

    print()
    print(f"=== Animation Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
