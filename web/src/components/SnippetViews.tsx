import { useMemo, useState } from 'react'
import hljs from 'highlight.js/lib/core'
import cpp from 'highlight.js/lib/languages/cpp'
import java from 'highlight.js/lib/languages/java'
import bash from 'highlight.js/lib/languages/bash'
import python from 'highlight.js/lib/languages/python'
import plaintext from 'highlight.js/lib/languages/plaintext'
import type { Snippet } from '../lib/types'
import { LatexText, descriptionPreview } from '../lib/latex'
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

export function SnippetDetail({
  snippet,
  byId,
  onSelectDep,
}: {
  snippet: Snippet
  byId: Map<string, Snippet>
  onSelectDep: (id: string) => void
}) {
  const [copied, setCopied] = useState<'code' | 'deps' | null>(null)

  const highlighted = useMemo(() => {
    try {
      return hljs.highlight(snippet.code, { language: langFor(snippet.name) }).value
    } catch {
      return hljs.highlight(snippet.code, { language: 'plaintext' }).value
    }
  }, [snippet])

  async function doCopy(mode: 'code' | 'deps') {
    const bundle =
      mode === 'code'
        ? snippet.code
        : formatSnippetBundle(orderWithDependencies(snippet, byId))
    const ok = await copyText(bundle)
    if (ok) {
      setCopied(mode)
      window.setTimeout(() => setCopied(null), 1600)
    }
  }

  return (
    <article className="detail">
      <header className="detail-header">
        <div>
          <p className="detail-chapter">{snippet.chapter}</p>
          <h2 className="detail-title">{snippet.name}</h2>
        </div>
        <div className="detail-actions">
          <button type="button" className="btn" onClick={() => void doCopy('code')}>
            {copied === 'code' ? 'Copied' : 'Copy'}
          </button>
          <button
            type="button"
            className="btn btn-accent"
            onClick={() => void doCopy('deps')}
            title="Copy this snippet and its transitive dependencies"
          >
            {copied === 'deps' ? 'Copied' : 'Copy with deps'}
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
          <pre className="usage-block">{snippet.usage}</pre>
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
        <pre className="code-block">
          <code
            className={`hljs language-${langFor(snippet.name)}`}
            dangerouslySetInnerHTML={{ __html: highlighted }}
          />
        </pre>
      </section>
    </article>
  )
}

export function SnippetListItem({
  snippet,
  active,
  onSelect,
}: {
  snippet: Snippet
  active: boolean
  onSelect: () => void
}) {
  return (
    <button
      type="button"
      className={active ? 'list-item active' : 'list-item'}
      onClick={onSelect}
    >
      <span className="list-name">
        {snippet.name}
        {!snippet.includedInPdf && <span className="dot-ex" title="Excluded from PDF" />}
      </span>
      <span className="list-preview">{descriptionPreview(snippet.description)}</span>
    </button>
  )
}
