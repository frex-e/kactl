#include "../../content/number-theory/Mobius.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	const int MOD = 998244353;
	int n;
	cin >> n;
	vector<ll> A(n + 1), B(n + 1);
	rep(i, 1, n + 1) cin >> A[i];
	rep(i, 1, n + 1) cin >> B[i];
	vi mu(n + 1);
	calcMobius(mu);
	for (int i = n; i >= 1; --i)
		for (int j = 2 * i; j <= n; j += i) {
			A[i] += A[j];
			B[i] += B[j];
		}
	vector<ll> D(n + 1), c(n + 1);
	rep(i, 1, n + 1) D[i] = (A[i] % MOD) * (B[i] % MOD) % MOD;
	rep(i, 1, n + 1) for (int j = i; j <= n; j += i) {
		c[i] += D[j] * mu[j / i];
	}
	rep(i, 1, n + 1) {
		ll v = c[i] % MOD;
		if (v < 0) v += MOD;
		cout << v << " \n"[i == n];
	}
}
