/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Implicit segment tree with lazy range updates.
 *  Creates children on demand. Bounds inclusive.
 *  Change $V$, $U$, $id$, $def$, $idU$, $binop$,
 *  $applyUpdate$, $mergeUpdate$. Default is range add /
 *  range max.
 * Time: $O(\log R)$
 * Status: untested
 */
#pragma once

struct SparseLazyNode {
	using V = int;
	using U = int;
	static const V id = INT_MIN;
	static const V def = 0;
	static const U idU = 0;
	V binop(V a, V b) { return max(a, b); }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	SparseLazyNode *lt = 0, *rt = 0;
	V val = def;
	U lazy = idU;
	void updateNode(int, int, U u) {
		lazy = mergeUpdate(lazy, u);
		val = applyUpdate(u, val);
	}
	void push(int l, int r) {
		if (!lt) {
			lt = new SparseLazyNode();
			rt = new SparseLazyNode();
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
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		push(l, r);
		int mid = l + (r - l) / 2;
		return binop(lt->query(l, mid, ql, qr),
			rt->query(mid + 1, r, ql, qr));
	}
	~SparseLazyNode() { delete lt; delete rt; }
};
