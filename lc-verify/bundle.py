#!/usr/bin/env python3
"""Inline KACTL quoted includes into a single translation unit for yosupo."""
from __future__ import annotations

import argparse
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
PREAMBLE = (Path(__file__).resolve().parent / "preamble.h").read_text()
INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"\s*$')
PRAGMA_ONCE_RE = re.compile(r"^\s*#\s*pragma\s+once\s*$")
SYSTEM_BITS = re.compile(r"^\s*#\s*include\s*<bits/stdc\+\+\.h>\s*$")
USING_NS = re.compile(r"^\s*using\s+namespace\s+std\s*;\s*$")
MACRO_SKIP = re.compile(
    r"^\s*#(define\s+(rep|all|sz|pb)\b|include\s*<)"
)


def inline(path: Path, seen: set[Path]) -> str:
    path = path.resolve()
    if path in seen:
        return f"// already included {path.name}\n"
    seen.add(path)
    out = []
    for line in path.read_text().splitlines(True):
        if PRAGMA_ONCE_RE.match(line) or SYSTEM_BITS.match(line) or USING_NS.match(line):
            continue
        m = INCLUDE_RE.match(line)
        if m:
            inc = (path.parent / m.group(1)).resolve()
            if not inc.exists():
                raise SystemExit(f"missing include {m.group(1)} from {path}")
            out.append(f"// begin {inc.relative_to(REPO)}\n")
            out.append(inline(inc, seen))
            out.append(f"// end {inc.relative_to(REPO)}\n")
            continue
        out.append(line)
        if not line.endswith("\n"):
            out.append("\n")
    return "".join(out)


def bundle(src: Path) -> str:
    body = inline(src, set())
    return PREAMBLE + "\n" + body


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("src", type=Path)
    ap.add_argument("-o", type=Path, required=True)
    args = ap.parse_args()
    args.o.parent.mkdir(parents=True, exist_ok=True)
    args.o.write_text(bundle(args.src))


if __name__ == "__main__":
    main()
