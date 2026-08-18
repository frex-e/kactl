import MiniSearch from 'minisearch'
import { tokenizeIndexedText, type SearchRecord } from './search'

const SEARCH_BOOST = { name: 4, title: 4, description: 2, searchText: 2 }

export function createSearchEngine(records: SearchRecord[]): MiniSearch<SearchRecord> {
  const ms = new MiniSearch<SearchRecord>({
    fields: ['name', 'title', 'searchText', 'description', 'usage', 'code', 'id'],
    storeFields: ['id'],
    tokenize: tokenizeIndexedText,
    processTerm: (term) => term.toLowerCase(),
    searchOptions: {
      // Keep query tokenization as MiniSearch default (space/punctuation only).
      // CamelCase splitting is index-side so "segmenttree" still matches
      // LazySegmentTree via the indexed compound, without OR-ing "tree".
      tokenize: MiniSearch.getDefault('tokenize'),
      boost: { ...SEARCH_BOOST },
      prefix: true,
      fuzzy: 0.15,
      processTerm: (term) => term.toLowerCase(),
    },
  })
  ms.addAll(records)
  return ms
}

export function searchDocument(engine: MiniSearch<SearchRecord>, query: string): string[] {
  const q = query.trim()
  if (!q) return []
  return engine
    .search(q, {
      prefix: true,
      fuzzy: 0.15,
      boost: { ...SEARCH_BOOST },
    })
    .map((hit) => String(hit.id))
}
