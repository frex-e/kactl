import { useEffect, useMemo, useState } from 'react'
import MiniSearch from 'minisearch'
import type { Snippet, SnippetIndex } from './lib/types'
import { SnippetDetail, SnippetListItem } from './components/SnippetViews'
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

export default function App() {
  const [data, setData] = useState<SnippetIndex | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [query, setQuery] = useState('')
  const [chapter, setChapter] = useState<string | 'all'>('all')
  const [showExcluded, setShowExcluded] = useState(true)
  const [selectedId, setSelectedId] = useState<string | null>(null)

  useEffect(() => {
    const url = `${import.meta.env.BASE_URL}snippets.json`
    fetch(url)
      .then((r) => {
        if (!r.ok) throw new Error(`Failed to load snippets (${r.status})`)
        return r.json()
      })
      .then((json: SnippetIndex) => {
        setData(json)
        const h = parseHash()
        if (h.id && json.snippets.some((s) => s.id === h.id)) {
          setSelectedId(h.id)
          setChapter(h.chapter ?? 'all')
        } else if (json.snippets.length) {
          setSelectedId(json.snippets[0].id)
        }
      })
      .catch((e: Error) => setError(e.message))
  }, [])

  useEffect(() => {
    const onHash = () => {
      const h = parseHash()
      if (h.id) {
        setSelectedId(h.id)
        if (h.chapter) setChapter(h.chapter)
      }
    }
    window.addEventListener('hashchange', onHash)
    return () => window.removeEventListener('hashchange', onHash)
  }, [])

  const byId = useMemo(() => {
    const m = new Map<string, Snippet>()
    data?.snippets.forEach((s) => m.set(s.id, s))
    return m
  }, [data])

  const searchEngine = useMemo(() => {
    if (!data) return null
    const ms = new MiniSearch<Snippet>({
      fields: ['name', 'description', 'usage', 'code', 'id'],
      storeFields: ['id'],
      processTerm: (term) => term.toLowerCase(),
      searchOptions: {
        boost: { name: 4, description: 2 },
        prefix: true,
        fuzzy: 0.15,
        processTerm: (term) => term.toLowerCase(),
      },
    })
    ms.addAll(data.snippets)
    return ms
  }, [data])

  const filtered = useMemo(() => {
    if (!data) return []
    let list = data.snippets
    if (chapter !== 'all') list = list.filter((s) => s.chapter === chapter)
    if (!showExcluded) list = list.filter((s) => s.includedInPdf)
    const q = query.trim()
    if (q && searchEngine) {
      const hits = new Set(
        searchEngine
          .search(q, { prefix: true, fuzzy: 0.15, boost: { name: 4, description: 2 } })
          .map((h) => h.id as string),
      )
      list = list.filter((s) => hits.has(s.id))
    }
    return list
  }, [data, chapter, showExcluded, query, searchEngine])

  useEffect(() => {
    if (!filtered.length) return
    if (selectedId && filtered.some((s) => s.id === selectedId)) return
    const next = filtered[0]
    setSelectedId(next.id)
    setHash(next.id)
  }, [filtered, selectedId])

  const selected =
    selectedId && filtered.some((s) => s.id === selectedId)
      ? (byId.get(selectedId) ?? null)
      : filtered.length
        ? (byId.get(filtered[0].id) ?? null)
        : null

  function selectSnippet(id: string) {
    setSelectedId(id)
    setHash(id)
    const s = byId.get(id)
    if (s && chapter !== 'all' && s.chapter !== chapter) {
      setChapter(s.chapter)
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

  return (
    <div className="app-shell">
      <aside className="sidebar">
        <div className="brand">
          <p className="brand-mark">KACTL</p>
          <h1 className="brand-title">Snippets</h1>
          <p className="brand-sub">Searchable contest reference</p>
        </div>
        <nav className="chapter-nav" aria-label="Chapters">
          <button
            type="button"
            className={chapter === 'all' ? 'nav-item active' : 'nav-item'}
            onClick={() => setChapter('all')}
          >
            All
          </button>
          {data.chapters.map((c) => (
            <button
              key={c.id}
              type="button"
              className={chapter === c.id ? 'nav-item active' : 'nav-item'}
              onClick={() => setChapter(c.id)}
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
        <p className="sidebar-note">Geometry deferred (figure headers).</p>
      </aside>

      <main className="main">
        <div className="search-bar">
          <input
            type="search"
            placeholder="Search name, description, code…"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            aria-label="Search snippets"
          />
          <span className="result-count">
            {filtered.length} / {data.snippets.length}
          </span>
        </div>

        <div className="content-split">
          <div className="list-pane" role="listbox" aria-label="Snippets">
            {filtered.length === 0 ? (
              <p className="empty">No matches.</p>
            ) : (
              filtered.map((s) => (
                <SnippetListItem
                  key={s.id}
                  snippet={s}
                  active={s.id === selectedId}
                  onSelect={() => selectSnippet(s.id)}
                />
              ))
            )}
          </div>
          <div className="detail-pane">
            {selected ? (
              <SnippetDetail
                snippet={selected}
                byId={byId}
                onSelectDep={(id) => selectSnippet(id)}
              />
            ) : (
              <p className="empty">Select a snippet.</p>
            )}
          </div>
        </div>
      </main>
    </div>
  )
}
