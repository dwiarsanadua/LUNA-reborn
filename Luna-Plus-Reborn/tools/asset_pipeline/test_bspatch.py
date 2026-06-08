#!/usr/bin/env python3
"""Integration test for Colin Percival bspatch (BSDIFF40)."""

import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

BASE = Path(__file__).resolve().parents[2]
BSPATCH_BIN = BASE / "build" / "bin" / "test_bspatch"


def main() -> int:
    bsdiff = shutil.which("bsdiff")
    bspatch_cli = shutil.which("bspatch")
    if not bsdiff or not bspatch_cli:
        print("SKIP: bsdiff/bspatch CLI not installed (brew install bsdiff)")
        return 0

    if not BSPATCH_BIN.is_file():
        print(f"FAIL: build test_bspatch first ({BSPATCH_BIN})")
        return 1

    with tempfile.TemporaryDirectory() as tmp:
        td = Path(tmp)
        old = td / "old.txt"
        new_ref = td / "new_ref.txt"
        patched = td / "patched.txt"
        patch = td / "file.patch"

        old.write_text("Hello Luna Plus Old content v1.0\n" * 64, encoding="utf-8")
        new_ref.write_text("Hello Luna Plus Reborn content v1.1\n" * 64, encoding="utf-8")

        subprocess.run([bsdiff, str(old), str(new_ref), str(patch)], check=True)
        rc = subprocess.run(
            [str(BSPATCH_BIN), str(old), str(patched), str(patch)],
            capture_output=True,
            text=True,
        ).returncode
        if rc != 0:
            print("FAIL: test_bspatch returned", rc)
            return 1
        if patched.read_bytes() != new_ref.read_bytes():
            print("FAIL: patched output mismatch")
            return 1

    print("test_bspatch: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
