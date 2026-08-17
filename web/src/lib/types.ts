export type Chapter = {
  id: string
  title: string
}

export type Snippet = {
  id: string
  name: string
  chapter: string
  description: string
  usage: string
  time: string
  memory: string
  status: string
  author: string
  source: string
  dependencies: string[]
  includedInPdf: boolean
  code: string
}

export type HeadingBlock = {
  type: 'heading'
  id: string
  chapter: string
  level: 1 | 2 | 3 | 4
  title: string
  searchText: string
}

export type ProseBlock = {
  type: 'prose'
  id: string
  chapter: string
  latex: string
  searchText: string
}

export type SnippetBlock = {
  type: 'snippet'
  id: string
  chapter: string
  includedInPdf: boolean
}

export type DocumentBlock = HeadingBlock | ProseBlock | SnippetBlock

export type SnippetIndex = {
  chapters: Chapter[]
  snippets: Snippet[]
  document: DocumentBlock[]
}
