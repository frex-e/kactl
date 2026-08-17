/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: folklore
 * Description: Centroid decomposition. Call
 *  \texttt{decomp(0, f)}. When \texttt{f(c)} runs, $c$ is
 *  the centroid of its current component and ancestor
 *  centroids are in \texttt{banned}. DFS the original
 *  tree skipping \texttt{banned} to see $c$'s component.
 *  Each node is passed to \texttt{f} once.
 * Time: $O(N\log N)$
 * Status: stress-tested
 */
#pragma once

struct Centroid {
	vector<vi> g;
	vector<bool> banned;
	vi siz;
	Centroid(int n) : g(n), banned(n), siz(n) {}
	void ae(int a, int b) {
		g[a].push_back(b);
		g[b].push_back(a);
	}
	int dfsSz(int u, int p) {
		siz[u] = 1;
		for (int v : g[u]) if (v != p && !banned[v])
			siz[u] += dfsSz(v, u);
		return siz[u];
	}
	int dfsCen(int u, int p, int n) {
		for (int v : g[u]) if (v != p && !banned[v])
			if (siz[v] > n / 2) return dfsCen(v, u, n);
		return u;
	}
	template<class F>
	void decomp(int u, F f) {
		int c = dfsCen(u, -1, dfsSz(u, -1));
		f(c);
		banned[c] = true;
		for (int v : g[c]) if (!banned[v]) decomp(v, f);
	}
};
