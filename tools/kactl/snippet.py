"""Strip contest annotations, parse snippet headers, hash listings."""

from __future__ import annotations

import subprocess
from dataclasses import dataclass, field
from pathlib import Path

from . import REPO_ROOT
from .texesc import codeescape, escape, ordoescape, pathescape

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
REQUIRED_COMMANDS = ["Author", "Description"]
COMMENT_TYPES = [
    ("/**", "*/"),
    ("'''", "'''"),
    ('"""', '"""'),
]

# Former \kactlimport[-l ...] flags, keyed by basename.
LISTINGS_BY_NAME: dict[str, tuple[str, str]] = {
    "template.cpp": ("raw", "C++"),
    ".bashrc": ("raw", "bash"),
    ".vimrc": ("raw", "raw"),
    "hash.sh": ("raw", "raw"),
    "troubleshoot.txt": ("raw", "raw"),
    "techniques.txt": ("raw", "raw"),
}

LISTINGS_BY_EXT: dict[str, tuple[str, str]] = {
    "cpp": ("comments", "C++"),
    "cc": ("comments", "C++"),
    "c": ("comments", "C++"),
    "h": ("comments", "C++"),
    "hpp": ("comments", "C++"),
    "java": ("comments", "Java"),
    "kt": ("comments", "Java"),
    "py": ("comments", "Python"),
    "ps": ("raw", "raw"),
    "sh": ("raw", "bash"),
    "txt": ("raw", "raw"),
}

CLI_LANG: dict[str, tuple[str, str]] = {
    "cpp": ("comments", "C++"),
    "cc": ("comments", "C++"),
    "c": ("comments", "C++"),
    "h": ("comments", "C++"),
    "hpp": ("comments", "C++"),
    "java": ("comments", "Java"),
    "kt": ("comments", "Java"),
    "py": ("comments", "Python"),
    "ps": ("raw", "raw"),
    "raw": ("raw", "raw"),
    "rawcpp": ("raw", "C++"),
    "sh": ("raw", "bash"),
    "rawpy": ("raw", "Python"),
}


@dataclass
class ProcessedSnippet:
    path: Path
    caption: str
    mode: str
    listings_lang: str
    commands: dict[str, str] = field(default_factory=dict)
    includes: list[str] = field(default_factory=list)
    code: str = ""
    hash_prefix: str = ""
    line_count: int = 0
    error: str = ""


def parse_include(line: str) -> str | None:
    line = line.strip()
    if line.startswith("#include"):
        return line[8:].strip()
    return None


def find_start_comment(source: str, start: int | None = None):
    first = (-1, -1, None)
    for s, e in COMMENT_TYPES:
        i = source.find(s, start)
        if i != -1 and (i < first[0] or first[0] == -1):
            first = (i, i + len(s), e)
    return first


def ext_of(name: str) -> str:
    if "." not in name:
        return ""
    return name.rsplit(".", 1)[-1]


def resolve_language(filename: str, lang_flag: str | None = None) -> tuple[str, str]:
    """Return (mode, listings_lang). mode is 'comments' or 'raw'."""
    if lang_flag:
        key = lang_flag.lower()
        if key not in CLI_LANG:
            raise ValueError("Unknown language: " + str(lang_flag))
        return CLI_LANG[key]
    if filename in LISTINGS_BY_NAME:
        return LISTINGS_BY_NAME[filename]
    ext = ext_of(filename).lower()
    if ext in LISTINGS_BY_EXT:
        return LISTINGS_BY_EXT[ext]
    if ext in CLI_LANG:
        return CLI_LANG[ext]
    raise ValueError("Unknown language: " + str(ext or filename))


def hash_source(code: str) -> str:
    script = REPO_ROOT / "content" / "contest" / "hash.sh"
    p = subprocess.Popen(
        ["sh", str(script)],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        encoding="utf-8",
        cwd=str(REPO_ROOT),
    )
    hsh, _ = p.communicate(code)
    return hsh.split(None, 1)[0]


def process_with_comments(path: Path, caption: str, listings_lang: str, text: str) -> ProcessedSnippet:
    error = ""
    includelist: list[str] = []
    nlines: list[str] = []
    for line in text.splitlines():
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
    while start >= 0 and not error:
        nsource = nsource.rstrip() + source[end:start]
        end = source.find(end_str, start2)
        if end < start:
            error = "Invalid %s %s comments." % (source[start:start2], end_str)
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
                    if command not in KNOWN_COMMANDS:
                        error = error + "Unknown command: " + command + ". "
                    commands[command] = value.lstrip()
                command = cline[:ind]
                value = cline[ind + 1 :].strip()
            else:
                value = value + "\n" + cline
        if command:
            if command not in KNOWN_COMMANDS:
                error = error + "Unknown command: " + command + ". "
            commands[command] = value.lstrip()
    for rcommand in sorted(set(REQUIRED_COMMANDS) - set(commands)):
        error = error + "Missing command: " + rcommand + ". "
    if end >= 0:
        nsource = nsource.rstrip() + source[end:]
    nsource = nsource.strip()

    hash_prefix = ""
    if not error and listings_lang in ("C++", "Java"):
        hash_prefix = hash_source(nsource) + ", "

    line_count = len(nsource.split("\n")) if nsource else 0
    return ProcessedSnippet(
        path=path,
        caption=caption,
        mode="comments",
        listings_lang=listings_lang,
        commands=commands,
        includes=includelist,
        code=nsource,
        hash_prefix=hash_prefix,
        line_count=line_count,
        error=error,
    )


def process_raw(path: Path, caption: str, listings_lang: str, text: str) -> ProcessedSnippet:
    code = text.strip()
    return ProcessedSnippet(
        path=path,
        caption=caption,
        mode="raw",
        listings_lang=listings_lang,
        code=code,
        line_count=len(code.split("\n")) if code else 0,
    )


def process_path(path: Path, lang_flag: str | None = None) -> ProcessedSnippet:
    caption = path.name
    try:
        mode, listings_lang = resolve_language(path.name, lang_flag)
    except ValueError as err:
        return ProcessedSnippet(
            path=path,
            caption=caption,
            mode="raw",
            listings_lang="raw",
            error=str(err),
        )
    try:
        text = path.read_text(encoding="utf-8")
    except OSError:
        return ProcessedSnippet(
            path=path,
            caption=caption,
            mode=mode,
            listings_lang=listings_lang,
            error="Could not read source.",
        )
    if mode == "raw":
        return process_raw(path, caption, listings_lang, text)
    return process_with_comments(path, caption, listings_lang, text)


def listing_tex(snippet: ProcessedSnippet) -> str:
    """Emit the lstlisting payload previously written by preprocessor.py."""
    caption = snippet.caption
    if snippet.error:
        return r"\kactlerror{%s: %s}" % (caption, snippet.error) + "\n"

    out: list[str] = []
    out.append(r"\kactlref{%s}" % pathescape(caption).strip())
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
    langstr = ", language=" + snippet.listings_lang
    out.append(r"\begin{lstlisting}[caption={%s}%s]" % (pathescape(caption), langstr))
    out.append(snippet.code)
    out.append(r"\end{lstlisting}")
    return "\n".join(out) + "\n"


def header_caption(snippet: ProcessedSnippet) -> str:
    return pathescape(snippet.caption).strip()
