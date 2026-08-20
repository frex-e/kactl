# Notebook content

Source of truth for the printed notebook and the snippets site is `content/`. One preprocess step (`make preprocess` / `python3 -m tools.kactl preprocess`) turns that tree into listings for `pdflatex` and `web/public/snippets.json` for the site. Changing a snippet usually means touching the `.h` (or `.cpp`/`.py`/…), its `\kactlimport` in `chapter.tex`, and often a stress test.

## Layout

Chapters live in `content/<chapter>/` with a `chapter.tex` that pulls in snippets and prose. PDF chapter order is the `\kactlchapter{...}` list in [content/kactl.tex](../content/kactl.tex) (the preprocessor reads that list; do not duplicate it elsewhere):

contest, math, data-structures, numerical, number-theory, combinatorial, graph, geometry, strings, various, then appendix (on a new `multicols*` block).

Team name, members, university, and contest line are also in `content/kactl.tex`. Do not put those in the README or web HTML as a second source of truth.

`content/tex/` and `content/test-session/` are not snippet chapters.

## Including a snippet

`\kactlimport{File.h}` in that chapter’s `chapter.tex`. Preprocess writes `build/listings/<chapter>/File.h.tex`; `\kactlimport` `\input`s that file. Language is inferred from the filename (`.h` → C++ with header comments; `template.cpp`, `.vimrc`, `hash.sh`, … are treated as raw listings).

Comment the `\kactlimport` line (`% \kactlimport{UnionFind.h}`) to drop the snippet from the PDF. Preprocess still lists it, with `includedInPdf: false`. The site hides those by default and has a toggle to show them.

`make showexcluded` lists `content/**/*.h` (and `.py`/`.java`) that have no `\kactlimport` at all.

## Header block

C++/Java/Python snippets start with a `/** ... */` (or `'''` / `"""`) metadata comment. The preprocessor requires **Author** and **Description**. Known keys:

`Author`, `Date`, `Description`, `Source`, `Time`, `Memory`, `License`, `Status`, `Usage`, `Details`

Unknown keys are a PDF build error (`\kactlerror`). Description/Usage/Time appear above the listing; `#include`s become the left caption; a 6-char hash and line count become the right caption.

Prefer `Status: stress-tested` when there is a matching file under `stress-tests/`.

## What the preprocessor strips

Implemented once in [tools/kactl/snippet.py](../tools/kactl/snippet.py). Before hashing and typesetting:

- Lines containing `exclude-line` are dropped.
- `include-line` uncomments a `// ` prefix so a line can live commented in the source but print in the PDF.
- `///` to end-of-line is stripped (and a now-empty line is dropped).
- `#pragma once` is dropped.
- `#include ...` is dropped from the listing **unless** the line also contains `keep-include` (needed for `<bits/extc++.h>`, `immintrin.h`, etc.). Those includes are still recorded as dependencies.

The 6-char hash is `content/contest/hash.sh`: `cpp -dD -P -fpreprocessed`, strip whitespace, MD5, first 6 chars. Same idea as `:Hash` in `content/contest/.vimrc`.

## Style

- Line width **63 characters**, **tabs** for indentation (tab = 2 spaces in the PDF).
- Terse contest macros from [content/contest/template.cpp](../content/contest/template.cpp): `rep`, `all`, `sz`, `pb`, `fr`, `sc`, `ll`, `pii`, `vi`. This fork’s template uses `pb` / `fr` / `sc` and does **not** use `cin.exceptions`.
- Do not make APIs overly generic; the code is typed by hand in contest.

## Adding a snippet

1. Put the file in the right `content/<chapter>/` directory.
2. Add a header block (Author + Description at minimum).
3. Add `\kactlimport{YourFile.h}` in that chapter’s `chapter.tex` (or leave it commented if it should stay out of the PDF).
4. Add a stress test under `stress-tests/<chapter>/` when the algorithm is non-trivial (see [verify.md](verify.md)).
5. If it is a local addition or replacement of upstream, update [doc/mine-integration.md](../doc/mine-integration.md).
6. Rebuild: `make kactl` or `make web-pdf` for the PDF (both run preprocess). The snippets site picks it up after `make preprocess` (or `npm run dev` / `npm run build`, whose `predev`/`prebuild` call that same Make target).
