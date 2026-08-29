/**
 * Author: me
 * Date: 2026-08-18
 * License: CC0
 * Source: me
 * Description: Implicit segment tree with lazy range updates
 *  and point set. Creates children on demand. Bounds inclusive.
 *  Change \texttt{V}, \texttt{U}, \texttt{id}, \texttt{def},
 *  \texttt{idU}, \texttt{binop}, \texttt{applyUpdate},
 *  \texttt{mergeUpdate}. \texttt{seglen} is the node length
 *  during \texttt{applyUpdate}. Default is range add / max.
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
	int seglen = 1;
	void updateNode(int l, int r, U u) {
		seglen = r - l + 1;
		lazy = mergeUpdate(lazy, u);
		val = applyUpdate(u, val);
	}
	void push(int l, int r) {
		if (!lt) {
			lt = new T();
			rt = new T();
		}
		int mid = l + (r - l) / 2;
		lt->updateNode(l, mid, lazy);
		rt->updateNode(mid + 1, r, lazy);
		lazy = idU;
	}
	void update(int l, int r, int ql, int qr, U u) {
		if (qr < l || r < ql) return;
		if (ql <= l && r <= qr) {
			updateNode(l, r, u);
			return;
		}
		push(l, r);
		int mid = l + (r - l) / 2;
		lt->update(l, mid, ql, qr, u);
		rt->update(mid + 1, r, ql, qr, u);
		val = binop(lt->val, rt->val);
	}
	void set(int l, int r, int i, V v) {
		if (i < l || r < i) return;
		if (l == r) { val = v; lazy = idU; return; }
		push(l, r);
		int mid = l + (r - l) / 2;
		if (i <= mid) lt->set(l, mid, i, v);
		else rt->set(mid + 1, r, i, v);
		val = binop(lt->val, rt->val);
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		push(l, r);
		int mid = l + (r - l) / 2;
		return binop(lt->query(l, mid, ql, qr),
			rt->query(mid + 1, r, ql, qr));
	}
	~Node() { delete lt; delete rt; }
};
