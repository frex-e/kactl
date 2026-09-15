#include "../utilities/template.h"

#include "../../content/graph/LinkCutTree.h"
#include "../../content/data-structures/UnionFind.h"

pii naive(int N, const vector<pii>& edges, const vi& vals, int s) {
	vector<vi> g(N);
	for (auto [a, b] : edges) {
		g[a].push_back(b);
		g[b].push_back(a);
	}
	int cnt = 0, mx = vals[s];
	vector<bool> vis(N);
	vi st = {s};
	vis[s] = true;
	while (!st.empty()) {
		int u = st.back();
		st.pop_back();
		cnt++;
		mx = max(mx, vals[u]);
		for (int v : g[u]) if (!vis[v]) {
			vis[v] = true;
			st.push_back(v);
		}
	}
	return {cnt, mx};
}

void checkAll(int N, LinkCut& lc, const vector<pii>& edges, const vi& vals) {
	UF uf(N);
	for (auto [a, b] : edges) uf.join(a, b);
	rep(i, 0, N) {
		assert(lc.compSize(i) == uf.size(i));
		auto [cnt, mx] = naive(N, edges, vals, i);
		assert(cnt == uf.size(i));
		assert(lc.compMax(i) == mx);
		rep(j, 0, N) assert(lc.connected(i, j) == uf.sameSet(i, j));
	}
}

int main() {
	srand(2);
	LinkCut lczero(0);

	// Issue-style example: every node in a 4-node tree has size 4.
	{
		LinkCut T(4);
		vi vals(4);
		T.link(1, 0);
		T.link(2, 1);
		T.link(3, 0);
		vector<pii> edges = {{1, 0}, {2, 1}, {3, 0}};
		rep(i, 0, 4) assert(T.compSize(i) == 4);
		checkAll(4, T, edges, vals);
		T.set(2, 10);
		T.set(0, 5);
		vals[2] = 10;
		vals[0] = 5;
		rep(i, 0, 4) assert(T.compMax(i) == 10);
		T.cut(2, 1);
		edges = {{1, 0}, {3, 0}};
		assert(T.compSize(2) == 1);
		assert(T.compMax(2) == 10);
		assert(T.compSize(0) == 3);
		assert(T.compMax(0) == 5);
		assert(T.compMax(1) == 5);
		assert(T.compMax(3) == 5);
		assert(!T.connected(2, 0));
		checkAll(4, T, edges, vals);
	}

	rep(it, 0, 10000) {
		int N = rand() % 20 + 1;
		LinkCut lc(N);
		UF uf(N);
		vector<pii> edges;
		vi vals(N);
		rep(it2, 0, 1000) {
			int v = (rand() >> 4) & 7;
			if (v == 0 && !edges.empty()) { // remove
				int r = (rand() >> 4) % sz(edges);
				pii ed = edges[r];
				swap(edges[r], edges.back());
				edges.pop_back();
				if (rand() & 16)
					lc.cut(ed.first, ed.second);
				else
					lc.cut(ed.second, ed.first);
			} else if (v == 1) { // set value
				int a = (rand() >> 4) % N;
				int x = (rand() % 41) - 20;
				vals[a] = x;
				lc.set(a, x);
			} else {
				int a = (rand() >> 4) % N;
				int b = (rand() >> 4) % N;
				uf.e.assign(N, -1);
				for (auto& ed : edges) uf.join(ed.first, ed.second);
				bool c = uf.sameSet(a, b);
				if (!c && v != 2) {
					lc.link(a, b);
					edges.emplace_back(a, b);
				} else {
					assert(lc.connected(a, b) == c);
					auto [cnt, mx] = naive(N, edges, vals, a);
					assert(lc.compSize(a) == cnt);
					assert(lc.compMax(a) == mx);
					if (c) {
						assert(lc.compSize(b) == cnt);
						assert(lc.compMax(b) == mx);
					}
				}
			}
			if (it2 % 50 == 0) {
				uf.e.assign(N, -1);
				for (auto& ed : edges) uf.join(ed.first, ed.second);
				int a = (rand() >> 4) % N;
				assert(lc.compSize(a) == uf.size(a));
				assert(lc.compMax(a) == naive(N, edges, vals, a).second);
			}
		}
	}
	cout << "Tests passed!" << endl;
}
