import { memo } from 'react'
import type { DocumentBlock, Snippet } from '../lib/types'
import { LatexText } from '../lib/latex'
import { SnippetDetail } from './SnippetViews'

const HEADING_TAG = {
  1: 'h1',
  2: 'h2',
  3: 'h3',
  4: 'h4',
} as const

/** Memoized so search-box keystrokes in App do not re-typeset the document. */
export const DocumentView = memo(function DocumentView({
  blocks,
  byId,
  selectedId,
  onSelectDep,
  scrollRoot,
}: {
  blocks: DocumentBlock[]
  byId: Map<string, Snippet>
  selectedId: string | null
  onSelectDep: (id: string) => void
  scrollRoot?: Element | null
}) {
  return (
    <div className="document">
      {blocks.map((block) => {
        if (block.type === 'heading') {
          const Tag = HEADING_TAG[block.level]
          return (
            <Tag
              key={block.id}
              className={`doc-heading doc-h${block.level}`}
              data-doc-id={block.id}
            >
              <LatexText text={block.title} as="span" />
            </Tag>
          )
        }
        if (block.type === 'prose') {
          return (
            <section key={block.id} className="doc-prose" data-doc-id={block.id}>
              <LatexText text={block.latex} className="doc-prose-body" />
            </section>
          )
        }
        const snippet = byId.get(block.id)
        if (!snippet) return null
        return (
          <SnippetDetail
            key={block.id}
            snippet={snippet}
            byId={byId}
            onSelectDep={onSelectDep}
            active={block.id === selectedId}
            scrollRoot={scrollRoot}
          />
        )
      })}
    </div>
  )
})
