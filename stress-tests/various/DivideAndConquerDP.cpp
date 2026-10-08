#include "../utilities/template.h"
#include "../../content/various/DivideAndConquerDP.h"

template<class F>
void check(int N, F C) {
	// Independent quadratic transition, checking every layer.
	vector<vector<ll>> dp(N + 1,
		vector<ll>(N + 1, LLONG_MAX));
	dp[0][0] = 0;
	rep(g,1,N+1) rep(j,g,N+1) rep(k,g-1,j)
		if (dp[g - 1][k] != LLONG_MAX)
			dp[g][j] = min(dp[g][j],
				dp[g - 1][k] + C(k, j));
	auto cost = [&](int i, int j) {
		assert(0 <= i && i < j && j <= N);
		return C(i, j);
	};
	rep(K,0,N+1) assert(partitionDP(N, K, cost) == dp[K][N]);
}

int main() {
	auto zero = [](int, int) { return 0LL; };
	check(0, zero);
	check(1, zero);
	check(30, zero); // Ties always choose the smallest split.
	assert(partitionDP(2, 1, [](int, int) {
		return LLONG_MIN;
	}) == LLONG_MIN); // Never add infinity to a negative cost.
	assert(partitionDP(2, 2, [](int, int) {
		return LLONG_MAX / 2;
	}) == LLONG_MAX - 1);
	rep(it,0,500) {
		int N = rand() % 30 + 1;
		vector<ll> pref(N + 1), left(N + 1), right(N + 1);
		rep(i,0,N) pref[i + 1] = pref[i] + rand() % 10;
		rep(i,0,N+1) {
			left[i] = rand() % 101 - 50;
			right[i] = rand() % 101 - 50;
		}
		// Squared nonnegative sums plus arbitrary endpoint costs.
		check(N, [&](int i, int j) {
			ll s = pref[j] - pref[i];
			return s * s + left[i] + right[j];
		});
		// Random Monge costs: adjacent mixed differences <= 0.
		vector<vector<ll>> C(N + 1, vector<ll>(N + 1));
		rep(i,0,N+1) C[i][0] = left[i], C[0][i] = right[i];
		rep(i,1,N+1) rep(j,1,N+1)
			C[i][j] = C[i - 1][j] + C[i][j - 1]
				- C[i - 1][j - 1] - rand() % 10;
		check(N, [&](int i, int j) { return C[i][j]; });
	}
	cout << "Tests passed!" << endl;
}
