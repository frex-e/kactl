import katex from 'katex'
import type { ReactNode } from 'react'

const TEXT_CMDS: Record<string, [string, string]> = {
  texttt: ['code', 'ltx-tt'],
  textbf: ['strong', 'ltx-bf'],
  emph: ['em', 'ltx-em'],
  textit: ['em', 'ltx-em'],
  textrm: ['span', 'ltx-rm'],
  text: ['span', 'ltx-text'],
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
    if (c === '{') depth++
    else if (c === '}') {
      depth--
      if (depth === 0) return [src.slice(open + 1, i), i + 1]
    }
  }
  return null
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
  // \begin{env}...\end{env} for common display envs
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

function latexToHtml(src: string): string {
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
    if (src[i] === '\\') {
      const cmdMatch = src.slice(i).match(/^\\([a-zA-Z]+)\*?/)
      if (cmdMatch) {
        const cmd = cmdMatch[1]
        const afterCmd = i + cmdMatch[0].length
        if (cmd in TEXT_CMDS) {
          const group = readBraceGroup(src, afterCmd)
          if (group) {
            const [inner, next] = group
            const [tag, cls] = TEXT_CMDS[cmd]
            out += `<${tag} class="${cls}">${latexToHtml(inner)}</${tag}>`
            i = next
            continue
          }
        }
        // Unknown command: keep as text (escaped)
        out += escapeHtml(cmdMatch[0])
        i = afterCmd
        continue
      }
    }
    // Newlines → <br> for readability in multi-line descriptions
    if (src[i] === '\n') {
      out += '<br/>'
      i++
      continue
    }
    // Consume a run of plain text
    let j = i + 1
    while (j < src.length) {
      if (src[j] === '$' || src[j] === '\\' || src[j] === '\n') break
      if (src.startsWith('\\[', j)) break
      j++
    }
    out += escapeHtml(src.slice(i, j))
    i = j
  }
  return out
}

export function LatexText({
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
}

/** One-line preview: strip display math, keep short. */
export function descriptionPreview(desc: string, max = 120): string {
  let s = desc.replace(/\\\[[\s\S]*?\\\]/g, ' ')
  s = s.replace(/\$\$[\s\S]*?\$\$/g, ' ')
  s = s.replace(/\$([^$]+)\$/g, '$1')
  s = s.replace(/\\[a-zA-Z]+\*?\{([^}]*)\}/g, '$1')
  s = s.replace(/\\[a-zA-Z]+\*?/g, '')
  s = s.replace(/\s+/g, ' ').trim()
  if (s.length <= max) return s
  return s.slice(0, max - 1) + '…'
}
