#!/usr/bin/env python3
"""Create the source-only submission archive used by the course grader."""

from pathlib import Path
from zipfile import ZIP_DEFLATED, ZipFile

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "src"
OUTPUT = ROOT / "handin.zip"
ALLOWED = {".c", ".cpp", ".hpp", ".h"}
MAX_SOURCE_BYTES = 8 * 1024 * 1024


def main() -> None:
    files = sorted(p for p in SOURCE.iterdir() if p.is_file() and p.suffix in ALLOWED)
    if not files or not any(p.name == "coursesat.cpp" for p in files):
        raise SystemExit("src/ is incomplete")
    if any(p.is_symlink() for p in files):
        raise SystemExit("symlinks are not accepted")
    if sum(p.stat().st_size for p in files) > MAX_SOURCE_BYTES:
        raise SystemExit("source exceeds 8 MiB")
    with ZipFile(OUTPUT, "w", compression=ZIP_DEFLATED) as archive:
        for path in files:
            archive.write(path, f"src/{path.name}")
    print(f"created {OUTPUT} with {len(files)} source files")


if __name__ == "__main__":
    main()
