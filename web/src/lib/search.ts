import type { SnippetIndex } from './types'

export type SearchRecord = {
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

export type SearchWorkerRequest =
  | { type: 'index'; records: SearchRecord[] }
  | { type: 'search'; query: string }

export type SearchWorkerResponse =
  | { type: 'ready' }
  | { type: 'results'; query: string; ids: string[] }

export function toSearchRecords(data: SnippetIndex): SearchRecord[] {
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
  let lastHeading = ''
  for (const block of data.document) {
    if (block.type === 'heading') {
      lastHeading = block.searchText || block.title
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
        name: lastHeading || block.searchText.slice(0, 80),
        title: lastHeading || 'Chapter text',
        searchText: block.searchText,
        description: block.searchText,
        usage: '',
        code: '',
      })
    }
  }
  return records
}

/** Split identifiers the way MiniSearch does: Unicode space or punctuation. */
const SPACE_OR_PUNCTUATION = /[\n\r\p{Z}\p{P}]+/u

/**
 * Chunk a C++-style identifier: LazySegmentTree → Lazy, Segment, Tree;
 * FenwickTree2d → Fenwick, Tree, 2d; HLD → HLD.
 */
export function splitIdentifier(token: string): string[] {
  return (
    token.match(
      /[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+|\d+[a-z]*|\d+/g,
    ) ?? [token]
  )
}

function uniquePreserve(terms: string[]): string[] {
  const seen = new Set<string>()
  const out: string[] = []
  for (const t of terms) {
    if (!t || seen.has(t)) continue
    seen.add(t)
    out.push(t)
  }
  return out
}

/**
 * Index an identifier so glued queries like "segmenttree" hit LazySegmentTree
 * and SparseLazySegmentTree, not only SegmentTree.h.
 *
 * Emits the original token, each camelCase/PascalCase part, every consecutive
 * compound (Segment+Tree → SegmentTree), and a 3+ letter acronym (FFT, NTT).
 */
export function expandIndexedIdentifier(token: string): string[] {
  const parts = splitIdentifier(token)
  const terms = [token]
  for (let i = 0; i < parts.length; i++) {
    let acc = parts[i]
    terms.push(acc)
    for (let j = i + 1; j < parts.length; j++) {
      acc += parts[j]
      terms.push(acc)
    }
  }
  if (parts.length >= 3) {
    const acronym = parts.map((p) => p[0]).join('')
    if (acronym.length >= 3) terms.push(acronym)
  }
  return uniquePreserve(terms)
}

/** Tokenizer for indexed MiniSearch fields (not for the query string). */
export function tokenizeIndexedText(text: string, _fieldName?: string): string[] {
  const terms: string[] = []
  for (const raw of text.split(SPACE_OR_PUNCTUATION)) {
    if (!raw) continue
    terms.push(...expandIndexedIdentifier(raw))
  }
  return terms
}
