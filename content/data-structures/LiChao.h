/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Li Chao tree for lines $ax+b$ on an integer
 *  domain $[L,R)$. Queries minimum. Negate $a,b$ and the
 *  result for maximum. Unlike LineContainer, lines can be
 *  restricted to a subrange by inserting on a node range.
 * Time: $O(\log(R-L))$
 * Status: untested
 */
#pragma once

struct LiChao {
	struct Fn {
		ll a, b;
		Fn(ll a = 0, ll b = 0) : a(a), b(b) {}
		ll operator()(ll x) const { return a * x + b; }
	};
	static const ll INF = LLONG_MAX / 4;
	struct Node {
		Fn f{0, INF};
		Node *l = 0, *r = 0;
	};
	Node *root = new Node();
	int L, R;
	LiChao(int L, int R) : L(L), R(R) {}
	void insert(Fn nw, Node *n, int l, int r) {
		if (!n) return;
		int m = l + (r - l) / 2;
		bool lef = nw(l) < n->f(l);
		bool mid = nw(m) < n->f(m);
		if (mid) swap(nw, n->f);
		if (l == r - 1) return;
		Node *&ch = lef != mid ? n->l : n->r;
		if (!ch) ch = new Node();
		insert(nw, ch, lef != mid ? l : m,
			lef != mid ? m : r);
	}
	void insert(Fn nw) { insert(nw, root, L, R); }
	ll query(int x, Node *n, int l, int r) {
		if (!n || l > x || r <= x) return INF;
		int m = l + (r - l) / 2;
		ll res = n->f(x);
		return x < m ? min(res, query(x, n->l, l, m))
			: min(res, query(x, n->r, m, r));
	}
	ll query(int x) { return query(x, root, L, R); }
};
