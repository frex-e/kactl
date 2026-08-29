# Library Checker verification

Solvers that wrap this fork's additions/API deltas vs
[upstream KACTL](https://github.com/kth-competitive-programming/kactl) and
submit them to [Library Checker](https://judge.yosupo.jp/).

Account used for this run: `inj` (`shadeditor@gmail.com`).

```
python3 lc-verify/bundle.py lc-verify/problems/set_xor_min.cpp -o /tmp/a.cpp
python3 lc-verify/submit.py set_xor_min   # needs YOSUPO_EMAIL / YOSUPO_PASSWORD
```

Quoted includes are inlined against `content/`. Snippets that tell you to
change `V` / `U` / `binop` are pasted with those edits (same algorithm).

| Problem | Snippet |
|---|---|
| `general_matching` | `Blossom.h` |
| `dominatortree` | `DominatorTree.h` |
| `minimum_steiner_tree` | `SteinerTree.h` |
| `set_xor_min` | `BinaryTrie.h` |
| `line_add_get_min` | `LiChao.h` |
| `range_chmin_chmax_add_range_sum` | `SegmentTreeBeats.h` |
| `primitive_root` | `PrimitiveRoot.h` |
| `factorize` | `Factor.h` (upstream; used by primitive root) |
| `suffixarray` | `SuffixArray.h` |
| `number_of_substrings` | `SuffixArray.h` LCP extras |
| `enumerate_quotients` | `FloorBlocks.h` |
| `static_range_sum` | `StaticRangeQuery.h` (`T=ll`) |
| `staticrmq` | `StaticRangeQuery.h` (`min`) |
| `min_plus_convolution_*` | `MinPlusConvolution.h` |
| `gcd_convolution` | `Mobius.h` |
| `intersection_of_f2_vector_spaces` | `XORBasis.h` `inSpan` + F2 intersection |
| `range_add_range_min` | `LazySegmentTree.h` add+min, `V=ll` |
| `point_set_range_composite` | `LazySegmentTree.h` affine compose |
| `point_set_range_composite_large_array` | `SparseLazySegmentTree.h` |
| `vertex_add_path_sum` | `HLD.h` + sum tree |
| `vertex_add_subtree_sum` | `HLD.h` subtree |
| `bipartitematching` | `HopcroftKarp.h` (header claims LC-tested) |

Not submitted (no matching LC problem, or API/limit mismatch):

- `LinearSieve.h` — `SIEVE_N=1e7`, LC `enumerate_primes` has `N≤5e8`
- `OfflineDynamicConnectivity.h` — component count only; LC wants vertex-add component **sum**
- range affine **sum** — `applyUpdate` has no segment length
- `RREF.h` — doubles; LC is modular
- `QuadRoots.h`, `MonotonicMap.h`, `Centroid.h`, `HalfplaneIntersection.h`, `KnuthDP.h`, `Treap.h` extras, `Random.h`, `Output.h`, `MemoryUsage.h`, `Pragmas.h`
