"""Preprocess KACTL content/ into listings, snippets.json, and header seed."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from . import BUILD
from .emit_json import document_payload, write_snippets_json
from .emit_tex import print_header, write_pdf_artifacts
from .pipeline import build_document


def preprocess() -> int:
    BUILD.mkdir(parents=True, exist_ok=True)
    document = build_document()
    listings_dest, header_dest = write_pdf_artifacts(document)
    web_dest = write_snippets_json(document_payload(document))

    n_h = sum(1 for block in document.blocks if block["type"] == "heading")
    n_p = sum(1 for block in document.blocks if block["type"] == "prose")
    n_s = sum(1 for block in document.blocks if block["type"] == "snippet")
    print(
        f"Wrote {len(document.snippets)} snippets, {len(document.blocks)} document blocks "
        f"({n_h} headings, {n_p} prose, {n_s} snippets) "
        f"in {len(document.chapters)} chapters → {web_dest}"
    )
    print(f"Listings → {listings_dest}; header seed → {header_dest}")
    return 0


def cmd_print_header(mark: str, output: str | None) -> int:
    if output:
        Path(output).parent.mkdir(parents=True, exist_ok=True)
        with open(output, "w", encoding="utf-8") as out:
            print_header(mark, out)
    else:
        print_header(mark, sys.stdout)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Preprocess KACTL content/ for PDF and site")
    parser.add_argument(
        "cmd",
        nargs="?",
        default="preprocess",
        choices=["preprocess", "print-header"],
        help="preprocess (default): listings, snippets.json, header seed; "
        "print-header: consume header.tmp for a page running head",
    )
    parser.add_argument(
        "mark",
        nargs="?",
        help="last snippet mark on the page (print-header)",
    )
    parser.add_argument("-o", "--output", help="write print-header TeX to FILE")
    args = parser.parse_args(argv)
    if args.cmd == "preprocess":
        if args.mark is not None:
            parser.error("preprocess does not take a mark")
        return preprocess()
    if args.cmd == "print-header":
        if args.mark is None:
            parser.error("print-header requires MARK")
        return cmd_print_header(args.mark, args.output)
    parser.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
