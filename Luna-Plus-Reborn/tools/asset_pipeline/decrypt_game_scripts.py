#!/usr/bin/env python3
"""Decrypt legacy Luna .bin scripts into assets/scripts/ for Reborn."""

import struct
import sys
from pathlib import Path

try:
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
    from cryptography.hazmat.backends import default_backend
except ImportError:
    print("Install cryptography: pip install cryptography")
    sys.exit(1)

OLD_SCRIPTS = Path(__file__).resolve().parents[3] / "Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/scripts"
OUT_DIR = Path(__file__).resolve().parents[2] / "assets/scripts"
KEY = bytes.fromhex("E76B2413958B00E193A1") + b"\x00" * 12


def decrypt_bin(path: Path) -> bytes | None:
    data = path.read_bytes()
    if len(data) < 13:
        return None
    version, file_type, data_size = struct.unpack_from("<III", data, 0)
    encrypted = data[13:-1]
    if not encrypted:
        return None
    cipher = Cipher(algorithms.AES(KEY), modes.ECB(), backend=default_backend())
    decryptor = cipher.decryptor()
    plain = decryptor.update(encrypted) + decryptor.finalize()
    return plain[:data_size]


def main():
    if not OLD_SCRIPTS.exists():
        print(f"Legacy scripts not found: {OLD_SCRIPTS}")
        return 1

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    ok = fail = 0
    for src in OLD_SCRIPTS.rglob("*.bin"):
        rel = src.relative_to(OLD_SCRIPTS)
        dst = OUT_DIR / rel.with_suffix(".bin.txt")
        dst.parent.mkdir(parents=True, exist_ok=True)
        try:
            plain = decrypt_bin(src)
            if plain is None:
                fail += 1
                continue
            dst.write_bytes(plain)
            ok += 1
        except Exception:
            fail += 1

    print(f"Decrypted {ok} scripts to {OUT_DIR} ({fail} failed)")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
