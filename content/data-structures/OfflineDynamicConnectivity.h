/**
 * Author: caterpillow, me
 * Date: 2026-08-31
 * License: CC0
 * Source: https://github.com/caterpillow/cactl Dynacon.h
 * Description: Offline dynamic connectivity. Times are
 *  query indices. \texttt{toggle} adds or deletes an
 *  undirected edge. \texttt{query} records a component-count
 *  query. \texttt{ans} returns answers in order.
 *  $q$ must be at least the number of \texttt{query} calls.
 * Time: $O(Q\log Q\log N)$
 * Status: stress-tested
 */
#pragma once

#include "UnionFindRollback.h"

struct DynCon {
	RollbackUF uf;
	vector<vector<pii>> st;
	map<pii, int> eds;
	int n, q, t = 0;
	DynCon(int n, int q) : uf(n), n(n), q(1) {
		while (this->q < max(q, 1)) this->q *= 2;
		st.resize(2 * this->q);
	}
	void add(int l, int r, pii e) {
		for (l += q, r += q; l < r; l /= 2, r /= 2) {
			if (l & 1) st[l++].push_back(e);
			if (r & 1) st[--r].push_back(e);
		}
	}
	void toggle(int u, int v) {
		if (u > v) swap(u, v);
		pii e{u, v};
		if (eds.count(e)) {
			add(eds[e], t, e);
			eds.erase(e);
		} else eds[e] = t;
	}
	void query() {
		assert(t < q);
		st[q + t++].push_back({-1, -1});
	}
	void rec(int i, vi& res) {
		int t0 = uf.time();
		for (auto [a, b] : st[i]) if (a >= 0) uf.join(a, b);
		for (auto [a, b] : st[i])
			if (a < 0) res.push_back(n - uf.time() / 2);
		if (i < q) rec(2 * i, res), rec(2 * i + 1, res);
		uf.rollback(t0);
	}
	vi ans() {
		for (auto [e, s] : eds) add(s, t, e);
		vi res;
		rec(1, res);
		return res;
	}
};
