#!/usr/bin/env python3
"""Build snippets.json from KACTL content/ for the web SPA."""

from __future__ import annotations

import json
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CONTENT = REPO_ROOT / "content"
OUT = Path(__file__).resolve().parents[1] / "public" / "snippets.json"

# Match PDF chapter order; geometry is intentionally omitted from the site.
CHAPTER_ORDER = [
    "contest",
    "math",
    "data-structures",
    "numerical",
    "number-theory",
    "combinatorial",
    # "geometry",  # TODO: figures / minipage headers
    "graph",
    "strings",
    "various",
    "appendix",
]

CHAPTER_TITLES = {
    "contest": "Contest",
    "math": "Mathematics",
    "data-structures": "Data structures",
    "numerical": "Numerical",
    "number-theory": "Number theory",
    "combinatorial": "Combinatorial",
    "geometry": "Geometry",
    "graph": "Graph",
    "strings": "Strings",
    "various": "Various",
    "appendix": "Techniques",
}

EXCLUDED_CHAPTERS = {"geometry", "tex", "test-session"}
CODE_SUFFIXES = {".h", ".hpp", ".cpp", ".cc", ".c", ".java", ".py", ".sh", ".txt"}
KNOWN_COMMANDS = [
    "Author",
    "Date",
    "Description",
    "Source",
    "Time",
    "Memory",
    "License",
    "Status",
    "Usage",
    "Details",
]

COMMENT_TYPES = [
    ("/**", "*/"),
    ("'''", "'''"),
    ('"""', '"""'),
]

IMPORT_RE = re.compile(r"^(\s*)%?\s*\\kactlimport(?:\[[^\]]*\])?\{([^}]+)\}")
INCLUDE_RE = re.compile(r'^\s*#include\s+([<"][^>"]+[>"])')


def find_start_comment(source: str, start: int = 0):
    first = (-1, -1, None)
    for s, e in COMMENT_TYPES:
        i = source.find(s, start)
        if i != -1 and (i < first[0] or first[0] == -1):
            first = (i, i + len(s), e)
    return first


def parse_include(line: str):
    line = line.strip()
    if line.startswith("#include"):
        return line[8:].strip()
    return None


def strip_quotes(include: str) -> str:
    include = include.strip()
    if len(include) >= 2 and include[0] in "<\"'" and include[-1] in ">\"'":
        return include[1:-1]
    return include


def parse_header_and_code(path: Path):
    """Return (commands, dependencies_raw, cleaned_code)."""
    text = path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    includelist = []
    nlines = []
    for line in lines:
        if "exclude-line" in line:
            continue
        if "include-line" in line:
            line = line.replace("// ", "", 1)
        had_comment = "///" in line
        keep_include = "keep-include" in line
        line = line.split("///")[0].rstrip()
        if line == "#pragma once":
            continue
        if had_comment and not line:
            continue
        include = parse_include(line)
        if include is not None and not keep_include:
            includelist.append(include)
            continue
        nlines.append(line)

    source = "\n".join(nlines)
    nsource = ""
    start, start2, end_str = find_start_comment(source)
    end = 0
    commands: dict[str, str] = {}
    while start >= 0:
        nsource = nsource.rstrip() + source[end:start]
        end = source.find(end_str, start2)
        if end < start:
            break
        comment = source[start2:end].strip()
        end += len(end_str)
        start, start2, end_str = find_start_comment(source, end)

        command = None
        value = ""
        for cline in comment.split("\n"):
            allow_command = False
            cline = cline.strip()
            if cline.startswith("*"):
                cline = cline[1:].strip()
                allow_command = True
            ind = cline.find(":")
            if (
                allow_command
                and ind != -1
                and " " not in cline[:ind]
                and cline
                and cline[0].isalpha()
                and cline[0].isupper()
            ):
                if command:
                    commands[command] = value.lstrip()
                command = cline[:ind]
                value = cline[ind + 1 :].strip()
            else:
                value = value + "\n" + cline
        if command:
            commands[command] = value.lstrip()

    if end >= 0:
        nsource = nsource.rstrip() + source[end:]
    nsource = nsource.strip()

    # Files without /** headers (template.cpp, hash.sh, …): keep full cleaned body.
    if not commands and not includelist:
        # Still strip pragma//// already done; use nsource or raw without header block.
        pass

    return commands, includelist, nsource


def parse_chapter_imports(chapter_id: str) -> dict[str, bool]:
    """Map filename -> includedInPdf (False if commented out)."""
    chapter_tex = CONTENT / chapter_id / "chapter.tex"
    result: dict[str, bool] = {}
    if not chapter_tex.exists():
        return result
    for line in chapter_tex.read_text(encoding="utf-8", errors="replace").splitlines():
        m = IMPORT_RE.match(line)
        if not m:
            continue
        indent, name = m.group(1), m.group(2)
        commented = line.lstrip().startswith("%")
        # First active wins; commented only recorded if unseen.
        if name not in result or not commented:
            result[name] = not commented
    return result


def resolve_include(from_id: str, include_raw: str, id_set: set[str]) -> str | None:
    """Resolve #include path to a snippet id, or None if system/missing."""
    target = strip_quotes(include_raw)
    if include_raw.strip().startswith("<"):
        return None  # system header

    # Odd style: ../../content/geometry/Point.h
    if "content/" in target:
        parts = target.split("content/", 1)[1]
        cand = parts
        if cand in id_set:
            return cand
        return cand if cand.endswith(tuple(CODE_SUFFIXES)) else None

    from_chapter, from_name = from_id.split("/", 1)
    from_dir = CONTENT / from_chapter

    # Relative path from the including file's directory
    resolved = (from_dir / target).resolve()
    try:
        rel = resolved.relative_to(CONTENT.resolve())
    except ValueError:
        return None
    cand = str(rel).replace("\\", "/")
    if cand in id_set:
        return cand

    # Same-chapter bare name
    same = f"{from_chapter}/{Path(target).name}"
    if same in id_set:
        return same

    return cand if (CONTENT / cand).exists() else None


def discover_files(chapter_id: str, imports: dict[str, bool]) -> list[Path]:
    chapter_dir = CONTENT / chapter_id
    if not chapter_dir.is_dir():
        return []
    files: dict[str, Path] = {}
    for path in sorted(chapter_dir.iterdir()):
        if not path.is_file():
            continue
        if path.suffix.lower() not in CODE_SUFFIXES and path.name not in imports:
            # allow extensionless? no
            continue
        if path.name.startswith(".") and path.name not in imports:
            # include .bashrc / .vimrc if imported
            if path.name not in imports:
                continue
        files[path.name] = path
    # Ensure imported names are present even if odd suffix
    for name in imports:
        p = chapter_dir / name
        if p.is_file():
            files[name] = p
    return list(files.values())


def wrap_time(t: str) -> str:
    t = t.strip()
    if not t:
        return ""
    if "$" in t:
        return t
    # Bare O(...) → math-ish for KaTeX
    if t.startswith("O(") or t.startswith("O ("):
        return f"${t}$"
    return t


def main():
    chapters = []
    snippets = []
    # First pass: collect all candidate files and raw deps
    pending = []  # (id, chapter, path, commands, raw_includes, code, included)

    for chapter_id in CHAPTER_ORDER:
        if chapter_id in EXCLUDED_CHAPTERS:
            continue
        imports = parse_chapter_imports(chapter_id)
        files = discover_files(chapter_id, imports)
        if not files and chapter_id == "math":
            # Math is formulas-only; still list chapter for completeness? Skip empty.
            continue

        chapter_snippets = []
        for path in files:
            sid = f"{chapter_id}/{path.name}"
            commands, raw_includes, code = parse_header_and_code(path)
            # Skip empty junk
            if not code and not commands.get("Description"):
                continue
            included = imports.get(path.name, False)
            # Prefer files that are either imported or look like library headers
            if path.name not in imports and path.suffix not in {".h", ".hpp", ".cpp", ".java"}:
                continue
            chapter_snippets.append(
                (sid, chapter_id, path, commands, raw_includes, code, included)
            )

        if chapter_snippets:
            chapters.append({"id": chapter_id, "title": CHAPTER_TITLES.get(chapter_id, chapter_id)})
            pending.extend(chapter_snippets)

    id_set = {p[0] for p in pending}

    for sid, chapter_id, path, commands, raw_includes, code, included in pending:
        deps = []
        for raw in raw_includes:
            resolved = resolve_include(sid, raw, id_set)
            if resolved and resolved not in deps:
                deps.append(resolved)

        desc = commands.get("Description", "")
        usage = commands.get("Usage", "")
        time = wrap_time(commands.get("Time", ""))
        memory = wrap_time(commands.get("Memory", ""))

        snippets.append(
            {
                "id": sid,
                "name": path.name,
                "chapter": chapter_id,
                "description": desc,
                "usage": usage,
                "time": time,
                "memory": memory,
                "status": commands.get("Status", ""),
                "author": commands.get("Author", ""),
                "source": commands.get("Source", ""),
                "dependencies": deps,
                "includedInPdf": included,
                "code": code,
            }
        )

    # Stable order: chapter order, then name
    chapter_rank = {c: i for i, c in enumerate(CHAPTER_ORDER)}
    snippets.sort(key=lambda s: (chapter_rank.get(s["chapter"], 99), s["name"].lower()))

    payload = {"chapters": chapters, "snippets": snippets}
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Wrote {len(snippets)} snippets in {len(chapters)} chapters → {OUT}")


if __name__ == "__main__":
    main()
