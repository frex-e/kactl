"""Emit per-snippet lstlisting TeX and consume the page-header caption queue."""

from __future__ import annotations

from pathlib import Path
from typing import TextIO

from . import BUILD
from .model import Document
from .snippet import ProcessedSnippet
from .texesc import codeescape, escape, ordoescape, pathescape


def listing_tex(snippet: ProcessedSnippet) -> str:
    """Format one processed snippet for the PDF listings adapter."""
    caption = snippet.caption
    if snippet.error:
        return r"\kactlerror{%s: %s}" % (caption, snippet.error) + "\n"

    out: list[str] = [r"\kactlref{%s}" % pathescape(caption).strip()]
    if snippet.mode == "raw":
        out.append(r"\rightcaption{%d lines}" % snippet.line_count)
        out.append(
            r"\begin{lstlisting}[language=%s,caption={%s}]"
            % (snippet.listings_lang, pathescape(caption))
        )
        out.append(snippet.code)
        out.append(r"\end{lstlisting}")
        return "\n".join(out) + "\n"

    commands = snippet.commands
    if commands.get("Description"):
        out.append(r"\defdescription{%s}" % escape(commands["Description"]))
    if commands.get("Usage"):
        out.append(r"\defusage{%s}" % codeescape(commands["Usage"]))
    if commands.get("Time"):
        out.append(r"\deftime{%s}" % ordoescape(commands["Time"]))
    if commands.get("Memory"):
        out.append(r"\defmemory{%s}" % ordoescape(commands["Memory"]))
    if snippet.includes:
        out.append(r"\leftcaption{%s}" % pathescape(", ".join(snippet.includes)))
    if snippet.code:
        out.append(
            r"\rightcaption{%s%d lines}" % (snippet.hash_prefix, snippet.line_count)
        )
    out.append(
        r"\begin{lstlisting}[caption={%s}, language=%s]"
        % (pathescape(caption), snippet.listings_lang)
    )
    out.append(snippet.code)
    out.append(r"\end{lstlisting}")
    return "\n".join(out) + "\n"


def header_caption(snippet: ProcessedSnippet) -> str:
    return pathescape(snippet.caption).strip()


def listing_path(chapter_id: str, filename: str) -> Path:
    return BUILD / "listings" / chapter_id / f"{filename}.tex"


def write_listing(chapter_id: str, snippet: ProcessedSnippet) -> Path:
    path = listing_path(chapter_id, snippet.caption)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(listing_tex(snippet), encoding="utf-8")
    return path


def write_header_seed(snippets: list[ProcessedSnippet]) -> Path:
    """Captions in typeset order. Make copies this to header.tmp before each pdflatex pass."""
    BUILD.mkdir(parents=True, exist_ok=True)
    path = BUILD / "header.tmp.seed"
    lines = [header_caption(s) for s in snippets if not s.error]
    path.write_text("".join(line + "\n" for line in lines), encoding="utf-8")
    return path


def write_pdf_artifacts(document: Document) -> tuple[Path, Path]:
    """Write all artifacts consumed by pdflatex from the shared document."""
    for snippet in document.snippets:
        if snippet.included_in_pdf:
            write_listing(snippet.chapter, snippet.processed)
    included = [
        snippet.processed
        for snippet in document.snippets
        if snippet.included_in_pdf and not snippet.processed.error
    ]
    return BUILD / "listings", write_header_seed(included)


def print_header(data: str, outstream: TextIO, header_tmp: Path | None = None) -> None:
    """Consume captions from header.tmp up through the last mark on this page."""
    path = header_tmp if header_tmp is not None else BUILD / "header.tmp"
    parts = data.split("|")
    until = parts[0].strip() or parts[1].strip()
    if not until:
        return
    if not path.is_file():
        return
    lines = [x.strip() for x in path.read_text(encoding="utf-8").splitlines()]
    if until not in lines:
        return

    ind = lines.index(until) + 1
    header_length = len("".join(lines[:ind]))

    def adjust(name: str) -> str:
        return name if name.startswith(".") else name.split(".")[0]

    output = r"\enspace{}".join(map(adjust, lines[:ind]))
    font_size = 8 if header_length > 150 else 10
    output = r"\hspace{3mm}\textbf{" + output + "}"
    output = "\\fontsize{%d}{%d}" % (font_size, font_size) + output
    print(output, file=outstream)
    path.write_text("".join(line + "\n" for line in lines[ind:]), encoding="utf-8")
