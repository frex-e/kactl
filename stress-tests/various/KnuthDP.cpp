#include "../utilities/template.h"

#include "../../content/various/KnuthDP.h"

template<class F>
ll bruteDP(int N, F C) {
	vector<vector<ll>> dp(N, vector<ll>(N));
	rep(len,1,N) rep(i,0,N-len) {
		int j = i + len;
		ll mn = LLONG_MAX, cost = C(i, j);
		rep(k,i,j) mn = min(mn, dp[i][k] + dp[k + 1][j] + cost);
		dp[i][j] = mn;
	}
	return dp[0][N - 1];
}

template<class F>
void check(int N, F C) {
	assert(knuthDP(N, C) == bruteDP(N, C));
}

int main() {
	assert(knuthDP(1, [](int, int) { return 0LL; }) == 0);
	// C(i,j) = sum_{k=i}^j w_k, w >= 0
	rep(it,0,200) {
		int N = rand() % 25 + 1;
		vector<ll> w(N), pref(N + 1);
		rep(i,0,N) w[i] = rand() % 10;
		rep(i,0,N) pref[i + 1] = pref[i] + w[i];
		auto C = [&](int i, int j) { return pref[j + 1] - pref[i]; };
		check(N, C);
	}
	// C(i,j) = (x[j] - x[i])^2 with increasing x (convex nondec. g)
	rep(it,0,50) {
		int N = rand() % 20 + 1;
		vector<ll> x(N);
		x[0] = rand() % 5;
		rep(i,1,N) x[i] = x[i - 1] + rand() % 5 + 1;
		auto C = [&](int i, int j) {
			ll d = x[j] - x[i];
			return d * d;
		};
		check(N, C);
	}
	// C(i,j) = min(A_i, B_j), A decreasing, B increasing
	rep(it,0,50) {
		int N = rand() % 20 + 1;
		vector<ll> A(N), B(N);
		A[0] = 50 + rand() % 10;
		B[0] = rand() % 5;
		rep(i,1,N) {
			A[i] = A[i - 1] - rand() % 3;
			B[i] = B[i - 1] + rand() % 3;
		}
		auto C = [&](int i, int j) { return min(A[i], B[j]); };
		check(N, C);
	}
	// C(i,j) = max(0, x_j - x_i - Delta), x increasing
	rep(it,0,50) {
		int N = rand() % 20 + 1;
		vector<ll> x(N);
		x[0] = rand() % 5;
		rep(i,1,N) x[i] = x[i - 1] + rand() % 5 + 1;
		ll Delta = rand() % 8 + 1;
		auto C = [&](int i, int j) {
			return max(0LL, x[j] - x[i] - Delta);
		};
		check(N, C);
	}
	cout << "Tests passed!" << endl;
}
