#!/usr/bin/env python3
"""Build snippets.json from KACTL content/ for the web SPA."""

from __future__ import annotations

import json
import re
import unicodedata
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CONTENT = REPO_ROOT / "content"
OUT = Path(__file__).resolve().parents[1] / "public" / "snippets.json"

# Match PDF chapter order (content/kactl.tex).
CHAPTER_ORDER = [
    "contest",
    "math",
    "data-structures",
    "numerical",
    "number-theory",
    "combinatorial",
    "graph",
    "geometry",
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

EXCLUDED_CHAPTERS = {"tex", "test-session"}
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

HEADING_LEVEL = {
    "chapter": 1,
    "section": 2,
    "subsection": 3,
    "subsubsection": 4,
}

IMPORT_RE = re.compile(r"^(\s*)%?\s*\\kactlimport(?:\[[^\]]*\])?\{([^}]+)\}")
HEADING_CMD_RE = re.compile(
    r"^\\(chapter|section|subsection|subsubsection)\*?"
)
IMPORT_FILE_RE = re.compile(r"^\\import\{([^}]+)\}")
INCLUDE_RE = re.compile(r'^\s*#include\s+([<"][^>"]+[>"])')
INCLUDEGRAPHICS_RE = re.compile(r"\\includegraphics(?:\[[^\]]*\])?\{[^}]*\}")
BEGIN_MINIPAGE_RE = re.compile(r"\\begin\{minipage\}(?:\[[^\]]*\])?\{[^}]*\}")
SLUG_STRIP_RE = re.compile(r"[^a-z0-9]+")


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
        _indent, name = m.group(1), m.group(2)
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
            continue
        if path.name.startswith(".") and path.name not in imports:
            if path.name not in imports:
                continue
        files[path.name] = path
    # Ensure imported names are present even if odd suffix
    for name in imports:
        p = chapter_dir / name
        if p.is_file():
            files[name] = p
    return list(files.values())


def wrap_ordo(text: str) -> str:
    """Wrap bare O(...) in $...$, leaving surrounding prose in text mode.

    Wrapping a whole Time line (e.g. Simplex.h) as math drops spaces.
    """
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


def read_brace_group(src: str, open_idx: int) -> tuple[str, int] | None:
    if open_idx >= len(src) or src[open_idx] != "{":
        return None
    depth = 0
    i = open_idx
    while i < len(src):
        c = src[i]
        if c == "\\" and i + 1 < len(src):
            i += 2
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return src[open_idx + 1 : i], i + 1
        i += 1
    return None


def strip_tex_comment(line: str) -> str:
    """Drop unescaped % comments, keeping \\%."""
    out: list[str] = []
    i = 0
    while i < len(line):
        if line[i] == "\\" and i + 1 < len(line):
            out.append(line[i : i + 2])
            i += 2
            continue
        if line[i] == "%":
            break
        out.append(line[i])
        i += 1
    return "".join(out).rstrip()


def strip_figures(text: str) -> str:
    """Drop includegraphics and unwrap minipage wrappers in snippet headers."""
    text = INCLUDEGRAPHICS_RE.sub("", text)
    text = BEGIN_MINIPAGE_RE.sub("\n", text)
    text = text.replace(r"\end{minipage}", "\n")
    text = re.sub(r"\\vspace(?:\*)?(?:\[[^\]]*\])?\{[^}]*\}", "", text)
    text = re.sub(r"\\hspace(?:\*)?(?:\[[^\]]*\])?\{[^}]*\}", "", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    text = text.strip()
    text = re.sub(r"^(?:\\\\|\s)+", "", text)
    return text.strip()


def latex_to_search_text(src: str) -> str:
    """Approximate plaintext for MiniSearch from a TeX fragment."""
    s = src

    def verb_sub(m: re.Match[str]) -> str:
        return m.group(2)

    s = re.sub(r"\\verb([^a-zA-Z\s])(.*?)(\1)", verb_sub, s)
    s = re.sub(r"\\\[(.+?)\\\]", r" \1 ", s, flags=re.DOTALL)
    s = re.sub(r"\$\$(.+?)\$\$", r" \1 ", s, flags=re.DOTALL)
    s = re.sub(r"\$([^$]+)\$", r" \1 ", s)
    s = INCLUDEGRAPHICS_RE.sub(" ", s)
    s = re.sub(r"\\begin\{[^}]+\}\[[^\]]*\]", " ", s)
    s = re.sub(r"\\(begin|end)\{[^}]+\}", " ", s)
    s = re.sub(r"\\item\b", " ", s)
    s = re.sub(r"\\hline\b", " ", s)
    for _ in range(10):
        n = re.sub(r"\\[a-zA-Z]+\*?\{([^{}]*)\}", r" \1 ", s)
        if n == s:
            break
        s = n
    s = re.sub(r"\\[a-zA-Z]+\*?", " ", s)
    s = re.sub(r"\\(.)", r"\1", s)
    s = re.sub(r"[{}&%#~]", " ", s)
    s = re.sub(r"\s+", " ", s).strip()
    return s


def slugify(title: str) -> str:
    text = latex_to_search_text(title).lower()
    text = unicodedata.normalize("NFKD", text)
    text = "".join(ch for ch in text if not unicodedata.combining(ch))
    text = SLUG_STRIP_RE.sub("-", text).strip("-")
    return text or "section"


def normalize_tex_prose(latex: str) -> str:
    """Join TeX line-wraps; keep blank lines as paragraph breaks."""
    latex = latex.strip()
    if not latex:
        return ""
    paras = re.split(r"\n\s*\n", latex)
    out = []
    for p in paras:
        out.append(re.sub(r"[ \t]*\n[ \t]*", " ", p.strip()))
    return "\n\n".join(x for x in out if x)


def unique_block_id(used: set[str], candidate: str) -> str:
    if candidate not in used:
        used.add(candidate)
        return candidate
    i = 2
    while f"{candidate}-{i}" in used:
        i += 1
    uid = f"{candidate}-{i}"
    used.add(uid)
    return uid


def parse_chapter_document(chapter_id: str) -> list[dict]:
    """Walk chapter.tex into heading / prose / snippet blocks (PDF order)."""
    chapter_dir = CONTENT / chapter_id
    chapter_tex = chapter_dir / "chapter.tex"
    if not chapter_tex.is_file():
        return []

    used_ids: set[str] = set()
    blocks: list[dict] = []
    prose_buf: list[str] = []
    prose_n = 0

    def flush_prose():
        nonlocal prose_n
        raw = normalize_tex_prose("\n".join(prose_buf))
        prose_buf.clear()
        if not raw:
            return
        search = latex_to_search_text(raw)
        leftover = INCLUDEGRAPHICS_RE.sub("", raw)
        leftover = re.sub(r"\\(?:begin|end)\{center\}", "", leftover).strip()
        if not search and len(leftover) < 12:
            return
        prose_n += 1
        blocks.append(
            {
                "type": "prose",
                "id": unique_block_id(used_ids, f"{chapter_id}/p/{prose_n}"),
                "chapter": chapter_id,
                "latex": raw,
                "searchText": search,
            }
        )

    def append_imported_tex(name: str):
        path = chapter_dir / name
        if not path.is_file():
            return
        text = path.read_text(encoding="utf-8", errors="replace")
        cleaned = []
        for line in text.splitlines():
            cleaned.append(strip_tex_comment(line))
        prose_buf.append("\n".join(cleaned))

    for line in chapter_tex.read_text(encoding="utf-8", errors="replace").splitlines():
        raw_line = line
        import_m = IMPORT_RE.match(raw_line)
        if import_m:
            flush_prose()
            name = import_m.group(2)
            commented = raw_line.lstrip().startswith("%")
            sid = f"{chapter_id}/{name}"
            used_ids.add(sid)
            blocks.append(
                {
                    "type": "snippet",
                    "id": sid,
                    "chapter": chapter_id,
                    "includedInPdf": not commented,
                }
            )
            continue

        stripped = strip_tex_comment(raw_line).strip()
        if not stripped:
            if prose_buf:
                prose_buf.append("")
            continue

        heading_m = HEADING_CMD_RE.match(stripped)
        if heading_m:
            rest = stripped[heading_m.end() :].lstrip()
            title = ""
            if rest.startswith("{"):
                group = read_brace_group(rest, 0)
                if group:
                    title = group[0]
            if title:
                flush_prose()
                level = HEADING_LEVEL[heading_m.group(1)]
                slug = slugify(title)
                blocks.append(
                    {
                        "type": "heading",
                        "id": unique_block_id(used_ids, f"{chapter_id}/h/{slug}"),
                        "chapter": chapter_id,
                        "level": level,
                        "title": title,
                        "searchText": latex_to_search_text(title) or title,
                    }
                )
                continue

        file_m = IMPORT_FILE_RE.match(stripped)
        if file_m:
            append_imported_tex(file_m.group(1))
            continue

        if stripped in {r"\appendix", r"\maketitle"}:
            continue

        prose_buf.append(stripped)

    flush_prose()
    return blocks


def main():
    chapters = []
    snippets = []
    document: list[dict] = []
    # First pass: collect all candidate files and raw deps
    pending = []  # (id, chapter, path, commands, raw_includes, code, included)

    for chapter_id in CHAPTER_ORDER:
        if chapter_id in EXCLUDED_CHAPTERS:
            continue
        imports = parse_chapter_imports(chapter_id)
        files = discover_files(chapter_id, imports)
        chapter_doc = parse_chapter_document(chapter_id)

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

        mentioned = {b["id"] for b in chapter_doc if b["type"] == "snippet"}
        for sid, ch, path, commands, raw_includes, code, included in chapter_snippets:
            if sid not in mentioned:
                chapter_doc.append(
                    {
                        "type": "snippet",
                        "id": sid,
                        "chapter": ch,
                        "includedInPdf": included,
                    }
                )

        if chapter_doc or chapter_snippets:
            chapters.append({"id": chapter_id, "title": CHAPTER_TITLES.get(chapter_id, chapter_id)})
            document.extend(chapter_doc)
            pending.extend(chapter_snippets)

    id_set = {p[0] for p in pending}

    for sid, chapter_id, path, commands, raw_includes, code, included in pending:
        deps = []
        for raw in raw_includes:
            resolved = resolve_include(sid, raw, id_set)
            if resolved and resolved not in deps:
                deps.append(resolved)

        desc = strip_figures(commands.get("Description", ""))
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

    payload = {"chapters": chapters, "snippets": snippets, "document": document}
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    n_h = sum(1 for b in document if b["type"] == "heading")
    n_p = sum(1 for b in document if b["type"] == "prose")
    n_s = sum(1 for b in document if b["type"] == "snippet")
    print(
        f"Wrote {len(snippets)} snippets, {len(document)} document blocks "
        f"({n_h} headings, {n_p} prose, {n_s} snippets) in {len(chapters)} chapters → {OUT}"
    )


if __name__ == "__main__":
    main()
