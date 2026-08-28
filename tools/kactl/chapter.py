"""Parse kactl.tex chapter order and chapter.tex document structure."""

from __future__ import annotations

import re
import unicodedata
from dataclasses import dataclass
from pathlib import Path

from . import CONTENT

CODE_SUFFIXES = {".h", ".hpp", ".cpp", ".cc", ".c", ".java", ".py", ".sh", ".txt"}

HEADING_LEVEL = {
    "chapter": 1,
    "section": 2,
    "subsection": 3,
    "subsubsection": 4,
}

CHAPTER_RE = re.compile(r"\\kactlchapter\{([^}]+)\}")
IMPORT_RE = re.compile(r"^(\s*)%?\s*\\kactlimport(?:\[([^\]]*)\])?\{([^}]+)\}")
HEADING_CMD_RE = re.compile(r"^\\(chapter|section|subsection|subsubsection)\*?")
IMPORT_FILE_RE = re.compile(r"^\\import\{([^}]+)\}")
INCLUDEGRAPHICS_RE = re.compile(r"\\includegraphics(?:\[[^\]]*\])?\{[^}]*\}")
BEGIN_MINIPAGE_RE = re.compile(r"\\begin\{minipage\}(?:\[[^\]]*\])?\{[^}]*\}")
LANG_FLAG_RE = re.compile(r"-l\s*(\S+)")
SLUG_STRIP_RE = re.compile(r"[^a-z0-9]+")


@dataclass
class KactlImport:
    name: str
    included_in_pdf: bool
    lang_flag: str | None = None


def chapter_order(kactl_tex: Path | None = None) -> list[str]:
    path = kactl_tex or (CONTENT / "kactl.tex")
    return CHAPTER_RE.findall(path.read_text(encoding="utf-8", errors="replace"))


def parse_lang_flag(optional: str | None) -> str | None:
    if not optional:
        return None
    m = LANG_FLAG_RE.search(optional)
    return m.group(1) if m else None


def parse_chapter_imports(chapter_id: str) -> dict[str, KactlImport]:
    """Map filename -> import. First active wins; commented only if unseen."""
    chapter_tex = CONTENT / chapter_id / "chapter.tex"
    result: dict[str, KactlImport] = {}
    if not chapter_tex.exists():
        return result
    for line in chapter_tex.read_text(encoding="utf-8", errors="replace").splitlines():
        m = IMPORT_RE.match(line)
        if not m:
            continue
        optional, name = m.group(2), m.group(3)
        commented = line.lstrip().startswith("%")
        if name not in result or not commented:
            result[name] = KactlImport(
                name=name,
                included_in_pdf=not commented,
                lang_flag=parse_lang_flag(optional),
            )
    return result


def strip_quotes(include: str) -> str:
    include = include.strip()
    if len(include) >= 2 and include[0] in "<\"'" and include[-1] in ">\"'":
        return include[1:-1]
    return include


def resolve_include(from_id: str, include_raw: str, id_set: set[str]) -> str | None:
    """Resolve #include path to a snippet id, or None if system/missing."""
    target = strip_quotes(include_raw)
    if include_raw.strip().startswith("<"):
        return None

    if "content/" in target:
        cand = target.split("content/", 1)[1]
        if cand in id_set:
            return cand
        return cand if cand.endswith(tuple(CODE_SUFFIXES)) else None

    from_chapter, _from_name = from_id.split("/", 1)
    from_dir = CONTENT / from_chapter
    resolved = (from_dir / target).resolve()
    try:
        rel = resolved.relative_to(CONTENT.resolve())
    except ValueError:
        return None
    cand = str(rel).replace("\\", "/")
    if cand in id_set:
        return cand

    same = f"{from_chapter}/{Path(target).name}"
    if same in id_set:
        return same

    return cand if (CONTENT / cand).exists() else None


def discover_files(chapter_id: str, imports: dict[str, KactlImport]) -> list[Path]:
    chapter_dir = CONTENT / chapter_id
    if not chapter_dir.is_dir():
        return []
    files: dict[str, Path] = {}
    import_names = set(imports)
    for path in sorted(chapter_dir.iterdir()):
        if not path.is_file():
            continue
        if path.suffix.lower() not in CODE_SUFFIXES and path.name not in import_names:
            continue
        if path.name.startswith(".") and path.name not in import_names:
            continue
        files[path.name] = path
    for name in imports:
        p = chapter_dir / name
        if p.is_file():
            files[name] = p
    return list(files.values())


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


def unwrap_kactlfigdesc(text: str) -> str:
    """Keep description text from \\kactlfigdesc{text}{figure}; drop the figure path."""
    out: list[str] = []
    i = 0
    needle = r"\kactlfigdesc"
    while True:
        j = text.find(needle, i)
        if j < 0:
            out.append(text[i:])
            break
        out.append(text[i:j])
        p = j + len(needle)
        while p < len(text) and text[p].isspace():
            p += 1
        g1 = read_brace_group(text, p) if p < len(text) and text[p] == "{" else None
        if not g1:
            out.append(text[j:])
            break
        body, p = g1
        while p < len(text) and text[p].isspace():
            p += 1
        g2 = read_brace_group(text, p) if p < len(text) and text[p] == "{" else None
        if g2:
            _path, p = g2
        out.append(body)
        i = p
    return "".join(out)


def strip_figures(text: str) -> str:
    """Drop figures from snippet headers (kactlfigdesc, includegraphics, minipages)."""
    text = unwrap_kactlfigdesc(text)
    text = INCLUDEGRAPHICS_RE.sub("", text)
    text = BEGIN_MINIPAGE_RE.sub("\n", text)
    # `%` after \end{minipage} is the usual "eat the following newline" comment.
    text = re.sub(r"\\end\{minipage\}%?", "\n", text)
    text = re.sub(r"\\vspace(?:\*)?(?:\[[^\]]*\])?\{[^}]*\}", "", text)
    text = re.sub(r"\\hspace(?:\*)?(?:\[[^\]]*\])?\{[^}]*\}", "", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    text = text.strip()
    text = re.sub(r"^(?:\\\\|\s)+", "", text)
    # A glued minipage pair can leave a leftover `%` at the end of the prose.
    text = re.sub(r"(?:\n\s*)+%+\s*$", "", text)
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


def chapter_title(chapter_id: str) -> str:
    chapter_tex = CONTENT / chapter_id / "chapter.tex"
    if not chapter_tex.is_file():
        return chapter_id
    for line in chapter_tex.read_text(encoding="utf-8", errors="replace").splitlines():
        stripped = strip_tex_comment(line).strip()
        m = HEADING_CMD_RE.match(stripped)
        if not m or m.group(1) != "chapter":
            continue
        rest = stripped[m.end() :].lstrip()
        if rest.startswith("{"):
            group = read_brace_group(rest, 0)
            if group:
                return latex_to_search_text(group[0]) or group[0]
    return chapter_id


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
        cleaned = [strip_tex_comment(line) for line in text.splitlines()]
        prose_buf.append("\n".join(cleaned))

    for line in chapter_tex.read_text(encoding="utf-8", errors="replace").splitlines():
        raw_line = line
        import_m = IMPORT_RE.match(raw_line)
        if import_m:
            flush_prose()
            name = import_m.group(3)
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
