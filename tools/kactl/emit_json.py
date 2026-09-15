"""Emit web/public/snippets.json from processed snippets and chapter documents."""

from __future__ import annotations

import json
from pathlib import Path

from . import WEB_PUBLIC
from .chapter import (
    resolve_include,
    strip_figures,
)
from .model import Document, Snippet


def wrap_ordo(text: str) -> str:
    """Wrap bare O(...) in $...$, leaving surrounding prose in text mode."""
    out: list[str] = []
    i = 0
    in_math = False
    while i < len(text):
        if text[i] == "$":
            in_math = not in_math
            out.append("$")
            i += 1
            continue
        if not in_math and text.startswith("O(", i):
            depth = 0
            j = i + 1
            while j < len(text):
                if text[j] == "(":
                    depth += 1
                elif text[j] == ")":
                    depth -= 1
                    if depth == 0:
                        j += 1
                        break
                j += 1
            out.append("$" + text[i:j] + "$")
            i = j
            continue
        out.append(text[i])
        i += 1
    return "".join(out)


def wrap_time(t: str) -> str:
    t = t.strip()
    if not t:
        return ""
    return wrap_ordo(t)


def snippet_json(
    snippet: Snippet,
    id_set: set[str],
) -> dict:
    processed = snippet.processed
    deps: list[str] = []
    for raw in processed.includes:
        resolved = resolve_include(snippet.id, raw, id_set)
        if resolved and resolved not in deps:
            deps.append(resolved)
    commands = processed.commands
    return {
        "id": snippet.id,
        "name": processed.path.name,
        "chapter": snippet.chapter,
        "language": processed.syntax_lang,
        "description": strip_figures(commands.get("Description", "")),
        "usage": commands.get("Usage", ""),
        "time": wrap_time(commands.get("Time", "")),
        "memory": wrap_time(commands.get("Memory", "")),
        "status": commands.get("Status", ""),
        "author": commands.get("Author", ""),
        "source": commands.get("Source", ""),
        "dependencies": deps,
        "includedInPdf": snippet.included_in_pdf,
        "code": processed.code,
    }


def document_payload(document: Document) -> dict:
    """Format the shared document model as the web snippet index."""
    id_set = {snippet.id for snippet in document.snippets}
    snippets = [
        snippet_json(snippet, id_set) for snippet in document.snippets
    ]
    chapter_rank = {
        chapter.id: rank for rank, chapter in enumerate(document.chapters)
    }
    snippets.sort(
        key=lambda snippet: (
            chapter_rank.get(snippet["chapter"], len(chapter_rank)),
            snippet["name"].lower(),
        )
    )
    return {
        "chapters": [
            {"id": chapter.id, "title": chapter.title}
            for chapter in document.chapters
        ],
        "snippets": snippets,
        "document": document.blocks,
    }


def write_snippets_json(payload: dict, out: Path | None = None) -> Path:
    dest = out or (WEB_PUBLIC / "snippets.json")
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return dest
