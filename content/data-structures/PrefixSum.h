/**
 * Author: Indra Kusumah-Kasim
 * Date: 2026-10-06
 * License: CC0
 * Description: Static sums in any number of dimensions.
 *  Give each dimension's width and flat row-major values
 *  (last coordinate varies fastest). Queries use half-open
 *  boxes $[l_i,r_i)$; empty widths and boxes are allowed.
 *  Requires at least one dimension and valid bounds.
 * Usage:
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
	vi n;
	vector<size_t> stride;
	vector<T> p;
	PrefixSum(vi widths, vector<T> values)
		: n(widths), stride(sz(n) + 1, 1), p(move(values)) {
		assert(!n.empty());
		for (int d = sz(n); d--;) {
			assert(n[d] >= 0);
			stride[d] = stride[d+1] * (size_t)n[d];
		}
		assert(p.size() == stride[0]);
		rep(d,0,sz(n)) for (size_t i = 0; i < p.size(); ++i)
			if (i / stride[d+1] % (size_t)n[d])
				p[i] += p[i - stride[d+1]];
	}
	T sum(const vi& l, const vi& r) const {
		assert(sz(l) == sz(n) && sz(r) == sz(n));
		bool empty = false;
		rep(d,0,sz(n)) {
			assert(0 <= l[d] && l[d] <= r[d] && r[d] <= n[d]);
			empty |= l[d] == r[d];
		}
		return empty ? T{} : query(l, r, 0, 0);
	}
	T query(const vi& l, const vi& r, int d, size_t i) const {
		if (d == sz(n)) return p[i];
		size_t s = stride[d+1];
		T ans = query(l, r, d+1, i + (size_t)(r[d]-1)*s);
		if (l[d])
			ans -= query(l, r, d+1, i + (size_t)(l[d]-1)*s);
		return ans;
	}
};
