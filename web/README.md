# KACTL web snippets

Static SPA that turns the KACTL `content/` tree into a searchable, PDF-like
document (chapter TeX + snippets in order, full-text search, KaTeX, copy /
copy-with-deps).

Search matches snippet names, descriptions, code, and chapter headings/prose.
Clicking a hit or outline row jumps to that place in the sequential view.

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
