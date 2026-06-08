#!/usr/bin/env python3
"""
LUNA Plus Reborn — Audio Converter
Copies/converts audio files. Currently just copies wav/mp3.
Future: convert to OGG Vorbis for smaller size.
"""
import os, sys, shutil, argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

SRC_DEFAULT = os.environ.get('LUNA_LEGACY_SRC', str(Path(__file__).parent.parent.parent.parent / 'Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/sound'))
DST_DEFAULT = os.environ.get('LUNA_REBORN_ROOT', str(Path(__file__).parent.parent.parent)) + '/assets/audio'

def copy_file(src, dst):
    shutil.copy2(src, dst)
    return True

def convert_to_ogg(src, dst):
    import subprocess
    result = subprocess.run(
        ['ffmpeg', '-i', str(src), '-c:a', 'libvorbis', '-q:a', '3', str(dst)],
        capture_output=True, text=True
    )
    return result.returncode == 0

def convert_audio_file(src_path, dst_base, fmt):
    src = Path(src_path)
    if fmt == 'ogg':
        dst = dst_base / src.with_suffix('.ogg').name
    else:
        dst = dst_base / src.name

    if dst.exists():
        return (src_path, 'skip', 'exists')

    try:
        if fmt == 'ogg':
            ok = convert_to_ogg(src, dst)
        else:
            ok = copy_file(str(src), str(dst))
        if ok:
            return (src_path, 'ok', None)
        else:
            return (src_path, 'fail', 'conversion failed')
    except Exception as e:
        return (src_path, 'fail', str(e))

def main():
    parser = argparse.ArgumentParser(description='Convert audio assets')
    parser.add_argument('--src', default=SRC_DEFAULT, help='Source audio directory')
    parser.add_argument('--dst', default=DST_DEFAULT, help='Output audio directory')
    parser.add_argument('--format', choices=['copy', 'ogg'], default='copy', help='Output format')
    parser.add_argument('--workers', type=int, default=4, help='Parallel workers')
    parser.add_argument('--all', action='store_true', help='Process all audio files')
    args = parser.parse_args()

    src = Path(args.src)
    dst = Path(args.dst)
    dst.mkdir(parents=True, exist_ok=True)

    if not src.exists():
        print(f"Source directory not found: {src}")
        return 1

    patterns = ['*.wav', '*.mp3', '*.ogg']
    files = []
    for ext in patterns:
        files.extend(sorted(src.rglob(ext)))
    files = sorted(set(str(f) for f in files))

    if not files:
        print("No audio files found.")
        return 0

    total = len(files)
    success = failed = skipped = 0
    print(f"Found {total} audio files")
    print(f"Output: {dst}")
    print()

    if args.workers > 1:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(convert_audio_file, f, dst, args.format): f for f in files}
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
        for f in files:
            path, status, err = convert_audio_file(f, dst, args.format)
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
    print(f"=== Audio Conversion Complete ===")
    print(f"  Converted: {success}")
    print(f"  Failed:    {failed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Total:     {total}")
    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
