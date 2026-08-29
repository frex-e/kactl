#include "../../content/graph/Blossom.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, m;
	cin >> n >> m;
	Blossom g(n);
	rep(i, 0, m) {
		int u, v;
		cin >> u >> v;
		g.ae(u, v);
	}
	int x = g.solve();
	cout << x << '\n';
	rep(u, 0, n) if (g.mate[u] > u)
		cout << u << ' ' << g.mate[u] << '\n';
}
