import { createSearchEngine, searchDocument } from './searchEngine'
import type { SearchWorkerRequest, SearchWorkerResponse } from './search'

let engine: ReturnType<typeof createSearchEngine> | null = null

function send(message: SearchWorkerResponse) {
  ;(self as unknown as { postMessage: (msg: SearchWorkerResponse) => void }).postMessage(message)
}

function handle(message: SearchWorkerRequest) {
  if (message.type === 'index') {
    engine = createSearchEngine(message.records)
    send({ type: 'ready' })
    return
  }
  send({
    type: 'results',
    query: message.query,
    ids: engine ? searchDocument(engine, message.query) : [],
  })
}

addEventListener('message', (event: MessageEvent<SearchWorkerRequest>) => {
  handle(event.data)
})
