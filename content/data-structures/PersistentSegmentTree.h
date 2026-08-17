/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: folklore
 * Description: Persistent implicit segment tree. Same
 *  inclusive bounds and \texttt{binop} as
 *  SparseSegmentTree. Point set, range query. Update
 *  returns a new root; old roots stay valid. No destructor
 *  (nodes are shared).
 * Time: $O(\log R)$
 * Status: stress-tested
 */
#pragma once

struct PersistNode {
	using V = int;
	static const V id = 0;
	V binop(V a, V b) { return a + b; }
	V val = id;
	PersistNode *lt = 0, *rt = 0;
	PersistNode* update(int l, int r, int i, V v) {
		PersistNode* n = new PersistNode(*this);
		if (l == r) { n->val = v; return n; }
		int m = l + (r - l) / 2;
		if (i <= m)
			n->lt = (lt ? lt : new PersistNode())
				->update(l, m, i, v);
		else
			n->rt = (rt ? rt : new PersistNode())
				->update(m + 1, r, i, v);
		n->val = binop(n->lt ? n->lt->val : id,
			n->rt ? n->rt->val : id);
		return n;
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		int m = l + (r - l) / 2;
		return binop(
			lt ? lt->query(l, m, ql, qr) : id,
			rt ? rt->query(m + 1, r, ql, qr) : id);
	}
};
