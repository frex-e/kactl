/**
 * Author: me
 * Date: 2026-08-20
 * License: CC0
 * Source: https://usaco.guide/adv/segtree-beats
 * Description: Segment tree beats. Range
 *  \texttt{chmin}/\texttt{chmax}/\texttt{add} and range
 *  sum/min/max. Bounds inclusive, 0-indexed.
 * Time: $O(\log^2 N)$ amortized
 * Status: stress-tested
 */
#pragma once

struct SegmentTreeBeats {
	static constexpr ll INF = LLONG_MAX / 4;
	struct Node {
		ll sum = 0, lazy = 0;
		ll mx1 = 0, mx2 = -INF, mxc = 0;
		ll mn1 = 0, mn2 = INF, mnc = 0;
	};
	vector<Node> t;
	int n;

	SegmentTreeBeats(int n) : t(4 * n + 4), n(n) {
		build(1, 0, n - 1, nullptr);
	}
	SegmentTreeBeats(const vector<ll>& a) :
		t(4 * sz(a) + 4), n(sz(a)) {
		build(1, 0, n - 1, &a);
	}

	void pull(int i) {
		Node &u = t[i], &l = t[i << 1], &r = t[i << 1 | 1];
		u.sum = l.sum + r.sum;
		if (l.mx1 == r.mx1) {
			u.mx1 = l.mx1;
			u.mx2 = max(l.mx2, r.mx2);
			u.mxc = l.mxc + r.mxc;
		} else if (l.mx1 > r.mx1) {
			u.mx1 = l.mx1;
			u.mx2 = max(l.mx2, r.mx1);
			u.mxc = l.mxc;
		} else {
			u.mx1 = r.mx1;
			u.mx2 = max(l.mx1, r.mx2);
			u.mxc = r.mxc;
		}
		if (l.mn1 == r.mn1) {
			u.mn1 = l.mn1;
			u.mn2 = min(l.mn2, r.mn2);
			u.mnc = l.mnc + r.mnc;
		} else if (l.mn1 < r.mn1) {
			u.mn1 = l.mn1;
			u.mn2 = min(l.mn2, r.mn1);
			u.mnc = l.mnc;
		} else {
			u.mn1 = r.mn1;
			u.mn2 = min(l.mn1, r.mn2);
			u.mnc = r.mnc;
		}
	}
	void applyAdd(int i, int l, int r, ll v) {
		if (!v) return;
		Node& u = t[i];
		u.sum += (r - l + 1) * v;
		u.mx1 += v;
		if (u.mx2 != -INF) u.mx2 += v;
		u.mn1 += v;
		if (u.mn2 != INF) u.mn2 += v;
		u.lazy += v;
	}
	void applyChmin(int i, ll v, bool leaf) {
		Node& u = t[i];
		if (v >= u.mx1) return;
		u.sum -= (u.mx1 - v) * u.mxc;
		u.mx1 = v;
		if (leaf) u.mn1 = v;
		else if (v <= u.mn1) u.mn1 = v;
		else if (v < u.mn2) u.mn2 = v;
	}
	void applyChmax(int i, ll v, bool leaf) {
		Node& u = t[i];
		if (v <= u.mn1) return;
		u.sum += (v - u.mn1) * u.mnc;
		u.mn1 = v;
		if (leaf) u.mx1 = v;
		else if (v >= u.mx1) u.mx1 = v;
		else if (v > u.mx2) u.mx2 = v;
	}
	void push(int i, int l, int r) {
		if (l == r) return;
		int m = (l + r) >> 1;
		applyAdd(i << 1, l, m, t[i].lazy);
		applyAdd(i << 1 | 1, m + 1, r, t[i].lazy);
		t[i].lazy = 0;
		applyChmin(i << 1, t[i].mx1, l == m);
		applyChmin(i << 1 | 1, t[i].mx1, m + 1 == r);
		applyChmax(i << 1, t[i].mn1, l == m);
		applyChmax(i << 1 | 1, t[i].mn1, m + 1 == r);
	}

	void build(int i, int l, int r, const vector<ll>* a) {
		t[i].lazy = 0;
		if (l == r) {
			ll v = a ? (*a)[l] : 0;
			t[i].sum = t[i].mx1 = t[i].mn1 = v;
			t[i].mxc = t[i].mnc = 1;
			t[i].mx2 = -INF;
			t[i].mn2 = INF;
			return;
		}
		int m = (l + r) >> 1;
		build(i << 1, l, m, a);
		build(i << 1 | 1, m + 1, r, a);
		pull(i);
	}

	void add(int ql, int qr, ll v,
		int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql) return;
		if (ql <= l && r <= qr) {
			applyAdd(i, l, r, v); return;
		}
		push(i, l, r);
		int m = (l + r) >> 1;
		add(ql, qr, v, i << 1, l, m);
		add(ql, qr, v, i << 1 | 1, m + 1, r);
		pull(i);
	}
	void chmin(int ql, int qr, ll v,
		int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql || v >= t[i].mx1) return;
		if (ql <= l && r <= qr && v > t[i].mx2) {
			applyChmin(i, v, l == r); return;
		}
		push(i, l, r);
		int m = (l + r) >> 1;
		chmin(ql, qr, v, i << 1, l, m);
		chmin(ql, qr, v, i << 1 | 1, m + 1, r);
		pull(i);
	}
	void chmax(int ql, int qr, ll v,
		int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql || v <= t[i].mn1) return;
		if (ql <= l && r <= qr && v < t[i].mn2) {
			applyChmax(i, v, l == r); return;
		}
		push(i, l, r);
		int m = (l + r) >> 1;
		chmax(ql, qr, v, i << 1, l, m);
		chmax(ql, qr, v, i << 1 | 1, m + 1, r);
		pull(i);
	}

	ll qsum(int ql, int qr, int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql) return 0;
		if (ql <= l && r <= qr) return t[i].sum;
		push(i, l, r);
		int m = (l + r) >> 1;
		return qsum(ql, qr, i << 1, l, m)
			+ qsum(ql, qr, i << 1 | 1, m + 1, r);
	}
	ll qmax(int ql, int qr, int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql) return -INF;
		if (ql <= l && r <= qr) return t[i].mx1;
		push(i, l, r);
		int m = (l + r) >> 1;
		return max(qmax(ql, qr, i << 1, l, m),
			qmax(ql, qr, i << 1 | 1, m + 1, r));
	}
	ll qmin(int ql, int qr, int i = 1, int l = 0, int r = -1) {
		if (r == -1) r = n - 1;
		if (qr < l || r < ql) return INF;
		if (ql <= l && r <= qr) return t[i].mn1;
		push(i, l, r);
		int m = (l + r) >> 1;
		return min(qmin(ql, qr, i << 1, l, m),
			qmin(ql, qr, i << 1 | 1, m + 1, r));
	}
};
