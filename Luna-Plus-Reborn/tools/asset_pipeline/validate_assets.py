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

def is_text_ui_layout(data: bytes) -> bool:
    return bool(data) and data[0] in (ord("$"), ord("#"), ord(";"))


def check_phase4_ui() -> dict:
    """Phase 4 gates: parser assets, dialog coverage, regression tooling."""
    issues = []
    win_dir = ASSETS / "interface" / "Windows"
    text_layouts = 0
    total_layouts = 0
    if win_dir.is_dir():
        for f in win_dir.glob("*.bin.txt"):
            total_layouts += 1
            if is_text_ui_layout(f.read_bytes()[:1]):
                text_layouts += 1

    if text_layouts < 150:
        issues.append(f"Phase4: {text_layouts} readable UI layouts (need >=150)")
    if total_layouts < 200:
        issues.append(f"Phase4: {total_layouts} interface layouts (need >=200)")

    sound_list = ASSETS / "data" / "SoundList.bin.txt"
    if not sound_list.is_file():
        issues.append("Phase4: missing assets/data/SoundList.bin.txt")

    manifest = REBORN / "tools/ui_regression/capture_manifest.json"
    compare = REBORN / "tools/ui_regression/compare_screenshots.py"
    capture = REBORN / "tools/ui_regression/run_capture.sh"
    for path, label in [
        (manifest, "capture_manifest.json"),
        (compare, "compare_screenshots.py"),
        (capture, "run_capture.sh"),
    ]:
        if not path.is_file():
            issues.append(f"Phase4: missing tools/ui_regression/{label}")

    baseline = REBORN / "screenshots" / "baseline"
    if not baseline.is_dir():
        issues.append("Phase4: missing screenshots/baseline/ (run tools/ui_regression/run_baseline_setup.sh)")

    return {
        "text_layouts": text_layouts,
        "total_layouts": total_layouts,
        "issues": issues,
        "status": "ok" if not issues else "error",
    }


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

    phase4 = check_phase4_ui()
    issues.extend(phase4["issues"])
    report['phase4'] = phase4
    report['issues'] = issues
    return report

def main():
    parser = argparse.ArgumentParser(description='Validate asset completeness')
    parser.add_argument('--report', action='store_true', help='Generate JSON report')
    parser.add_argument('--verbose', action='store_true', help='Verbose output')
    parser.add_argument('--phase4', action='store_true', help='Enforce Phase 4 UI gates only')
    args = parser.parse_args()

    if args.phase4:
        phase4 = check_phase4_ui()
        print("Phase 4 UI validation:")
        print(f"  Text layouts: {phase4['text_layouts']}")
        print(f"  Total layouts: {phase4['total_layouts']}")
        if phase4['issues']:
            for issue in phase4['issues']:
                print(f"  ! {issue}")
            return 1
        print("  OK")
        return 0

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

    if 'phase4' in report:
        p4 = report['phase4']
        print(f"Phase 4 UI: {p4['status'].upper()} ({p4['text_layouts']} text / {p4['total_layouts']} total layouts)")
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
