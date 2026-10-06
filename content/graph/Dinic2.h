/**
 * Author: caterpillow, Claude
 * Date: 2026-09-10
 * License: CC0
 * Source: caterpillow/cactl, content/graph/Dinic2.h
 * https://github.com/caterpillow/cactl
 * Description: Dinic without capacity scaling: smaller edges,
 * one blocking-flow DFS per phase, and dead-end pruning.
 * Often faster than Dinic.h; scaling can win on some graphs.
 * Supports self-loops and parallel edges. DFS is recursive.
 * Requires nonnegative capacities, $s\ne t$, $lim\ge0$.
 * Residual sums and total flow must fit in \texttt{ll}.
 * \texttt{calc(s,t,lim)} returns at most lim additional units;
 * repeated calls continue from the current residual graph.
 * \texttt{leftOfMinCut} requires exhausted augmenting paths.
 * If stopped at lim, finish with \texttt{calc(s,t)}.
 * No original caps are stored. For edge e, signed net flow is
 * \texttt{adj[e.to][e.rev].c - original\_rcap}.
 * Usually rcap $=0$.
 * See chapter notes for capacity edits between calls.
 * Time: $O(V^2E)$; $O(\min(E^{1/2},V^{2/3})E)$ for unit caps;
 * $O(\sqrt{V}E)$ for bipartite matching.
 * Status: stress-tested
 */
#pragma once

struct Dinic2 {
	struct Edge { int to, rev; ll c; };
	vector<vector<Edge>> adj;
	vi lvl, ptr, q;
	Dinic2(int n) : adj(n), lvl(n), ptr(n), q(n) {}
	void addEdge(int a, int b, ll c, ll rcap = 0) {
		int i = sz(adj[a]), j = sz(adj[b]) + (a == b);
		adj[a].push_back({b, j, c});
		adj[b].push_back({a, i, rcap});
	}
	ll dfs(int v, int t, ll f) {
		if (v == t || !f) return f;
		ll g = 0;
		for (int& i = ptr[v]; i < sz(adj[v]); i++) {
			auto& [to, rev, c] = adj[v][i];
			if (lvl[to] != lvl[v] + 1 || !c) continue;
			ll w = min(f - g, c), p = dfs(to, t, w);
			if (p < w) lvl[to] = -1; // dead end this phase
			c -= p, adj[to][rev].c += p, g += p;
			if (g == f) break; // edge may still have room
		}
		return g;
	}
	ll calc(int s, int t, ll lim = LLONG_MAX) {
		assert(s != t && lim >= 0);
		ll flow = 0; q[0] = s;
		do {
			fill(all(lvl), 0), fill(all(ptr), 0);
			int qi = 0, qe = lvl[s] = 1;
			while (qi < qe && !lvl[t]) {
				int v = q[qi++];
				for (Edge e : adj[v]) if (!lvl[e.to] && e.c)
					q[qe++] = e.to, lvl[e.to] = lvl[v] + 1;
			}
			ll p = dfs(s, t, lim); flow += p, lim -= p;
		} while (lvl[t] && lim);
		return flow;
	}
	bool leftOfMinCut(int a) { return lvl[a] != 0; }
};
