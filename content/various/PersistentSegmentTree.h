/**
 * Author: me
 * Date: 2026-08-18
 * License: CC0
 * Source: folklore
 * Description: Persistent implicit segment tree with lazy
 *  range updates and point set. Update/set return a new
 *  root; old roots stay valid. No destructor (nodes are
 *  shared). Bounds inclusive. Change \texttt{V}, \texttt{U},
 *  \texttt{id}, \texttt{def}, \texttt{idU}, \texttt{binop},
 *  \texttt{applyUpdate}, \texttt{mergeUpdate}. Default is
 *  range add / range max.
 * Time: $O(\log R)$
 * Status: stress-tested
 */
#pragma once

struct Node {
	using V = int;
	using U = int;
	using T = Node;
	static constexpr V id = INT_MIN;
	static constexpr V def = 0;
	static constexpr U idU = 0;
	V binop(V a, V b) { return max(a, b); }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	T *lt = 0, *rt = 0;
	V val = def;
	U lazy = idU;
	void updateNode(int, int, U u) {
		lazy = mergeUpdate(lazy, u);
		val = applyUpdate(u, val);
	}
	void push(int l, int r) {
		int mid = l + (r - l) / 2;
		lt = new T(lt ? *lt : T());
		rt = new T(rt ? *rt : T());
		lt->updateNode(l, mid, lazy);
		rt->updateNode(mid + 1, r, lazy);
		lazy = idU;
	}
	T* update(int l, int r, int ql, int qr, U u) {
		if (qr < l || r < ql) return this;
		T* n = new T(*this);
		if (ql <= l && r <= qr) {
			n->updateNode(l, r, u);
			return n;
		}
		n->push(l, r);
		int mid = l + (r - l) / 2;
		n->lt = n->lt->update(l, mid, ql, qr, u);
		n->rt = n->rt->update(mid + 1, r, ql, qr, u);
		n->val = binop(n->lt->val, n->rt->val);
		return n;
	}
	T* set(int l, int r, int i, V v) {
		if (i < l || r < i) return this;
		T* n = new T(*this);
		if (l == r) { n->val = v; n->lazy = idU; return n; }
		n->push(l, r);
		int mid = l + (r - l) / 2;
		if (i <= mid) n->lt = n->lt->set(l, mid, i, v);
		else n->rt = n->rt->set(mid + 1, r, i, v);
		n->val = binop(n->lt->val, n->rt->val);
		return n;
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		int mid = l + (r - l) / 2;
		T a = lt ? *lt : T();
		T b = rt ? *rt : T();
		a.updateNode(l, mid, lazy);
		b.updateNode(mid + 1, r, lazy);
		return binop(a.query(l, mid, ql, qr),
			b.query(mid + 1, r, ql, qr));
	}
};
