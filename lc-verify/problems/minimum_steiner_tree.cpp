#include "../../content/graph/SteinerTree.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, m;
	cin >> n >> m;
	vector<array<ll, 3>> e(m);
	rep(i, 0, m) cin >> e[i][0] >> e[i][1] >> e[i][2];
	int k;
	cin >> k;
	vi s(k);
	rep(i, 0, k) cin >> s[i];
	auto [y, ids] = steinerTree(n, s, e);
	cout << y << ' ' << sz(ids) << '\n';
	rep(i, 0, sz(ids)) cout << ids[i] << " \n"[i + 1 == sz(ids)];
	if (ids.empty()) cout << '\n';
}
