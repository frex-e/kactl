/**
 * Author: me
 * Date: 2026-10-06
 * License: CC0
 * Description: Static range order statistics on an int array.
 *  Compresses values; preserves input. Indices and value
 *  ranges are half-open. kth(l,r,k) is the 0-indexed kth
 *  smallest in a[l,r), including duplicates; requires
 *  $0 \le k < r-l$. countLess(l,r,x) counts values $<x$;
 *  count(l,r,lo,hi) counts $lo \le a_i < hi$.
 *  Counts allow empty ranges. Thresholds are ll so x+1LL
 *  is safe for inclusive bounds, even at \texttt{INT\_MAX}.
 * Usage:
 *  WaveletTree w({5, 1, 5, -2, 7});
 *  w.kth(1, 5, 1); // 1: sorted subarray is -2,1,5,7
 *  w.count(0, 5, 5, 6); // 2 copies of 5
 *  w.kth(l, r, (r-l-1)/2); // lower median (l < r)
 *  int k = w.countLess(l, r, x);
 *  if (k < r-l) w.kth(l, r, k); // smallest >= x
 * Time: $O(N\log N)$ build, $O(\log(\sigma+1))$ query.
 * Memory: $O(N\log(\sigma+1))$, $\sigma$ distinct values.
 * Status: stress-tested
 */
#pragma once

struct WaveletTree {
	int n;
	vi vals;
	vector<vi> b;
	WaveletTree(const vi& a) : n(sz(a)), vals(a) {
		sort(all(vals));
		vals.erase(unique(all(vals)), vals.end());
		b.resize(4 * sz(vals));
		if (n) build(a, 1, 0, sz(vals));
	}
	void build(const vi& a, int v, int lo, int hi) {
		if (hi-lo <= 1) return;
		int m = lo + (hi-lo)/2;
		vi c[2];
		b[v].resize(sz(a) + 1);
		rep(i,0,sz(a)) {
			int left = a[i] < vals[m];
			b[v][i+1] = b[v][i] + left;
			c[!left].push_back(a[i]);
		}
		build(c[0], 2*v, lo, m);
		build(c[1], 2*v+1, m, hi);
	}
	int kth(int l, int r, int k) const {
		assert(0 <= l && l <= r && r <= n);
		assert(0 <= k && k < r-l);
		int v = 1, lo = 0, hi = sz(vals);
		while (hi-lo > 1) {
			int m = lo + (hi-lo)/2, lb = b[v][l], rb = b[v][r];
			if (k < rb-lb) {
				l = lb; r = rb; hi = m; v *= 2;
			} else {
				k -= rb-lb; l -= lb; r -= rb;
				lo = m; v = 2*v+1;
			}
		}
		return vals[lo];
	}
	int countLess(int l, int r, ll x) const {
		assert(0 <= l && l <= r && r <= n);
		if (l == r) return 0;
		int v = 1, lo = 0, hi = sz(vals), ans = 0;
		while (hi-lo > 1) {
			int m = lo + (hi-lo)/2, lb = b[v][l], rb = b[v][r];
			if (x <= vals[m]) {
				l = lb; r = rb; hi = m; v *= 2;
			} else {
				ans += rb-lb; l -= lb; r -= rb;
				lo = m; v = 2*v+1;
			}
		}
		return ans + (vals[lo] < x ? r-l : 0);
	}
	int count(int l, int r, ll lo, ll hi) const {
		assert(lo <= hi);
		return countLess(l, r, hi) - countLess(l, r, lo);
	}
};
