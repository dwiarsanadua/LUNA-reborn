#!/usr/bin/env python3
"""
LUNA Plus Reborn — Asset Validation
Validates asset completeness and integrity
"""

import os
import sys
import json
import hashlib
import argparse
from pathlib import Path
from datetime import datetime

REBORN = Path("/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn")
ASSETS = REBORN / "assets"
ASSETS_CONVERTED = REBORN / "assets_converted"
PIPELINE_DIR = REBORN / "tools/asset_pipeline"

REQUIRED_DIRS = [
    'textures', 'models', 'animations', 'audio', 'maps',
    'shaders', 'fonts', 'data', 'characters', 'interface', 'scripts',
]

def check_dir_structure() -> dict:
    results = {'status': 'ok', 'issues': []}
    for d in REQUIRED_DIRS:
        path = ASSETS / d
        if not path.exists():
            results['issues'].append(f"MISSING: assets/{d}/")
            results['status'] = 'error'
        elif not path.is_dir():
            results['issues'].append(f"NOT A DIR: assets/{d}/")
            results['status'] = 'error'
    return results

def count_files(start: Path, pattern: str = '*') -> dict:
    total = 0
    by_type = {}
    for f in start.rglob(pattern):
        if f.is_file():
            total += 1
            ext = f.suffix.lower()
            by_type[ext] = by_type.get(ext, 0) + 1
    return {'total': total, 'by_type': by_type}

def validate_assets_report() -> dict:
    report = {
        'timestamp': datetime.now().isoformat(),
        'assets_dir': str(ASSETS),
        'assets_converted_dir': str(ASSETS_CONVERTED),
        'directory_structure': check_dir_structure(),
        'counts': {},
    }

    for d in REQUIRED_DIRS:
        path = ASSETS / d
        if path.exists():
            report['counts'][d] = count_files(path)

    if ASSETS_CONVERTED.exists():
        report['counts']['converted'] = count_files(ASSETS_CONVERTED)

    # Check shader completeness
    shader_bins = list(ASSETS.glob('shaders/*.bin'))
    shader_sc = list(ASSETS.glob('shaders/*.sc'))
    report['shaders'] = {
        'bin_files': len(shader_bins),
        'sc_files': len(shader_sc),
        'bin_list': [f.name for f in shader_bins],
    }

    # Check for common issues
    issues = []
    for d in REQUIRED_DIRS:
        path = ASSETS / d
        if path.exists():
            fc = count_files(path)
            if d == 'shaders' and fc['total'] == 0:
                issues.append(f"EMPTY: assets/{d}/")
            if d == 'fonts' and fc['total'] == 0:
                issues.append(f"EMPTY: assets/{d}/")

    report['issues'] = issues
    return report

def main():
    parser = argparse.ArgumentParser(description='Validate asset completeness')
    parser.add_argument('--report', action='store_true', help='Generate JSON report')
    parser.add_argument('--verbose', action='store_true', help='Verbose output')
    args = parser.parse_args()

    report = validate_assets_report()

    print("=" * 60)
    print("LUNA Plus Reborn — Asset Validation Report")
    print("=" * 60)
    print(f"Timestamp: {report['timestamp']}")
    print()

    ds = report['directory_structure']
    print(f"Directory Structure: {ds['status'].upper()}")
    for issue in ds['issues']:
        print(f"  ! {issue}")
    print()

    print("Asset Counts:")
    for cat, info in sorted(report['counts'].items()):
        if isinstance(info, dict):
            print(f"  {cat}: {info['total']} files")
            if args.verbose and info['by_type']:
                for ext, count in sorted(info['by_type'].items()):
                    print(f"    {ext}: {count}")
    print()

    if 'shaders' in report:
        s = report['shaders']
        print(f"Shaders: {s['bin_files']} .bin, {s['sc_files']} .sc source")
        if args.verbose:
            for name in s['bin_list']:
                print(f"  {name}")
    print()

    if report['issues']:
        print("Issues:")
        for issue in report['issues']:
            print(f"  ! {issue}")
    else:
        print("No issues found.")
    print()

    total_files = sum(v['total'] for v in report['counts'].values() if isinstance(v, dict))
    print(f"Total assets: {total_files}")

    if args.report:
        report_path = PIPELINE_DIR / "validation_report.json"
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2)
        print(f"Report saved: {report_path}")

    return 0

if __name__ == "__main__":
    sys.exit(main())
