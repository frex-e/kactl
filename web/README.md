# KACTL web snippets

Static SPA that indexes the KACTL `content/` headers into a searchable library
(browse by chapter, full-text search, KaTeX descriptions, copy / copy-with-deps).

Geometry is omitted for now (figure/`minipage` headers); see the indexer TODO.

## Local development

From `web/`:

```bash
npm install
npm run dev
```

`predev` / `prebuild` run `python3 scripts/index-snippets.py`, which writes
`public/snippets.json` from the repo’s `content/` tree (repo root = parent of `web/`).

## Production build

```bash
npm run build
```

Output is `web/dist/`. Default Vite `base` is `/kactl/` for GitHub project Pages
(`https://<user>.github.io/kactl/`). For a custom domain or root deploy:

```bash
VITE_BASE=/ npm run build
```

## GitHub Pages

The workflow `.github/workflows/pages.yml` builds this app and deploys `web/dist`.
Enable **Settings → Pages → Build and deployment → Source: GitHub Actions**.
