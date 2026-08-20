# Local snippet changes

Notes which binder snippets were added, replaced, or merged from `mine.typ`.
KACTL’s iterative `SegmentTree.h` is unchanged (simple point-update ops).
Geometry still uses KACTL `Point.h` (the `complex` preamble was not added).

## Added

| File | What |
|---|---|
| `content/contest/Random.h` | RNG + how to draw random ints / odd hash bases |
| `content/contest/Output.h` | `std::format` binary printing + fixed precision |
| `content/data-structures/BinaryTrie.h` | insert/erase, XOR-min/max, count, lazy XOR, mex, merge |
| `content/data-structures/SparseLazySegmentTree.h` | implicit lazy tree with point set |
| `content/data-structures/LiChao.h` | min Li Chao (kept alongside `LineContainer.h`) |
| `content/number-theory/LinearSieve.h` | linear sieve + least prime factor (kept alongside Eratosthenes) |
| `content/number-theory/Mobius.h` | Möbius sieve (formulas stay in `chapter.tex`) |
| `content/numerical/RREF.h` | rectangular reduced row echelon form |
| `content/numerical/XORBasis.h` | incremental XOR basis (kept alongside `SolveLinearBinary.h`) |
| `content/numerical/MinPlusConvolution.h` | min-plus convolution (SMAWK / border; from cactl; untested) |
| `content/various/MemoryUsage.h` | `getrusage` RSS |
| `content/various/Pragmas.h` | pasteable GCC pragmas |
| `content/geometry/HalfplaneIntersection.h` | half-plane intersection (left of $s\to e$) |
| `content/graph/Centroid.h` | centroid decomposition |
| `content/data-structures/PersistentSegmentTree.h` | persistent implicit lazy tree with point set |
| `content/data-structures/OfflineDynamicConnectivity.h` | D\&C on time + rollback DSU (toggle/query/ans) |
| `content/data-structures/StaticRangeQuery.h` | disjoint sparse table, any associative op |
| `content/graph/Blossom.h` | Edmonds matching, 0-indexed |
| `content/number-theory/FloorBlocks.h` | $\lfloor n/i\rfloor$ blocks |

Also in chapter text (no new `.h`): Johnson’s algorithm, extra bit builtins, flow demands / lower bounds.

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
| `content/graph/chapter.tex` | Johnson’s notes; Dinic; centroid; demands / lower bounds; blossom |
| `content/number-theory/chapter.tex` | Möbius; linear sieve; moduli; highly composite; floor blocks |
| `content/contest/chapter.tex` | Random + Output.h |
| `content/various/chapter.tex` | builtins, pragmas, memory |
| `content/data-structures/chapter.tex` | trees / Li Chao / trie / persistent / dyncon / static RQ |
| `content/numerical/chapter.tex` | RREF, XOR basis, min-plus convolution; Fourier → Convolutions |
| `content/geometry/chapter.tex` | half-plane intersection |

Compile scripts (`doc/scripts/*.sh`) pick `g++-15` if present, otherwise `g++` (override with `CXX=...`), and use `-std=c++20`. The contest `.bashrc` `c` alias stays as `g++` for Linux contest VMs.
`Pragmas.h` and `SIMD.h` are skipped in header compile tests on non-x86 (avx2 is invalid on ARM) and compiled on x86 Linux.

## Tests

- `stress-tests/data-structures/LazySegmentTree.cpp` rewritten for `LazyUpdateTree`
- `stress-tests/graph/HLD.cpp` no longer calls `tree->set` (defaults are 0)
- New stress tests: SparseLazySegmentTree, LiChao, BinaryTrie, KnuthDP, XORBasis, RREF, LinearSieve, Mobius, HalfplaneIntersection, Centroid, PersistentSegmentTree, FloorBlocks, OfflineDynamicConnectivity, StaticRangeQuery, Blossom
- `stress-tests/strings/SuffixArray.cpp` now also checks rank, `getLCP`, and `cmpSubstr`
