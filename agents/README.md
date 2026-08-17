# Agent notes

This directory is for coding agents working in this repo. Humans can start at the root [README](../README.md). Cursor always loads root [AGENTS.md](../AGENTS.md); read the files here when the task needs more than the three `make` commands.

This is a **build/test project, not a service**. There is no web server, API, database, or port to bind. “Running the app” means typesetting the PDF and running compile/stress tests — the same steps as CI (`.github/workflows/ccpp.yml`). Prefer `Makefile` targets (`make help`) over reinventing commands.

## Directory map

| Path | Role |
|---|---|
| `content/` | Notebook source. One subdirectory per chapter; `content/kactl.tex` is the root TeX file (team page, chapter order). |
| `content/tex/` | LaTeX package (`kactlpkg.sty`) and [preprocessor.py](../content/tex/preprocessor.py) (snippet → listings). |
| `web/` | Searchable snippets SPA. Indexer: [web/scripts/index-snippets.py](../web/scripts/index-snippets.py). |
| `stress-tests/` | Stress tests (`make test`). Utilities in `stress-tests/utilities/`. |
| `doc/scripts/` | Compile/test helpers (`compile-all.sh`, `run-all.sh`, `cxx.sh`, `skip_headers`). |
| `.github/workflows/` | `ccpp.yml` (PDF + compiles + stress), `pages.yml` (PDF + snippets site deploy). |
| `old-unit-tests/` | Dead. Untouched for ~10 years; not wired into Make or CI. Ignore it. |

## Golden rules

- Use `make kactl` / `make fast` / `make web-pdf` / `make test-compiles` / `make test` from the repo root.
- `make kactl` and `make web-pdf` rewrite repo-root `kactl.pdf` and copy it to `web/public/kactl.pdf`. Dirty `kactl.pdf` is expected — **do not commit that churn** unless the task is to ship an updated PDF. `web/public/kactl.pdf` and `web/public/snippets.json` are gitignored.
- Do not treat `web/` as something that must be “served” for algorithm or PDF work. The snippets site is a static SPA; see [web.md](web.md) only when changing it.
- Do not blindly copy [upstream KACTL](https://github.com/kth-competitive-programming/kactl). This fork has branding, snippet, and API deltas — [fork.md](fork.md).
- Keep the printed notebook to 25 pages + cover.

## When to read what

- Adding or editing a snippet / chapter TeX: [content.md](content.md)
- Building the PDF, compiling headers, running stress tests, CI: [verify.md](verify.md)
- Snippets site, indexer, search, copy-with-deps: [web.md](web.md)
- How this fork differs from upstream: [fork.md](fork.md)
