/**
 * Author: caterpillow, me
 * Date: 2026-08-20
 * License: CC0
 * Source: https://github.com/caterpillow/cactl Dynacon.h
 * Description: Offline dynamic connectivity. Times are
 *  operation indices. \texttt{toggle} adds or deletes an
 *  undirected edge. \texttt{addVal(v,x)} adds $x$ to vertex
 *  $v$. \texttt{query()} records a component-count;
 *  \texttt{query(v)} records that vertex's component sum.
 *  \texttt{ans} returns answers in order (as \texttt{ll}s).
 *  $q$ must be at least the number of ops. Pass initial
 *  vertex values as the optional third constructor arg.
 * Time: $O(Q\log Q\log N)$
 * Status: stress-tested, Library Checker
 *  dynamic\_graph\_vertex\_add\_component\_sum
 */
#pragma once

#include "UnionFindRollback.h"

struct DynCon {
	RollbackUF uf;
	vector<ll> sm;
	vector<vector<array<ll, 3>>> st;
	map<pii, int> eds;
	int n, q, t = 0;
	DynCon(int n, int q, const vector<ll>& a = {}) :
		uf(n), sm(n), n(n), q(1) {
		rep(i,0,n) sm[i] = i < sz(a) ? a[i] : 0;
		while (this->q < max(q, 1)) this->q *= 2;
		st.resize(2 * this->q);
	}
	void add(int l, int r, array<ll, 3> e) {
		for (l += q, r += q; l < r; l /= 2, r /= 2) {
			if (l & 1) st[l++].push_back(e);
			if (r & 1) st[--r].push_back(e);
		}
	}
	void toggle(int u, int v) {
		assert(t < q);
		if (u > v) swap(u, v);
		pii e{u, v};
		if (eds.count(e)) {
			add(eds[e], t, {u, v, 0});
			eds.erase(e);
		} else eds[e] = t;
		++t;
	}
	void addVal(int v, ll x) {
		assert(t < q);
		add(t, q, {-2, v, x});
		++t;
	}
	void query(int v = -1) {
		assert(t < q);
		st[q + t++].push_back({-1, v, 0});
	}
	void rec(int i, vector<ll>& res) {
		int t0 = uf.time();
		vector<pair<int, ll>> sav;
		auto save = [&](int r) { sav.push_back({r, sm[r]}); };
		for (auto [a, b, x] : st[i]) {
			if (a >= 0) {
				int ra = uf.find((int)a);
				int rb = uf.find((int)b);
				if (ra != rb) {
					save(ra); save(rb);
					uf.join(ra, rb);
					sm[uf.find(ra)] = sm[ra] + sm[rb];
				}
			} else if (a == -2) {
				int r = uf.find((int)b);
				save(r); sm[r] += x;
			}
		}
		for (auto [a, b, x] : st[i]) if (a == -1) {
			if (b < 0) res.push_back(n - uf.time() / 2);
			else res.push_back(sm[uf.find((int)b)]);
		}
		if (i < q) rec(2 * i, res), rec(2 * i + 1, res);
		uf.rollback(t0);
		while (sz(sav)) {
			sm[sav.back().first] = sav.back().second;
			sav.pop_back();
		}
	}
	vector<ll> ans() {
		for (auto [e, s] : eds) add(s, t, {e.first, e.second, 0});
		vector<ll> res;
		rec(1, res);
		return res;
	}
};
