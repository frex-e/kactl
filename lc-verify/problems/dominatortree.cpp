#include "../../content/graph/DominatorTree.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, m, s;
	cin >> n >> m >> s;
	vector<vi> g(n);
	rep(i, 0, m) {
		int a, b;
		cin >> a >> b;
		g[a].pb(b);
	}
	Dominator D(g, s);
	vi p(n, -1);
	p[s] = s;
	rep(u, 0, n) for (int v : D.ans[u]) p[v] = u;
	rep(i, 0, n) cout << p[i] << " \n"[i + 1 == n];
}
