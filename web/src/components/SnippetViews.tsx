import { useEffect, useMemo, useRef, useState } from 'react'
import hljs from 'highlight.js/lib/core'
import cpp from 'highlight.js/lib/languages/cpp'
import java from 'highlight.js/lib/languages/java'
import bash from 'highlight.js/lib/languages/bash'
import python from 'highlight.js/lib/languages/python'
import plaintext from 'highlight.js/lib/languages/plaintext'
import type { Snippet } from '../lib/types'
import { LatexText, unescapeLatexText } from '../lib/latex'
import { copyText, formatSnippetBundle, orderWithDependencies } from '../lib/copy'

hljs.registerLanguage('cpp', cpp)
hljs.registerLanguage('java', java)
hljs.registerLanguage('bash', bash)
hljs.registerLanguage('python', python)
hljs.registerLanguage('plaintext', plaintext)

function langFor(name: string): string {
  if (/\.(h|hpp|cpp|cc|c)$/i.test(name)) return 'cpp'
  if (/\.java$/i.test(name)) return 'java'
  if (/\.py$/i.test(name)) return 'python'
  if (/\.sh$/i.test(name) || name === '.bashrc') return 'bash'
  return 'plaintext'
}

function highlightCode(code: string, language: string): string {
  try {
    return hljs.highlight(code, { language }).value
  } catch {
    return hljs.highlight(code, { language: 'plaintext' }).value
  }
}

export function LazyCode({
  code,
  language,
  eager,
  root,
}: {
  code: string
  language: string
  eager?: boolean
  root?: Element | null
}) {
  const ref = useRef<HTMLPreElement>(null)
  const [html, setHtml] = useState<string | null>(() =>
    eager ? highlightCode(code, language) : null,
  )

  useEffect(() => {
    setHtml(eager ? highlightCode(code, language) : null)
  }, [code, language, eager])

  useEffect(() => {
    if (html !== null) return
    const el = ref.current
    if (!el) return
    const io = new IntersectionObserver(
      ([entry]) => {
        if (entry.isIntersecting) {
          setHtml(highlightCode(code, language))
        }
      },
      { root: root ?? null, rootMargin: '240px' },
    )
    io.observe(el)
    return () => io.disconnect()
  }, [code, language, html, root])

  return (
    <pre ref={ref} className="code-block">
      {html ? (
        <code
          className={`hljs language-${language}`}
          dangerouslySetInnerHTML={{ __html: html }}
        />
      ) : (
        <code className="hljs">{code}</code>
      )}
    </pre>
  )
}

export function SnippetDetail({
  snippet,
  byId,
  onSelectDep,
  active,
  scrollRoot,
}: {
  snippet: Snippet
  byId: Map<string, Snippet>
  onSelectDep: (id: string) => void
  active?: boolean
  scrollRoot?: Element | null
}) {
  const [copied, setCopied] = useState<'code' | 'deps' | 'fail' | null>(null)

  const language = useMemo(() => langFor(snippet.name), [snippet.name])

  async function doCopy(mode: 'code' | 'deps') {
    setCopied(mode)
    try {
      const bundle =
        mode === 'code'
          ? snippet.code
          : formatSnippetBundle(orderWithDependencies(snippet, byId))
      const ok = await copyText(bundle)
      if (!ok) setCopied('fail')
    } catch {
      setCopied('fail')
    }
    window.setTimeout(() => setCopied(null), 1600)
  }

  return (
    <article
      className={active ? 'detail snippet-card active' : 'detail snippet-card'}
      data-doc-id={snippet.id}
    >
      <header className="detail-header">
        <div>
          <p className="detail-chapter">{snippet.chapter}</p>
          <h2 className="detail-title">{snippet.name}</h2>
        </div>
        <div className="detail-actions">
          <button type="button" className="btn" onClick={() => void doCopy('code')}>
            {copied === 'code' ? 'Copied' : copied === 'fail' ? 'Copy failed' : 'Copy'}
          </button>
          <button
            type="button"
            className="btn btn-accent"
            onClick={() => void doCopy('deps')}
            title="Copy this snippet and its transitive dependencies"
          >
            {copied === 'deps' ? 'Copied' : copied === 'fail' ? 'Copy failed' : 'Copy with deps'}
          </button>
        </div>
      </header>

      {!snippet.includedInPdf && (
        <p className="badge-warn">Not included in the default kactl.pdf</p>
      )}

      {snippet.description && (
        <section className="detail-section">
          <h3>Description</h3>
          <LatexText text={snippet.description} className="detail-body" />
        </section>
      )}

      {(snippet.time || snippet.memory) && (
        <section className="detail-meta">
          {snippet.time && (
            <div>
              <span className="meta-label">Time</span>
              <LatexText text={snippet.time} as="span" />
            </div>
          )}
          {snippet.memory && (
            <div>
              <span className="meta-label">Memory</span>
              <LatexText text={snippet.memory} as="span" />
            </div>
          )}
        </section>
      )}

      {snippet.usage && (
        <section className="detail-section">
          <h3>Usage</h3>
          <pre className="usage-block">{unescapeLatexText(snippet.usage)}</pre>
        </section>
      )}

      {snippet.dependencies.length > 0 && (
        <section className="detail-section">
          <h3>Dependencies</h3>
          <div className="dep-row">
            {snippet.dependencies.map((id) => (
              <button
                key={id}
                type="button"
                className="dep-chip"
                onClick={() => onSelectDep(id)}
              >
                {id.split('/').pop()}
              </button>
            ))}
          </div>
        </section>
      )}

      {(snippet.status || snippet.author) && (
        <p className="detail-footer-meta">
          {snippet.status && <span>Status: {snippet.status}</span>}
          {snippet.author && <span>Author: {snippet.author}</span>}
        </p>
      )}

      <section className="detail-section">
        <h3>Code</h3>
        <LazyCode
          code={snippet.code}
          language={language}
          eager={active}
          root={scrollRoot}
        />
      </section>
    </article>
  )
}
