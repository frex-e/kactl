import { useCallback, useEffect, useLayoutEffect, useMemo, useRef, useState } from 'react'
import MiniSearch from 'minisearch'
import type { DocumentBlock, Snippet, SnippetIndex } from './lib/types'
import { descriptionPreview } from './lib/latex'
import { DocumentView } from './components/DocumentView'
import './App.css'

type SearchRecord = {
  id: string
  kind: 'snippet' | 'heading' | 'prose'
  chapter: string
  name: string
  title: string
  searchText: string
  description: string
  usage: string
  code: string
}

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

function toSearchRecords(data: SnippetIndex): SearchRecord[] {
  const records: SearchRecord[] = data.snippets.map((s) => ({
    id: s.id,
    kind: 'snippet' as const,
    chapter: s.chapter,
    name: s.name,
    title: s.name,
    searchText: `${s.name} ${s.description} ${s.usage}`,
    description: s.description,
    usage: s.usage,
    code: s.code,
  }))
  for (const block of data.document) {
    if (block.type === 'heading') {
      records.push({
        id: block.id,
        kind: 'heading',
        chapter: block.chapter,
        name: block.title,
        title: block.title,
        searchText: block.searchText,
        description: '',
        usage: '',
        code: '',
      })
    } else if (block.type === 'prose') {
      records.push({
        id: block.id,
        kind: 'prose',
        chapter: block.chapter,
        name: block.searchText.slice(0, 80),
        title: 'Chapter text',
        searchText: block.searchText,
        description: block.searchText,
        usage: '',
        code: '',
      })
    }
  }
  return records
}

function outlineFrom(blocks: DocumentBlock[]): { id: string; indent: number; label: string; kind: string; excluded: boolean }[] {
  let headingLevel = 1
  const items: { id: string; indent: number; label: string; kind: string; excluded: boolean }[] = []
  for (const block of blocks) {
    if (block.type === 'heading') {
      headingLevel = block.level
      items.push({
        id: block.id,
        indent: block.level,
        label: block.title,
        kind: 'heading',
        excluded: false,
      })
    } else if (block.type === 'snippet') {
      items.push({
        id: block.id,
        indent: Math.max(headingLevel, 1) + 1,
        label: block.id.split('/').slice(1).join('/'),
        kind: 'snippet',
        excluded: !block.includedInPdf,
      })
    }
  }
  return items
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

  const searchEngine = useMemo(() => {
    if (!searchRecords.length) return null
    const ms = new MiniSearch<SearchRecord>({
      fields: ['name', 'title', 'searchText', 'description', 'usage', 'code', 'id'],
      storeFields: ['id', 'kind', 'chapter', 'title', 'name'],
      processTerm: (term) => term.toLowerCase(),
      searchOptions: {
        boost: { name: 4, title: 4, description: 2, searchText: 2 },
        prefix: true,
        fuzzy: 0.15,
        processTerm: (term) => term.toLowerCase(),
      },
    })
    ms.addAll(searchRecords)
    return ms
  }, [searchRecords])

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
    const q = query.trim()
    if (!q || !searchEngine) return []
    let list = searchEngine
      .search(q, {
        prefix: true,
        fuzzy: 0.15,
        boost: { name: 4, title: 4, description: 2, searchText: 2 },
      })
      .map((h) => recordById.get(h.id as string))
      .filter((r): r is SearchRecord => !!r)
    if (chapter !== 'all') list = list.filter((r) => r.chapter === chapter)
    if (!showExcluded) {
      list = list.filter((r) => {
        if (r.kind !== 'snippet') return true
        return byId.get(r.id)?.includedInPdf !== false
      })
    }
    return list
  }, [query, searchEngine, recordById, chapter, showExcluded, byId])

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
    if (docPaneRef.current) docPaneRef.current.scrollTop = 0
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
  const navItems = searching ? hits : outline
  const snippetCount = data.snippets.length

  return (
    <div className="app-shell">
      <aside className="sidebar">
        <div className="brand">
          <p className="brand-mark">KACTL</p>
          <h1 className="brand-title">Reference</h1>
          <p className="brand-sub">Searchable contest document</p>
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
          <span className="result-count">
            {searching ? `${hits.length} hits` : `${visibleDocument.filter((b) => b.type === 'snippet').length} / ${snippetCount}`}
          </span>
        </div>

        <div className="content-split">
          <div className="list-pane" role="listbox" aria-label={searching ? 'Search results' : 'Outline'}>
            {navItems.length === 0 ? (
              <p className="empty">{searching ? 'No matches.' : 'Nothing to show.'}</p>
            ) : searching ? (
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
                  <span className="list-preview">
                    {hit.kind === 'heading'
                      ? hit.chapter
                      : descriptionPreview(hit.kind === 'snippet' ? hit.description : hit.searchText)}
                  </span>
                </button>
              ))
            ) : (
              outline.map((item) => (
                <button
                  key={item.id}
                  type="button"
                  className={item.id === selectedId ? 'list-item active' : 'list-item'}
                  style={{ paddingLeft: `${0.45 + (item.indent - 1) * 0.7}rem` }}
                  onClick={() => jumpTo(item.id)}
                >
                  <span className="list-name">
                    {item.label}
                    {item.excluded && <span className="dot-ex" title="Excluded from PDF" />}
                  </span>
                </button>
              ))
            )}
          </div>
          <div className="detail-pane doc-pane" ref={docPaneRef}>
            {visibleDocument.length ? (
              <DocumentView
                blocks={visibleDocument}
                byId={byId}
                selectedId={selectedId}
                onSelectDep={(id) => jumpTo(id)}
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
