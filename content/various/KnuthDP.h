/**
 * Author: Simon Lindholm
 * License: CC0
 * Source: http://codeforces.com/blog/entry/8219
 * Description: Interval DP
 *  $dp(i,j)=\min_{i\le k<j}(dp(i,k)+dp(k+1,j))+C(i,j)$.
 *  Any fixed offsets from $k$ work. Let $opt(i,j)$ be an
 *  optimal $k$. Need
 *  $opt(i,j-1)\le opt(i,j)\le opt(i+1,j)$.
 *  Enough if $C(b,c)\le C(a,d)$ and quadrangle
 *  $C(a,c)+C(b,d)\le C(a,d)+C(b,c)$ for
 *  $a\le b\le c\le d$ (``wider gets worse faster'').
 *  Useful $C$: $C(i,j)+C(i+1,j+1)\le C(i,j+1)+C(i+1,j)$;
 *  $C(i,j)=\sum_{k=i}^j w_k$ ($w\ge 0$);
 *  $C(i,j)=g(x_j-x_i)$, $x$ incr, $g$ convex and
 *  nondecreasing;
 *  $C(i,j)=\min(A_i,B_j)$ if $A$ decreasing, $B$ increasing;
 *  $C(i,j)=\max(0,x_j-x_i-\Delta)$ for $\Delta>0$.
 *  Also consider LineContainer, Li Chao, monotone queues.
 * Time: O(N^2)
 * Status: stress-tested
 */
#pragma once

template<class F>
ll knuthDP(int N, F C) {
	vector<vector<ll>> dp(N, vector<ll>(N));
	vector<vi> opt(N, vi(N));
	rep(i,0,N) opt[i][i] = i; // set dp[i][i] if needed
	for (int i = N - 2; i >= 0; i--) {
		rep(j,i+1,N) {
			ll mn = LLONG_MAX;
			ll cost = C(i, j);
			int hi = min(j - 1, opt[i + 1][j]);
			rep(k, opt[i][j - 1], hi + 1) {
				ll val = dp[i][k] + dp[k + 1][j] + cost;
				if (mn >= val) {
					opt[i][j] = k;
					mn = val;
				}
			}
			dp[i][j] = mn;
		}
	}
	return dp[0][N - 1];
}
