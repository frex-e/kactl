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
