# Building and testing

All commands below run from the **repo root**. `make help` lists targets.

## PDF

| Target | What |
|---|---|
| `make preprocess` | Shared step: listings under `build/listings/`, `web/public/snippets.json`, `build/header.tmp.seed`. |
| `make kactl` | Preprocess, `test-session.pdf`, then two-pass `pdflatex` of `content/kactl.tex`. Writes `kactl.pdf` and copies it to `web/public/kactl.pdf`. |
| `make fast` | Preprocess plus a single LaTeX pass (quicker, worse refs/TOC). Same install step. |
| `make web-pdf` | Preprocess plus two-pass PDF **without** `test-session.pdf`. This is what GitHub Pages uses. |
| `make showexcluded` | Headers/sources in `content/` with no `\kactlimport`. |
| `make test-preprocess` | Unit tests for snippet stripping, chapter parsing, and print-header. |

`pdflatex` is invoked with **`-shell-escape`** (required: page headers shell out to `python3 -m tools.kactl print-header`). Make copies `build/header.tmp.seed` to `build/header.tmp` before each pass; print-header is the only remaining `write18`. Snippet listings are generated *before* LaTeX, not per `\kactlimport`.

TeX packages needed: `texlive-latex-base`, `texlive-latex-recommended`, `texlive-latex-extra`, `texlive-plain-generic` (`ulem.sty`), `texlive-fonts-recommended`. Listings use Inconsolata from repo-local `texmf/` (`make` sets `TEXMFHOME`). Python 3 is required for preprocess.

Dirty repo-root `kactl.pdf` after a build is expected. **Do not commit it** unless the task is to update the shipped PDF. `web/public/kactl.pdf` and `web/public/snippets.json` are gitignored.

This fork does **not** keep the 25-page ICPC notebook cap. Do not drop or comment out snippets just to shrink the PDF.

## Header compile check

`make test-compiles` → `doc/scripts/compile-all.sh`.

For each `content/**/*.h` not in the skip list, it writes a tiny `build/temp.cpp` that includes [content/contest/template.cpp](../content/contest/template.cpp) then the header, and compiles with `-std=c++20 -Wall -Wextra -Wfatal-errors -Wconversion`.

Skip list: [doc/scripts/skip_headers](../doc/scripts/skip_headers) (filename only, one per line). On non-x86, `Pragmas.h` and `SIMD.h` are appended to the skip list because they need avx2.

Compiler: [doc/scripts/cxx.sh](../doc/scripts/cxx.sh). Prefers `g++-15`, then `g++-14`, `g++-13`, then `g++`. Rejects a `g++` that is actually clang (Apple). Override with `CXX=...`.

## Stress tests

`make test` → `doc/scripts/run-all.sh`.

- Finds every `stress-tests/**/*.cpp`, compiles with `-std=c++20 -O2 -Wall -Wfatal-errors -Wconversion`, runs `./a.out`.
- Raises the stack limit (`ulimit -s 524288`) for the 2-SAT test.
- Needs `bc` for timing.

A typical test includes `../utilities/template.h` (**not** the contest template — no `pb`/`fr`/`sc`) and the header under test, then prints `Tests passed!` on success. Mirror the chapter path, e.g. `content/data-structures/LiChao.h` → `stress-tests/data-structures/LiChao.cpp`. If two snippets both define `Node` (or another common name), wrap each `#include` in a namespace, as in `PersistentSegmentTree.cpp` and `SegmentTree.cpp`.

Helpers live in `stress-tests/utilities/` (`template.h`, graph generators, etc.).

`old-unit-tests/` is broken and unused. Ignore it.

## CI

- [`.github/workflows/ccpp.yml`](../.github/workflows/ccpp.yml) — on push/PR to `main`: `make kactl`, `make test-preprocess`, `make test-compiles`, `make test`.
- [`.github/workflows/pages.yml`](../.github/workflows/pages.yml) — on push to `main` (paths: `web/**`, `content/**`, `tools/**`, `Makefile`, the workflow itself): `make web-pdf`, then `npm ci && npm run build` in `web/`, deploy `web/dist`.

When changing snippets, the usual bar is: header still compiles, a stress test exists and passes if the algorithm is new/non-trivial, and the PDF still builds if `chapter.tex` or headers changed. Full `make test` takes a couple of minutes.
