import { useEffect, useRef, useState } from 'react'
import type { SearchRecord, SearchWorkerRequest, SearchWorkerResponse } from './search'

/**
 * Run MiniSearch off the UI thread so the search box stays responsive.
 * Keystrokes always update `query`; in-flight work is dropped in favor of the
 * latest string, and results for a stale query are ignored.
 */
export function useDocumentSearch(records: SearchRecord[], query: string) {
  const [hitIds, setHitIds] = useState<string[]>([])
  const [resultQuery, setResultQuery] = useState('')

  const workerRef = useRef<Worker | null>(null)
  const readyRef = useRef(false)
  const inFlightRef = useRef(false)
  const queryRef = useRef(query)
  queryRef.current = query

  const requestSearchRef = useRef(() => {})
  requestSearchRef.current = () => {
    const worker = workerRef.current
    if (!worker || !readyRef.current || inFlightRef.current) return
    const q = queryRef.current.trim()
    if (!q) {
      setHitIds([])
      setResultQuery('')
      return
    }
    inFlightRef.current = true
    const message: SearchWorkerRequest = { type: 'search', query: q }
    worker.postMessage(message)
  }

  useEffect(() => {
    if (!records.length) return

    const worker = new Worker(new URL('./search.worker.ts', import.meta.url), {
      type: 'module',
    })
    workerRef.current = worker

    worker.onerror = (event) => {
      console.error('Search worker failed', event.message)
    }

    worker.onmessage = (event: MessageEvent<SearchWorkerResponse>) => {
      const msg = event.data
      if (msg.type === 'ready') {
        readyRef.current = true
        requestSearchRef.current()
        return
      }
      inFlightRef.current = false
      const current = queryRef.current.trim()
      if (!current) {
        setHitIds([])
        setResultQuery('')
        return
      }
      if (msg.query === current) {
        setHitIds(msg.ids)
        setResultQuery(msg.query)
        return
      }
      requestSearchRef.current()
    }

    const index: SearchWorkerRequest = { type: 'index', records }
    worker.postMessage(index)

    return () => {
      worker.terminate()
      workerRef.current = null
      readyRef.current = false
      inFlightRef.current = false
    }
  }, [records])

  useEffect(() => {
    if (!query.trim()) {
      setHitIds([])
      setResultQuery('')
      return
    }
    requestSearchRef.current()
  }, [query])

  const trimmed = query.trim()
  const pending = trimmed.length > 0 && trimmed !== resultQuery
  return { hitIds, pending }
}
