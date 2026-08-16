# AGENTS.md

## Cursor Cloud specific instructions

KACTL (KTH's ICPC reference document) is a **build/test project, not a service**. There is
nothing to "serve": no web server, API, database, or network ports. "Running the app" means
building the PDF and running the compile/stress tests, i.e. the same steps as CI
(`.github/workflows/ccpp.yml`). Standard commands live in the `Makefile` (run `make help`);
prefer those instead of duplicating them.

### The three verification commands (all run from repo root)
- `make kactl` — build `kactl.pdf` (runs `pdflatex` twice). `make fast` does a single pass.
- `make test-compiles` — compile-check every `content/**/*.h` header (~143 headers).
- `make test` — build and run every stress test in `stress-tests/` (~81 tests, takes a
  couple of minutes; it raises the stack limit via `ulimit -s` for the 2-SAT test).

### Non-obvious notes
- System dependencies are provided by the base image/snapshot, **not** by the update script:
  TeX Live (`texlive-latex-base`, `texlive-latex-recommended`, `texlive-latex-extra`,
  `texlive-plain-generic` — needed for `ulem.sty`, `texlive-fonts-recommended`), `g++`, and
  `bc`. There are no language package managers, lockfiles, or `.env`/secrets in this repo.
- `pdflatex` is invoked with `-shell-escape` (required by the build).
- The C++ compiler is auto-detected by `doc/scripts/cxx.sh` (prefers `g++-15/14/13`, falls
  back to `g++`; rejects Apple clang). Override with `CXX=...` if needed. Build uses
  `-std=c++17`.
- `make kactl` rewrites the committed `kactl.pdf` (a build artifact checked into the repo).
  This shows up as a dirty working tree — do **not** commit that churn unless you intend to
  update the shipped PDF.
- `old-unit-tests/` is deprecated/broken (untouched for ~10 years) and is not wired into
  Make or CI — ignore it.
