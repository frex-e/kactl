#include "../utilities/template.h"
#include "../../content/extras/EulerTourTree.h"

struct Forest {
	int n;
	vector<vi> adj;
	vector<ll> val;
	Forest(int n) : n(n), adj(n, vi(n)), val(n) {}
	vi component(int u) {
		vi q{u}, seen(n);
		seen[u] = 1;
		rep(i,0,sz(q)) rep(v,0,n)
			if (adj[q[i]][v] && !seen[v])
				seen[v] = 1, q.push_back(v);
		return q;
	}
	void check(EulerTourTree& ett) {
		rep(u,0,n) {
			vi c = component(u), seen(n);
			ll sum = 0;
			for (int v : c) sum += val[v], seen[v] = 1;
			assert(ett.componentQuery(u) == sum);
			assert(ett.componentSize(u) == sz(c));
			assert(ett.get(u) == val[u]);
			rep(v,0,n) assert(ett.connected(u,v) == seen[v]);
		}
		// Check parent pointers, heap order and stored aggregates.
		vi seen(sz(ett.t));
		function<pair<int,ll>(int,int,vi&)> dfs =
			[&](int x, int p, vi& seq) -> pair<int,ll> {
			if (!x) return {0, 0};
			assert(!seen[x]++);
			ett.push(x);
			auto& t = ett.t[x];
			assert(t.p == p);
			if (p) assert(t.y <= ett.t[p].y);
			auto [lc, ls] = dfs(t.l, x, seq);
			seq.push_back(x);
			auto [rc, rs] = dfs(t.r, x, seq);
			assert(t.c == lc + rc + 1);
			assert(t.cnt == t.own + ett.t[t.l].cnt
				+ ett.t[t.r].cnt);
			assert(t.agg == ls + rs + t.val);
			return {t.c, t.agg};
		};
		vector<pii> ends(sz(ett.t), {-1, -1});
		for (auto [e, xy] : ett.edges) {
			assert(adj[e.first][e.second]);
			ends[xy.first] = ends[xy.second] = e;
		}
		rep(u,0,n) if (!seen[u+1]) {
			vi seq;
			dfs(ett.root(u+1), 0, seq);
			// Read the cyclic tour from a vertex marker.
			int k = 0;
			while (seq[k] > n) ++k;
			int start = seq[k] - 1, cur = start;
			rep(j,0,sz(seq)) {
				int x = seq[(k+j) % sz(seq)];
				if (x <= n) assert(x - 1 == cur);
				else {
					auto [a, b] = ends[x];
					assert(a == cur || b == cur);
					cur = a ^ b ^ cur;
					assert(ett.t[x].val == 0);
				}
			}
			assert(cur == start);
		}
		for (int x : ett.spare) {
			assert(x > n && !seen[x]++);
			assert(!ett.t[x].p && ett.t[x].c == 1);
		}
		rep(x,1,sz(seen)) assert(seen[x] == 1);
		assert(sz(ett.t) <= max(1, 3*n - 1));
		assert(ett.t[0].c == 0 && ett.t[0].agg == 0);
	}
};

int main() {
	EulerTourTree empty(0);
	Forest(0).check(empty);
	mt19937 rng(123456);
	rep(seed,0,100) {
		int n = 1 + int(rng() % 40);
		EulerTourTree ett(n);
		ett.rng.seed(seed);
		Forest f(n);
		vector<pii> edges;
		rep(step,0,1200) {
			int u = int(rng() % n), v = int(rng() % n);
			int op = int(rng() % 7);
			if (op == 0 && !edges.empty()) {
				int k = int(rng() % edges.size());
				tie(u, v) = edges[k];
				edges[k] = edges.back(); edges.pop_back();
				if (rng() & 1) swap(u, v);
				ett.cut(u, v);
				f.adj[u][v] = f.adj[v][u] = 0;
			} else if (op <= 2) {
				vi c = f.component(u);
				bool same = find(all(c), v) != c.end();
				assert(ett.connected(u, v) == same);
				if (!same) {
					ett.link(u, v); edges.emplace_back(u, v);
					f.adj[u][v] = f.adj[v][u] = 1;
				}
			} else if (op <= 4) {
				ll add = ll(rng()) - (1LL << 31);
				ett.componentUpdate(u, add);
				for (int w : f.component(u)) f.val[w] += add;
			} else {
				ll val = (ll(rng()) - (1LL << 31)) * 100;
				ett.set(u, val); f.val[u] = val;
			}
			if (step % 20 == 0) f.check(ett);
		}
		f.check(ett);
	}
	// Subtree update via cut/add/link, with pending lazy tags.
	{
		EulerTourTree ett(5);
		ett.link(0, 1); ett.link(1, 2);
		ett.link(1, 3); ett.link(3, 4);
		ett.componentUpdate(0, 10);
		ett.cut(1, 3);
		ett.componentUpdate(3, -7);
		assert(ett.componentQuery(3) == 6);
		ett.link(1, 3);
		assert(ett.componentQuery(0) == 36);
		ett.componentUpdate(4, 5);
		ett.set(3, -100); // Overwrite after lazy addition.
		assert(ett.get(4) == 8);
		assert(ett.componentQuery(2) == -47);
	}
	// Large path/star; repeatedly reuse the same edge tokens.
	const int n = 30000;
	rep(shape,0,2) {
		EulerTourTree ett(n);
		auto parent = [&](int v) { return shape ? 0 : v-1; };
		rep(v,0,n) ett.set(v, ll(v) * v);
		ll total = ll(n-1) * n * (2*n-1) / 6;
		rep(v,1,n) ett.link(v, parent(v));
		assert(ett.componentQuery(n/2) == total);
		assert(ett.componentSize(0) == n);
		rep(step,0,30000) {
			int v = 1 + int(rng() % (n-1)), p = parent(v);
			ett.cut(p, v);
			assert(!ett.connected(p, v));
			assert(ett.componentSize(v) == (shape ? 1 : n-v));
			ll sum = shape ? ll(v)*v :
				total - ll(v-1)*v*(2*v-1)/6;
			assert(ett.componentQuery(v) == sum);
			assert(ett.componentQuery(p) == total - sum);
			ett.link(p, v);
			assert(ett.componentQuery(v) == total);
			assert(sz(ett.t) == 3*n - 1);
		}
		for (int v = n-1; v > 0; --v) {
			ett.cut(v, parent(v));
			assert(ett.componentSize(v) == 1);
			assert(ett.componentQuery(v) == ll(v)*v);
		}
		assert(ett.componentSize(0) == 1);
		assert(ett.edges.empty());
		assert(sz(ett.spare) == 2*(n-1));
	}
	cout << "Tests passed!" << endl;
}
