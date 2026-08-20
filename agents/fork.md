# Fork vs upstream KACTL

This is not a drop-in of [kth-competitive-programming/kactl](https://github.com/kth-competitive-programming/kactl). Do not restore upstream files or APIs without checking the deltas below. The detailed snippet list is [doc/mine-integration.md](../doc/mine-integration.md) — do not duplicate that table here; update it when you add/replace snippets.

## Branding

[content/kactl.tex](../content/kactl.tex):

- University: Monash / Monash University / `monash`
- Team: **A Succulent Chinese Algorithm** — Indra Kusumah-Kasim, Alex Yu, and Parsa Pordastan

The snippets site is this fork’s addition (not upstream). See [web.md](web.md).

This fork does **not** keep upstream’s 25-page ICPC notebook cap.

## Contest template

[content/contest/template.cpp](../content/contest/template.cpp) defines `pb`, `fr`, `sc` and dropped `cin.exceptions`. [content/contest/.vimrc](../content/contest/.vimrc) is personal; `:Hash` is kept.

Compile/test scripts prefer `g++-15` via [doc/scripts/cxx.sh](../doc/scripts/cxx.sh) and use `-std=c++20`. The contest `.bashrc` `c` alias stays `g++` for Linux contest VMs — do not “fix” that to `g++-15`.

## Snippet API that diverges

**Lazy segment tree.** [content/data-structures/LazySegmentTree.h](../content/data-structures/LazySegmentTree.h) is `LazyUpdateTree`, not upstream’s pointer `Node` with range `set`/`add`. Bounds are **inclusive on both sides**. Range `update`, point `set`, range `query`. Default op is range add + range max; customize `V`/`U`/`binop`/`applyUpdate`/`mergeUpdate`.

**HLD.** [content/graph/HLD.h](../content/graph/HLD.h) uses `LazyUpdateTree`. Internal `process` still talks in half-open `[l, r)` then converts with `r - 1` for `tree.update` / `tree.query`. Do not “simplify” those calls back to half-open, and do not call a removed `tree->set` API. Subtree query is already inclusive: `pos[v] + VALS_EDGES` .. `pos[v] + siz[v] - 1`.

**Suffix array.** Same SA/LCP as upstream, plus rank, RMQ, `getLCP`, `cmpSubstr`. Stress test covers the extras.

**Offline dynamic connectivity.** [content/data-structures/OfflineDynamicConnectivity.h](../content/data-structures/OfflineDynamicConnectivity.h) is sequential: `toggle(u, v)` adds or deletes an undirected edge, `query()` records a component-count snapshot, `ans()` returns answers. $q$ is an upper bound on the number of `toggle`/`query` calls.

**Binary trie.** [content/data-structures/BinaryTrie.h](../content/data-structures/BinaryTrie.h) is a pointer trie with set insert/erase, multiset insert, XOR-min/max, XOR-count, lazy XOR-all, mex, and merge. Values are in $[0,2^{30})$.

**Unchanged on purpose.** Iterative [content/data-structures/SegmentTree.h](../content/data-structures/SegmentTree.h) (point updates) and geometry [content/geometry/Point.h](../content/geometry/Point.h) are still KACTL-style. Do not replace `Point` with a `complex` preamble unless asked.

## Local additions (summary)

Added (kept alongside related upstream files where noted): `Random.h`, `Output.h`, `BinaryTrie.h`, implicit `SparseLazySegmentTree.h`, `LiChao.h` (alongside `LineContainer.h`), `LinearSieve.h` (alongside Eratosthenes), `Mobius.h`, `RREF.h`, `XORBasis.h` (alongside `SolveLinearBinary.h`), `MemoryUsage.h`, `Pragmas.h`, `StaticRangeQuery.h`, `MonotonicMap.h`, `Blossom.h` (alongside `GeneralMatching.h`). Johnson’s algorithm notes, bit builtins; Dinic is included in the PDF.

Replaced/modified: contest template and vimrc; lazy tree as above; SuffixArray extras; HLD; Knuth DP and Divide-and-Conquer DP notes/code.

`Pragmas.h` and `SIMD.h` are skipped in header compile tests on non-x86.

When porting an upstream patch, rebase it onto these APIs rather than overwriting the local files.
