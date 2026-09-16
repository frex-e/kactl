/**
 * Author: folklore
 * Date: 2026-09-16
 * License: CC0
 * Source: folklore
 * Description: Unrooted link-cut tree. Path sum and lazy
 *  path add on nodes. Keep it unrooted: \texttt{makeRoot}
 *  reroots by reversing a path, which is what you want
 *  for arbitrary $u$--$v$ paths. After
 *  \texttt{makeRoot(u); access(v);} the splay at $v$ is
 *  the $u$--$v$ path (read \texttt{t[v].sum} or
 *  \texttt{apply}). Change \texttt{pull}/\texttt{apply}
 *  for min/max. Point set: \texttt{access} then
 *  \texttt{val}. $p$ is the splay parent, or the path
 *  parent if \texttt{!nroot}. Subtree queries need
 *  virtual-child data (not included). Static tree: HLD.
 *  Offline connectivity: DynCon. Edge weights: extra
 *  nodes. Rooted LCA: \texttt{lca(r,u,v)}.
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
	struct N {
		int c[2] = {-1, -1}, p = -1, sz = 1;
		int flip = 0;
		ll val = 0, sum = 0, add = 0;
	};
	vector<N> t;
	LinkCut(int n) : t(n) {}
	bool nroot(int x) {
		int p = t[x].p;
		return p != -1 &&
			(t[p].c[0] == x || t[p].c[1] == x);
	}
	void pull(int x) { // path aggregate
		int a = t[x].c[0], b = t[x].c[1];
		t[x].sz = 1 + (a<0?0:t[a].sz) + (b<0?0:t[b].sz);
		t[x].sum = t[x].val + (a<0?0:t[a].sum) +
			(b<0?0:t[b].sum);
	}
	void apply(int x, ll v) { // path add
		t[x].val += v; t[x].sum += v * t[x].sz;
		t[x].add += v;
	}
	void push(int x) {
		int a = t[x].c[0], b = t[x].c[1];
		if (t[x].flip) {
			swap(t[x].c[0], t[x].c[1]);
			if (a>=0) t[a].flip ^= 1;
			if (b>=0) t[b].flip ^= 1;
			t[x].flip = 0;
		}
		if (t[x].add) {
			if (a>=0) apply(a, t[x].add);
			if (b>=0) apply(b, t[x].add);
			t[x].add = 0;
		}
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
	void access(int x) {
		int last = -1;
		for (int y = x; y>=0; y = t[y].p) {
			splay(y); t[y].c[1] = last;
			pull(y); last = y;
		}
		splay(x);
	}
	void makeRoot(int x) {
		access(x); t[x].flip ^= 1;
	}
	int findRoot(int x) {
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
	ll query(int u, int v) { // path sum
		makeRoot(u); access(v);
		return t[v].sum;
	}
	void update(int u, int v, ll x) { // path add
		makeRoot(u); access(v); apply(v, x);
	}
	void set(int u, ll x) {
		access(u); t[u].val = x; pull(u);
	}
	ll get(int u) {
		access(u); return t[u].val;
	}
	int lca(int r, int u, int v) { // wrt root r
		makeRoot(r); access(u); access(v);
		splay(u);
		return t[u].p<0 ? u : t[u].p;
	}
};
