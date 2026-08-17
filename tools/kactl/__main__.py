"""Preprocess KACTL content/ into listings, snippets.json, and header seed."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from . import BUILD, CONTENT
from .chapter import (
    chapter_order,
    chapter_title,
    discover_files,
    parse_chapter_document,
    parse_chapter_imports,
)
from .emit_json import snippet_json, write_snippets_json
from .emit_tex import print_header, write_header_seed, write_listing
from .snippet import process_path


def preprocess() -> int:
    BUILD.mkdir(parents=True, exist_ok=True)
    chapters_out = []
    snippets_out = []
    document: list[dict] = []
    pdf_snippets = []  # ProcessedSnippet in typeset order
    pending = []  # (sid, chapter_id, processed, included)

    order = chapter_order()
    for chapter_id in order:
        if not (CONTENT / chapter_id / "chapter.tex").is_file():
            continue
        imports = parse_chapter_imports(chapter_id)
        files = discover_files(chapter_id, imports)
        chapter_doc = parse_chapter_document(chapter_id)

        by_name = {}
        for path in files:
            spec = imports.get(path.name)
            lang_flag = spec.lang_flag if spec else None
            processed = process_path(path, lang_flag)
            if not processed.code and not processed.commands.get("Description"):
                continue
            included = spec.included_in_pdf if spec else False
            if path.name not in imports and path.suffix not in {".h", ".hpp", ".cpp", ".java"}:
                continue
            by_name[path.name] = (processed, included)

        chapter_pending = []
        # Header captions must follow \kactlimport / typeset order, not directory sort.
        for name, spec in imports.items():
            if name not in by_name:
                continue
            processed, included = by_name[name]
            sid = f"{chapter_id}/{name}"
            chapter_pending.append((sid, chapter_id, processed, included))
            if included:
                write_listing(chapter_id, processed)
                if not processed.error:
                    pdf_snippets.append(processed)
        for name, (processed, included) in by_name.items():
            if name in imports:
                continue
            sid = f"{chapter_id}/{name}"
            chapter_pending.append((sid, chapter_id, processed, included))

        mentioned = {b["id"] for b in chapter_doc if b["type"] == "snippet"}
        for sid, ch, processed, included in chapter_pending:
            if sid not in mentioned:
                chapter_doc.append(
                    {
                        "type": "snippet",
                        "id": sid,
                        "chapter": ch,
                        "includedInPdf": included,
                    }
                )

        if chapter_doc or chapter_pending:
            chapters_out.append({"id": chapter_id, "title": chapter_title(chapter_id)})
            document.extend(chapter_doc)
            pending.extend(chapter_pending)

    id_set = {p[0] for p in pending}
    for sid, chapter_id, processed, included in pending:
        snippets_out.append(snippet_json(sid, chapter_id, processed, included, id_set))

    chapter_rank = {c: i for i, c in enumerate(order)}
    snippets_out.sort(key=lambda s: (chapter_rank.get(s["chapter"], 99), s["name"].lower()))

    payload = {"chapters": chapters_out, "snippets": snippets_out, "document": document}
    dest = write_snippets_json(payload)
    write_header_seed(pdf_snippets)

    n_h = sum(1 for b in document if b["type"] == "heading")
    n_p = sum(1 for b in document if b["type"] == "prose")
    n_s = sum(1 for b in document if b["type"] == "snippet")
    print(
        f"Wrote {len(snippets_out)} snippets, {len(document)} document blocks "
        f"({n_h} headings, {n_p} prose, {n_s} snippets) in {len(chapters_out)} chapters → {dest}"
    )
    print(f"Listings → {BUILD / 'listings'}; header seed → {BUILD / 'header.tmp.seed'}")
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
