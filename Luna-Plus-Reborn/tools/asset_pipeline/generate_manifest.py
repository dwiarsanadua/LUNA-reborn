#!/usr/bin/env python3
"""Generate assets/manifest.json with SHA256 checksums for critical files."""

import hashlib
import json
from datetime import datetime, timezone
from pathlib import Path

REBORN = Path(__file__).resolve().parents[2]
ASSETS = REBORN / "assets"
OUT = ASSETS / "manifest.json"

GLOBS = [
    "maps/*.hgt",
    "maps/*.json",
    "data/*.json",
    "data/*.db",
    "shaders/*.bin",
    "interface/Windows/*.bin.txt",
]


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def main():
    files = {}
    for pattern in GLOBS:
        for path in ASSETS.glob(pattern):
            if not path.is_file():
                continue
            rel = str(path.relative_to(ASSETS)).replace("\\", "/")
            files[rel] = {"sha256": sha256(path), "bytes": path.stat().st_size}

    manifest = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "file_count": len(files),
        "files": files,
    }
    OUT.write_text(json.dumps(manifest, indent=2))
    print(f"Wrote manifest with {len(files)} files to {OUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
