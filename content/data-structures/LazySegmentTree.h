/**
 * Author: me
 * Date: 2026-08-18
 * License: CC0
 * Source: mine.typ
 * Description: Recursive segment tree with range updates,
 *  point set, and range queries. Bounds inclusive on both
 *  sides. Change \texttt{V}, \texttt{U}, \texttt{id},
 *  \texttt{def}, \texttt{idU}, \texttt{binop},
 *  \texttt{applyUpdate}, \texttt{mergeUpdate}. Default is
 *  range add and range max. Used by HLD.
 * Time: $O(\log N)$
 * Status: stress-tested
 */
#pragma once

struct LazyUpdateTree {
	using V = int;
	using U = int;
	const V id = INT_MIN;
	const V def = 0;
	const U idU = 0;
	V binop(V a, V b) { return max(a, b); }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	vector<V> arr;
	vector<U> lazy;
	int size;
	LazyUpdateTree(int n) :
		arr(4 * n + 2, def), lazy(4 * n + 2, idU), size(n) {}
	void updateNode(int cur, int, int, U u) {
		lazy[cur] = mergeUpdate(lazy[cur], u);
		arr[cur] = applyUpdate(u, arr[cur]);
	}
	void push(int cur, int l, int r) {
		int mid = l + (r - l) / 2;
		updateNode(2 * cur, l, mid, lazy[cur]);
		updateNode(2 * cur + 1, mid + 1, r, lazy[cur]);
		lazy[cur] = idU;
	}
	void update(int cur, int l, int r, int ql, int qr, U u) {
		if (qr < l || r < ql) return;
		if (ql <= l && r <= qr) {
			updateNode(cur, l, r, u); return;
		}
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		update(2 * cur, l, mid, ql, qr, u);
		update(2 * cur + 1, mid + 1, r, ql, qr, u);
		arr[cur] = binop(arr[2 * cur], arr[2 * cur + 1]);
	}
	void update(int ql, int qr, U u) {
		update(1, 0, size - 1, ql, qr, u);
	}
	void set(int cur, int l, int r, int i, V v) {
		if (i < l || r < i) return;
		if (l == r) { arr[cur] = v; lazy[cur] = idU; return; }
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		if (i <= mid) set(2 * cur, l, mid, i, v);
		else set(2 * cur + 1, mid + 1, r, i, v);
		arr[cur] = binop(arr[2 * cur], arr[2 * cur + 1]);
	}
	void set(int i, V v) { set(1, 0, size - 1, i, v); }
	V query(int cur, int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return arr[cur];
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		return binop(query(2 * cur, l, mid, ql, qr),
			query(2 * cur + 1, mid + 1, r, ql, qr));
	}
	V query(int ql, int qr) {
		return query(1, 0, size - 1, ql, qr);
	}
};
