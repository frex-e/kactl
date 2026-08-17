/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Implicit (pointer) segment tree. Creates
 *  children on demand. Point update, range query.
 *  Bounds inclusive. Change \texttt{V}, \texttt{id}, \texttt{binop}.
 * Time: $O(\log R)$
 * Status: stress-tested
 */
#pragma once

struct SparseNode {
	using V = int;
	static const V id = 0;
	V binop(V a, V b) { return a + b; }
	V val = id;
	SparseNode *lt = 0, *rt = 0;
	void push() {
		if (!lt) { lt = new SparseNode(); rt = new SparseNode(); }
	}
	void update(int l, int r, int i, V v) {
		if (i < l || r < i) return;
		if (l == r && r == i) { val = v; return; }
		push();
		int mid = l + (r - l) / 2;
		lt->update(l, mid, i, v);
		rt->update(mid + 1, r, i, v);
		val = binop(lt->val, rt->val);
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		push();
		int mid = l + (r - l) / 2;
		return binop(lt->query(l, mid, ql, qr),
			rt->query(mid + 1, r, ql, qr));
	}
	~SparseNode() { delete lt; delete rt; }
};
