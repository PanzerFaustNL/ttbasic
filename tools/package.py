#!/usr/bin/env python3
"""Package the Atari program, examples, manuals and corresponding source."""
import argparse
import hashlib
from pathlib import Path
import zipfile


def package(program: Path, output: Path) -> None:
    root = Path(__file__).resolve().parent.parent
    binary = program.read_bytes()
    if binary[:2] != b"\x60\x1a":
        raise ValueError("Expected an Atari GEMDOS PRG executable")
    files = {}
    for name in ("README.md", "LICENSE", "STATUS.md", "RELEASE.md"):
        files[name] = (root / name).read_bytes()
    for folder in ("src", "atari", "examples", "docs", "tools", "tests"):
        for path in sorted((root / folder).rglob("*")):
            if path.is_file() and "__pycache__" not in path.parts and path.suffix != ".pyc":
                files[path.relative_to(root).as_posix()] = path.read_bytes()
    files["TTBASIC/TTBASIC.PRG"] = binary
    files["TTBASIC/LEESMIJ.TXT"] = (root / "atari/LEESMIJ.TXT").read_bytes()
    for path in sorted((root / "examples").iterdir()):
        if path.suffix.upper() in (".BAS", ".TBP"):
            files["TTBASIC/" + path.name] = path.read_bytes()
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for name, data in sorted(files.items()):
            info = zipfile.ZipInfo("ttbasic-0.10/" + name)
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o644 << 16
            archive.writestr(info, data)
    digest = hashlib.sha256(output.read_bytes()).hexdigest()
    output.with_suffix(output.suffix + ".sha256").write_text(f"{digest}  {output.name}\n")
    print(f"Packaged {len(files)} files: {output}")
    print(f"SHA-256: {digest}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--program", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    package(args.program, args.output)
