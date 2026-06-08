#!/usr/bin/env python3
"""
LUNA Plus Reborn — PHOENIX Asset Pipeline v2.0
Orchestrates asset conversion pipeline steps
"""

import os
import sys
import argparse
import subprocess
import json
import logging
from pathlib import Path
from datetime import datetime

PIPELINE_DIR = Path(__file__).parent
REBORN = Path(os.environ.get('LUNA_REBORN_ROOT', Path(__file__).parent.parent.parent))
STATUS_FILE = PIPELINE_DIR / "pipeline_status.json"

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.FileHandler(PIPELINE_DIR / 'pipeline.log'),
        logging.StreamHandler(),
    ]
)
log = logging.getLogger('pipeline')

STEPS = {
    'textures': {
        'script': PIPELINE_DIR / 'convert_textures.py',
        'args': ['--all', '--workers=4'],
        'description': 'Texture Conversion (.dds/.tif/.tga -> .png)',
    },
    'models': {
        'script': PIPELINE_DIR / 'convert_models.py',
        'args': ['--all', '--workers=4'],
        'description': 'Model Conversion (.mod -> .obj)',
    },
    'heightmaps': {
        'script': PIPELINE_DIR / 'convert_heightmaps.py',
        'args': ['--all'],
        'description': 'Heightmap Conversion (.hfl -> .hgt)',
    },
    'chardefs': {
        'script': PIPELINE_DIR / 'convert_chardefs.py',
        'args': ['--all'],
        'description': 'Character Definition Conversion (.chx -> .json)',
    },
    'animations': {
        'script': PIPELINE_DIR / 'convert_anm.py',
        'args': ['--all'],
        'description': 'Animation Conversion (.anm -> .anm.json)',
    },
    'audio': {
        'script': PIPELINE_DIR / 'convert_audio.py',
        'args': ['--format=copy', '--workers=4'],
        'description': 'Audio Conversion (.wav/.mp3 -> copy/ogg)',
    },
    'inventory': {
        'script': PIPELINE_DIR / 'generate_inventory.py',
        'args': [],
        'description': 'Generate Asset Inventory',
    },
    'validate': {
        'script': PIPELINE_DIR / 'validate_assets.py',
        'args': ['--report'],
        'description': 'Validate Asset Completeness',
    },
}

def load_status():
    if STATUS_FILE.exists():
        with open(STATUS_FILE) as f:
            return json.load(f)
    return {}

def save_status(status):
    with open(STATUS_FILE, 'w') as f:
        json.dump(status, f, indent=2)

def run_step(step_name, force=False):
    step = STEPS[step_name]
    script = step['script']
    args = step['args']

    if not script.exists():
        log.error(f"Script not found: {script}")
        return False

    cmd = [sys.executable, str(script)] + args
    log.info(f"Running: {' '.join(cmd)}")

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=3600)
        if result.stdout:
            log.info(f"[{step_name}] stdout:\n{result.stdout}")
        if result.stderr:
            log.warning(f"[{step_name}] stderr:\n{result.stderr}")

        if result.returncode == 0:
            log.info(f"Step '{step_name}' completed successfully")
            return True
        else:
            log.error(f"Step '{step_name}' FAILED with code {result.returncode}")
            return False
    except subprocess.TimeoutExpired:
        log.error(f"Step '{step_name}' timed out")
        return False
    except Exception as e:
        log.error(f"Step '{step_name}' error: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='PHOENIX Asset Pipeline')
    parser.add_argument('--all', action='store_true', help='Run all steps')
    parser.add_argument('--force', action='store_true', help='Force re-run completed steps')
    parser.add_argument('--dry-run', action='store_true', help='Print what would be done')
    for step_name in STEPS:
        parser.add_argument(f'--{step_name}', action='store_true', help=f"Run {step_name} step")

    args = parser.parse_args()

    start_time = datetime.now()
    log.info("=" * 60)
    log.info("PHOENIX Asset Pipeline v2.0 — Starting")
    log.info("=" * 60)

    status = load_status()

    if args.all or args.dry_run:
        selected_steps = list(STEPS.keys())
    else:
        selected_steps = [k for k in STEPS if getattr(args, k.replace('-', '_'))]

    if not selected_steps:
        parser.print_help()
        return 0

    if args.dry_run:
        log.info("*** DRY RUN MODE — no files will be modified ***")
        for step_name in selected_steps:
            log.info(f"\n--- Step: {step_name} ---")
            log.info(f"=== {STEPS[step_name]['description']} ===")
            if STEPS[step_name]['script'].exists():
                log.info(f"[DRY-RUN] Would execute: {' '.join([sys.executable, str(STEPS[step_name]['script'])] + STEPS[step_name]['args'])}")
            else:
                log.warning(f"[DRY-RUN] Script missing: {STEPS[step_name]['script']}")
        log.info("=" * 60)
        log.info(f"Pipeline dry-run completed in {(datetime.now() - start_time).total_seconds():.1f}s")
        log.info("=" * 60)
        return 0

    errors = []
    for step_name in selected_steps:
        log.info(f"\n--- Step: {step_name} ---")
        log.info(f"=== {STEPS[step_name]['description']} ===")

        if not args.force and status.get(step_name, {}).get('status') == 'completed':
            log.info(f"{STEPS[step_name]['description']} already completed. Use --force to re-run.")
            continue

        success = run_step(step_name, force=args.force)
        status[step_name] = {
            'status': 'completed' if success else 'failed',
            'timestamp': datetime.now().isoformat(),
        }
        save_status(status)

        if not success:
            errors.append(step_name)

    elapsed = (datetime.now() - start_time).total_seconds()
    log.info("=" * 60)
    if errors:
        log.warning(f"Pipeline finished with errors in {elapsed:.1f}s")
        log.warning(f"Failed steps: {', '.join(errors)}")
    else:
        log.info(f"Pipeline completed successfully in {elapsed:.1f}s")
    log.info("=" * 60)

    return 1 if errors else 0

if __name__ == "__main__":
    sys.exit(main())
