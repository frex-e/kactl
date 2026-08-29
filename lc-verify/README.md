# Library Checker verification

Solvers that wrap this fork's additions/API deltas vs
[upstream KACTL](https://github.com/kth-competitive-programming/kactl) and
submit them to [Library Checker](https://judge.yosupo.jp/).

Submissions in this run used account [`inj`](https://judge.yosupo.jp/submissions?user=inj).
**24/24 accepted.**

```
python3 lc-verify/bundle.py lc-verify/problems/set_xor_min.cpp -o /tmp/a.cpp
python3 lc-verify/submit.py set_xor_min   # needs YOSUPO_EMAIL / YOSUPO_PASSWORD
```

Quoted includes are inlined against `content/`. Snippets that tell you to
change `V` / `U` / `binop` are pasted with those edits (same algorithm).

## Verdicts

| Problem | Snippet | Sub | Verdict |
|---|---|---|---|
| [general_matching](https://judge.yosupo.jp/problem/general_matching) | `Blossom.h` | [397517](https://judge.yosupo.jp/submission/397517) | AC |
| [dominatortree](https://judge.yosupo.jp/problem/dominatortree) | `DominatorTree.h` | [397513](https://judge.yosupo.jp/submission/397513) | AC |
| [minimum_steiner_tree](https://judge.yosupo.jp/problem/minimum_steiner_tree) | `SteinerTree.h` | [397523](https://judge.yosupo.jp/submission/397523) | AC |
| [set_xor_min](https://judge.yosupo.jp/problem/set_xor_min) | `BinaryTrie.h` | [397530](https://judge.yosupo.jp/submission/397530) | AC |
| [line_add_get_min](https://judge.yosupo.jp/problem/line_add_get_min) | `LiChao.h` | [397519](https://judge.yosupo.jp/submission/397519) | AC |
| [range_chmin_chmax_add_range_sum](https://judge.yosupo.jp/problem/range_chmin_chmax_add_range_sum) | `SegmentTreeBeats.h` | [397529](https://judge.yosupo.jp/submission/397529) | AC |
| [primitive_root](https://judge.yosupo.jp/problem/primitive_root) | `PrimitiveRoot.h` | [397527](https://judge.yosupo.jp/submission/397527) | AC |
| [factorize](https://judge.yosupo.jp/problem/factorize) | `Factor.h` (upstream; used by primitive root) | [397515](https://judge.yosupo.jp/submission/397515) | AC |
| [suffixarray](https://judge.yosupo.jp/problem/suffixarray) | `SuffixArray.h` | [397533](https://judge.yosupo.jp/submission/397533) | AC |
| [number_of_substrings](https://judge.yosupo.jp/problem/number_of_substrings) | `SuffixArray.h` LCP extras | [397524](https://judge.yosupo.jp/submission/397524) | AC |
| [enumerate_quotients](https://judge.yosupo.jp/problem/enumerate_quotients) | `FloorBlocks.h` | [397514](https://judge.yosupo.jp/submission/397514) | AC |
| [static_range_sum](https://judge.yosupo.jp/problem/static_range_sum) | `StaticRangeQuery.h` (`T=ll`) | [397531](https://judge.yosupo.jp/submission/397531) | AC |
| [staticrmq](https://judge.yosupo.jp/problem/staticrmq) | `StaticRangeQuery.h` (`min`) | [397532](https://judge.yosupo.jp/submission/397532) | AC |
| [min_plus_convolution_convex_convex](https://judge.yosupo.jp/problem/min_plus_convolution_convex_convex) | `MinPlusConvolution.h` | [397522](https://judge.yosupo.jp/submission/397522) | AC |
| [min_plus_convolution_convex_arbitrary](https://judge.yosupo.jp/problem/min_plus_convolution_convex_arbitrary) | `MinPlusConvolution.h` | [397521](https://judge.yosupo.jp/submission/397521) | AC |
| [min_plus_convolution_concave_arbitrary](https://judge.yosupo.jp/problem/min_plus_convolution_concave_arbitrary) | `MinPlusConvolution.h` | [397520](https://judge.yosupo.jp/submission/397520) | AC |
| [gcd_convolution](https://judge.yosupo.jp/problem/gcd_convolution) | `Mobius.h` | [397536](https://judge.yosupo.jp/submission/397536) | AC |
| [intersection_of_f2_vector_spaces](https://judge.yosupo.jp/problem/intersection_of_f2_vector_spaces) | `XORBasis.h` | [397518](https://judge.yosupo.jp/submission/397518) | AC |
| [range_add_range_min](https://judge.yosupo.jp/problem/range_add_range_min) | `LazySegmentTree.h` add+min, `V=ll` | [397528](https://judge.yosupo.jp/submission/397528) | AC |
| [point_set_range_composite](https://judge.yosupo.jp/problem/point_set_range_composite) | `LazySegmentTree.h` affine compose | [397525](https://judge.yosupo.jp/submission/397525) | AC |
| [point_set_range_composite_large_array](https://judge.yosupo.jp/problem/point_set_range_composite_large_array) | `SparseLazySegmentTree.h` | [397526](https://judge.yosupo.jp/submission/397526) | AC |
| [vertex_add_path_sum](https://judge.yosupo.jp/problem/vertex_add_path_sum) | `HLD.h` + sum tree | [397534](https://judge.yosupo.jp/submission/397534) | AC |
| [vertex_add_subtree_sum](https://judge.yosupo.jp/problem/vertex_add_subtree_sum) | `HLD.h` subtree | [397535](https://judge.yosupo.jp/submission/397535) | AC |
| [bipartitematching](https://judge.yosupo.jp/problem/bipartitematching) | `HopcroftKarp.h` | [397512](https://judge.yosupo.jp/submission/397512) | AC |

`gcd_convolution` first submission ([397516](https://judge.yosupo.jp/submission/397516)) was WA from a wrong multiples-zeta *in the wrapper* (descending loop double-counted). `Mobius.h` itself matched the stress test; the resubmit is AC.

## Not submitted

- `LinearSieve.h` — `SIEVE_N=1e7`, LC `enumerate_primes` has `N≤5e8`
- `OfflineDynamicConnectivity.h` — component count only; LC wants vertex-add component **sum**
- range affine **sum** — `applyUpdate` has no segment length
- `RREF.h` — doubles; LC is modular
- `QuadRoots.h`, `MonotonicMap.h`, `Centroid.h`, `HalfplaneIntersection.h`, `KnuthDP.h`, `Treap.h` extras, `Random.h`, `Output.h`, `MemoryUsage.h`, `Pragmas.h`
