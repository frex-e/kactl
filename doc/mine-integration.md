# Local snippet changes

Notes which binder snippets were added, replaced, or merged from `mine.typ`.
KACTL’s iterative `SegmentTree.h` is unchanged (simple point-update ops).
Geometry still uses KACTL `Point.h` (the `complex` preamble was not added).

## Added

| File | What |
|---|---|
| `content/contest/Random.h` | RNG + how to draw random ints / odd hash bases |
| `content/data-structures/BinaryTrie.h` | insert, XOR-max, mex |
| `content/data-structures/SparseSegmentTree.h` | implicit point-update tree |
| `content/data-structures/SparseLazySegmentTree.h` | implicit lazy tree |
| `content/data-structures/LiChao.h` | min Li Chao (kept alongside `LineContainer.h`) |
| `content/number-theory/LinearSieve.h` | linear sieve + least prime factor (kept alongside Eratosthenes) |
| `content/number-theory/Mobius.h` | Möbius sieve (formulas stay in `chapter.tex`) |
| `content/numerical/RREF.h` | rectangular reduced row echelon form |
| `content/numerical/XORBasis.h` | incremental XOR basis (kept alongside `SolveLinearBinary.h`) |
| `content/various/MemoryUsage.h` | `getrusage` RSS |
| `content/various/Pragmas.h` | pasteable GCC pragmas |

Also in chapter text (no new `.h`): contest I/O notes, Johnson’s algorithm, extra bit builtins.

## Replaced

| File | What |
|---|---|
| `content/contest/template.cpp` | `pb` / `fr` / `sc`; dropped `cin.exceptions` |
| `content/contest/.vimrc` | personal settings; kept KACTL `:Hash` |
| `content/data-structures/LazySegmentTree.h` | KACTL pointer `Node` (range set+add) → `LazyUpdateTree` (inclusive, generic `binop` / lazy) |
| `content/strings/SuffixArray.h` | same SA/LCP, plus rank, RMQ, `getLCP`, `cmpSubstr` |

**Deleted behaviour:** KACTL’s lazy tree (`Node` with `set`/`add`, half-open, bump allocator). HLD now uses `LazyUpdateTree`.

## Modified

| File | What |
|---|---|
| `content/graph/HLD.h` | uses `LazyUpdateTree`; converts half-open HLD ranges to inclusive `[l, r-1]` |
| `content/various/KnuthDP.h` | quadrangle notes + `knuthDP` implementation |
| `content/various/DivideAndConquerDP.h` | layered recurrence + usage of `lo`/`hi`/`f`/`store` |
| `content/graph/chapter.tex` | Johnson’s notes; Dinic included in the PDF |
| `content/number-theory/chapter.tex` | Möbius code import; linear sieve |
| `content/contest/chapter.tex` | Random + I/O |
| `content/various/chapter.tex` | builtins, pragmas, memory |
| `content/data-structures/chapter.tex` | new trees / Li Chao / trie |
| `content/numerical/chapter.tex` | RREF, XOR basis |

Compile scripts (`doc/scripts/*.sh`) pick `g++-15` if present, otherwise `g++` (override with `CXX=...`). The contest `.bashrc` `c` alias stays as `g++` for Linux contest VMs.
`Pragmas.h` and `SIMD.h` are skipped in header compile tests on non-x86 (avx2 is invalid on ARM) and compiled on x86 Linux.

## Tests

- `stress-tests/data-structures/LazySegmentTree.cpp` rewritten for `LazyUpdateTree`
- `stress-tests/graph/HLD.cpp` no longer calls `tree->set` (defaults are 0)
- New stress tests: SparseLazySegmentTree, SparseSegmentTree, LiChao, BinaryTrie, KnuthDP, XORBasis, RREF, LinearSieve, Mobius
- `stress-tests/strings/SuffixArray.cpp` now also checks rank, `getLCP`, and `cmpSubstr`
