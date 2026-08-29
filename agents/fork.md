# Fork vs upstream KACTL

This is not a drop-in of [kth-competitive-programming/kactl](https://github.com/kth-competitive-programming/kactl). Do not restore upstream files or APIs without checking the deltas below. When you add or replace a snippet relative to upstream, update the tables in this file.

KACTL’s iterative `SegmentTree.h` is unchanged (simple point-update ops). Geometry still uses KACTL `Point.h` (the `complex` preamble was not added).

## Branding

[content/kactl.tex](../content/kactl.tex):

- University: Monash / Monash University / `monash`
- Team: **A Succulent Chinese Algorithm** — Indra Kusumah-Kasim, Alex Yu, and Parsa Pordastan

Code listings use **Inconsolata** (`varl` + `varqu` + `mono`, vendored under [`texmf/`](../texmf/README.md)) instead of Courier, so `1` / `l` / `I` and `0` / `O` stay distinct in the printed notebook. listings `basewidth` is `0.5em` to match the glyph width (Courier’s `0.6em` default wrapped ~54-character lines).

The snippets site is this fork’s addition (not upstream). See [web.md](web.md).

This fork does **not** keep upstream’s 25-page ICPC notebook cap.

## Contest template

[content/contest/template.cpp](../content/contest/template.cpp) defines `pb`, `fr`, `sc` and dropped `cin.exceptions`. [content/contest/.vimrc](../content/contest/.vimrc) is personal; `:Hash` is kept.

Compile/test scripts prefer `g++-15` via [doc/scripts/cxx.sh](../doc/scripts/cxx.sh) and use `-std=c++20`. The contest `.bashrc` `c` alias stays `g++` for Linux contest VMs — do not “fix” that to `g++-15`.

`Pragmas.h` and `SIMD.h` are skipped in header compile tests on non-x86 (avx2 is invalid on ARM).

## Snippet API that diverges

**Lazy segment tree.** [content/data-structures/LazySegmentTree.h](../content/data-structures/LazySegmentTree.h) is `LazyUpdateTree`, not upstream’s pointer `Node` with range `set`/`add`. Bounds are **inclusive on both sides**. Range `update`, point `set`, range `query`. Default op is range add + range max; customize `V`/`U`/`binop`/`applyUpdate`/`mergeUpdate`.

**HLD.** [content/graph/HLD.h](../content/graph/HLD.h) uses `LazyUpdateTree`. Internal `process` still talks in half-open `[l, r)` then converts with `r - 1` for `tree.update` / `tree.query`. Do not “simplify” those calls back to half-open, and do not call a removed `tree->set` API. Subtree query is already inclusive: `pos[v] + VALS_EDGES` .. `pos[v] + siz[v] - 1`.

**Suffix array.** Same SA/LCP as upstream, plus rank, RMQ, `getLCP`, `cmpSubstr`. Stress test covers the extras.

**Offline dynamic connectivity.** [content/data-structures/OfflineDynamicConnectivity.h](../content/data-structures/OfflineDynamicConnectivity.h) is sequential: `toggle(u, v)` adds or deletes an undirected edge, `query()` records a component-count snapshot, `ans()` returns answers. $q$ is an upper bound on the number of `toggle`/`query` calls.

**Binary trie.** [content/data-structures/BinaryTrie.h](../content/data-structures/BinaryTrie.h) is a pointer trie with set insert/erase, multiset `insert<1>`, XOR-min/max, XOR-count, lazy XOR-all, mex, `each`, and merge. XOR queries (`minxor`/`maxxor`/`count`/`mex`) take `xr` (default 0). `each(f)` calls `f(x, cnt)` for each stored value. `merge` is set-union (so `cnt`/`mex` stay unique after overlapping `insert`s) and destroys the other trie (safe to delete); `merge<1>` adds multiplicities from `insert<1>`. Values are in $[0,2^{30})$.

**Unchanged on purpose.** Iterative [content/data-structures/SegmentTree.h](../content/data-structures/SegmentTree.h) (point updates) and geometry [content/geometry/Point.h](../content/geometry/Point.h) are still KACTL-style. Do not replace `Point` with a `complex` preamble unless asked.

When porting an upstream patch, rebase it onto these APIs rather than overwriting the local files.

## Added

| File | What |
|---|---|
| `content/contest/Random.h` | RNG + random ints / shuffle / odd hash bases |
| `content/contest/Output.h` | `std::format` binary printing + fixed precision |
| `content/data-structures/BinaryTrie.h` | insert/`insert<1>`/erase, XOR-min/max, count, lazy XOR, mex, each, set-union merge / `merge<1>` |
| `content/data-structures/SparseLazySegmentTree.h` | implicit lazy tree with point set |
| `content/data-structures/LiChao.h` | min Li Chao (kept alongside `LineContainer.h`) |
| `content/number-theory/LinearSieve.h` | linear sieve + least prime factor (kept alongside Eratosthenes) |
| `content/number-theory/Mobius.h` | Möbius sieve (formulas stay in `chapter.tex`) |
| `content/numerical/RREF.h` | rectangular reduced row echelon form |
| `content/numerical/XORBasis.h` | incremental unsigned XOR basis (kept alongside `SolveLinearBinary.h`) |
| `content/numerical/QuadRoots.h` | stable real quadratic roots (from cactl / cp-geo) |
| `content/numerical/MinPlusConvolution.h` | min-plus convolution (SMAWK / border; from cactl; untested) |
| `content/various/MemoryUsage.h` | `getrusage` peak RSS (lifetime, not current) |
| `content/various/Pragmas.h` | pasteable GCC pragmas |
| `content/geometry/HalfplaneIntersection.h` | half-plane intersection (left of $s\to e$) |
| `content/graph/Centroid.h` | centroid decomposition |
| `content/data-structures/PersistentSegmentTree.h` | persistent implicit lazy tree with point set |
| `content/data-structures/OfflineDynamicConnectivity.h` | D\&C on time + rollback DSU (toggle/query/ans) |
| `content/data-structures/StaticRangeQuery.h` | disjoint sparse table, any associative op |
| `content/data-structures/SegmentTreeBeats.h` | range chmin/chmax/add + sum/min/max (USACO Guide) |
| `content/data-structures/MonotonicMap.h` | prefix/suffix min/max with insertions (monotonic map) |
| `content/graph/Blossom.h` | Gabow--Edmonds matching (ei1333 / LC), 0-indexed |
| `content/graph/DominatorTree.h` | Lengauer--Tarjan dominator tree (from cactl / Benq); runtime $n$, ctor takes adj+root |
| `content/graph/SteinerTree.h` | Dreyfus--Wagner Steiner tree; Library Checker `correct.cpp` rewrite; cost + edge indices |
| `content/number-theory/FloorBlocks.h` | $\lfloor n/i\rfloor$ blocks |
| `content/number-theory/PrimitiveRoot.h` | order of $a$ mod prime $p$, plus smallest primitive root |

Also in chapter text (no new `.h`): Johnson’s algorithm, extra bit builtins, flow demands / lower bounds, 12-fold way, matching (Kőnig, path cover, Dilworth).

## Replaced

| File | What |
|---|---|
| `content/contest/template.cpp` | `pb` / `fr` / `sc`; dropped `cin.exceptions` |
| `content/contest/.vimrc` | personal settings; kept KACTL `:Hash` |
| `content/data-structures/LazySegmentTree.h` | KACTL pointer `Node` (range set+add) → `LazyUpdateTree` (inclusive, generic `binop` / lazy, point set) |
| `content/strings/SuffixArray.h` | same SA/LCP, plus rank, RMQ, `getLCP`, `cmpSubstr` |

**Deleted behaviour:** KACTL’s lazy tree (`Node` with `set`/`add`, half-open, bump allocator). HLD now uses `LazyUpdateTree`.

## Modified

| File | What |
|---|---|
| `content/data-structures/Treap.h` | subtree sum, lazy range add |
| `content/graph/HLD.h` | uses `LazyUpdateTree`; converts half-open HLD ranges to inclusive `[l, r-1]` |
| `content/various/KnuthDP.h` | quadrangle notes (verified patterns) + `knuthDP` implementation |
| `content/various/DivideAndConquerDP.h` | layered recurrence + usage of `lo`/`hi`/`f`/`store` |
| `content/graph/chapter.tex` | Johnson’s notes; Dinic; centroid; demands / lower bounds; blossom; dominator tree; Steiner tree; Kőnig / path cover / Dilworth |
| `content/combinatorial/chapter.tex` | 12-fold way table (balls/bins / functions $[n]\to[k]$) |
| `content/number-theory/chapter.tex` | Möbius; linear sieve; moduli; highly composite; floor blocks; order / primitive roots |
| `content/contest/chapter.tex` | Random + Output.h |
| `content/various/chapter.tex` | builtins, pragmas, memory |
| `content/data-structures/chapter.tex` | trees / Li Chao / trie / persistent / dyncon / static RQ / beats / monotonic map |
| `content/numerical/chapter.tex` | RREF, XOR basis, QuadRoots, min-plus convolution, MatrixInverse-mod; Fourier → Convolutions |
| `content/strings/chapter.tex` | Hashing-codeforces in the PDF (alongside `Hashing.h`) |
| `content/geometry/chapter.tex` | half-plane intersection |
| geometry figure captions | same glued 15mm minipages as upstream (with their `\vspace`); text width is `\linewidth-15mm` instead of `75mm` so they fit the printable-margin columns |

## Tests

- `stress-tests/data-structures/LazySegmentTree.cpp` rewritten for `LazyUpdateTree`
- `stress-tests/graph/HLD.cpp` no longer calls `tree->set` (defaults are 0)
- New stress tests: SparseLazySegmentTree, LiChao, BinaryTrie, KnuthDP, XORBasis, RREF, QuadRoots, LinearSieve, Mobius, HalfplaneIntersection, Centroid, PersistentSegmentTree, FloorBlocks, OfflineDynamicConnectivity, StaticRangeQuery, MonotonicMap, Blossom, SegmentTreeBeats, DominatorTree, SteinerTree, PrimitiveRoot
- `stress-tests/strings/SuffixArray.cpp` now also checks rank, `getLCP`, and `cmpSubstr`
