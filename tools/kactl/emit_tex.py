"""Emit per-snippet lstlisting TeX and the page-header caption seed."""

from __future__ import annotations

from pathlib import Path

from . import BUILD
from .snippet import ProcessedSnippet, header_caption, listing_tex


def listing_path(chapter_id: str, filename: str) -> Path:
    return BUILD / "listings" / chapter_id / f"{filename}.tex"


def write_listing(chapter_id: str, snippet: ProcessedSnippet) -> Path:
    path = listing_path(chapter_id, snippet.caption)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(listing_tex(snippet), encoding="utf-8")
    return path


def write_header_seed(snippets: list[ProcessedSnippet]) -> Path:
    """Captions in typeset order for print-header. Copied to header.tmp each pass."""
    BUILD.mkdir(parents=True, exist_ok=True)
    path = BUILD / "header.tmp.seed"
    lines = [header_caption(s) for s in snippets if not s.error]
    path.write_text("".join(line + "\n" for line in lines), encoding="utf-8")
    return path
