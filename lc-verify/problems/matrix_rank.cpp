#include "../../content/numerical/RREF.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, m;
	cin >> n >> m;
	vector<vector<ll>> a(n, vector<ll>(m));
	rep(i, 0, n) rep(j, 0, m) cin >> a[i][j];
	cout << rref(a, 998244353).first << '\n';
}
