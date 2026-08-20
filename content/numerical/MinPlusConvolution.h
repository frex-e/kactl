/**
 * Author: brunodccarvalho
 * Date: 2025-10-24
 * Source: https://github.com/caterpillow/cactl
 * Description: Min-plus convolution with one convex or
 *  concave input. SMAWK handles arbitrary/convex; the border
 *  algorithm handles arbitrary/concave. Here, convex = smiley
 *  and concave = frowny. \texttt{min\_smawk(f,r,c)} returns a
 *  minimizing column for each row of the matrix f. Totally
 *  monotone means every submatrix's leftmost row-minimum
 *  indices are nondecreasing. Queried through f, not stored.
 * Time: $O(N+M)$ convex, $O(N\log M+M)$ concave
 * Status: untested (no local stress test)
 */
#pragma once

// Row-minima indices for totally monotone f(x,y). O(N + M)
template<class Fn>
vi min_smawk_rec(Fn& f, const vi& row, vi col) {
	int n = sz(row);
	if (!n) return {};
	vi red;
	for (int c : col) {
		while (sz(red) &&
			f(row[sz(red)-1], c) < f(row[sz(red)-1], red.back()))
			red.pop_back();
		if (sz(red) < n) red.pb(c);
	}
	col = move(red);

	vi odd;
	for (int i = 1; i < n; i += 2) odd.pb(row[i]);
	vi oddans = min_smawk_rec(f, odd, col), ans(n);
	rep(i,0,sz(odd)) ans[2*i+1] = oddans[i];
	for (int i = 0, j = 0; i < n; i += 2) {
		ans[i] = col[j];
		int last = i+1 < n ? ans[i+1] : col.back();
		while (col[j] != last) {
			j++;
			if (f(row[i], col[j]) < f(row[i], ans[i]))
				ans[i] = col[j];
		}
	}
	return ans;
}
template<class Fn>
vi min_smawk(Fn f, int r, int c) {
	vi row(r), col(c);
	iota(all(row), 0), iota(all(col), 0);
	return min_smawk_rec(f, row, col);
}

// Min-plus c[k]=min_{i+j=k}(a[i]+b[j]), convex b. O(N+M)
template<class V>
vector<V> min_plus_smawk(const vector<V>& a,
		const vector<V>& b) {
	int n = sz(a), m = sz(b);
	if (!n || !m) return n ? a : b;
	auto f = [&](int r, int c) -> tuple<int,V,int> {
		if (r < c) return {1, V{}, c};
		if (r-c >= m) return {1, V{}, -c};
		return {0, a[c]+b[r-c], 0};
	};
	vi cols = min_smawk(f, n+m-1, n);
	vector<V> d(n+m-1);
	rep(r,0,n+m-1) d[r] = a[cols[r]]+b[r-cols[r]];
	return d;
}

// Min-plus conv for concave b. O(N log M + M)
template<class V>
vector<V> min_plus_concave_one(const vector<V>& a,
		const vector<V>& b) {
	int n = sz(a), m = sz(b), z = n+m-1;
	if (!n || !m) return n ? a : b;
	vector<V> c(z, numeric_limits<V>::max());
	auto solve = [&](int l, int r, bool rev) {
		auto val = [&](int j, int k) {
			if (rev) j = n-1-j, k = z-1-k;
			return a[j]+b[k-j];
		};
#define better(i,j,k) val(i,k) <= val(j,k)
		auto improve = [&](int u, int v, int l, int r) {
			while (r-l > 1) {
				int m = (l+r)/2;
				(better(u,v,m) ? r : l) = m;
			}
			return l;
		};
		vector<array<int,2>> stk;
		for (int i = l, k = l, s = -1; k < r; i++, k++) {
			while (s >= 0 && i < n &&
					better(i, stk[s][0], stk[s][1])) {
				stk.pop_back(), s--;
			}
			if (i < n) {
				int t = s < 0 ? r-1
					: improve(stk[s][0], i, k-1, stk[s][1]);
				if (t >= k) stk.pb({i,t}), s++;
			}
			int out = rev ? z-1-k : k;
			c[out] = min(c[out], val(stk[s][0], k));
			if (stk[s][1] == k) stk.pop_back(), s--;
		}
	};

	solve(0,m,0), solve(0,m,1);
	for (int k = m; k < z-m; k += m+1)
		solve(k+1,k+m+1,0), solve(z-k-m,z-k,1);
#undef better
	return c;
}
