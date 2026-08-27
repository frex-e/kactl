/**
 * Author: me
 * Date: 2026-08-27
 * License: CC0
 * Source: Dreyfus--Wagner; Dijkstra variant folklore
 * Description: Minimum Steiner tree in an undirected graph
 * with nonnegative weights. \texttt{g[u]} holds
 * \texttt{\{v,w\}} pairs (both directions). Returns
 * (min cost, tree edges as $u$--$v$ pairs). Returns
 * $\{-1,\{\}\}$ if the terminals cannot be connected.
 * $k=0$ costs 0. If $k$ is large and $n$ is small,
 * enumerate extra vertices and MST instead.
 * Time: $O(3^k n + 2^k(n\log n+m))$
 * Memory: $O(2^k n)$
 * Status: stress-tested
 */
#pragma once

const ll inf = 1LL << 60;
pair<ll, vector<pii>> steinerTree(
	vector<vector<pair<int, ll>>>& g, vi t) {
	int n = sz(g), k = sz(t);
	if (!k) return {0, {}};
	int N = 1 << k;
	vector<vector<ll>> dp(N, vector<ll>(n, inf));
	vector<vector<pii>> pr(N, vector<pii>(n, {-1,-1}));
	rep(i,0,k) dp[1 << i][t[i]] = 0;
	rep(m,1,N) {
		for (int s = m; s; s = (s - 1) & m) if (s < (m ^ s))
			rep(v,0,n) {
				ll a = dp[s][v], b = dp[m ^ s][v];
				if (a < inf && b < inf && a + b < dp[m][v])
					dp[m][v] = a + b, pr[m][v] = {s, -1};
			}
		typedef pair<ll, int> pli;
		priority_queue<pli, vector<pli>, greater<pli>> q;
		rep(v,0,n) if (dp[m][v] < inf) q.push({dp[m][v], v});
		while (!q.empty()) {
			auto [d, v] = q.top(); q.pop();
			if (d != dp[m][v]) continue;
			for (auto [u, w] : g[v]) if (d + w < dp[m][u]) {
				dp[m][u] = d + w, pr[m][u] = {v, -2};
				q.push({dp[m][u], u});
			}
		}
	}
	int full = N - 1, root = t[0];
	if (dp[full][root] >= inf) return {-1, {}};
	vector<pii> ed, st = {{full, root}};
	while (!st.empty()) {
		auto [m, v] = st.back(); st.pop_back();
		auto [a, b] = pr[m][v];
		if (a < 0) continue;
		if (b == -1) st.pb({a, v}), st.pb({m ^ a, v});
		else ed.pb({a, v}), st.pb({m, a});
	}
	return {dp[full][root], ed};
}
