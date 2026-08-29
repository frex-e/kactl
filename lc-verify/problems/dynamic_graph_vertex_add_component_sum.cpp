#include "../../content/data-structures/OfflineDynamicConnectivity.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	vector<ll> a(n);
	rep(i, 0, n) cin >> a[i];
	DynCon dc(n, q, a);
	rep(i, 0, q) {
		int ty;
		cin >> ty;
		if (ty == 0 || ty == 1) {
			int u, v;
			cin >> u >> v;
			dc.toggle(u, v);
		} else if (ty == 2) {
			int v, x;
			cin >> v >> x;
			dc.addVal(v, x);
		} else {
			int v;
			cin >> v;
			dc.query(v);
		}
	}
	for (ll x : dc.ans()) cout << x << '\n';
}
