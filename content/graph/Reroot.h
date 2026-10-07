/**
 * Author: me
 * Date: 2026-10-07
 * License: CC0
 * Source: folklore
 * Description: DP for every root of an undirected tree
 *  on vertices $0$ through $N-1$. Modify T and the hooks.
 *  merge is associative with identity id; preserves
 *  adjacency order. vertex(a,v) includes v as the root
 *  above the merged branches a. edge(a,from,to) turns
 *  the component rooted at from into a branch of to,
 *  accounting for the edge but excluding to itself.
 *  Prefix/suffix merges exclude each child's branch.
 * Usage:
 *  Reroot dp(g); dp.run(root); root may be any vertex.
 *  As written, ans[v].sum is the sum of distances from v.
 *  T = (count, distance sum). vertex adds v at distance
 *  zero; edge measures the same vertices from to, so
 *  every distance increases by one: sum += cnt.
 *  For leaf u next to v: vertex(id,u) = (1,0),
 *  edge((1,0),u,v) = (1,1), vertex((1,1),v) = (2,1).
 * Time: $O(N)$ with constant-size T and $O(1)$ hooks.
 * Memory: $O(N)$; recursion depth can be $N$.
 * Status: stress-tested
 */
#pragma once

struct Reroot {
	struct T { ll cnt, sum; };
	T id{0, 0};
	vector<vi> g;
	vector<T> down, ans;
	Reroot(vector<vi> g)
		: g(g), down(sz(g)), ans(sz(g)) {}

	// Modify these hooks for the problem:
	T merge(T a, T b) {
		return {a.cnt + b.cnt, a.sum + b.sum};
	}
	// Include v above the branches a, at distance zero.
	T vertex(T a, int v) { ++a.cnt; return a; }
	// Measure from's component from to; to stays excluded.
	T edge(T a, int from, int to) {
		a.sum += a.cnt;
		return a;
	}

	void dfs1(int v, int p) {
		T cur = id;
		for (int u : g[v]) if (u != p) {
			dfs1(u, v);
			cur = merge(cur, edge(down[u], u, v));
		}
		down[v] = vertex(cur, v);
	}
	void dfs2(int v, int p, T up) {
		int m = sz(g[v]);
		vector<T> val(m), pre(m + 1, id);
		rep(i, 0, m) {
			int u = g[v][i];
			val[i] = u == p ? up : edge(down[u], u, v);
			pre[i + 1] = merge(pre[i], val[i]);
		}
		ans[v] = vertex(pre[m], v);
		T suf = id;
		for (int i = m; i--;) {
			int u = g[v][i];
			if (u != p) {
				T cur = vertex(merge(pre[i], suf), v);
				dfs2(u, v, edge(cur, v, u));
			}
			suf = merge(val[i], suf);
		}
	}
	void run(int root = 0) {
		if (g.empty()) return;
		dfs1(root, -1);
		dfs2(root, -1, id);
	}
};
