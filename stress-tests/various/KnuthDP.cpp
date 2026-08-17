#include "../utilities/template.h"

#include "../../content/various/KnuthDP.h"

ll brute(int N, const vector<ll>& pref) {
	vector<vector<ll>> dp(N, vector<ll>(N));
	auto C = [&](int i, int j) { return pref[j + 1] - pref[i]; };
	rep(len,1,N) rep(i,0,N-len) {
		int j = i + len;
		ll mn = LLONG_MAX, cost = C(i, j);
		rep(k,i,j) mn = min(mn, dp[i][k] + dp[k + 1][j] + cost);
		dp[i][j] = mn;
	}
	return dp[0][N - 1];
}

int main() {
	assert(knuthDP(1, [](int, int) { return 0LL; }) == 0);
	rep(it,0,200) {
		int N = rand() % 25 + 1;
		vector<ll> w(N), pref(N + 1);
		rep(i,0,N) w[i] = rand() % 10 + 1;
		rep(i,0,N) pref[i + 1] = pref[i] + w[i];
		auto C = [&](int i, int j) { return pref[j + 1] - pref[i]; };
		assert(knuthDP(N, C) == brute(N, pref));
	}
	// C(i,j) = (x[j] - x[i])^2 with increasing x (convex g)
	rep(it,0,50) {
		int N = rand() % 20 + 1;
		vector<ll> x(N);
		x[0] = rand() % 5;
		rep(i,1,N) x[i] = x[i - 1] + rand() % 5 + 1;
		auto C = [&](int i, int j) {
			ll d = x[j] - x[i];
			return d * d;
		};
		vector<vector<ll>> dp(N, vector<ll>(N));
		rep(len,1,N) rep(i,0,N-len) {
			int j = i + len;
			ll mn = LLONG_MAX, cost = C(i, j);
			rep(k,i,j) mn = min(mn, dp[i][k] + dp[k + 1][j] + cost);
			dp[i][j] = mn;
		}
		assert(knuthDP(N, C) == dp[0][N - 1]);
	}
	cout << "Tests passed!" << endl;
}
