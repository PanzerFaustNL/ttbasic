#!/usr/bin/env python3
"""Prepare an Atari GEMDOS drive without replacing user BASIC sources."""
import argparse
from pathlib import Path


def install(drive_c: Path) -> None:
    root = Path(__file__).resolve().parent.parent
    target = drive_c / "TTBASIC"
    target.mkdir(parents=True, exist_ok=True)
    for folder in ("src", "atari", "examples"):
        for source in sorted((root / folder).iterdir()):
            if not source.is_file():
                continue
            destination = target / source.name
            if source.suffix.upper() in (".BAS", ".TBP") and destination.exists():
                print(f"Kept existing source/project: {destination}")
                continue
            data = source.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", b"\r\n")
            destination.write_bytes(data)
            print(f"Installed: {destination}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--drive-c", required=True, type=Path,
                        help="Host directory exposed as Atari drive C:")
    install(parser.parse_args().drive_c)
