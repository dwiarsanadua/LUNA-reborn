#!/usr/bin/env python3
"""
LUNA Plus Reborn — Asset Inventory Generator
Generates a JSON inventory of all assets
"""

import os
import sys
import json
import hashlib
import argparse
from pathlib import Path
from datetime import datetime

ASSETS = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets")
ASSETS_CONVERTED = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets_converted")
PIPELINE_DIR = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/asset_pipeline")
INVENTORY_FILE = PIPELINE_DIR / "asset_inventory.json"

def file_hash(path: Path) -> str:
    try:
        with open(path, 'rb') as f:
            return hashlib.md5(f.read()).hexdigest()
    except:
        return "ERROR"

def scan_directory(base: Path, prefix: str = "") -> list:
    entries = []
    if not base.exists():
        return entries
    for f in sorted(base.rglob('*')):
        if f.is_file():
            rel = f.relative_to(base)
            entries.append({
                'path': str(rel),
                'size': f.stat().st_size,
                'ext': f.suffix.lower(),
                'md5': file_hash(f),
            })
    return entries

def main():
    parser = argparse.ArgumentParser(description='Generate asset inventory JSON')
    parser.add_argument('--output', type=str, help='Output file path')
    args = parser.parse_args()

    inventory = {
        'generated': datetime.now().isoformat(),
        'assets_dir': str(ASSETS),
        'assets_converted_dir': str(ASSETS_CONVERTED),
        'files': [],
    }

    print("Scanning assets/ ...")
    inventory['files'].extend(scan_directory(ASSETS, 'assets'))

    print("Scanning assets_converted/ ...")
    inventory['files'].extend(scan_directory(ASSETS_CONVERTED, 'assets_converted'))

    total = len(inventory['files'])
    total_size = sum(f['size'] for f in inventory['files'])
    inventory['summary'] = {
        'total_files': total,
        'total_size_bytes': total_size,
        'total_size_mb': round(total_size / (1024 * 1024), 2),
    }

    # Group by extension
    by_ext = {}
    for f in inventory['files']:
        ext = f['ext']
        by_ext[ext] = by_ext.get(ext, 0) + 1
    inventory['summary']['by_extension'] = dict(sorted(by_ext.items()))

    output = args.output or str(INVENTORY_FILE)
    with open(output, 'w') as f:
        json.dump(inventory, f, indent=2)

    print(f"\nInventory generated: {output}")
    print(f"Total files: {total}")
    print(f"Total size: {inventory['summary']['total_size_mb']} MB")
    print(f"File types: {len(by_ext)}")

    return 0

if __name__ == "__main__":
    sys.exit(main())
