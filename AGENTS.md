# AGENTS.md

Longer notes live in [`agents/`](agents/README.md) (content, tests, snippets site, fork vs upstream). Read those when the task needs more than the commands below.

## Cursor Cloud specific instructions

This repo is a **build/test project, not a service**. There is
nothing to "serve": no web server, API, database, or network ports. "Running the app" means
building the PDF and running the compile/stress tests, i.e. the same steps as CI
(`.github/workflows/ccpp.yml`). Standard commands live in the `Makefile` (run `make help`);
prefer those instead of duplicating them.

### The three verification commands (all run from repo root)
- `make kactl` — build `kactl.pdf` (runs `pdflatex` twice). `make fast` does a single pass. `make web-pdf` is the same two-pass PDF without `test-session.pdf`, and is what GitHub Pages uses.
- `make test-compiles` — compile-check every `content/**/*.h` header (~143 headers).
- `make test` — build and run every stress test in `stress-tests/` (~81 tests, takes a
  couple of minutes; it raises the stack limit via `ulimit -s` for the 2-SAT test).

### Non-obvious notes
- System dependencies are provided by the base image/snapshot, **not** by the update script:
  TeX Live (`texlive-latex-base`, `texlive-latex-recommended`, `texlive-latex-extra`,
  `texlive-plain-generic` — needed for `ulem.sty`, `texlive-fonts-recommended`), `g++`, and
  `bc`. There are no language package managers, lockfiles, or `.env`/secrets in this repo.
- `pdflatex` is invoked with `-shell-escape` (required: page headers shell out to
  `python3 -m tools.kactl print-header`). Make copies `header.tmp.seed` to
  `header.tmp` before each pass. Snippet listings are generated first by
  `make preprocess` (`python3 -m tools.kactl preprocess`).
- The C++ compiler is auto-detected by `doc/scripts/cxx.sh` (prefers `g++-15/14/13`, falls
  back to `g++`; rejects Apple clang). Override with `CXX=...` if needed. Build uses
  `-std=c++17`.
- `make kactl` (and `make web-pdf`) rewrite the committed `kactl.pdf` and copy it to
  `web/public/kactl.pdf` for the snippets site. The repo-root PDF showing as dirty is
  expected — do **not** commit that churn unless you intend to update the shipped PDF.
  `web/public/kactl.pdf` and `web/public/snippets.json` are gitignored.
- `old-unit-tests/` is deprecated/broken (untouched for ~10 years) and is not wired into
  Make or CI — ignore it.
