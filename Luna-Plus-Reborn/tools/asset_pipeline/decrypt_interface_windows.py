#!/usr/bin/env python3
"""Decrypt legacy interface Windows/*.bin into readable .bin.txt."""

import struct
import sys
from pathlib import Path

try:
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
    from cryptography.hazmat.backends import default_backend
except ImportError:
    print("Install cryptography: pip install cryptography")
    sys.exit(1)

REBORN = Path(__file__).resolve().parents[2]
OLD_WIN = Path(__file__).resolve().parents[3] / "Luna-Plus-Old/LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/interface/Windows"
OUT_DIR = REBORN / "assets/interface/Windows"
KEY = (bytes.fromhex("E76B2413958B00E193A1") + b"\x00" * 12)[:16]


def decrypt_bin(data: bytes) -> bytes | None:
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


def is_text_script(data: bytes) -> bool:
    if not data or data[0] not in (ord("$"), ord("#"), ord(";")):
        return False
    try:
        data[:200].decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False


def main() -> int:
    sources = list(OLD_WIN.glob("*.bin")) if OLD_WIN.exists() else []

    seen = set()
    ok = skip = fail = 0
    for src in sources:
        if src.name in seen:
            continue
        seen.add(src.name)
        dst = OUT_DIR / src.with_suffix(".bin.txt").name
        if dst.exists() and is_text_script(dst.read_bytes()):
            skip += 1
            continue
        raw = src.read_bytes()
        if is_text_script(raw):
            skip += 1
            continue
        try:
            plain = decrypt_bin(raw)
            if plain is None or not is_text_script(plain):
                fail += 1
                continue
            dst.write_bytes(plain)
            ok += 1
            print(f"  decrypted {src.name} -> {dst.name}")
        except Exception as exc:
            print(f"  failed {src.name}: {exc}")
            fail += 1

    print(f"Interface decrypt: {ok} ok, {skip} already text, {fail} failed")
    return 0 if ok or skip else 1


if __name__ == "__main__":
    raise SystemExit(main())
