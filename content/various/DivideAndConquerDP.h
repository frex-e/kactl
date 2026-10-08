/**
 * Author: Codex
 * License: CC0
 * Source: Codeforces
 * Description: Minimum cost to partition $[0,N)$ into
 *  exactly $K$ nonempty segments, where $0\le K\le N$.
 *  C(i,j) returns the cost of $[i,j)$, i included and
 *  j excluded; called only with $0\le i<j\le N$.
 *  $dp(g,j)=\min_{g-1\le k<j}(dp(g-1,k)+C(k,j))$.
 *  Requires the smallest optimal split k to be
 *  nondecreasing in j within each layer. Sufficient:
 *  $C(a,c)+C(b,d)\le C(a,d)+C(b,c)$ for $a\le b<c\le d$.
 *  All finite costs and sums must fit in ll and be
 *  less than LLONG\_MAX, which denotes infinity.
 *  Returns infinity if $K=0<N$; returns 0 if $K=N=0$.
 * Usage: ll ans = partitionDP(N, K, cost);
 * Time: O(KN \log N) with O(1) cost queries.
 * Memory: O(N)
 * Status: stress-tested
 */
#pragma once

template<class F>
ll partitionDP(int N, int K, F C) {
	assert(0 <= K && K <= N);
	vector<ll> prev(N + 1, LLONG_MAX), cur(N + 1);
	prev[0] = 0;
	auto rec = [&](auto&& self, int L, int R,
	               int lo, int hi) -> void {
		if (L >= R) return;
		int mid = (L + R) / 2, opt = lo;
		ll best = LLONG_MAX;
		rep(k,lo,min(mid,hi)) if (prev[k] != LLONG_MAX) {
			ll val = prev[k] + C(k, mid);
			if (val < best) best = val, opt = k;
		}
		cur[mid] = best;
		self(self, L, mid, lo, opt + 1);
		self(self, mid + 1, R, opt, hi);
	};
	rep(g,1,K+1) {
		fill(all(cur), LLONG_MAX);
		rec(rec, g, N + 1, g - 1, N);
		prev.swap(cur);
	}
	return prev[N];
}
