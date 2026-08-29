#include "../../content/numerical/MinPlusConvolution.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, m;
	cin >> n >> m;
	vector<ll> a(n), b(m);
	rep(i, 0, n) cin >> a[i];
	rep(i, 0, m) cin >> b[i];
	// library wants concave second argument; LC marks a concave
	auto c = min_plus_concave_one(b, a);
	rep(i, 0, sz(c)) cout << c[i] << " \n"[i + 1 == sz(c)];
}
