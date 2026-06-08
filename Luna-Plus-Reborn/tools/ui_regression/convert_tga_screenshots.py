#!/usr/bin/env python3
"""Convert bgfx .tga / .png.tga screenshots to .png for comparison."""

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Install Pillow: pip install Pillow", file=sys.stderr)
    sys.exit(2)


def convert_dir(folder: Path) -> int:
    count = 0
    for tga in sorted(folder.glob("*.tga")):
        png = tga.with_suffix("")
        if png.suffix == ".png":
            out = png
        else:
            out = tga.with_suffix(".png")
        if out == tga:
            out = Path(str(tga).replace(".png.tga", ".png"))
        img = Image.open(tga)
        if img.mode not in ("RGB", "RGBA"):
            img = img.convert("RGB")
        img.save(out)
        print(f"  {tga.name} -> {out.name}")
        count += 1
    return count


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("directory", type=Path)
    args = parser.parse_args()
    if not args.directory.is_dir():
        print(f"Not a directory: {args.directory}", file=sys.stderr)
        return 1
    n = convert_dir(args.directory)
    print(f"Converted {n} files in {args.directory}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
