#include "../utilities/template.h"

#define pb push_back
#include "../../content/graph/DominatorTree.h"

const int N = 12;

bool reachable(const vector<vi>& g, int src, int dst, int blocked) {
	if (src == blocked) return false;
	vi seen(sz(g));
	vi q = {src};
	seen[src] = 1;
	rep(qi,0,sz(q)) {
		int u = q[qi];
		if (u == dst) return true;
		for (int v : g[u]) if (v != blocked && !seen[v]) {
			seen[v] = 1;
			q.pb(v);
		}
	}
	return false;
}

vi idomsBrute(const vector<vi>& g, int root) {
	int n = sz(g);
	vi idom(n, -1);
	idom[root] = root;
	rep(b,0,n) if (b != root) {
		vi doms;
		rep(a,0,n) {
			if (a == b || !reachable(g, root, b, a))
				doms.pb(a);
		}
		assert(!doms.empty());
		int best = -1;
		for (int a : doms) if (a != b) {
			bool ok = true;
			for (int c : doms) if (c != b && c != a) {
				if (reachable(g, root, a, c)) { ok = false; break; }
			}
			if (ok) {
				assert(best < 0);
				best = a;
			}
		}
		assert(best >= 0);
		idom[b] = best;
	}
	return idom;
}

void check(const vector<vi>& g, int root) {
	int n = sz(g);
	Dominator D(g, root);
	assert(D.co == n);

	vi idom(n, -1);
	idom[root] = root;
	rep(u,0,n) for (int v : D.ans[u]) {
		assert(idom[v] < 0);
		idom[v] = u;
	}
	rep(i,0,n) assert(idom[i] >= 0);

	vi want = idomsBrute(g, root);
	rep(i,0,n) assert(idom[i] == want[i]);
}

int main() {
	// Tiny hand checks
	{
		vector<vi> g(1);
		check(g, 0);
	}
	{
		// 0 -> 1 -> 2, 0 -> 2
		vector<vi> g(3);
		g[0] = {1, 2};
		g[1] = {2};
		check(g, 0);
	}
	{
		// diamond: 0->1,0->2,1->3,2->3
		vector<vi> g(4);
		g[0] = {1, 2};
		g[1] = {3};
		g[2] = {3};
		check(g, 0);
	}

	mt19937 rng(123456);
	rep(n,1,N+1) rep(it,0,200) {
		vector<vi> g(n);
		int m = n == 1 ? 0 : int(rng() % (n * n + 1));
		rep(e,0,m) {
			int a = int(rng() % n), b = int(rng() % n);
			g[a].pb(b);
		}
		// Ensure all reachable from 0 via a spine
		rep(i,1,n) g[int(rng() % i)].pb(i);
		check(g, 0);
	}

	cout << "Tests passed!" << endl;
}
