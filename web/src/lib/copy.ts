import type { Snippet } from './types'

/** Transitive dependencies in topological order, then the snippet itself. */
export function orderWithDependencies(
  snippet: Snippet,
  byId: Map<string, Snippet>,
): Snippet[] {
  const result: Snippet[] = []
  const visiting = new Set<string>()
  const done = new Set<string>()

  function visit(id: string) {
    if (done.has(id) || !byId.has(id)) return
    if (visiting.has(id)) return
    visiting.add(id)
    const s = byId.get(id)!
    for (const dep of s.dependencies) visit(dep)
    visiting.delete(id)
    done.add(id)
    result.push(s)
  }

  visit(snippet.id)
  return result
}

export function formatSnippetBundle(snippets: Snippet[]): string {
  return snippets
    .map((s) => {
      const banner = `// ${s.id}`
      return `${banner}\n${s.code}`.trimEnd()
    })
    .join('\n\n')
}

export async function copyText(text: string): Promise<boolean> {
  const fallback = (): boolean => {
    try {
      const ta = document.createElement('textarea')
      ta.value = text
      ta.style.position = 'fixed'
      ta.style.left = '-9999px'
      document.body.appendChild(ta)
      ta.select()
      const ok = document.execCommand('copy')
      document.body.removeChild(ta)
      return ok
    } catch {
      return false
    }
  }

  try {
    if (navigator.clipboard?.writeText) {
      await Promise.race([
        navigator.clipboard.writeText(text),
        new Promise<never>((_, reject) =>
          window.setTimeout(() => reject(new Error('clipboard timeout')), 400),
        ),
      ])
      return true
    }
  } catch {
    return fallback()
  }
  return fallback()
}
