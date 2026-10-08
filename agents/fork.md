# Fork vs upstream KACTL

This is not a drop-in of [kth-competitive-programming/kactl](https://github.com/kth-competitive-programming/kactl). Do not restore upstream files or APIs without checking the deltas below. When you add or replace a snippet relative to upstream, update the tables in this file.

KACTL’s iterative `SegmentTree.h` is unchanged (simple point-update ops). Planar geometry uses `std::complex` with `dd`/`pp` aliases and free predicates from `Point.h`.

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

**Suffix array.** Same SA/LCP algorithm as upstream, plus rank, RMQ, `getLCP`, `cmpSubstr`. Construction and substring comparisons use unsigned byte order. Stress test covers the extras and nonzero bytes through 255.

**Offline dynamic connectivity.** [content/data-structures/OfflineDynamicConnectivity.h](../content/data-structures/OfflineDynamicConnectivity.h) is sequential: `toggle(u, v)` adds or deletes an undirected edge, `query()` records a component-count snapshot, `ans()` returns answers. $q$ is an upper bound on the number of `toggle`/`query` calls.

**Binary trie.** [content/various/BinaryTrie.h](../content/various/BinaryTrie.h) is a pointer trie with set insert/erase, multiset `insert<1>`, XOR-min/max, XOR-count, lazy XOR-all, mex, `each`, and merge. `count(x)` counts copies of a value after lazy XOR-all. `countLG<0>(xr, k)` counts values with `(value ^ xr) < k`; `countLG<1>(xr, k)` counts those with `(value ^ xr) > k`. XOR queries (`minxor`/`maxxor`/`countLG`/`mex`) take `xr` (default 0). `each(f)` calls `f(x, cnt)` for each stored value. `merge` is set-union (so `cnt`/`mex` stay unique after overlapping `insert`s) and destroys the other trie (safe to delete); `merge<1>` adds multiplicities from `insert<1>`. Values are in $[0,2^{30})$.

**Geometry.** [Point.h](../content/geometry/Point.h) defines `pp = complex<double>` and free `dotp`, `crossp`, `orient`, `perp`, `dist` helpers. All geometry snippets use `pp`; rewrite it and dependent numeric types manually when exact integer geometry is needed. Sorting/sets use `PointLess`, not an added `std` overload. `Angle` stores a complex direction plus a turn count. `Point3D` remains separate because complex numbers represent only two coordinates.

**Unchanged on purpose.** Iterative [content/data-structures/SegmentTree.h](../content/data-structures/SegmentTree.h) (point updates) remains KACTL-style.

When porting an upstream patch, rebase it onto these APIs rather than overwriting the local files.

## Added

| File | What |
|---|---|
| `content/data-structures/CartesianTree.h` | linear min Cartesian tree; parent/left/right indices, leftmost tie-breaking |
| `content/data-structures/Rope.h` | GNU rope API reference: editable sequences, shared snapshots, substring and concatenation |
| `content/data-structures/WaveletTree.h` | static range kth, countLess, and value-range count; compressed signed int values, half-open bounds, 64-bit thresholds |
| `content/data-structures/PrefixSum.h` | static box sums with runtime dimension count and per-axis widths; flat row-major input, half-open bounds |
| `content/contest/Random.h` | RNG + random ints / shuffle / odd hash bases |
| `content/contest/Input.h` | `scanf` type specifiers, bounded strings, whitespace, and assignment counts |
| `content/contest/Output.h` | binary output, precision, padding/alignment with streams and `std::format`, plus `printf` type specifiers and runtime width/precision |
| `content/data-structures/UnorderedMap-codeforces.h` | web-only standard `unordered_map` with a per-run randomized SplitMix64 hash for Codeforces |
| `content/various/BinaryTrie.h` | insert/`insert<1>`/erase, count, XOR-min/max, countLG, lazy XOR, mex, each, set-union merge / `merge<1>` |
| `content/various/SparseLazySegmentTree.h` | implicit lazy tree with point set |
| `content/data-structures/LiChao.h` | min Li Chao (kept alongside `LineContainer.h`) |
| `content/data-structures/MonotonicCHT.h` | max CHT with nondecreasing slopes and queries; linear total time, 128-bit comparisons (from cactl) |
| `content/number-theory/LinearSieve.h` | linear sieve + least prime factor (kept alongside Eratosthenes) |
| `content/number-theory/Mobius.h` | Möbius sieve (formulas stay in `chapter.tex`) |
| `content/numerical/RREF.h` | rectangular reduced row echelon form |
| `content/numerical/XORBasis.h` | incremental unsigned XOR basis (kept alongside `SolveLinearBinary.h`) |
| `content/numerical/QuadRoots.h` | stable real quadratic roots (from cactl / cp-geo) |
| `content/numerical/Lagrange.h` | modular Lagrange evaluation from consecutive samples; template prime modulus, signed query indices |
| `content/various/MinPlusConvolution.h` | min-plus convolution (SMAWK / border; from cactl; untested) |
| `content/various/BigInt.h` | fixed-width unsigned base-10^9 bigint (adapted from cactl); linear decimal parsing/formatting, modular add/subtract, small-integer multiply/divide/remainder |
| `content/various/MemoryUsage.h` | `getrusage` peak RSS (lifetime, not current) |
| `content/various/Pragmas.h` | pasteable GCC pragmas |
| `content/geometry/HalfplaneIntersection.h` | half-plane intersection (left of $s\to e$) |
| `content/graph/Centroid.h` | centroid decomposition |
| `content/graph/Reroot.h` | two-pass rerooting DP; vertex/edge hooks, prefix/suffix child exclusion, and distance-sum example |
| `content/graph/Dinic2.h` | cactl Dinic without scaling; blocking-flow DFS, dead-end pruning, flow limits, constructor/addEdge/leftOfMinCut API, correct self-loop pairs; kept alongside Dinic |
| `content/various/EulerTourTree.h` | treap Euler tours: link/cut/connectivity, generic point set/get and lazy component update/query/size; recycled edge tokens |
| `content/various/PersistentSegmentTree.h` | persistent implicit lazy tree with point set; moved from data structures |
| `content/data-structures/OfflineDynamicConnectivity.h` | D\&C on time + rollback DSU (toggle/query/ans) |
| `content/data-structures/StaticRangeQuery.h` | disjoint sparse table, any associative op |
| `content/various/SegmentTreeBeats.h` | range chmin/chmax/add + sum/min/max (USACO Guide) |
| `content/various/SparseSegmentTree2d.h` | online sparse 2D tree: point assignment, half-open rectangle queries, customizable commutative aggregate |
| `content/various/LinkCutTree.h` | moved from graph; unrooted path lazy via `binop`/`rev`/`applyUpdate`/`mergeUpdate` (default add+sum) |
| `content/data-structures/MonotonicMap.h` | prefix/suffix min/max with insertions (monotonic map) |
| `content/graph/Blossom.h` | Gabow--Edmonds matching (ei1333 / LC), 0-indexed |
| `content/graph/DominatorTree.h` | Lengauer--Tarjan dominator tree (from cactl / Benq); runtime $n$, ctor takes adj+root |
| `content/graph/SteinerTree.h` | Dreyfus--Wagner Steiner tree; Library Checker `correct.cpp` rewrite; cost + edge indices |
| `content/graph/Yen.h` | k shortest simple paths; compact kmyk adaptation for directed/undirected graphs without parallel edges; costs + vertex sequences |
| `content/number-theory/FloorBlocks.h` | $\lfloor n/i\rfloor$ blocks |
| `content/number-theory/PrimitiveRoot.h` | order of $a$ mod prime $p$, plus smallest primitive root |

Also in chapter text (no new `.h`): Johnson’s algorithm, extra bit builtins, flow demands / lower bounds, 12-fold way, matching (Kőnig, path cover, Dilworth).

## Replaced

| File | What |
|---|---|
| `content/contest/template.cpp` | `pb` / `fr` / `sc`; dropped `cin.exceptions` |
| `content/contest/.vimrc` | personal settings; kept KACTL `:Hash` |
| `content/data-structures/LazySegmentTree.h` | KACTL pointer `Node` (range set+add) → `LazyUpdateTree` (inclusive, generic `binop` / lazy, point set) |
| `content/strings/SuffixArray.h` | unsigned byte order; same SA/LCP algorithm, plus rank, RMQ, `getLCP`, `cmpSubstr` |
| `content/geometry/ClosestPair.h` | Alex Li / algorithm-anthology divide-and-conquer closest pair (GPL-2.0), adapted to complex doubles; squared-difference strip checks |
| `content/geometry/Point.h` | double complex point preamble (`pp`), free predicates, explicit lexicographic comparator |

**Deleted behaviour:** KACTL’s lazy tree (`Node` with `set`/`add`, half-open, bump allocator). HLD now uses `LazyUpdateTree`.

## Modified

| File | What |
|---|---|
| `content/math/chapter.tex` | mathematical constants (pi, e, tau, square roots, logs, golden ratio, Euler--Mascheroni); C++20 names and precision, angle/log conversions, harmonic estimate |
| `content/number-theory/ModPow.h` | explicit runtime modulus or `modpow<mod>(b, e)` for a compile-time modulus (default 10^9+7); shared by NTT and modular square root without a global modulus |
| `content/numerical/NumberTheoreticTransform.h` | shared modular exponentiation; convolution uses the smallest sufficient power-of-two transform, including at the $2^{23}$ coefficient limit |
| `content/numerical/BerlekampMassey.h`, `MatrixInverse-mod.h` | template modulus (default 10^9+7); use shared modular exponentiation |
| `content/graph/GeneralMatching.h` | local modulus passed to matrix inversion and modular exponentiation |
| `content/data-structures/HashMap.h` | shared `chash` for PBDS and standard `unordered_map`, with optional per-run randomization |
| `content/data-structures/Treap.h` | generic aggregate and lazy update; defaults to range add/sum |
| `content/graph/HLD.h` | uses `LazyUpdateTree`; converts half-open HLD ranges to inclusive `[l, r-1]` |
| `content/various/KnuthDP.h` | quadrangle notes (verified patterns) + `knuthDP` implementation |
| `content/various/DivideAndConquerDP.h` | callable `partitionDP(N, K, C)` for exactly K nonempty segments with half-open costs; rolling layers and initialization included |
| `content/graph/chapter.tex` | Johnson’s notes; Yen; Dinic; centroid; demands / lower bounds; blossom; dominator tree; Steiner tree; Kőnig / path cover / Dilworth |
| `content/combinatorial/chapter.tex` | 12-fold way table (balls/bins / functions $[n]\to[k]$) |
| `content/number-theory/chapter.tex` | Möbius; linear sieve; moduli; highly composite; floor blocks; primitive roots |
| `content/contest/chapter.tex` | Random + Input.h + Output.h |
| `content/various/chapter.tex` | tree snippets and min-plus convolution directly in Various; builtins, pragmas, memory |
| `content/data-structures/chapter.tex` | trees / Li Chao / trie / persistent / dyncon / static RQ / monotonic map |
| `content/numerical/chapter.tex` | RREF, XOR basis, QuadRoots, MatrixInverse-mod; Fourier → Convolutions |
| `content/strings/chapter.tex` | Hashing-codeforces in the PDF (alongside `Hashing.h`) |
| `content/geometry/chapter.tex` | half-plane intersection; remaining upstream snippets in the PDF (`LineProjectionReflection`, `CircleLine`, `PolygonUnion`, `ManhattanMST`, `DelaunayTriangulation`); geometry is last before appendix |
| `content/geometry/*.h` | double coordinates and free predicates; `arg` macro removed |
| `content/geometry/PolygonArea.h`, `PolygonCenter.h` | local coordinates avoid cancellation after large translations |
| `content/geometry/HalfplaneIntersection.h` | unit directions, local line intersections, distance tolerance; retains small polygons and near-parallel boundaries |
| `content/geometry/sphericalDistance.h` | chord/sum `atan2` formula preserves accuracy near coincident and antipodal points |
| `content/numerical/PolyRoots.h` | distinct repeated roots, compensated Horner evaluation, constant/trailing-zero handling, and bisection to floating-point precision |
| `content/numerical/GoldenSectionSearch.h` | stops when rounding prevents further subdivision; documents the precision limit |
| `content/various/FastInput.h` | widened parsing accumulator and sign handling cover the full signed int range |
| `content/combinatorial/multinomial.h` | documents that intermediate products must fit in ll, even when the final result fits |
| `content/geometry/PolygonUnion.h` | common origin avoids translation-induced cancellation in union areas |
| `content/strings/MinRotation.h` | unsigned byte comparisons, matching standard string lexicographic order |
| `content/strings/Hashing.h`, `Hashing-codeforces.h` | hash unsigned bytes consistently in whole-string, interval, and rolling-window APIs, including nul and bytes above 127 |
| `content/various/SIMD.h` | sign-extends signed pair sums in the filtered dot-product example |
| `content/math/chapter.tex`, `content/combinatorial/chapter.tex` | corrected normal means/independence, column-stochastic Markov formulas and convergence, power sums, and Stirling table |
| geometry figure captions | same glued 15mm minipages as upstream (with their `\vspace`); text width is `\linewidth-15mm` instead of `75mm` so they fit the printable-margin columns |

## Tests

- `stress-tests/various/FastInput.cpp`: tests the actual header, signed int extremes, buffer boundaries, and randomized integers
- `stress-tests/combinatorial/multinomial.cpp`: Pascal-triangle and factorial oracles within the documented intermediate-product bound
- `stress-tests/numerical/GoldenSectionSearch.cpp`: minima at endpoints/interior, translated intervals, and rounding-induced stagnation
- `stress-tests/numerical/PolyRoots.cpp`: repeated roots of both signs, irrational/dyadic roots, small positive minima, coefficient scaling, wide bounds, and randomized factored polynomials
- `stress-tests/geometry/PolygonUnion.cpp`: translated rectangles and multi-polygon unions; random polygons are checked for simplicity before comparison
- `stress-tests/strings/MinRotation.cpp`: all two-byte strings and randomized byte sequences, against unsigned string comparison
- `stress-tests/strings/Hashing.cpp`, `Hashing-codeforces.cpp`: all two-byte strings, single-byte encoding, and randomized byte sequences; compare whole-string, interval, and rolling-window hashes
- `stress-tests/various/SIMD.cpp`: signed extremes, vector/scalar boundaries, and random filtered dot products; skips non-x86/AVX2 hosts
- `stress-tests/graph/Reroot.cpp`: distance sums against BFS from every vertex; paths, stars, random trees, shuffled adjacency lists, and repeated runs with different traversal roots
- `stress-tests/numerical/Lagrange.cpp`: exhaustive small-field polynomials, randomized Horner comparisons, signed and large indices, and sums of powers
- `stress-tests/various/BigInt.cpp`: decimal-digit and native 128-bit oracles; parsing/formatting, modular arithmetic, small-integer multiplication/division/remainder, aliasing, carries/borrows, and long input
- `stress-tests/various/EulerTourTree.cpp`: naive forest comparisons, treap/tour invariants, edge recycling, large paths and stars

- `stress-tests/data-structures/LazySegmentTree.cpp` rewritten for `LazyUpdateTree`
- `stress-tests/graph/HLD.cpp` no longer calls `tree->set` (defaults are 0)
- `stress-tests/graph/Dinic2.cpp`: brute min-cut and Dinic comparisons, residual pairs and conservation, self-loops, reverse/parallel edges, flow limits, capacity edits, and 64-bit capacities
- `stress-tests/various/LinkCutTree.cpp` covers link/cut connectivity plus path sum/add, point set, and rooted LCA
- New stress tests: SparseLazySegmentTree, LiChao, MonotonicCHT, BinaryTrie, KnuthDP, XORBasis, RREF, QuadRoots, LinearSieve, Mobius, HalfplaneIntersection, Centroid, PersistentSegmentTree, FloorBlocks, OfflineDynamicConnectivity, StaticRangeQuery, MonotonicMap, Blossom, SegmentTreeBeats, DominatorTree, SteinerTree, PrimitiveRoot, Yen
- `stress-tests/strings/SuffixArray.cpp` also checks rank, `getLCP`, and `cmpSubstr`, including nonzero bytes through 255 and bounded substring comparisons

- Geometry stress tests use complex coordinates; `ComplexGeometry`, `Angle`, and `kdTree` cover primitives, integer precision, floating hulls, cross-header use, rotations, transformations, and nearest-neighbor queries.
- Geometry precision regressions cover translated area/centroid, small half-plane polygons, direction scaling, rotated duplicate boundaries, near-parallel lines, and antipodal spherical distances.
