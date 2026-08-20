/**
 * Author: 12tqian
 * Date: 2022-07-21
 * License: Unknown
 * Source: https://github.com/12tqian/cp-library
 * Description: Segment tree beats supporting range
 *  \texttt{chmin}/\texttt{chmax}/\texttt{add} and range
 *  sum/min/max. Bounds inclusive, 0-indexed. Init to 0, or
 *  pass an array. \texttt{upd}: $t{=}0$ chmin, $t{=}1$
 *  chmax, $t{=}2$ add.
 * Time: $O(\log^2 N)$ amortized
 * Status: stress-tested
 */
#pragma once

struct SegmentTreeBeats {
	using T = pair<pair<ll, ll>, int>;
	const ll INF = numeric_limits<ll>::max();
	vector<ll> mx_mod, mn_mod, mod, sum;
	vector<T> mx, mn;
	int n, sz;

	SegmentTreeBeats(int n) : n(n) {
		sz = 1;
		while (sz < n) sz *= 2;
		mx_mod.assign(2 * sz, 0);
		mn_mod.assign(2 * sz, 0);
		mod.assign(2 * sz, 0);
		sum.assign(2 * sz, 0);
		mx.assign(2 * sz, {{0, 0}, 0});
		mn.assign(2 * sz, {{0, 0}, 0});
		build();
	}
	SegmentTreeBeats(const vector<ll>& a) :
		SegmentTreeBeats(sz(a)) {
		rep(i,0,n) {
			ll v = a[i];
			int ind = sz + i;
			mx[ind] = {{v, -INF}, 1};
			mn[ind] = {{v, INF}, 1};
			sum[ind] = v;
		}
		for (int i = sz - 1; i >= 1; --i) pull(i);
	}

	void build(int ind = 1, int L = 0, int R = -1) {
		if (R == -1) R += sz;
		mx_mod[ind] = INF, mn_mod[ind] = -INF, mod[ind] = 0;
		if (L == R) {
			mx[ind] = {{0, -INF}, 1};
			mn[ind] = {{0, INF}, 1};
			sum[ind] = 0;
			return;
		}
		int M = (L + R) / 2;
		build(2 * ind, L, M);
		build(2 * ind + 1, M + 1, R);
		pull(ind);
	}

	T comb_mn(T a, T b) {
		if (a > b) swap(a, b);
		if (a.fr.fr == b.fr.fr)
			return {{a.fr.fr, min(a.fr.sc, b.fr.sc)},
				a.sc + b.sc};
		return {{a.fr.fr, min(a.fr.sc, b.fr.fr)}, a.sc};
	}
	T comb_mx(T a, T b) {
		if (a < b) swap(a, b);
		if (a.fr.fr == b.fr.fr)
			return {{a.fr.fr, max(a.fr.sc, b.fr.sc)},
				a.sc + b.sc};
		return {{a.fr.fr, max(a.fr.sc, b.fr.fr)}, a.sc};
	}
	void pull(int ind) {
		sum[ind] = sum[2 * ind] + sum[2 * ind + 1];
		mn[ind] = comb_mn(mn[2 * ind], mn[2 * ind + 1]);
		mx[ind] = comb_mx(mx[2 * ind], mx[2 * ind + 1]);
	}

	void push(int ind, int L, int R) {
		auto chk = [](ll& a, ll b, ll c) {
			if (a == b) a = c;
		};
		if (mn_mod[ind] != -INF) {
			if (mn_mod[ind] > mn[ind].fr.fr) {
				sum[ind] += (mn_mod[ind] - mn[ind].fr.fr)
					* mn[ind].sc;
				chk(mx[ind].fr.fr, mn[ind].fr.fr, mn_mod[ind]);
				chk(mx[ind].fr.sc, mn[ind].fr.fr, mn_mod[ind]);
				mn[ind].fr.fr = mn_mod[ind];
				if (L != R) rep(i,0,2) {
					int pos = 2 * ind + i;
					mn_mod[pos] = max(mn_mod[pos],
						mn_mod[ind] - mod[pos]);
					mx_mod[pos] = max(mx_mod[pos], mn_mod[pos]);
				}
			}
			mn_mod[ind] = -INF;
		}
		if (mx_mod[ind] != INF) {
			if (mx_mod[ind] < mx[ind].fr.fr) {
				sum[ind] += (mx_mod[ind] - mx[ind].fr.fr)
					* mx[ind].sc;
				chk(mn[ind].fr.fr, mx[ind].fr.fr, mx_mod[ind]);
				chk(mn[ind].fr.sc, mx[ind].fr.fr, mx_mod[ind]);
				mx[ind].fr.fr = mx_mod[ind];
				if (L != R) rep(i,0,2) {
					int pos = 2 * ind + i;
					mx_mod[pos] = min(mx_mod[pos],
						mx_mod[ind] - mod[pos]);
					mn_mod[pos] = min(mn_mod[pos], mx_mod[pos]);
				}
			}
			mx_mod[ind] = INF;
		}
		if (mod[ind] != 0) {
			sum[ind] += mod[ind] * (R - L + 1);
			auto inc = [&](T& a, ll b) {
				if (abs(a.fr.fr) != INF) a.fr.fr += b;
				if (abs(a.fr.sc) != INF) a.fr.sc += b;
			};
			inc(mx[ind], mod[ind]);
			inc(mn[ind], mod[ind]);
			if (L != R) {
				mod[2 * ind] += mod[ind];
				mod[2 * ind + 1] += mod[ind];
			}
			mod[ind] = 0;
		}
	}

	ll qsum(int lo, int hi, int ind = 1, int L = 0, int R = -1) {
		if (R == -1) R += sz;
		push(ind, L, R);
		if (R < lo || hi < L) return 0;
		if (lo <= L && R <= hi) return sum[ind];
		int M = (L + R) / 2;
		return qsum(lo, hi, 2 * ind, L, M)
			+ qsum(lo, hi, 2 * ind + 1, M + 1, R);
	}
	ll qmax(int lo, int hi, int ind = 1, int L = 0, int R = -1) {
		if (R == -1) R += sz;
		push(ind, L, R);
		if (R < lo || hi < L) return -INF;
		if (lo <= L && R <= hi) return mx[ind].fr.fr;
		int M = (L + R) / 2;
		return max(qmax(lo, hi, 2 * ind, L, M),
			qmax(lo, hi, 2 * ind + 1, M + 1, R));
	}
	ll qmin(int lo, int hi, int ind = 1, int L = 0, int R = -1) {
		if (R == -1) R += sz;
		push(ind, L, R);
		if (R < lo || hi < L) return INF;
		if (lo <= L && R <= hi) return mn[ind].fr.fr;
		int M = (L + R) / 2;
		return min(qmin(lo, hi, 2 * ind, L, M),
			qmin(lo, hi, 2 * ind + 1, M + 1, R));
	}

	void upd(int t, int lo, int hi, ll b,
		int ind = 1, int L = 0, int R = -1) {
		if (R == -1) R += sz;
		push(ind, L, R);
		if (R < lo || hi < L) return;
		if (t == 0) {
			if (b >= mx[ind].fr.fr) return;
		} else if (t == 1) {
			if (b <= mn[ind].fr.fr) return;
		}
		if (lo <= L && R <= hi) {
			if (t == 0) {
				if (b > mx[ind].fr.sc) {
					mx_mod[ind] = b;
					push(ind, L, R);
					return;
				}
			} else if (t == 1) {
				if (b < mn[ind].fr.sc) {
					mn_mod[ind] = b;
					push(ind, L, R);
					return;
				}
			} else {
				mod[ind] = b;
				push(ind, L, R);
				return;
			}
		}
		int M = (L + R) / 2;
		upd(t, lo, hi, b, 2 * ind, L, M);
		upd(t, lo, hi, b, 2 * ind + 1, M + 1, R);
		pull(ind);
	}
	void chmin(int l, int r, ll x) { upd(0, l, r, x); }
	void chmax(int l, int r, ll x) { upd(1, l, r, x); }
	void add(int l, int r, ll x) { upd(2, l, r, x); }
};
