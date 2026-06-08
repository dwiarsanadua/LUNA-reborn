#!/usr/bin/env python3
"""Phase 7 bootstrap: parity checklist + release manifest stub."""

import subprocess
import sys
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
GEN = BASE / "tools" / "asset_pipeline" / "generate_parity_checklist.py"
MANIFEST = BASE / "dist" / "manifest.json"


def main() -> int:
    print("Phase 7 bootstrap:")
    subprocess.run([sys.executable, str(GEN)], check=True)
    MANIFEST.parent.mkdir(parents=True, exist_ok=True)
    if not MANIFEST.is_file():
        MANIFEST.write_text(
            '{\n  "version": "1.1.0",\n  "files": []\n}\n',
            encoding="utf-8",
        )
        print(f"  created stub {MANIFEST}")
    print("Phase 7 bootstrap OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
