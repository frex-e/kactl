#include "../utilities/template.h"
#include "../../content/graph/Reroot.h"

mt19937 rng(42);

void check(vector<vi> g) {
	int n = sz(g);
	vector<ll> expected(n);
	rep(s, 0, n) {
		vi dist(n, -1), q{s};
		dist[s] = 0;
		rep(i, 0, sz(q)) for (int u : g[q[i]]) {
			if (dist[u] != -1) continue;
			dist[u] = dist[q[i]] + 1;
			q.push_back(u);
		}
		expected[s] = accumulate(all(dist), 0LL);
	}
	for (auto& adj : g) shuffle(all(adj), rng);
	Reroot dp(g);
	// Reusing the object and changing the traversal root
	// must give the same answer for every vertex.
	rep(it, 0, 3) {
		dp.run(it == 0 ? 0 : (int)(rng() % n));
		rep(v, 0, n) {
			assert(dp.ans[v].cnt == n);
			assert(dp.ans[v].sum == expected[v]);
		}
	}
}

int main() {
	Reroot empty({});
	empty.run();
	assert(empty.ans.empty());
	rep(n, 1, 101) rep(it, 0, 20) {
		vector<vi> g(n);
		rep(v, 1, n) {
			int p = it == 0 ? v - 1 :
				it == 1 ? 0 : (int)(rng() % v);
			g[v].push_back(p);
			g[p].push_back(v);
		}
		check(g);
	}
	cout << "Tests passed!" << endl;
}
