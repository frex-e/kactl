import katex from 'katex'
import { memo, type ReactNode } from 'react'

/** Maps a TeX `\<char>` text-mode escape to the character it prints. */
const TEXT_ESCAPES: Record<string, string> = {
  _: '_',
  '#': '#',
  '&': '&',
  '%': '%',
  '{': '{',
  '}': '}',
  $: '$',
  '~': '~',
  ' ': ' ',
}

const SKIP_CMDS = new Set([
  'small',
  'normalsize',
  'large',
  'Large',
  'tiny',
  'scriptsize',
  'appendix',
  'noindent',
  'centering',
  'leavevmode',
  'par',
  'hfill',
  'vfill',
  'qquad',
  'quad',
  'relax',
  'displaystyle',
  'textstyle',
  'limits',
  'itemsep',
  'noitemsep',
  'maketitle',
  'hardcolumnbreak',
  'columnbreak',
])

const MATH_ENVS = new Set([
  'aligned',
  'align',
  'align*',
  'array',
  'matrix',
  'pmatrix',
  'bmatrix',
  'cases',
])

/** Undo LaTeX text-mode escapes written in KACTL headers (\&, \_, \#, …). */
export function unescapeLatexText(src: string): string {
  let out = ''
  for (let i = 0; i < src.length; i++) {
    if (src[i] === '\\' && i + 1 < src.length && src[i + 1] in TEXT_ESCAPES) {
      out += TEXT_ESCAPES[src[i + 1]]
      i++
      continue
    }
    out += src[i]
  }
  return out
}

const TEXT_CMDS: Record<string, [string, string]> = {
  texttt: ['code', 'ltx-tt'],
  textbf: ['strong', 'ltx-bf'],
  emph: ['em', 'ltx-em'],
  textit: ['em', 'ltx-em'],
  textrm: ['span', 'ltx-rm'],
  text: ['span', 'ltx-text'],
  mathrm: ['span', 'ltx-rm'],
  operatorname: ['span', 'ltx-rm'],
  scriptsize: ['span', 'ltx-small'],
  textstyle: ['span', 'ltx-text'],
}

function renderMath(tex: string, display: boolean): string {
  try {
    return katex.renderToString(tex, {
      displayMode: display,
      throwOnError: false,
      strict: 'ignore',
      trust: false,
    })
  } catch {
    return escapeHtml(tex)
  }
}

function escapeHtml(s: string): string {
  return s
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
}

/** Parse balanced {...} starting after an opening brace at `open`. */
function readBraceGroup(src: string, open: number): [string, number] | null {
  if (src[open] !== '{') return null
  let depth = 0
  for (let i = open; i < src.length; i++) {
    const c = src[i]
    if (c === '\\') {
      i++
      continue
    }
    if (c === '{') depth++
    else if (c === '}') {
      depth--
      if (depth === 0) return [src.slice(open + 1, i), i + 1]
    }
  }
  return null
}

function skipOptionalBrackets(src: string, i: number): number {
  if (src[i] !== '[') return i
  const close = src.indexOf(']', i)
  return close === -1 ? i : close + 1
}

function findDisplayMath(src: string, i: number): [string, number] | null {
  if (src.startsWith('\\[', i)) {
    const end = src.indexOf('\\]', i + 2)
    if (end === -1) return null
    return [src.slice(i + 2, end), end + 2]
  }
  if (src.startsWith('$$', i)) {
    const end = src.indexOf('$$', i + 2)
    if (end === -1) return null
    return [src.slice(i + 2, end), end + 2]
  }
  const begin = src.slice(i).match(/^\\begin\{(aligned|align\*?|array|matrix|pmatrix|bmatrix|cases)\}/)
  if (begin) {
    const env = begin[1]
    const endToken = `\\end{${env}}`
    const end = src.indexOf(endToken, i + begin[0].length)
    if (end === -1) return null
    return [src.slice(i, end + endToken.length), end + endToken.length]
  }
  return null
}

function findInlineMath(src: string, i: number): [string, number] | null {
  if (src[i] !== '$' || src.startsWith('$$', i)) return null
  let j = i + 1
  while (j < src.length) {
    if (src[j] === '\\') {
      j += 2
      continue
    }
    if (src[j] === '$') return [src.slice(i + 1, j), j + 1]
    j++
  }
  return null
}

type EnvMatch = {
  name: string
  body: string
  spec: string | null
  next: number
}

function findEnvironment(src: string, i: number): EnvMatch | null {
  const m = src.slice(i).match(/^\\begin\{([a-zA-Z0-9*]+)\}/)
  if (!m) return null
  const name = m[1]
  let p = i + m[0].length
  p = skipOptionalBrackets(src, p)
  let spec: string | null = null
  if ((name === 'tabular' || name === 'minipage') && src[p] === '{') {
    const group = readBraceGroup(src, p)
    if (group) {
      spec = group[0]
      p = group[1]
    }
  }
  const beginTok = `\\begin{${name}}`
  const endTok = `\\end{${name}}`
  let depth = 1
  let j = p
  while (j < src.length) {
    if (src.startsWith(beginTok, j)) {
      depth++
      j += beginTok.length
      continue
    }
    if (src.startsWith(endTok, j)) {
      depth--
      if (depth === 0) {
        return { name, body: src.slice(p, j), spec, next: j + endTok.length }
      }
      j += endTok.length
      continue
    }
    j++
  }
  return null
}

function splitTopLevel(src: string, kind: 'rows' | 'cells' | 'items'): string[] {
  const parts: string[] = []
  let start = 0
  let braces = 0
  let envs = 0
  let math = false
  let i = 0
  const push = (end: number, next: number) => {
    parts.push(src.slice(start, end))
    start = next
    i = next
  }
  while (i < src.length) {
    if (src.startsWith('$$', i)) {
      math = !math
      i += 2
      continue
    }
    if (src[i] === '$') {
      math = !math
      i++
      continue
    }
    if (src.startsWith('\\[', i)) {
      const end = src.indexOf('\\]', i + 2)
      i = end === -1 ? src.length : end + 2
      continue
    }
    if (src.startsWith('\\begin{', i) && !math) {
      envs++
      i += 7
      continue
    }
    if (src.startsWith('\\end{', i) && !math) {
      envs = Math.max(0, envs - 1)
      i += 5
      continue
    }
    if (src[i] === '{' && !math) {
      braces++
      i++
      continue
    }
    if (src[i] === '}' && !math) {
      braces = Math.max(0, braces - 1)
      i++
      continue
    }
    const top = !math && braces === 0 && envs === 0
    if (top && kind === 'rows' && src.startsWith('\\\\', i)) {
      push(i, i + 2)
      continue
    }
    if (top && kind === 'cells' && src[i] === '&') {
      push(i, i + 1)
      continue
    }
    if (top && kind === 'items' && src.startsWith('\\item', i)) {
      const after = i + 5
      if (after >= src.length || !/[a-zA-Z]/.test(src[after])) {
        push(i, after)
        continue
      }
    }
    if (src[i] === '\\' && i + 1 < src.length) {
      i += 2
      continue
    }
    i++
  }
  parts.push(src.slice(start))
  return parts
}

function renderItemize(body: string): string {
  const chunks = splitTopLevel(body, 'items')
  const items = chunks.slice(1).map((c) => c.trim()).filter(Boolean)
  if (!items.length) return latexToHtml(body)
  return `<ul class="ltx-list">${items
    .map((item) => `<li>${latexToHtml(item)}</li>`)
    .join('')}</ul>`
}

function renderTabular(body: string): string {
  const rows = splitTopLevel(body, 'rows')
  const htmlRows: string[] = []
  let pendingRule = false
  for (const raw of rows) {
    let row = raw.trim()
    if (!row) continue
    const hadRule = /\\hline\b/.test(row)
    row = row.replace(/\\hline\b/g, '').trim()
    if (!row) {
      pendingRule = pendingRule || hadRule
      continue
    }
    const cells = splitTopLevel(row, 'cells').map((c) => c.trim())
    const cls = pendingRule || hadRule ? ' class="ltx-rule"' : ''
    pendingRule = false
    htmlRows.push(
      `<tr${cls}>${cells.map((c) => `<td>${latexToHtml(c)}</td>`).join('')}</tr>`,
    )
  }
  return `<table class="ltx-table">${htmlRows.join('')}</table>`
}

function skipIncludeGraphics(src: string, afterCmd: number): number {
  let p = skipOptionalBrackets(src, afterCmd)
  if (src[p] === '{') {
    const group = readBraceGroup(src, p)
    if (group) return group[1]
  }
  return p
}

/** Convert a TeX fragment to HTML (KaTeX for math, light text-mode markup). */
export function latexToHtml(src: string): string {
  let out = ''
  let i = 0
  while (i < src.length) {
    const display = findDisplayMath(src, i)
    if (display) {
      const [tex, next] = display
      out += renderMath(tex.trim(), true)
      i = next
      continue
    }
    const inline = findInlineMath(src, i)
    if (inline) {
      const [tex, next] = inline
      out += renderMath(tex, false)
      i = next
      continue
    }
    if (src.startsWith('\\verb', i)) {
      let p = i + 5
      if (src[p] === '*') p++
      const delim = src[p]
      if (delim && !/[a-zA-Z\s]/.test(delim)) {
        const end = src.indexOf(delim, p + 1)
        if (end !== -1) {
          out += `<code class="ltx-tt ltx-verb">${escapeHtml(src.slice(p + 1, end))}</code>`
          i = end + 1
          continue
        }
      }
    }
    if (src.startsWith('\\begin{', i)) {
      const env = findEnvironment(src, i)
      if (env) {
        if (MATH_ENVS.has(env.name)) {
          out += renderMath(src.slice(i, env.next).trim(), true)
          i = env.next
          continue
        }
        if (env.name === 'itemize' || env.name === 'enumerate') {
          out += renderItemize(env.body)
          i = env.next
          continue
        }
        if (env.name === 'tabular') {
          out += renderTabular(env.body)
          i = env.next
          continue
        }
        if (env.name === 'center' || env.name === 'minipage') {
          const inner = latexToHtml(env.body.trim())
          if (inner) out += `<div class="ltx-center">${inner}</div>`
          i = env.next
          continue
        }
        out += latexToHtml(env.body)
        i = env.next
        continue
      }
    }
    if (src.startsWith('\n\n', i)) {
      out += '<br/><br/>'
      i += 2
      while (src[i] === '\n') i++
      continue
    }
    if (src[i] === '\n') {
      out += ' '
      i++
      continue
    }
    if (src[i] === '\\') {
      const cmdMatch = src.slice(i).match(/^\\([a-zA-Z]+)\*?/)
      if (cmdMatch) {
        const cmd = cmdMatch[1]
        let afterCmd = i + cmdMatch[0].length
        if (cmd === 'includegraphics') {
          i = skipIncludeGraphics(src, afterCmd)
          continue
        }
        if (cmd === 'hline' || cmd === 'newline') {
          i = afterCmd
          continue
        }
        if (cmd in TEXT_CMDS) {
          afterCmd = skipOptionalBrackets(src, afterCmd)
          while (src[afterCmd] === ' ') afterCmd++
          const group = readBraceGroup(src, afterCmd)
          const [tag, cls] = TEXT_CMDS[cmd]
          if (group) {
            const [inner, next] = group
            out += `<${tag} class="${cls}">${latexToHtml(inner)}</${tag}>`
            i = next
            continue
          }
          if (src[afterCmd] && src[afterCmd] !== '\\') {
            out += `<${tag} class="${cls}">${escapeHtml(src[afterCmd])}</${tag}>`
            i = afterCmd + 1
            continue
          }
        }
        if (SKIP_CMDS.has(cmd)) {
          afterCmd = skipOptionalBrackets(src, afterCmd)
          if (src[afterCmd] === '{') {
            const group = readBraceGroup(src, afterCmd)
            if (group) {
              out += latexToHtml(group[0])
              i = group[1]
              continue
            }
          }
          i = afterCmd
          continue
        }
        afterCmd = skipOptionalBrackets(src, afterCmd)
        if (src[afterCmd] === '{') {
          const group = readBraceGroup(src, afterCmd)
          if (group) {
            out += latexToHtml(group[0])
            i = group[1]
            continue
          }
        }
        out += escapeHtml(cmdMatch[0])
        i = afterCmd
        continue
      }
      if (src.startsWith('\\\\', i)) {
        out += '<br/>'
        i += 2
        continue
      }
      const next = src[i + 1]
      if (next && next in TEXT_ESCAPES) {
        out += escapeHtml(TEXT_ESCAPES[next])
        i += 2
        continue
      }
    }
    // TeX text-mode: `~` is nbsp; `{...}` is grouping (KACTL uses `-{}-` to
    // stop `--` becoming an en-dash inside `\texttt`).
    if (src[i] === '~') {
      out += '&nbsp;'
      i++
      continue
    }
    if (src[i] === '{') {
      const group = readBraceGroup(src, i)
      if (group) {
        out += latexToHtml(group[0])
        i = group[1]
        continue
      }
      out += '{'
      i++
      continue
    }
    if (src[i] === '}') {
      i++
      continue
    }
    let j = i + 1
    while (j < src.length) {
      if (src[j] === '$' || src[j] === '\\' || src[j] === '\n') break
      if (src[j] === '{' || src[j] === '}' || src[j] === '~') break
      if (src.startsWith('\\[', j)) break
      j++
    }
    out += escapeHtml(src.slice(i, j))
    i = j
  }
  return out
}

export const LatexText = memo(function LatexText({
  text,
  className,
  as: Tag = 'div',
}: {
  text: string
  className?: string
  as?: 'div' | 'span' | 'p'
}): ReactNode {
  if (!text) return null
  const html = latexToHtml(text)
  return (
    <Tag
      className={className ? `latex-text ${className}` : 'latex-text'}
      dangerouslySetInnerHTML={{ __html: html }}
    />
  )
})

/** One-line preview: strip display math, keep short. */
export function descriptionPreview(desc: string, max = 120): string {
  let s = desc.replace(/\\verb([^a-zA-Z\s])(.*?)\1/g, '$2')
  s = s.replace(/\\\[[\s\S]*?\\\]/g, ' ')
  s = s.replace(/\$\$[\s\S]*?\$\$/g, ' ')
  s = s.replace(/\$([^$]+)\$/g, '$1')
  s = s.replace(/\\begin\{[^}]+\}(?:\[[^\]]*\])?(?:\{[^}]*\})?/g, ' ')
  s = s.replace(/\\end\{[^}]+\}/g, ' ')
  // Empty groups break ligatures in TeX (`-{}-` → `--`); strip before
  // unwrapping `\texttt{...}` so nested `}` does not truncate the argument.
  s = s.replace(/\{\}/g, '')
  s = s.replace(/\\[a-zA-Z]+\*?\{([^}]*)\}/g, '$1')
  s = s.replace(/\\[a-zA-Z]+\*?/g, '')
  s = unescapeLatexText(s)
  s = s.replace(/~/g, ' ')
  s = s.replace(/\s+/g, ' ').trim()
  if (s.length <= max) return s
  return s.slice(0, max - 1) + '…'
}
