/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: folklore (D\&C on time + rollback DSU)
 * Description: Offline dynamic connectivity. Times are
 *  $[0, T)$. \texttt{add(l, r, a, b)} inserts undirected
 *  edge $a$--$b$ on time interval $[l, r)$.
 *  \texttt{run(f)} calls \texttt{f(t, uf)} at each time
 *  $t$ with the DSU of edges alive at $t$.
 * Time: $O(K\log K\log N)$ for $K$ interval-edges
 * Status: stress-tested
 */
#pragma once

#include "UnionFindRollback.h"

struct DynCon {
	RollbackUF uf;
	vector<vector<pii>> st;
	int T;
	DynCon(int n, int t) : uf(n), st(4 * max(t, 1)), T(t) {}
	void add(int l, int r, int a, int b,
			int i = 1, int L = 0, int R = -1) {
		if (R < 0) R = T;
		if (l >= R || r <= L) return;
		if (l <= L && R <= r) {
			st[i].push_back({a, b});
			return;
		}
		int m = (L + R) / 2;
		add(l, r, a, b, 2 * i, L, m);
		add(l, r, a, b, 2 * i + 1, m, R);
	}
	template<class F>
	void rec(int i, int L, int R, F& f) {
		int t0 = uf.time();
		for (auto [a, b] : st[i]) uf.join(a, b);
		if (L + 1 == R) f(L, uf);
		else {
			int m = (L + R) / 2;
			rec(2 * i, L, m, f);
			rec(2 * i + 1, m, R, f);
		}
		uf.rollback(t0);
	}
	template<class F>
	void run(F f) { rec(1, 0, T, f); }
};
