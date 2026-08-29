#include "../../content/graph/HopcroftKarp.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int L, R, m;
	cin >> L >> R >> m;
	vector<vi> g(L);
	rep(i, 0, m) {
		int a, b;
		cin >> a >> b;
		g[a].pb(b);
	}
	vi r(R, -1);
	int k = hopcroftKarp(g, r);
	cout << k << '\n';
	rep(v, 0, R) if (r[v] != -1)
		cout << r[v] << ' ' << v << '\n';
}
