"""Select stress tests affected by a set of changed files.

Used by ``make test-relevant`` so PR CI can skip the full ~81-test suite
when only a few snippets (or none) changed. Pushes to main still run
``make test``.

A test is selected if:

- it was itself added/modified, or
- it quoted-includes a changed file (transitively), or
- path convention matches: ``content/<chapter>/<stem>.*`` →
  ``stress-tests/<chapter>/<stem>.cpp`` (case-insensitive stem), which
  covers tests that paste an algorithm instead of ``#include``-ing it.

Infrastructure edits (test runner, selector, Makefile, C/C++ workflow)
fall back to the full suite. Doc/web/TeX-only changes select nothing.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.MULTILINE)

INFRA_FILES = frozenset(
    {
        "Makefile",
        "doc/scripts/run-all.sh",
        "doc/scripts/run-relevant.sh",
        "doc/scripts/cxx.sh",
        "tools/stress_select.py",
        ".github/workflows/ccpp.yml",
    }
)

BASE_CANDIDATES = ("origin/main", "main")


@dataclass
class Selection:
    mode: str  # "all" | "skip" | "subset"
    tests: list[str] = field(default_factory=list)
    reason: str = ""


def rel_posix(repo: Path, path: Path) -> str | None:
    try:
        return path.resolve().relative_to(repo.resolve()).as_posix()
    except ValueError:
        return None


def list_tests(repo: Path) -> list[str]:
    root = repo / "stress-tests"
    if not root.is_dir():
        return []
    tests = []
    for path in root.rglob("*.cpp"):
        rel = rel_posix(repo, path)
        if rel:
            tests.append(rel)
    return sorted(tests)


def quoted_includes(path: Path) -> list[str]:
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return []
    return INCLUDE_RE.findall(text)


def direct_includes(cur: Path, repo: Path) -> list[Path]:
    if not cur.is_file():
        return []
    out: list[Path] = []
    seen: set[Path] = set()
    for inc in quoted_includes(cur):
        nxt = (cur.parent / inc).resolve()
        if nxt in seen:
            continue
        if rel_posix(repo, nxt) is None:
            continue
        seen.add(nxt)
        out.append(nxt)
    return out


def reachable_includes(start: Path, repo: Path, cache: dict[Path, list[Path]]) -> set[str]:
    seen: set[str] = set()
    stack = [start.resolve()]
    while stack:
        cur = stack.pop()
        key = rel_posix(repo, cur)
        if key is None or key in seen:
            continue
        seen.add(key)
        if cur not in cache:
            cache[cur] = direct_includes(cur, repo)
        stack.extend(cache[cur])
    return seen


def file_to_tests(repo: Path) -> dict[str, set[str]]:
    index: dict[str, set[str]] = {}
    cache: dict[Path, list[Path]] = {}
    for test in list_tests(repo):
        for dep in reachable_includes(repo / test, repo, cache):
            index.setdefault(dep, set()).add(test)
    return index


def convention_tests(repo: Path, changed_rel: str) -> list[str]:
    parts = Path(changed_rel).parts
    if len(parts) < 3 or parts[0] != "content":
        return []
    chapter = parts[1]
    stem = Path(parts[-1]).stem.lower()
    testdir = repo / "stress-tests" / chapter
    if not testdir.is_dir():
        return []
    matched = []
    for path in testdir.glob("*.cpp"):
        if path.stem.lower() == stem:
            rel = rel_posix(repo, path)
            if rel:
                matched.append(rel)
    return matched


def is_infra(rel_path: str) -> bool:
    return rel_path in INFRA_FILES


def normalize_changed(repo: Path, files: list[str]) -> list[str]:
    out: list[str] = []
    seen: set[str] = set()
    repo = repo.resolve()
    for raw in files:
        raw = raw.strip().replace("\\", "/")
        if not raw:
            continue
        path = Path(raw)
        if path.is_absolute():
            rel = rel_posix(repo, path)
            if rel is None:
                continue
        else:
            rel = path.as_posix()
            while rel.startswith("./"):
                rel = rel[2:]
        if rel not in seen:
            seen.add(rel)
            out.append(rel)
    return out


def select(repo: Path, changed: list[str]) -> Selection:
    repo = repo.resolve()
    changed = normalize_changed(repo, changed)
    all_tests = list_tests(repo)
    if not all_tests:
        return Selection("skip", reason="no stress tests found")

    if any(is_infra(path) for path in changed):
        hits = [p for p in changed if is_infra(p)]
        return Selection(
            "all",
            list(all_tests),
            reason=f"infrastructure change ({', '.join(hits)})",
        )

    index = file_to_tests(repo)
    selected: set[str] = set()
    for path in changed:
        if path in index:
            selected.update(index[path])
        if (
            path.startswith("stress-tests/")
            and path.endswith(".cpp")
            and (repo / path).is_file()
        ):
            selected.add(path)
        selected.update(convention_tests(repo, path))

    tests = sorted(selected)
    if not tests:
        preview = ", ".join(changed[:8]) or "(none)"
        extra = f" and {len(changed) - 8} more" if len(changed) > 8 else ""
        return Selection(
            "skip",
            reason=f"no stress tests depend on changed files: {preview}{extra}",
        )
    return Selection(
        "subset",
        tests,
        reason=f"{len(tests)}/{len(all_tests)} tests depend on {len(changed)} changed file(s)",
    )


def git_ok(repo: Path, *args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["git", *args],
        cwd=repo,
        check=False,
        capture_output=True,
        text=True,
    )


def rev_exists(repo: Path, rev: str) -> bool:
    return git_ok(repo, "rev-parse", "--verify", rev).returncode == 0


def resolve_base(repo: Path, base: str | None) -> str | None:
    candidates: list[str] = []
    if base:
        candidates.append(base)
    env_base = os.environ.get("BASE")
    if env_base:
        candidates.append(env_base)
    gh = os.environ.get("GITHUB_BASE_REF")
    if gh:
        candidates.append(gh if gh.startswith("origin/") else f"origin/{gh}")
    candidates.extend(BASE_CANDIDATES)
    seen: set[str] = set()
    for cand in candidates:
        if not cand or cand in seen:
            continue
        seen.add(cand)
        if rev_exists(repo, cand):
            return cand
    return None


def git_changed_files(repo: Path, base: str | None) -> tuple[list[str] | None, str]:
    """Return (files, note). files is None when git cannot decide (run all)."""
    resolved = resolve_base(repo, base)
    if resolved is None:
        return None, "could not resolve a git base ref"
    mb = git_ok(repo, "merge-base", resolved, "HEAD")
    if mb.returncode != 0 or not mb.stdout.strip():
        return None, f"git merge-base {resolved} HEAD failed"
    merge_base = mb.stdout.strip()
    diff = git_ok(repo, "diff", "--name-only", merge_base)
    if diff.returncode != 0:
        return None, "git diff failed"
    files = [line.strip() for line in diff.stdout.splitlines() if line.strip()]
    untracked = git_ok(repo, "ls-files", "--others", "--exclude-standard")
    if untracked.returncode == 0:
        files.extend(line.strip() for line in untracked.stdout.splitlines() if line.strip())
    # Preserve order, drop dupes.
    seen: set[str] = set()
    unique: list[str] = []
    for path in files:
        path = path.replace("\\", "/")
        if path not in seen:
            seen.add(path)
            unique.append(path)
    return unique, f"vs {resolved} (merge-base {merge_base[:12]})"


def select_from_git(repo: Path, base: str | None) -> Selection:
    changed, note = git_changed_files(repo, base)
    if changed is None:
        tests = list_tests(repo)
        return Selection("all", tests, reason=note)
    selection = select(repo, changed)
    if note and selection.mode != "all":
        selection.reason = f"{selection.reason} [{note}]"
    elif note:
        selection.reason = f"{selection.reason}; {note}"
    return selection


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Print stress tests affected by changed files"
    )
    parser.add_argument("--repo", default=".", help="repository root")
    parser.add_argument("--base", default=None, help="git ref to diff against")
    parser.add_argument(
        "--changed",
        nargs="*",
        default=None,
        help="explicit changed files (skip git). Pass with no files for an empty diff.",
    )
    parser.add_argument("--all", action="store_true", help="select every stress test")
    parser.add_argument(
        "--mode-line",
        action="store_true",
        help="print mode (all|skip|subset) as the first stdout line",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    repo = Path(args.repo).resolve()
    if args.all:
        selection = Selection(
            "all", list_tests(repo), reason="--all"
        )
    elif args.changed is not None:
        selection = select(repo, args.changed)
    else:
        selection = select_from_git(repo, args.base)

    print(selection.reason, file=sys.stderr)
    if args.mode_line:
        print(selection.mode)
    if selection.mode == "subset":
        for test in selection.tests:
            print(test)
    return 0


if __name__ == "__main__":
    sys.exit(main())
