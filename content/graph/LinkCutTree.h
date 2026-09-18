/**
 * Author: folklore
 * Date: 2026-09-16
 * License: CC0
 * Source: folklore
 * Description: Unrooted link-cut tree. Online link/cut.
 *  Default path add + path sum.
 * Time: amortized $O(\log N)$
 * Status: stress-tested
 * Usage:
 *  LinkCut lc(n);
 *  lc.link(u, v); lc.cut(u, v);
 *  lc.connected(u, v); lc.query(u, v);
 *  lc.update(u, v, x); lc.set(u, x);
 */
#pragma once

struct LinkCut {
	// Path LCT. Not component/subtree update (ETT),
	// not a static tree (HLD). Subtree query needs vsub.
	using V = ll;
	using U = ll;
	static constexpr V id = 0, def = 0;
	static constexpr U idU = 0;
	V binop(V a, V b) { return a + b; }
	V rev(V a) { return a; } // non-commutative: reverse
	V applyUpdate(U u, V v, int c) {
		return v + u * c;
	}
	U mergeUpdate(U a, U b) { return a + b; }
	struct N {
		int c[2] = {-1, -1}, p = -1, sz = 1;
		int flip = 0; // reverse; keep
		V val = def, agg = def;
		U lz = idU;
	};
	vector<N> t;
	LinkCut(int n) : t(n) {}
	bool nroot(int x) { // else p is path-parent
		int p = t[x].p;
		return p != -1 &&
			(t[p].c[0] == x || t[p].c[1] == x);
	}
	V aggOf(int x) { return x<0 ? id : t[x].agg; }
	void pull(int x) { // path aggregate
		int a = t[x].c[0], b = t[x].c[1];
		t[x].sz = 1 + (a<0?0:t[a].sz) + (b<0?0:t[b].sz);
		t[x].agg = binop(binop(aggOf(a), t[x].val),
			aggOf(b));
	}
	void apply(int x, U u) { // path add
		t[x].val = applyUpdate(u, t[x].val, 1);
		t[x].agg = applyUpdate(u, t[x].agg, t[x].sz);
		t[x].lz = mergeUpdate(t[x].lz, u);
	}
	void pushFlip(int x) { // structural; not a value tag
		if (!t[x].flip) return;
		swap(t[x].c[0], t[x].c[1]);
		t[x].agg = rev(t[x].agg);
		int a = t[x].c[0], b = t[x].c[1];
		if (a>=0) t[a].flip ^= 1, t[a].agg = rev(t[a].agg);
		if (b>=0) t[b].flip ^= 1, t[b].agg = rev(t[b].agg);
		t[x].flip = 0;
	}
	void pushLazy(int x) { // skip if idU
		U u = t[x].lz;
		if (u == idU) return;
		int a = t[x].c[0], b = t[x].c[1];
		if (a>=0) apply(a, u);
		if (b>=0) apply(b, u);
		t[x].lz = idU;
	}
	void push(int x) {
		pushFlip(x); pushLazy(x);
	}
	void rot(int x) {
		int y = t[x].p, z = t[y].p;
		int k = t[y].c[1] == x;
		if (nroot(y)) t[z].c[t[z].c[1]==y] = x;
		int w = t[x].c[k^1];
		t[y].c[k] = w;
		if (w>=0) t[w].p = y;
		t[x].c[k^1] = y;
		t[y].p = x; t[x].p = z;
		pull(y);
	}
	void splay(int x) {
		vi stk = {x};
		for (int y = x; nroot(y); )
			stk.push_back(y = t[y].p);
		while (!stk.empty())
			push(stk.back()), stk.pop_back();
		while (nroot(x)) {
			int y = t[x].p, z = t[y].p;
			if (nroot(y))
				rot((t[y].c[0]==x)==(t[z].c[0]==y)?y:x);
			rot(x);
		}
		pull(x);
	}
	void access(int x) { // preferred path through x
		int last = -1;
		for (int y = x; y>=0; y = t[y].p) {
			splay(y); t[y].c[1] = last;
			pull(y); last = y;
		}
		splay(x);
	}
	void makeRoot(int x) { // evert; why this is unrooted
		access(x); t[x].flip ^= 1;
	}
	int findRoot(int x) { // current evert; use connected
		access(x);
		for (;;) {
			push(x);
			if (t[x].c[0]<0) break;
			x = t[x].c[0];
		}
		splay(x);
		return x;
	}
	void link(int u, int v) { // add edge (u, v)
		assert(!connected(u, v));
		makeRoot(u); t[u].p = v;
	}
	void cut(int u, int v) { // remove edge (u, v)
		makeRoot(u); access(v);
		assert(t[v].c[0]==u && t[u].c[0]<0 &&
			t[u].c[1]<0);
		t[v].c[0] = t[u].p = -1;
		pull(v);
	}
	bool connected(int u, int v) {
		return findRoot(u) == findRoot(v);
	}
	// makeRoot(u); access(v); => v's splay is path u-v
	V query(int u, int v) {
		makeRoot(u); access(v);
		return t[v].agg;
	}
	void update(int u, int v, U x) {
		makeRoot(u); access(v); apply(v, x);
	}
	void set(int u, V x) {
		access(u); t[u].val = x; t[u].lz = idU;
		pull(u);
	}
	V get(int u) {
		access(u); return t[u].val;
	}
	int lca(int r, int u, int v) { // wrt root r
		makeRoot(r); access(u); access(v);
		splay(u);
		return t[u].p<0 ? u : t[u].p;
	}
};
