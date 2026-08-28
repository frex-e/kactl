/** Load latex.tsx through Vite SSR and check TeX text-mode rendering. */
import assert from 'node:assert/strict'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { createServer } from 'vite'

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..')

const server = await createServer({
  configFile: path.join(root, 'vite.config.ts'),
  root,
  server: { middlewareMode: true },
  appType: 'custom',
})

const { latexToHtml, descriptionPreview, unescapeLatexText } =
  await server.ssrLoadModule('/src/lib/latex.tsx')

const spanningTrees = [
  'Create an $N\\times N$ matrix \\texttt{mat}, and for each edge $a \\rightarrow b \\in G$, do',
  '\\texttt{mat[a][b]-{}-}, \\texttt{mat[b][b]++} (and \\texttt{mat[b][a]-{}-}, \\texttt{mat[a][a]++} if $G$ is undirected).',
  'Delete row and column $i$ and take the determinant to get the number of directed spanning trees rooted at~$i$ (if undirected, any row/column).',
].join(' ')

const html = latexToHtml(spanningTrees)

assert.match(html, /mat\[a\]\[b\]--/)
assert.match(html, /mat\[b\]\[a\]--/)
assert.doesNotMatch(html, /-\{\}-/)
assert.doesNotMatch(html, /at~/)
assert.match(html, /rooted at(?:&nbsp;|\u00a0)/)
assert.match(html, /<code class="ltx-tt">mat<\/code>/)
assert.match(html, /<code class="ltx-tt">mat\[b\]\[b\]\+\+<\/code>/)
assert.match(html, /katex/)

assert.equal(latexToHtml('\\texttt{\\{u,v,w\\}}'), '<code class="ltx-tt">{u,v,w}</code>')
assert.equal(
  latexToHtml('\\kactlfigdesc{signed distance \\texttt{p}}{content/geometry/lineDistance}'),
  'signed distance <code class="ltx-tt">p</code>',
)
assert.equal(latexToHtml('/usr/lib/gcc/{*}/4.9/include/'), '/usr/lib/gcc/*/4.9/include/')
assert.equal(unescapeLatexText('\\~user'), '~user')
assert.equal(unescapeLatexText('a\\_b'), 'a_b')

const preview = descriptionPreview(spanningTrees, 200)
assert.match(preview, /mat\[a\]\[b\]--/)
assert.doesNotMatch(preview, /-\{\}-/)
assert.doesNotMatch(preview, /~/)

await server.close()
console.log('latex tests ok')
