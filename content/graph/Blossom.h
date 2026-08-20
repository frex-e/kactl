/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Source: https://github.com/caterpillow/cactl Blossom.h
 * Description: Maximum matching in general graphs.
 *  0-indexed. \texttt{ae} adds an undirected edge.
 *  After \texttt{solve()}, \texttt{mate[u]} is the partner
 *  of $u$, or $-1$ if unmatched. Returns matching size.
 * Time: $O(NM)$
 * Status: stress-tested
 */
#pragma once

struct Blossom {
	int n, h, t, cnt;
	vector<pii> edges;
	vi vis, q, mt, col, fa, pre, he, mate;
	int get(int u) {
		return fa[u] == u ? u : fa[u] = get(fa[u]);
	}
	Blossom(int n) : n(n), cnt(0) {
		vis = q = mt = col = fa = pre = he = vi(n + 1);
		edges.push_back({0, 0});
	}
	void ae(int u, int v) {
		if (u == v) return;
		++u, ++v;
		edges.push_back({he[u], v}); he[u] = sz(edges) - 1;
		edges.push_back({he[v], u}); he[v] = sz(edges) - 1;
	}
	void aug(int u, int v) {
		for (int p; u; u = p, v = pre[p])
			p = mt[v], mt[mt[u] = v] = u;
	}
	int lca(int u, int v) {
		for (cnt++;; u = pre[mt[u]]) {
			if (v) swap(u, v);
			if (vis[u = get(u)] == cnt) return u;
			vis[u] = cnt;
		}
	}
	void blo(int u, int v, int f) {
		for (int p; get(u) != f; v = p, u = pre[p]) {
			p = mt[u]; pre[u] = v; fa[u] = fa[p] = f;
			if (col[p] != 1) col[q[++t] = p] = 1;
		}
	}
	bool bfs(int u) {
		rep(i,1,n+1) col[i] = 0, fa[i] = i;
		h = 0; q[t = 1] = u; col[u] = 1;
		while (h != t) {
			int x = q[++h];
			for (int i = he[x]; i; i = edges[i].first) {
				int y = edges[i].second;
				if (!col[y]) {
					if (!mt[y]) { aug(y, x); return 1; }
					pre[y] = x;
					col[y] = 2;
					col[q[++t] = mt[y]] = 1;
				} else if (col[y] == 1 && get(x) != get(y)) {
					int p = lca(x, y);
					blo(x, y, p);
					blo(y, x, p);
				}
			}
		}
		return 0;
	}
	int solve() {
		int ans = 0;
		rep(i,1,n+1) if (!mt[i]) ans += bfs(i);
		mate.assign(n, -1);
		rep(i,1,n+1) if (mt[i]) mate[i - 1] = mt[i] - 1;
		return ans;
	}
};
