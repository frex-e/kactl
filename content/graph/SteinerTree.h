/**
 * Author: Misuki743 (adapted)
 * Date: 2026-08-27
 * License: CC0
 * Source: https://github.com/yosupo06/library-checker-problems
 *  graph/minimum\_steiner\_tree/sol/correct.cpp
 *  (Library Checker minimum\_steiner\_tree)
 * Description: Minimum Steiner tree in an undirected graph
 * with nonnegative weights. \texttt{e} is
 * \texttt{\{u,v,w\}} edges; \texttt{s} is the terminals.
 * Returns (min cost, indices into \texttt{e}). Returns
 * $\{-1,\{\}\}$ if the terminals cannot be connected.
 * $k=0$ costs 0. If $k$ is large and $n$ is small,
 * enumerate extra vertices and MST instead.
 * Time: $O(3^k n + 2^k(n\log n+m))$
 * Memory: $O(2^k n)$
 * Status: stress-tested
 */
#pragma once

const ll inf = 1LL << 60;
pair<ll, vi> steinerTree(int n, vi s, vector<array<ll, 3>> e) {
	vi in(n), f(n);
	for (int x : s) in[x] = 1;
	int k = 0, nxt = 0;
	rep(v,0,n) k += in[v];
	if (!k) return {0, {}};
	rep(z,0,2) rep(v,0,n) if (in[v] == 1 - z) f[v] = nxt++;
	for (auto& a : e) a[0] = f[(int)a[0]], a[1] = f[(int)a[1]];
	struct E { int to, id; ll w; };
	vector<vector<E>> g(n);
	rep(i,0,sz(e)) {
		int u = (int)e[i][0], v = (int)e[i][1];
		g[u].pb({v, i, e[i][2]});
		g[v].pb({u, i, e[i][2]});
	}
	int N = 1 << k;
	vector<vector<ll>> dp(N, vector<ll>(n, inf));
	vector<vector<pii>> pre(N, vector<pii>(n, {-1,-1}));
	typedef pair<ll, int> pli;
	rep(x,1,N) {
		if ((x & -x) == x) {
			int i = 0;
			while ((1 << i) < x) i++;
			dp[x][i] = 0;
		} else {
			rep(r,0,n)
			for (int y = (x - 1) & x; y; y = (y - 1) & x) {
				ll tmp = dp[y][r] + dp[x ^ y][r];
				if (tmp < dp[x][r])
					dp[x][r] = tmp, pre[x][r] = {y, -1};
			}
		}
		priority_queue<pli, vector<pli>, greater<pli>> pq;
		rep(r,0,n) if (dp[x][r] < inf) pq.push({dp[x][r], r});
		while (!pq.empty()) {
			auto [d, v] = pq.top(); pq.pop();
			if (d != dp[x][v]) continue;
			for (auto [to, id, w] : g[v])
				if (d + w < dp[x][to]) {
					dp[x][to] = d + w, pre[x][to] = {v, id};
					pq.push({d + w, to});
				}
		}
	}
	vi use(sz(e));
	auto rec = [&](auto rec, int x, int r) -> void {
		auto [a, b] = pre[x][r];
		if (a < 0) return;
		if (b < 0) rec(rec, a, r), rec(rec, x ^ a, r);
		else use[b] = 1, rec(rec, x, a);
	};
	int full = N - 1, best = 0;
	rep(r,0,n) if (dp[full][r] < dp[full][best]) best = r;
	if (dp[full][best] >= inf) return {-1, {}};
	rec(rec, full, best);
	vi t;
	rep(i,0,sz(e)) if (use[i]) t.pb(i);
	return {dp[full][best], t};
}
