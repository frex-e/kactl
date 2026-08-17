"""Emit web/public/snippets.json from processed snippets and chapter documents."""

from __future__ import annotations

import json
from pathlib import Path

from . import WEB_PUBLIC
from .chapter import (
    resolve_include,
    strip_figures,
)
from .snippet import ProcessedSnippet


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
    sid: str,
    chapter_id: str,
    snippet: ProcessedSnippet,
    included: bool,
    id_set: set[str],
) -> dict:
    deps: list[str] = []
    for raw in snippet.includes:
        resolved = resolve_include(sid, raw, id_set)
        if resolved and resolved not in deps:
            deps.append(resolved)
    commands = snippet.commands
    return {
        "id": sid,
        "name": snippet.path.name,
        "chapter": chapter_id,
        "description": strip_figures(commands.get("Description", "")),
        "usage": commands.get("Usage", ""),
        "time": wrap_time(commands.get("Time", "")),
        "memory": wrap_time(commands.get("Memory", "")),
        "status": commands.get("Status", ""),
        "author": commands.get("Author", ""),
        "source": commands.get("Source", ""),
        "dependencies": deps,
        "includedInPdf": included,
        "code": snippet.code,
    }


def write_snippets_json(payload: dict, out: Path | None = None) -> Path:
    dest = out or (WEB_PUBLIC / "snippets.json")
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return dest
