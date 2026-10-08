#include "../utilities/template.h"
#define pb push_back
#define sc second
#include "../../content/graph/Yen.h"

using Graph = vector<vector<pair<int, ll>>>;
using Path = pair<ll, vi>;

vector<Path> brute(const Graph& g, int s, int t) {
	vector<Path> ans;
	vi used(sz(g)), path{s};
	used[s] = 1;
	auto dfs = [&](auto&& dfs, int v, ll cost) -> void {
		if (v == t) {
			ans.pb({cost, path});
			return;
		}
		for (auto [u, w] : g[v]) if (!used[u]) {
			used[u] = 1; path.pb(u);
			dfs(dfs, u, cost + w);
			used[u] = 0; path.pop_back();
		}
	};
	dfs(dfs, s, 0);
	sort(all(ans));
	return ans;
}

void validate(const Graph& g, int s, int t,
		const vector<Path>& ans) {
	set<vi> seen;
	ll last = -1;
	for (const auto& [cost, p] : ans) {
		assert(!p.empty() && p.front() == s && p.back() == t);
		assert(cost >= last);
		last = cost;
		assert(seen.insert(p).second);
		vi used(sz(g));
		ll sum = 0;
		rep(i,0,sz(p)) {
			assert(0 <= p[i] && p[i] < sz(g));
			assert(!used[p[i]]++);
			if (!i) continue;
			bool found = false;
			for (auto [u, w] : g[p[i-1]]) if (u == p[i]) {
				found = true; sum += w;
			}
			assert(found);
		}
		assert(sum == cost);
	}
}

void check(const Graph& g, int s, int t, int k) {
	auto want = brute(g, s, t), got = yen(g, s, t, k);
	assert(sz(got) == min(k, sz(want)));
	validate(g, s, t, got);
	rep(i,0,sz(got)) {
		assert(got[i].first == want[i].first);
		assert(binary_search(all(want), got[i]));
	}
}

int main() {
	check(Graph(1), 0, 0, 0);
	check(Graph(1), 0, 0, 5);
	check(Graph(3), 0, 2, 3);
	check(Graph{{{1, 5}}, {{2, 7}}, {}}, 0, 2, 5);
	check(Graph{{{1, 0}}, {{0, 0}, {2, 0}}, {{1, 0}}},
		0, 2, 5);
	// Large integer weights: the reference used double lookup.
	ll big = (1LL << 54) + 1;
	check(Graph{{{1, big}, {2, big + 1}}, {{3, big + 3}},
		{{3, big + 4}}, {}}, 0, 3, 5);
	ll near = (1LL << 60) / 4 - 1;
	check(Graph{{{1, near}, {2, 3 * near}}, {{2, near}}, {}},
		0, 2, 3);

	// All directed graphs on four vertices; zero weights and
	// cycles exercise prefix blocking and candidate persistence.
	vector<pii> edges;
	rep(u,0,4) rep(v,0,4) if (u != v) edges.pb({u, v});
	rep(mask,0,1 << sz(edges)) {
		Graph g(4);
		rep(i,0,sz(edges)) if (mask >> i & 1) {
			auto [u, v] = edges[i];
			g[u].pb({v, (u * 4 + v) % 3});
		}
		check(g, 0, 3, 2);
		check(g, 0, 3, 10); // includes path exhaustion
	}

	mt19937 rng(20261008);
	rep(it,0,2000) {
		int n = 1 + (int)(rng() % 8);
		Graph g(n);
		bool undirected = it % 2;
		rep(u,0,n) rep(v,0,n) {
			if (undirected && u > v) continue;
			if (rng() % 100 >= 40) continue;
			ll w = rng() % 8;
			if (it % 5 == 0) w += big;
			g[u].pb({v, w});
			if (undirected && u != v) g[v].pb({u, w});
		}
		int s = (int)(rng() % n), t = (int)(rng() % n);
		int k = (int)(rng() % 40);
		if (n <= 5 && it % 3 == 0) k = 100;
		check(g, s, t, k);
	}

	// Regional-sized dense graph, many tied paths. A path with
	// d edges costs d; counts are falling factorials of n-2.
	Graph dense(50);
	rep(u,0,50) rep(v,0,50) if (u != v)
		dense[u].pb({v, 1});
	auto ans = yen(dense, 0, 49, 200);
	assert(sz(ans) == 200);
	validate(dense, 0, 49, ans);
	rep(i,0,200) assert(ans[i].first == (i == 0 ? 1 :
		(i <= 48 ? 2 : 3)));

	cout << "Tests passed!" << endl;
}
