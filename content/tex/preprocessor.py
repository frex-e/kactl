#!/usr/bin/env python3
"""Page-header helper for kactlpkg.sty (runs during pdflatex via write18).

Snippet listings are generated ahead of time by `python3 -m tools.kactl preprocess`.
This script only consumes build/header.tmp to print the per-page filename list.
"""

from __future__ import annotations

import getopt
import sys
from pathlib import Path

HEADER_TMP = Path("build/header.tmp")


def print_header(data: str, outstream) -> None:
    parts = data.split("|")
    until = parts[0].strip() or parts[1].strip()
    if not until:
        return
    if not HEADER_TMP.is_file():
        return
    lines = [x.strip() for x in HEADER_TMP.read_text(encoding="utf-8").splitlines()]
    if until not in lines:
        return

    ind = lines.index(until) + 1
    header_length = len("".join(lines[:ind]))

    def adjust(name: str) -> str:
        return name if name.startswith(".") else name.split(".")[0]

    output = r"\enspace{}".join(map(adjust, lines[:ind]))
    font_size = 10
    if header_length > 150:
        font_size = 8
    output = r"\hspace{3mm}\textbf{" + output + "}"
    output = "\\fontsize{%d}{%d}" % (font_size, font_size) + output
    print(output, file=outstream)
    HEADER_TMP.write_text("".join(line + "\n" for line in lines[ind:]), encoding="utf-8")


def main() -> int:
    outstream = sys.stdout
    print_header_value = None
    try:
        opts, _args = getopt.getopt(
            sys.argv[1:],
            "ho:",
            ["help", "output=", "print-header="],
        )
        for option, value in opts:
            if option in ("-h", "--help"):
                print("Print the KACTL page header from build/header.tmp.")
                print("  --print-header MARK  last snippet mark on the page")
                print("  -o --output FILE")
                return 0
            if option in ("-o", "--output"):
                outstream = open(value, "w")
            if option == "--print-header":
                print_header_value = value
        if print_header_value is not None:
            print_header(print_header_value, outstream)
            return 0
        print("preprocessor.py now only supports --print-header.", file=sys.stderr)
        print("Run: python3 -m tools.kactl preprocess", file=sys.stderr)
        return 2
    except (getopt.GetoptError, OSError) as err:
        print(str(err), file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
