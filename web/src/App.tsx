import { useCallback, useEffect, useLayoutEffect, useMemo, useRef, useState } from 'react'
import type { DocumentBlock, Snippet, SnippetIndex } from './lib/types'
import { descriptionPreview } from './lib/latex'
import { toSearchRecords, type SearchRecord } from './lib/search'
import { useDocumentSearch } from './lib/useDocumentSearch'
import { DocumentView } from './components/DocumentView'
import './App.css'

function parseHash(): { chapter: string | null; id: string | null } {
  const raw = window.location.hash.replace(/^#\/?/, '')
  if (!raw) return { chapter: null, id: null }
  const parts = raw.split('/')
  if (parts.length >= 2) {
    return { chapter: parts[0], id: parts.join('/') }
  }
  return { chapter: parts[0] || null, id: null }
}

function setHash(id: string | null) {
  if (!id) {
    history.replaceState(null, '', window.location.pathname + window.location.search)
    return
  }
  history.replaceState(null, '', `#/${id}`)
}

function cssEscape(value: string): string {
  if (typeof CSS !== 'undefined' && typeof CSS.escape === 'function') {
    return CSS.escape(value)
  }
  return value.replace(/"/g, '\\"')
}

function outlineFrom(
  blocks: DocumentBlock[],
): {
  id: string
  indent: number
  label: string
  kind: 'heading' | 'snippet'
  level: number
  excluded: boolean
}[] {
  let headingLevel = 1
  const items: {
    id: string
    indent: number
    label: string
    kind: 'heading' | 'snippet'
    level: number
    excluded: boolean
  }[] = []
  for (const block of blocks) {
    if (block.type === 'heading') {
      headingLevel = block.level
      items.push({
        id: block.id,
        indent: block.level,
        label: block.title,
        kind: 'heading',
        level: block.level,
        excluded: false,
      })
    } else if (block.type === 'snippet') {
      items.push({
        id: block.id,
        indent: Math.max(headingLevel, 1) + 1,
        label: block.id.split('/').slice(1).join('/'),
        kind: 'snippet',
        level: 0,
        excluded: !block.includedInPdf,
      })
    }
  }
  return items
}

function hitPreview(hit: SearchRecord): string {
  const excerpt =
    hit.kind === 'heading'
      ? ''
      : descriptionPreview(hit.kind === 'snippet' ? hit.description : hit.searchText, 90)
  if (excerpt) return `${hit.chapter} · ${excerpt}`
  return hit.chapter
}

export default function App() {
  const [data, setData] = useState<SnippetIndex | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [query, setQuery] = useState('')
  const [chapter, setChapter] = useState<string | 'all'>('all')
  const [showExcluded, setShowExcluded] = useState(true)
  const [selectedId, setSelectedId] = useState<string | null>(null)
  const [scrollRoot, setScrollRoot] = useState<Element | null>(null)
  const docPaneRef = useRef<HTMLDivElement>(null)
  const pendingScroll = useRef<{ id: string; smooth: boolean } | null>(null)
  const queryRef = useRef(query)
  queryRef.current = query

  useEffect(() => {
    const url = `${import.meta.env.BASE_URL}snippets.json`
    fetch(url)
      .then((r) => {
        if (!r.ok) throw new Error(`Failed to load snippets (${r.status})`)
        return r.json()
      })
      .then((json: SnippetIndex) => {
        json.document ??= []
        setData(json)
        const h = parseHash()
        if (h.id) {
          setSelectedId(h.id)
          setChapter(h.chapter ?? 'all')
          pendingScroll.current = { id: h.id, smooth: false }
        } else if (h.chapter && json.chapters.some((c) => c.id === h.chapter)) {
          setChapter(h.chapter)
        }
      })
      .catch((e: Error) => setError(e.message))
  }, [])

  useEffect(() => {
    setScrollRoot(docPaneRef.current)
  }, [data])

  // Escape always clears search, even when the box is not focused.
  useEffect(() => {
    const onKeyDown = (e: KeyboardEvent) => {
      if (e.key !== 'Escape' || e.isComposing) return
      if (e.altKey || e.ctrlKey || e.metaKey) return
      if (!queryRef.current) return
      e.preventDefault()
      setQuery('')
    }
    window.addEventListener('keydown', onKeyDown)
    return () => window.removeEventListener('keydown', onKeyDown)
  }, [])

  const byId = useMemo(() => {
    const m = new Map<string, Snippet>()
    data?.snippets.forEach((s) => m.set(s.id, s))
    return m
  }, [data])

  const byDocId = useMemo(() => {
    const m = new Map<string, DocumentBlock>()
    data?.document.forEach((b) => m.set(b.id, b))
    return m
  }, [data])

  const searchRecords = useMemo(() => (data ? toSearchRecords(data) : []), [data])
  const { hitIds, pending: searchPending } = useDocumentSearch(searchRecords, query)

  const visibleDocument = useMemo(() => {
    if (!data) return []
    return data.document.filter((block) => {
      if (chapter !== 'all' && block.chapter !== chapter) return false
      if (block.type === 'snippet' && !showExcluded && !block.includedInPdf) return false
      return true
    })
  }, [data, chapter, showExcluded])

  const recordById = useMemo(() => {
    const m = new Map<string, SearchRecord>()
    searchRecords.forEach((r) => m.set(r.id, r))
    return m
  }, [searchRecords])

  const hits = useMemo(() => {
    if (!query.trim()) return []
    let list = hitIds
      .map((id) => recordById.get(id))
      .filter((r): r is SearchRecord => !!r)
    if (!showExcluded) {
      list = list.filter((r) => {
        if (r.kind !== 'snippet') return true
        return byId.get(r.id)?.includedInPdf !== false
      })
    }
    return list
  }, [query, hitIds, recordById, showExcluded, byId])

  const outline = useMemo(() => outlineFrom(visibleDocument), [visibleDocument])

  const jumpTo = useCallback(
    (id: string, smooth = true) => {
      setSelectedId(id)
      setHash(id)
      const block = byDocId.get(id)
      const snippet = byId.get(id)
      const targetChapter = block?.chapter ?? snippet?.chapter
      if (targetChapter && chapter !== 'all' && targetChapter !== chapter) {
        setChapter(targetChapter)
      }
      if (snippet && !snippet.includedInPdf) setShowExcluded(true)
      pendingScroll.current = { id, smooth }
    },
    [byDocId, byId, chapter],
  )

  useEffect(() => {
    const onHash = () => {
      const h = parseHash()
      if (h.id) jumpTo(h.id, false)
    }
    window.addEventListener('hashchange', onHash)
    return () => window.removeEventListener('hashchange', onHash)
  }, [jumpTo])

  useLayoutEffect(() => {
    const pending = pendingScroll.current
    if (!pending || !data) return
    const root = docPaneRef.current
    if (!root) return
    const el = root.querySelector(`[data-doc-id="${cssEscape(pending.id)}"]`)
    if (!el) return
    pendingScroll.current = null
    el.scrollIntoView({ block: 'start', behavior: pending.smooth ? 'smooth' : 'auto' })
  }, [data, visibleDocument, selectedId, scrollRoot])

  function selectChapter(next: string | 'all') {
    setChapter(next)
    if (next === 'all') {
      setSelectedId(null)
      setHash(null)
      pendingScroll.current = null
      if (docPaneRef.current) docPaneRef.current.scrollTop = 0
      return
    }
    const heading = data?.document.find(
      (b) => b.type === 'heading' && b.chapter === next && b.level === 1,
    )
    if (heading) {
      setSelectedId(heading.id)
      setHash(heading.id)
      pendingScroll.current = { id: heading.id, smooth: false }
    } else if (docPaneRef.current) {
      docPaneRef.current.scrollTop = 0
    }
  }

  if (error) {
    return (
      <div className="boot-error">
        <h1>KACTL Snippets</h1>
        <p>{error}</p>
      </div>
    )
  }

  if (!data) {
    return (
      <div className="boot-error">
        <h1>KACTL Snippets</h1>
        <p>Loading library…</p>
      </div>
    )
  }

  const searching = query.trim().length > 0
  const visibleSnippets = visibleDocument.filter((b) => b.type === 'snippet').length
  const countLabel = searching
    ? searchPending && hits.length === 0
      ? 'Searching…'
      : `${hits.length} hits`
    : visibleSnippets
      ? `${visibleSnippets} snippet${visibleSnippets === 1 ? '' : 's'}`
      : ''

  return (
    <div className="app-shell">
      <aside className="sidebar">
        <div className="brand">
          <p className="brand-mark">KACTL</p>
          <h1 className="brand-title">Reference</h1>
          <p className="brand-sub">Searchable contest document</p>
          <a
            className="pdf-link"
            href={`${import.meta.env.BASE_URL}kactl.pdf`}
            target="_blank"
            rel="noreferrer"
          >
            Latest PDF
          </a>
        </div>
        <nav className="chapter-nav" aria-label="Chapters">
          <button
            type="button"
            className={chapter === 'all' ? 'nav-item active' : 'nav-item'}
            onClick={() => selectChapter('all')}
          >
            All
          </button>
          {data.chapters.map((c) => (
            <button
              key={c.id}
              type="button"
              className={chapter === c.id ? 'nav-item active' : 'nav-item'}
              onClick={() => selectChapter(c.id)}
            >
              {c.title}
            </button>
          ))}
        </nav>
        <label className="toggle-row">
          <input
            type="checkbox"
            checked={showExcluded}
            onChange={(e) => setShowExcluded(e.target.checked)}
          />
          Show PDF-excluded
        </label>
      </aside>

      <main className="main">
        <div className="search-bar">
          <input
            type="search"
            placeholder="Search snippets, headings, chapter text…"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            aria-label="Search document"
          />
          <span className="result-count" data-pending={searchPending ? 'true' : undefined}>
            {countLabel}
          </span>
        </div>

        <div className="content-split">
          <div
            className="list-pane"
            role="listbox"
            aria-busy={searchPending}
            aria-label={searching ? 'Search results' : 'Outline'}
          >
            {searching && searchPending && hits.length === 0 && (
              <p className="empty">Searching…</p>
            )}
            {searching && !searchPending && hits.length === 0 && <p className="empty">No matches.</p>}
            {searching &&
              hits.map((hit) => (
                <button
                  key={`${hit.kind}:${hit.id}`}
                  type="button"
                  className={hit.id === selectedId ? 'list-item active' : 'list-item'}
                  onClick={() => jumpTo(hit.id)}
                >
                  <span className="list-name">
                    <span className={`kind-tag kind-${hit.kind}`}>{hit.kind}</span>
                    {hit.kind === 'snippet' ? hit.name : hit.title}
                  </span>
                  <span className="list-preview">{hitPreview(hit)}</span>
                </button>
              ))}
            {!searching && outline.length === 0 && <p className="empty">Nothing to show.</p>}
            {!searching &&
              outline.map((item) => (
                <button
                  key={item.id}
                  type="button"
                  className={
                    (item.id === selectedId ? 'list-item active ' : 'list-item ') +
                    (item.kind === 'snippet' ? 'outline-snippet' : `outline-h${item.level}`)
                  }
                  style={{ paddingLeft: `${0.5 + (item.indent - 1) * 0.85}rem` }}
                  onClick={() => jumpTo(item.id)}
                >
                  <span className="list-name">
                    {item.label}
                    {item.excluded && <span className="dot-ex" title="Excluded from PDF" />}
                  </span>
                </button>
              ))}
          </div>
          <div className="detail-pane doc-pane" ref={docPaneRef}>
            {visibleDocument.length ? (
              <DocumentView
                blocks={visibleDocument}
                byId={byId}
                selectedId={selectedId}
                onSelectDep={jumpTo}
                scrollRoot={scrollRoot}
              />
            ) : (
              <p className="empty">Nothing to show.</p>
            )}
          </div>
        </div>
      </main>
    </div>
  )
}
