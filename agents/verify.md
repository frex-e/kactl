# Building and testing

All commands below run from the **repo root**. `make help` lists targets.

## PDF

| Target | What |
|---|---|
| `make preprocess` | Shared step: listings under `build/listings/`, `web/public/snippets.json`, `build/header.tmp.seed`. |
| `make kactl` | Preprocess, `test-session.pdf`, then three-pass `pdflatex` of `content/kactl.tex` with `makeindex` before the last pass. Writes `kactl.pdf` and copies it to `web/public/kactl.pdf`. |
| `make fast` | Preprocess plus a single LaTeX pass (quicker, worse refs/TOC). Same install step. |
| `make web-pdf` | Preprocess plus three-pass PDF **without** `test-session.pdf`. This is what GitHub Pages uses. |
| `make showexcluded` | Headers/sources in `content/` with no `\kactlimport`. |
| `make test-preprocess` | Unit tests for snippet stripping, chapter parsing, print-header, and stress-test selection. |

`pdflatex` is invoked with **`-shell-escape`** (required: page headers shell out to `python3 -m tools.kactl print-header`). Make copies `build/header.tmp.seed` to `build/header.tmp` before each pass; print-header is the only remaining `write18`. Snippet listings are generated *before* LaTeX, not per `\kactlimport`.

The full builds resolve the TOC in two passes, then run `makeindex build/kactl.idx` and a third LaTeX pass to refresh the alphabetical index with the final content page numbers. Like the TOC, the index can be stale or absent with the single-pass `make fast`; use `make kactl` or `make web-pdf` for the final PDF.

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

`make test-relevant` → `doc/scripts/run-relevant.sh` → `python3 -m tools.stress_select`, then the same runner on a subset. A test is selected if the changed file is the test itself, is quoted-included (transitively) from the test, or matches `content/<chapter>/<stem>.*` → `stress-tests/<chapter>/<stem>.cpp` (case-insensitive stem; covers tests that paste the algorithm). Changes to the runner/selector/`Makefile`/C++ workflow run the full suite. Doc/web/TeX-only diffs skip stress tests. Override the git base with `BASE=...` (CI sets it to the PR base SHA). Force an explicit file list with `STRESS_CHANGED="content/graph/2sat.h"`. If git cannot resolve a base, it falls back to all tests.

A typical test includes `../utilities/template.h` (**not** the contest template — no `pb`/`fr`/`sc`) and the header under test, then prints `Tests passed!` on success. Mirror the chapter path, e.g. `content/data-structures/LiChao.h` → `stress-tests/data-structures/LiChao.cpp`. If two snippets both define `Node` (or another common name), wrap each `#include` in a namespace, as in `PersistentSegmentTree.cpp` and `SegmentTree.cpp`.

Helpers live in `stress-tests/utilities/` (`template.h`, graph generators, etc.).

`old-unit-tests/` is broken and unused. Ignore it.

### Apple Silicon macOS

Native macOS arm64 GCC uses an 8-byte `long double` with 53 bits of mantissa,
the same precision as `double`. [ModMulLL.h](../content/number-theory/ModMulLL.h)
assumes x87 80-bit extended precision for its full range (moduli up to about
`7.2e18`); with 64-bit floating point, its documented range is only below `2^52`.
Installing Homebrew GCC fixes the Apple clang/compiler issue, but does not give
native arm64 builds x87 precision.

Known effects in `stress-tests/number-theory/`:

| Test | Native Apple Silicon behavior |
|---|---|
| `ModMulLL.cpp` | Aborts when the large-modulus result differs from the exact `__uint128_t` reference. |
| `MillerRabin.cpp` | Aborts on a primality mismatch; it uses `ModMulLL.h`. |
| `Factor.cpp` | Can hang in Pollard rho on large inputs; it uses both `ModMulLL.h` and `MillerRabin.h`. |
| `PrimitiveRoot.cpp` | Depends on the same snippets, but the current stress test uses small moduli within the reduced range. Do not assume it fails just because of that dependency. |

These results were reproduced with native Homebrew GCC 15.3.0; `Factor.cpp`
exceeded a 20-second time limit, while `PrimitiveRoot.cpp` passed.
The former `ModSum.cpp` failure was a test bug: macOS's libc `rand()` produced a
short progression whose correct sum differed from the assumed `to*m/2` average
by more than the tolerance. The test now checks that input exactly and compares
large cases with a `__uint128_t` reference using a fixed `mt19937_64` seed.
This was unrelated to x87 precision; `ModSum.h` uses integer arithmetic.

Validate the full-range number theory tests on x86-64 Linux/GCC, as in CI.
If running `Factor.cpp` locally, use a time limit rather than waiting indefinitely.
Do not lower test bounds or weaken assertions to make the native arm64 suite green.
The header compile check also skips `Pragmas.h` and `SIMD.h` on non-x86 targets
because their AVX2 target pragmas are architecture-specific; those are intentional
skips, not stress-test failures.

## CI

- [`.github/workflows/ccpp.yml`](../.github/workflows/ccpp.yml) — on push/PR to `main`: `make kactl`, `make test-preprocess`, `make test-compiles`, then stress tests (`make test-relevant` on PRs vs the base SHA, `make test` on `main`). Concurrent runs on the same ref cancel in progress.
- [`.github/workflows/pages.yml`](../.github/workflows/pages.yml) — on push to `main` (paths: `web/**`, `content/**`, `tools/**`, `Makefile`, the workflow itself): `make web-pdf`, then `npm ci && npm run build` in `web/`, deploy `web/dist`.

When changing snippets, the usual bar is: header still compiles, a stress test exists and passes if the algorithm is new/non-trivial, and the PDF still builds if `chapter.tex` or headers changed. Full `make test` takes a couple of minutes.
