/**
 * Author: caterpillow
 * Date: 2026-08-20
 * License: CC0
 * Source: https://github.com/caterpillow/cactl
 *  StaticRangeQuery.h
 * Description: Static range queries for any associative
 *  operation. Change \texttt{T}, \texttt{unit} and
 *  \texttt{f}. Default is range sum. Bounds half-open
 *  $[l, r)$ like RMQ.
 * Time: $O(N\log N)$ build, $O(1)$ query
 * Status: stress-tested
 */
#pragma once

struct RangeQuery {
	typedef int T;
	static constexpr T unit = 0;
	T f(T a, T b) { return a + b; }
	vector<vector<T>> stor;
	vector<T> a;
	int n = 0;
	RangeQuery() {}
	RangeQuery(const vector<T>& v) { build(v); }
	void fill(int l, int r, int ind) {
		if (ind < 0) return;
		int m = (l + r) / 2;
		T prod = unit;
		rep(i,m,r) stor[i][ind] = prod = f(prod, a[i]);
		prod = unit;
		for (int i = m; i-- > l;)
			stor[i][ind] = prod = f(a[i], prod);
		fill(l, m, ind - 1);
		fill(m, r, ind - 1);
	}
	void build(const vector<T>& v) {
		int lg = 0, m = max(sz(v), 1);
		while ((1 << lg) < m) lg++;
		a.assign(1 << lg, unit);
		rep(i,0,sz(v)) a[i] = v[i];
		n = 1 << lg;
		stor.assign(n, vector<T>(max(lg, 1)));
		fill(0, n, lg - 1);
	}
	T query(int l, int r) { // [l, r)
		assert(l < r);
		if (l == --r) return a[l];
		int t = 31 - __builtin_clz(l ^ r);
		return f(stor[l][t], stor[r][t]);
	}
};
