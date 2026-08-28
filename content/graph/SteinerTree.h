/**
 * Author: Misuki743 (adapted)
 * Date: 2026-08-28
 * License: CC0
 * Source: https://github.com/yosupo06/library-checker-problems
 *  graph/minimum\_steiner\_tree/sol/correct.cpp
 *  (Library Checker minimum\_steiner\_tree)
 * Description: Minimum Steiner tree in an undirected graph
 * with nonnegative weights. \texttt{e} is
 * \texttt{\{u,v,w\}} edges; \texttt{s} is the terminals.
 * Returns (min cost, indices into \texttt{e}). Tree
 * reconstruction is optional. Returns $\{-1,\{\}\}$ if
 * the terminals cannot be connected. $k=0$ costs 0.
 * If $k$ is large and $n$ is small, enumerate extra
 * vertices and MST instead.
 * Time: $O(3^k n + 2^k(n\log n+m))$
 * Memory: $O(2^k n)$
 * Status: stress-tested
 */
#pragma once

const ll inf = 1LL << 60;
pair<ll, vi> steinerTree(int n, vi s, vector<array<ll, 3>> e) {
	int k = sz(s), N = 1 << k;
	if (!k) return {0, {}};
	struct E { int to, id; ll w; };
	vector<vector<E>> g(n);
	rep(i,0,sz(e)) {
		int u = (int)e[i][0], v = (int)e[i][1];
		g[u].pb({v, i, e[i][2]});
		g[v].pb({u, i, e[i][2]});
	}
	vector<ll> dp(N * n, inf);
	vector<pii> pre(N * n, {-1,-1}); // (optional)
	rep(i,0,k) dp[(1 << i) * n + s[i]] = 0;
	typedef pair<ll, int> pli;
	rep(x,1,N) {
		int xn = x * n;
		for (int y = (x - 1) & x; y; y = (y - 1) & x) {
			int yn = y * n, zn = (x ^ y) * n;
			rep(r,0,n) {
				ll tmp = dp[yn + r] + dp[zn + r];
				if (tmp < dp[xn + r]) {
					dp[xn + r] = tmp;
					pre[xn + r] = {y, -1}; // (optional)
				}
			}
		}
		priority_queue<pli, vector<pli>, greater<pli>> pq;
		rep(r,0,n) if (dp[xn + r] < inf)
			pq.push({dp[xn + r], r});
		while (!pq.empty()) {
			auto [d, v] = pq.top(); pq.pop();
			if (d != dp[xn + v]) continue;
			for (auto [to, id, w] : g[v])
				if (d + w < dp[xn + to]) {
					dp[xn + to] = d + w;
					pre[xn + to] = {v, id}; // (optional)
					pq.push({d + w, to});
				}
		}
	}
	int fn = (N - 1) * n, best = 0;
	rep(r,0,n) if (dp[fn + r] < dp[fn + best]) best = r;
	if (dp[fn + best] >= inf) return {-1, {}};
	vi t; // reconstruct (optional)
	vi use(sz(e));
	auto rec = [&](auto rec, int x, int r) -> void {
		auto [a, b] = pre[x * n + r];
		if (a < 0) return;
		if (b < 0) rec(rec, a, r), rec(rec, x ^ a, r);
		else use[b] = 1, rec(rec, x, a);
	};
	rec(rec, N - 1, best);
	rep(i,0,sz(e)) if (use[i]) t.pb(i);
	return {dp[fn + best], t};
}
