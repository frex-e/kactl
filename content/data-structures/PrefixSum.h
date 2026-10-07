/**
 * Author: Indra Kusumah-Kasim
 * Date: 2026-10-06
 * License: CC0
 * Description: Static sums in any number of dimensions.
 *  Construct with each dimension's width and flat row-major
 *  values (last coordinate varies fastest). T is the sum type.
 *  \texttt{sum(l, r)} sums cells in
 *  $[l_0,r_0) \times [l_1,r_1) \times \cdots \times
 *  [l_{d-1},r_{d-1})$. Each lower bound is included and each
 *  upper bound is excluded. Both vectors have $d$ coordinates,
 *  with $0 \le l_i \le r_i \le w_i$. Empty boxes return zero.
 *  Requires at least one dimension, nonnegative widths, and
 *  exactly $\prod_i w_i$ values. All suffix products must fit
 *  in int; T must hold all intermediate sums.
 * Usage:
 *  // Matrix {{1, 2, 3}, {4, 5, 6}}, flattened by row:
 *  PrefixSum<ll> p({2, 3}, {1, 2, 3, 4, 5, 6});
 *  p.sum({0, 1}, {2, 3}); // 2 + 3 + 5 + 6 = 16
 * Time: $O(dN)$ build, $O(2^d)$ query, where
 *  $N=\prod_i w_i$ and $d$ is the number of dimensions.
 * Memory: $O(N)$ prefix table.
 * Status: stress-tested
 */
#pragma once

template<class T>
struct PrefixSum {
	vi n, stride;
	vector<T> p;
	PrefixSum(vi widths, vector<T> values)
		: n(widths), stride(sz(n) + 1, 1), p(move(values)) {
		for (int d = sz(n); d--;)
			stride[d] = stride[d+1] * n[d];
		rep(d,0,sz(n)) rep(i,0,sz(p))
			if (i / stride[d+1] % n[d])
				p[i] += p[i - stride[d+1]];
	}
	T sum(const vi& l, const vi& r) const {
		rep(d,0,sz(n)) if (l[d] == r[d]) return T{};
		return query(l, r, 0, 0);
	}
	T query(const vi& l, const vi& r, int d, int i) const {
		if (d == sz(n)) return p[i];
		int s = stride[d+1];
		T ans = query(l, r, d+1, i + (r[d]-1)*s);
		if (l[d])
			ans -= query(l, r, d+1, i + (l[d]-1)*s);
		return ans;
	}
};
