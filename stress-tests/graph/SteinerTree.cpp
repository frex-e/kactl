#include "../utilities/template.h"
#include "../utilities/random.h"
#include "../utilities/genGraph.h"
#include "../utilities/genTree.h"

#define pb push_back
#include "../../content/graph/SteinerTree.h"
#include "../../content/data-structures/UnionFind.h"

typedef vector<vector<pair<int, ll>>> Graph;

Graph fromEdges(int n, const vector<array<ll, 3>>& ed) {
	Graph g(n);
	for (auto [u, v, w] : ed) {
		g[(int)u].pb({(int)v, w});
		g[(int)v].pb({(int)u, w});
	}
	return g;
}

ll edgeW(const Graph& g, int u, int v) {
	ll w = inf;
	for (auto [x, wt] : g[u]) if (x == v) w = min(w, wt);
	return w;
}

ll bruteMST(int n, const vector<array<ll, 3>>& ed, vi t) {
	if (t.empty()) return 0;
	vi isT(n);
	for (int x : t) isT[x] = 1;
	vi other;
	rep(i,0,n) if (!isT[i]) other.pb(i);
	int o = sz(other);
	ll ans = inf;
	vector<array<ll, 3>> sed = ed;
	sort(all(sed), [](auto a, auto b) { return a[2] < b[2]; });
	rep(mask,0,1 << o) {
		vi used(n);
		for (int x : t) used[x] = 1;
		int extra = 0;
		rep(i,0,o) if (mask >> i & 1) used[other[i]] = 1, extra++;
		UF uf(n);
		ll cost = 0;
		int joins = 0;
		for (auto [u, v, w] : sed) {
			int a = (int)u, b = (int)v;
			if (!used[a] || !used[b]) continue;
			if (uf.join(a, b)) cost += w, joins++;
		}
		if (joins == sz(t) + extra - 1) ans = min(ans, cost);
	}
	return ans >= inf ? -1 : ans;
}

ll apspSteiner(const Graph& g, vi t) {
	int n = sz(g), k = sz(t);
	if (!k) return 0;
	vector<vector<ll>> dist(n, vector<ll>(n, inf));
	rep(i,0,n) dist[i][i] = 0;
	rep(u,0,n) for (auto [v, w] : g[u])
		dist[u][v] = min(dist[u][v], w);
	rep(kk,0,n) rep(i,0,n) if (dist[i][kk] < inf)
		rep(j,0,n) if (dist[kk][j] < inf)
			dist[i][j] = min(dist[i][j], dist[i][kk] + dist[kk][j]);
	int N = 1 << k;
	vector<vector<ll>> dp(N, vector<ll>(n, inf));
	rep(i,0,k) rep(v,0,n) dp[1 << i][v] = dist[t[i]][v];
	rep(m,1,N) {
		if (__builtin_popcount(m) < 2) continue;
		vector<ll> tmp(n, inf);
		for (int s = m; s; s = (s - 1) & m) if (s < (m ^ s))
			rep(v,0,n) {
				ll a = dp[s][v], b = dp[m ^ s][v];
				if (a < inf && b < inf) tmp[v] = min(tmp[v], a + b);
			}
		rep(v,0,n) rep(u,0,n)
			if (tmp[u] < inf && dist[u][v] < inf)
				dp[m][v] = min(dp[m][v], tmp[u] + dist[u][v]);
	}
	ll ans = dp[N - 1][t[0]];
	return ans >= inf ? -1 : ans;
}

ll dijkstra(const Graph& g, int s, int t) {
	int n = sz(g);
	vector<ll> d(n, inf);
	d[s] = 0;
	priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<>> q;
	q.push({0, s});
	while (!q.empty()) {
		auto [cd, v] = q.top(); q.pop();
		if (cd != d[v]) continue;
		for (auto [u, w] : g[v]) if (cd + w < d[u]) {
			d[u] = cd + w;
			q.push({d[u], u});
		}
	}
	return d[t] >= inf ? -1 : d[t];
}

ll treeSteiner(int n, const vector<pii>& edges,
		const vector<ll>& wt, vi t) {
	if (t.empty()) return 0;
	vector<vector<pii>> adj(n);
	rep(i,0,sz(edges)) {
		auto [a, b] = edges[i];
		adj[a].pb({b, i});
		adj[b].pb({a, i});
	}
	vi want(n);
	for (int x : t) want[x] = 1;
	ll cost = 0;
	function<int(int,int)> dfs = [&](int v, int p) {
		int has = want[v];
		for (auto [u, i] : adj[v]) if (u != p) {
			int ch = dfs(u, v);
			if (ch) cost += wt[i];
			has |= ch;
		}
		return has;
	};
	dfs(t[0], -1);
	return cost;
}

void checkRecon(const Graph& g, const vi& t, ll cost,
		const vector<pii>& ed) {
	if (cost < 0) {
		assert(ed.empty());
		return;
	}
	int n = sz(g);
	UF uf(n);
	set<pii> used;
	ll sum = 0;
	for (auto [u, v] : ed) {
		assert(0 <= u && u < n && 0 <= v && v < n);
		assert(u != v);
		ll w = min(edgeW(g, u, v), edgeW(g, v, u));
		assert(w < inf);
		pii e = minmax(u, v);
		if (used.insert(e).second) {
			sum += w;
			assert(uf.join(u, v));
		} else {
			assert(w == 0);
		}
	}
	assert(sum == cost);
	if (!t.empty()) {
		int r = uf.find(t[0]);
		for (int x : t) assert(uf.find(x) == r);
	}
}

void check(int n, const vector<array<ll, 3>>& ed, vi t) {
	Graph g = fromEdges(n, ed);
	auto [cost, rec] = steinerTree(g, t);
	checkRecon(g, t, cost, rec);
	if (n <= 8) assert(cost == bruteMST(n, ed, t));
	if (n <= 12 && sz(t) <= 8) assert(cost == apspSteiner(g, t));
	if (sz(t) == 2) assert(cost == dijkstra(g, t[0], t[1]));
	if (sz(t) == 1 || t.empty()) assert(cost == 0);
}

int main() {
	// k = 0
	check(3, {{0,1,1},{1,2,1}}, {});
	// k = 1
	check(4, {{0,1,5},{1,2,5},{2,3,5}}, {2});
	// path
	check(4, {{0,1,1},{1,2,2},{2,3,3}}, {0, 3});
	// star: Steiner vertex is cheaper than leaf MST
	check(4, {{0,1,1},{0,2,1},{0,3,1},{1,2,10},{2,3,10},{3,1,10}},
		{1,2,3});
	// disconnected terminals
	check(3, {{0,1,1}}, {0, 2});
	// two nodes
	check(2, {{0,1,7}}, {0, 1});
	check(1, {}, {0});

	rep(it,0,200) {
		int n = randIncl(1, 8);
		int m = n == 1 ? 0 : randIncl(0, n * (n - 1) / 2);
		auto el = randomSimpleGraphAsEdgeList(n, m);
		vector<array<ll, 3>> ed;
		for (auto [a, b] : el)
			ed.push_back({a, b, (ll)randIncl(0, 20)});
		int k = randIncl(0, n);
		vi t(n); iota(all(t), 0);
		shuffle_vec(t);
		t.resize(k);
		check(n, ed, t);
	}

	rep(it,0,80) {
		int n = randIncl(1, 12);
		int m = n == 1 ? 0 : randIncl(0, min(n * (n - 1) / 2, 30));
		auto el = randomSimpleGraphAsEdgeList(n, m);
		vector<array<ll, 3>> ed;
		for (auto [a, b] : el)
			ed.push_back({a, b, (ll)randIncl(1, 15)});
		int k = randIncl(0, min(n, 6));
		vi t(n); iota(all(t), 0);
		shuffle_vec(t);
		t.resize(k);
		check(n, ed, t);
	}

	rep(it,0,100) {
		int n = randIncl(2, 25);
		auto el = genRandomTree(n);
		vector<ll> wt(sz(el));
		vector<array<ll, 3>> ed;
		rep(i,0,sz(el)) {
			wt[i] = randIncl(1, 20);
			ed.push_back({el[i].first, el[i].second, wt[i]});
		}
		int k = randIncl(1, min(n, 10));
		vi t(n); iota(all(t), 0);
		shuffle_vec(t);
		t.resize(k);
		Graph g = fromEdges(n, ed);
		auto [cost, rec] = steinerTree(g, t);
		checkRecon(g, t, cost, rec);
		assert(cost == treeSteiner(n, el, wt, t));
	}

	rep(it,0,80) {
		int n = randIncl(2, 30);
		int m = randIncl(n - 1, n + 20);
		m = min(m, n * (n - 1) / 2);
		auto el = randomSimpleGraphAsEdgeList(n, m);
		vector<array<ll, 3>> ed;
		for (auto [a, b] : el)
			ed.push_back({a, b, (ll)randIncl(0, 50)});
		int a = randRange(n), b = randRange(n);
		while (b == a && n > 1) b = randRange(n);
		check(n, ed, {a, b});
	}

	cout << "Tests passed!" << endl;
}
