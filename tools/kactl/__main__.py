"""Preprocess KACTL content/ into listings, snippets.json, and header seed."""

from __future__ import annotations

import argparse
import sys

from . import BUILD, CONTENT
from .chapter import (
    chapter_order,
    chapter_title,
    discover_files,
    parse_chapter_document,
    parse_chapter_imports,
)
from .emit_json import snippet_json, write_snippets_json
from .emit_tex import write_header_seed, write_listing
from .snippet import process_path


def preprocess(json_only: bool = False) -> int:
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

        chapter_pending = []
        for path in files:
            spec = imports.get(path.name)
            lang_flag = spec.lang_flag if spec else None
            processed = process_path(path, lang_flag)
            if not processed.code and not processed.commands.get("Description"):
                continue
            included = spec.included_in_pdf if spec else False
            if path.name not in imports and path.suffix not in {".h", ".hpp", ".cpp", ".java"}:
                continue
            sid = f"{chapter_id}/{path.name}"
            chapter_pending.append((sid, chapter_id, processed, included))
            if included and not processed.error:
                if not json_only:
                    write_listing(chapter_id, processed)
                pdf_snippets.append(processed)
            elif included and processed.error and not json_only:
                write_listing(chapter_id, processed)

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
    if not json_only:
        write_header_seed(pdf_snippets)

    n_h = sum(1 for b in document if b["type"] == "heading")
    n_p = sum(1 for b in document if b["type"] == "prose")
    n_s = sum(1 for b in document if b["type"] == "snippet")
    print(
        f"Wrote {len(snippets_out)} snippets, {len(document)} document blocks "
        f"({n_h} headings, {n_p} prose, {n_s} snippets) in {len(chapters_out)} chapters → {dest}"
    )
    if not json_only:
        print(f"Listings → {BUILD / 'listings'}; header seed → {BUILD / 'header.tmp.seed'}")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Preprocess KACTL content/ for PDF and site")
    parser.add_argument(
        "cmd",
        nargs="?",
        default="preprocess",
        choices=["preprocess"],
        help="preprocess (default): write listings, snippets.json, and header seed",
    )
    parser.add_argument(
        "--json-only",
        action="store_true",
        help="Only write snippets.json (skip listings and header seed)",
    )
    args = parser.parse_args(argv)
    if args.cmd == "preprocess":
        return preprocess(json_only=args.json_only)
    parser.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
