/**
 * Author: Simon Lindholm
 * License: CC0
 * Source: Codeforces
 * Description: Given $a[i] = \min_{lo(i) \le k < hi(i)}(f(i, k))$ where the (minimal)
 *  optimal $k$ increases with $i$, computes $a[i]$ for $i = L..R-1$.
 *  Layered form (quadrangle on $C$ is sufficient):
 *  $\mathrm{dp}(t,j)=\min_{0\le k\le j}\mathrm{dp}(t-1,k-1)+C(k,j)$.
 *  See KnuthDP.h for quadrangle patterns.
 * Usage:
 *  Fill in the four hooks, then call solve(L, R) to fill
 *  a[L..R-1].
 *  lo(i), hi(i): allowed k is the half-open range [lo(i), hi(i)).
 *  f(i, k): cost of choosing k at index i.
 *  store(i, k, v): save a[i] = v (argmin k).
 *  rec(L, R, LO, HI) solves i in [L, R), knowing the
 *  optimal k lies in [LO, HI).
 *  solve(L, R) = rec(L, R, -inf, +inf).
 *  For the layered DP above: lo(j)=0, hi(j)=j+1,
 *  f(j,k)=(k ? dpbefore[k-1] : 0)+C(k,j),
 *  store into dpafter[j], then swap layers.
 * Time: O((N + (hi-lo)) \log N)
 * Status: tested on http://codeforces.com/contest/321/problem/E
 */
#pragma once

struct DP { // Modify at will:
	int lo(int ind) { return 0; }
	int hi(int ind) { return ind; }
	ll f(int ind, int k) { return dp[ind][k]; }
	void store(int ind, int k, ll v) { res[ind] = pii(k, v); }

	void rec(int L, int R, int LO, int HI) {
		if (L >= R) return;
		int mid = (L + R) >> 1;
		pair<ll, int> best(LLONG_MAX, LO);
		rep(k, max(LO,lo(mid)), min(HI,hi(mid)))
			best = min(best, make_pair(f(mid, k), k));
		store(mid, best.second, best.first);
		rec(L, mid, LO, best.second+1);
		rec(mid+1, R, best.second, HI);
	}
	void solve(int L, int R) { rec(L, R, INT_MIN, INT_MAX); }
};
