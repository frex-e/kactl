#include "../utilities/template.h"
#include "../utilities/random.h"
#include "../utilities/genTree.h"

#include "../../content/extras/LinkCutTree.h"
#include "../../content/data-structures/UnionFind.h"

vi getPath(const vector<vi>& adj, int u, int v) {
	int n = sz(adj);
	vi par(n, -2);
	queue<int> q;
	par[u] = -1;
	q.push(u);
	while (!q.empty()) {
		int x = q.front(); q.pop();
		if (x == v) break;
		for (int y : adj[x]) if (par[y] == -2) {
			par[y] = x;
			q.push(y);
		}
	}
	if (par[v] == -2) return {};
	vi p;
	for (int x = v; x != -1; x = par[x]) p.push_back(x);
	return p;
}

int bruteLca(const vector<vi>& adj, int r, int u, int v) {
	vi a = getPath(adj, r, u), b = getPath(adj, r, v);
	if (a.empty() || b.empty()) return -1;
	reverse(all(a)); reverse(all(b));
	int k = 0, m = min(sz(a), sz(b));
	while (k < m && a[k] == b[k]) k++;
	return a[k - 1];
}

void testDynamic(int trials, int nMax, int ops) {
	rep(it, 0, trials) {
		int n = randIncl(1, nMax);
		LinkCut lc(n);
		UF uf(n);
		vector<vi> adj(n);
		vector<ll> val(n);
		vector<pii> edges;
		rep(op, 0, ops) {
			int t = randRange(6);
			if (t == 0 && !edges.empty()) {
				int r = randRange(sz(edges));
				auto [a, b] = edges[r];
				swap(edges[r], edges.back());
				edges.pop_back();
				adj[a].erase(find(all(adj[a]), b));
				adj[b].erase(find(all(adj[b]), a));
				if (rand() & 16) lc.cut(a, b);
				else lc.cut(b, a);
			} else if (t <= 2) {
				int a = randRange(n), b = randRange(n);
				uf.e.assign(n, -1);
				for (auto [x, y] : edges) uf.join(x, y);
				bool same = uf.sameSet(a, b);
				assert(lc.connected(a, b) == same);
				if (!same && a != b && t == 1) {
					lc.link(a, b);
					edges.emplace_back(a, b);
					adj[a].push_back(b);
					adj[b].push_back(a);
				}
			} else if (t == 3) {
				int a = randRange(n), b = randRange(n);
				vi p = getPath(adj, a, b);
				if (p.empty()) {
					assert(!lc.connected(a, b));
					continue;
				}
				ll x = randIncl(-5, 5);
				lc.update(a, b, x);
				for (int v : p) val[v] += x;
			} else if (t == 4) {
				int a = randRange(n), b = randRange(n);
				vi p = getPath(adj, a, b);
				if (p.empty()) {
					assert(!lc.connected(a, b));
					continue;
				}
				ll s = 0;
				for (int v : p) s += val[v];
				assert(lc.query(a, b) == s);
				assert(lc.query(b, a) == s);
			} else {
				int a = randRange(n);
				ll x = randIncl(-10, 10);
				lc.set(a, x);
				val[a] = x;
				assert(lc.get(a) == x);
				assert(lc.query(a, a) == x);
			}
			if (!edges.empty() && randRange(5) == 0) {
				int r = randRange(n), u = randRange(n), v = randRange(n);
				vi pr = getPath(adj, r, u), pv = getPath(adj, r, v);
				if (pr.empty() || pv.empty()) continue;
				assert(lc.lca(r, u, v) == bruteLca(adj, r, u, v));
			}
		}
		rep(i, 0, n) assert(lc.get(i) == val[i]);
	}
}

void testStaticTree(int n, int queries) {
	auto g = genRandomTree(n);
	LinkCut lc(n);
	vector<vi> adj(n);
	vector<ll> val(n);
	for (auto [a, b] : g) {
		lc.link(a, b);
		adj[a].push_back(b);
		adj[b].push_back(a);
	}
	rep(i, 0, n) {
		ll x = randIncl(-20, 20);
		lc.set(i, x);
		val[i] = x;
	}
	rep(q, 0, queries) {
		int t = randRange(4);
		int a = randRange(n), b = randRange(n);
		if (t == 0) {
			ll x = randIncl(-5, 5);
			lc.update(a, b, x);
			for (int v : getPath(adj, a, b)) val[v] += x;
		} else if (t == 1) {
			ll s = 0;
			for (int v : getPath(adj, a, b)) s += val[v];
			assert(lc.query(a, b) == s);
		} else if (t == 2) {
			ll x = randIncl(-20, 20);
			lc.set(a, x);
			val[a] = x;
			assert(lc.get(a) == x);
		} else {
			int r = randRange(n);
			assert(lc.lca(r, a, b) == bruteLca(adj, r, a, b));
			assert(lc.connected(a, b));
			assert(lc.findRoot(a) == lc.findRoot(b));
		}
	}
}

int main() {
	srand(2);
	LinkCut lczero(0);
	LinkCut one(1);
	one.set(0, 7);
	assert(one.get(0) == 7);
	assert(one.query(0, 0) == 7);
	one.update(0, 0, 3);
	assert(one.query(0, 0) == 10);
	assert(one.findRoot(0) == 0);
	assert(one.lca(0, 0, 0) == 0);
	{
		LinkCut lc(3);
		lc.set(0, 3); lc.set(1, 1); lc.set(2, 4);
		lc.link(0, 1); lc.link(1, 2);
		assert(lc.query(0, 2) ==
			lc.binop(lc.binop(3, 1), 4));
		assert(lc.query(2, 0) == lc.query(0, 2));
		lc.update(0, 2, 2);
		assert(lc.query(0, 2) ==
			lc.applyUpdate(2, lc.binop(lc.binop(3, 1), 4), 3));
	}

	testDynamic(2000, 12, 200);
	testStaticTree(2, 50);
	testStaticTree(80, 2000);
	cout << "Tests passed!" << endl;
}
