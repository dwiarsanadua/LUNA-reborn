#!/usr/bin/env python3
"""
Compare UI regression screenshots (baseline vs current).
Usage:
  python compare_screenshots.py --baseline screenshots/baseline --current screenshots/current
"""

import argparse
import json
import sys
from pathlib import Path

try:
    from PIL import Image, ImageChops
except ImportError:
    print("Install Pillow: pip install Pillow", file=sys.stderr)
    sys.exit(2)


def image_diff_ratio(a: Image.Image, b: Image.Image) -> float:
    if a.size != b.size:
        b = b.resize(a.size, Image.Resampling.BILINEAR)
    if a.mode != "RGB":
        a = a.convert("RGB")
    if b.mode != "RGB":
        b = b.convert("RGB")
    diff = ImageChops.difference(a, b)
    hist = diff.histogram()
    sq = sum((i % 256) * c for i, c in enumerate(hist))
    max_sq = len(hist) // 3 * 255 * 255 * a.size[0] * a.size[1]
    return (sq / max_sq) ** 0.5 if max_sq else 0.0


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare UI screenshot folders")
    parser.add_argument("--baseline", type=Path, required=True)
    parser.add_argument("--current", type=Path, required=True)
    parser.add_argument("--threshold", type=float, default=0.05,
                        help="Max allowed RMS diff ratio (0-1)")
    parser.add_argument("--report", type=Path, default=None)
    args = parser.parse_args()

    baseline = args.baseline
    current = args.current
    if not baseline.is_dir() or not current.is_dir():
        print("baseline and current must be existing directories", file=sys.stderr)
        return 2

    results = []
    def stems(folder: Path) -> set[str]:
        names: set[str] = set()
        for ext in ("*.png", "*.tga", "*.png.tga"):
            for p in folder.glob(ext):
                stem = p.name
                for suffix in (".png.tga", ".tga", ".png"):
                    if stem.endswith(suffix):
                        stem = stem[: -len(suffix)]
                        break
                names.add(stem)
        return names

    files = sorted(stems(baseline) | stems(current))
    failed = 0
    missing = 0

    for name in files:
        def resolve(folder: Path, stem: str) -> Path | None:
            for candidate in (f"{stem}.png", f"{stem}.tga", f"{stem}.png.tga"):
                p = folder / candidate
                if p.exists():
                    return p
            return None

        b = resolve(baseline, name)
        c = resolve(current, name)
        entry = {"name": name, "status": "ok", "diff": 0.0}
        if b is None or c is None:
            entry["status"] = "missing"
            missing += 1
            results.append(entry)
            continue
        ratio = image_diff_ratio(Image.open(b), Image.open(c))  # type: ignore[arg-type]
        entry["diff"] = round(ratio, 5)
        if ratio > args.threshold:
            entry["status"] = "fail"
            failed += 1
        results.append(entry)

    summary = {
        "baseline": str(baseline),
        "current": str(current),
        "threshold": args.threshold,
        "total": len(files),
        "failed": failed,
        "missing": missing,
        "results": results,
    }

    if args.report:
        args.report.write_text(json.dumps(summary, indent=2))
        print(f"Report written to {args.report}")

    for r in results:
        if r["status"] == "ok":
            print(f"  OK  {r['name']} (diff={r['diff']:.4f})")
        elif r["status"] == "missing":
            print(f"  MISS {r['name']}")
        else:
            print(f"  FAIL {r['name']} (diff={r['diff']:.4f})")

    print(f"\n{len(files)} layouts | {failed} failed | {missing} missing")
    return 1 if failed or missing else 0


if __name__ == "__main__":
    sys.exit(main())
