/** Compile and run the actual copy-with-dependencies output. */
import assert from 'node:assert/strict'
import { execFileSync } from 'node:child_process'
import { mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs'
import { tmpdir } from 'node:os'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { createServer } from 'vite'

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..')
const repo = path.dirname(root)
const compiler = execFileSync('bash', ['-c', '. doc/scripts/cxx.sh; printf "%s" "$CXX"'], {
  cwd: repo, encoding: 'utf8',
}).trim()
const { snippets } = JSON.parse(readFileSync(path.join(root, 'public/snippets.json'), 'utf8'))
const byId = new Map(snippets.map((snippet) => [snippet.id, snippet]))
const dir = mkdtempSync(path.join(tmpdir(), 'kactl-copy-'))
const server = await createServer({
  configFile: path.join(root, 'vite.config.ts'),
  root,
  server: { middlewareMode: true, ws: false },
  appType: 'custom',
})

try {
  const { orderWithDependencies, formatSnippetBundle } =
    await server.ssrLoadModule('/src/lib/copy.ts')

  const ntt = 'numerical/NumberTheoreticTransform.h'
  const sqrt = 'number-theory/ModSqrt.h'
  const nttChecks = `
    assert(modpow(1000000006LL, 2LL) == 1);
    assert((conv({mod - 1, 2}, {3, 4}) == vl{mod - 3, 2, 8}));
    assert(conv({}, {1}).empty());
  `
  const sqrtChecks = `
    for (ll p : {2LL, 13LL, 17LL, 998244353LL, 1000000007LL}) {
      ll a = 12345 % p;
      ll x = sqrt(a * a % p, p);
      assert(0 <= x && x < p && x * x % p == a * a % p);
      assert(sqrt(0LL, p) == 0);
    }
    ll x = sqrt(-13LL, 17LL);
    assert(x * x % 17 == 4);
  `
  const cases = [
    [['number-theory/ModPow.h'], `
      assert(modpow(2LL, 0LL) == 1);
      assert(modpow(2LL, 30LL) == 73741817);
      assert(modpow(1000000006LL, 2LL) == 1);
      assert(modpow(2LL, 0LL, 17LL) == 1);
      assert(modpow(2LL, 10LL, 17LL) == 4);
      assert(modpow(2LL, 10LL, 19LL) == 17);
      assert(modpow<17>(2LL, 0LL) == 1);
      assert(modpow<17>(2LL, 10LL) == 4);
      assert(modpow<19>(2LL, 10LL) == 17);
    `],
    [[ntt], nttChecks],
    [[sqrt], sqrtChecks],
    [[ntt, sqrt], nttChecks + sqrtChecks],
    [[sqrt, ntt], nttChecks + sqrtChecks],
    [['numerical/BerlekampMassey.h'], `
      vector<ll> s{0, 1, 1, 3, 5, 11};
      assert((berlekampMassey(s) == vector<ll>{1, 2}));
      assert((berlekampMassey<5>(s) == vector<ll>{1, 2}));
    `],
    [['numerical/MatrixInverse-mod.h'], `
      for (ll p : {5LL, 1000000007LL}) {
        vector<vector<ll>> a{{1, 2}, {3, 4}}, inv = a;
        assert((p == 5 ? matInv<5>(inv) : matInv(inv)) == 2);
        rep(i,0,2) rep(j,0,2) {
          ll sum = 0;
          rep(k,0,2) sum += a[i][k] * inv[k][j];
          assert(sum % p == (i == j));
        }
      }
    `],
    [['graph/GeneralMatching.h'], `
      vector<pii> ed{{0, 1}, {1, 2}, {0, 2}};
      assert(sz(generalMatching(3, ed)) == 1);
    `],
  ]

  for (const [ids, checks] of cases) {
    const ordered = new Map()
    for (const id of ids) {
      assert(byId.has(id), `Missing snippet: ${id}`)
      for (const snippet of orderWithDependencies(byId.get(id), byId)) {
        ordered.set(snippet.id, snippet)
      }
    }
    assert(ordered.has('number-theory/ModPow.h'))
    const source = path.join(dir, 'bundle.cpp')
    const executable = path.join(dir, 'bundle')
    writeFileSync(source, '#include "stress-tests/utilities/template.h"\n' +
      formatSnippetBundle([...ordered.values()]) + `\nint main() {${checks}}\n`)
    execFileSync(compiler, ['-std=c++20', '-O2', '-I', repo, source, '-o', executable], {
      stdio: 'inherit',
    })
    execFileSync(executable, [], { stdio: 'inherit' })
    console.log(`copy-with-dependencies ok: ${ids.join(' + ')}`)
  }
} finally {
  await server.close()
  rmSync(dir, { recursive: true, force: true })
}
