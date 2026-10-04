/**
 * Author: OpenAI
 * Date: 2026-09-18
 * Source: folklore
 * Description: Unrooted dynamic forest, vertices $[0,N)$.
 *  Initially isolated with value 0. Link requires different
 *  components; cut requires an existing edge (either order).
 *  Point set/get and component update/query/size. Each tour
 *  stores one token per vertex and two zero-valued tokens per
 *  edge.
 *  Only vertex tokens contribute to sums. No path queries.
 *  Edge tokens are recycled. Change \texttt{V}, \texttt{U},
 *  \texttt{id}, \texttt{def}, \texttt{idU}, \texttt{binop},
 *  \texttt{applyUpdate}, \texttt{mergeUpdate}. Default is
 *  component add/sum. \texttt{binop} must be commutative and
 *  edge tokens must stay neutral. For a rooted subtree, cut
 *  its parent edge, operate on its component, then link back.
 * Time: Expected $O(\log N)$ per operation.
 * Memory: $O(N)$
 * Status: stress-tested
 */
#pragma once

struct EulerTourTree {
	using V = ll;
	using U = ll;
	static constexpr V id = 0;
	static constexpr V def = 0;
	static constexpr U idU = 0;
	V binop(V a, V b) { return a + b; }
	V applyUpdate(U u, V v, int len) { return v + u * len; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	struct Node {
		int l = 0, r = 0, p = 0, c = 1;
		int own = 0, cnt = 0; // Vertex token / vertex count.
		unsigned y = 0;
		V val = id, agg = id;
		U lazy = idU;
	};
	vector<Node> t;
	vi spare;
	map<pii, pii> edges;
	mt19937 rng{712367};
	EulerTourTree(int n) : t(n + 1) {
		t.reserve(3*n + 1); spare.reserve(2*n);
		t[0].c = 0;
		rep(i,1,n+1) {
			t[i].y = (unsigned)rng();
			t[i].own = t[i].cnt = 1;
			t[i].val = t[i].agg = def;
		}
	}
	void apply(int x, U u) {
		if (!x) return;
		t[x].val = applyUpdate(u, t[x].val, t[x].own);
		t[x].agg = applyUpdate(u, t[x].agg, t[x].cnt);
		t[x].lazy = mergeUpdate(t[x].lazy, u);
	}
	void push(int x) {
		if (t[x].lazy == idU) return;
		apply(t[x].l, t[x].lazy);
		apply(t[x].r, t[x].lazy);
		t[x].lazy = idU;
	}
	void pull(int x) {
		int l = t[x].l, r = t[x].r;
		t[x].c = 1 + t[l].c + t[r].c;
		t[x].cnt = t[x].own + t[l].cnt + t[r].cnt;
		t[x].agg = binop(binop(t[l].agg, t[x].val),
			t[r].agg);
		if (l) t[l].p = x;
		if (r) t[r].p = x;
	}
	int root(int x) {
		while (t[x].p) x = t[x].p;
		return x;
	}
	int rank(int x) {
		int k = t[t[x].l].c;
		for (int p; (p = t[x].p); x = p)
			if (t[p].r == x) k += t[t[p].l].c + 1;
		return k;
	}
	int merge(int a, int b) {
		if (!a || !b) {
			int x = a + b;
			if (x) t[x].p = 0;
			return x;
		}
		if (t[a].y < t[b].y) {
			push(b);
			t[b].l = merge(a, t[b].l);
			pull(b); t[b].p = 0; return b;
		}
		push(a);
		t[a].r = merge(t[a].r, b);
		pull(a); t[a].p = 0; return a;
	}
	pii split(int x, int k) { // First k tokens go left.
		if (!x) return {0, 0};
		push(x);
		int a, b;
		if (t[t[x].l].c >= k) {
			tie(a, b) = split(t[x].l, k);
			t[x].l = b; b = x;
		} else {
			tie(a, b) = split(t[x].r, k-t[t[x].l].c-1);
			t[x].r = a; a = x;
		}
		pull(x);
		if (a) t[a].p = 0;
		if (b) t[b].p = 0;
		return {a, b};
	}
	int rotate(int x) { // Start the cyclic tour at x.
		auto [a, b] = split(root(x), rank(x));
		return merge(b, a);
	}
	int token() {
		int x;
		if (spare.empty()) x = sz(t), t.emplace_back();
		else x = spare.back(), spare.pop_back();
		t[x] = Node{}; t[x].y = (unsigned)rng();
		return x;
	}
	bool connected(int u, int v) {
		return root(u + 1) == root(v + 1);
	}
	void link(int u, int v) {
		assert(!connected(u, v));
		int a = rotate(u + 1), b = rotate(v + 1);
		int x = token(), y = token();
		edges[minmax(u, v)] = {x, y};
		merge(merge(a, x), merge(b, y));
	}
	void cut(int u, int v) {
		auto it = edges.find(minmax(u, v));
		assert(it != edges.end());
		auto [x, y] = it->second;
		int r = rotate(x);
		auto [a, b] = split(r, rank(y));
		split(a, 1); split(b, 1); // Drop x and y.
		spare.push_back(x); spare.push_back(y);
		edges.erase(it);
	}
	void expose(int x) { // Push ancestors before point access.
		if (t[x].p) expose(t[x].p);
		push(x);
	}
	void set(int u, V val) {
		int x = u + 1;
		expose(x);
		t[x].val = val;
		for (; x; x = t[x].p) pull(x);
	}
	V get(int u) { expose(u + 1); return t[u + 1].val; }
	void componentUpdate(int u, U upd) {
		apply(root(u + 1), upd);
	}
	V componentQuery(int u) { return t[root(u+1)].agg; }
	int componentSize(int u) {
		return t[root(u+1)].cnt;
	}
};
