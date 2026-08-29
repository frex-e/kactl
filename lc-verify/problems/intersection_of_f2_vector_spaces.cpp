#include "../../content/numerical/XORBasis.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int T;
	cin >> T;
	rep(tc, 0, T) {
		int n;
		cin >> n;
		vi xs(n);
		rep(i, 0, n) cin >> xs[i];
		int m;
		cin >> m;
		vi ys(m);
		rep(i, 0, m) cin >> ys[i];
		XorBasis xb, yb;
		rep(i, 0, n) xb.add(xs[i]);
		rep(i, 0, m) yb.add(ys[i]);
		const int B = 30, W = 60;
		vector<ll> piv(W, 0);
		auto ins = [&](ll x) {
			for (int i = W - 1; i >= 0; --i) if (x >> i & 1) {
				if (!piv[i]) { piv[i] = x; return; }
				x ^= piv[i];
			}
		};
		for (int v : ys) ins((ll)v << B);
		for (int u : xs) ins(((ll)u << B) | u);
		for (int i = W - 1; i >= 0; --i) if (piv[i])
			rep(j, 0, W) if (j != i && (piv[j] >> i & 1))
				piv[j] ^= piv[i];
		vi ans;
		rep(i, 0, W) if (piv[i] && !(piv[i] >> B))
			ans.pb((int)piv[i]);
		for (int w : ans) assert(xb.inSpan(w) && yb.inSpan(w));
		cout << sz(ans);
		for (int w : ans) cout << ' ' << w;
		cout << '\n';
	}
}
