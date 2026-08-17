# Snippets site

Static Vite + React SPA in `web/`. It turns `content/` into a sequential, PDF-like document (chapter TeX + snippets in order) with full-text search, KaTeX, copy, and copy-with-deps.

Human-oriented notes: [web/README.md](../web/README.md). Live site: https://frex-e.github.io/kactl/ (project Pages, base `/kactl/`).

This is **not** a backend. There is nothing to start except `vite` for local UI work.

## Pipeline

1. [web/scripts/index-snippets.py](../web/scripts/index-snippets.py) walks `content/` in the same chapter order as `content/kactl.tex` and writes `web/public/snippets.json`. That file is gitignored.
2. `npm run copy-pdf` copies repo-root `kactl.pdf` → `web/public/kactl.pdf` when the PDF exists (for the sidebar “Latest PDF” link). That copy is gitignored.
3. `predev` / `prebuild` run both of the above. `npm run build` emits `web/dist/`.

Vite `base` is `/kactl/` unless `VITE_BASE` is set (`VITE_BASE=/ npm run build` for a root deploy).

GitHub Pages: [`.github/workflows/pages.yml`](../.github/workflows/pages.yml) runs `make web-pdf` first so the deployed `kactl.pdf` is fresh, then builds the SPA.

## Indexer vs preprocessor

The indexer duplicates [content/tex/preprocessor.py](../content/tex/preprocessor.py) include/header stripping (`exclude-line`, `include-line`, `keep-include`, `///`, `#pragma once`, dropping `#include`s). Keep them in sync if either changes.

It also parses `chapter.tex`: headings, prose, and `\kactlimport` (including commented-out imports). Commented imports become snippets with `includedInPdf: false`. Types: [web/src/lib/types.ts](../web/src/lib/types.ts).

`web/public/snippets.json` is generated and gitignored; do not hand-edit or commit it.

## UI behavior worth knowing

- Search matches snippet names, descriptions, code, and chapter headings/prose ([web/src/lib/search.ts](../web/src/lib/search.ts)). Identifier indexing splits camelCase and emits compounds so a query like `segmenttree` hits `LazySegmentTree` / `SparseSegmentTree`, not only `SegmentTree.h`.
- Copy vs copy-with-deps: [web/src/lib/copy.ts](../web/src/lib/copy.ts) walks `#include` dependencies in topological order, then the snippet itself.
- Hash URLs: `#/<id>` (chapter-prefixed snippet or heading id). Jumping to an excluded snippet turns on the “show excluded” toggle.
- Default view hides snippets that are commented out of the PDF.

## Local development

From `web/`:

```bash
npm install
npm run dev
```

After editing `content/`, re-index (`npm run index` in `web/`) or restart `npm run dev` so `predev` runs again. For a freshly typeset PDF in the sidebar, `make web-pdf` (or `make kactl`) from the repo root first.

`npm run lint` runs oxlint. There is no app server, database, or e2e harness in-repo; UI changes are verified by running the SPA and checking the sequential document / search / copy actions.
