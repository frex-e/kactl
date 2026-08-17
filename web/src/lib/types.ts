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

export type SnippetIndex = {
  chapters: Chapter[]
  snippets: Snippet[]
}
