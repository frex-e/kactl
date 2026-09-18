/**
 * Author: someone on Codeforces
 * Date: 2026-08-18
 * Source: folklore
 * Description: Implicit treap on a sequence. Split/merge by
 *  index. \texttt{split(t, k)} puts the first $k$ elements
 *  on the left. Lazy range updates and queries. Half-open
 *  index ranges $[l,r)$. Change \texttt{V}, \texttt{U},
 *  \texttt{id}, \texttt{def}, \texttt{idU}, \texttt{binop},
 *  \texttt{applyUpdate}, \texttt{mergeUpdate}. Default is
 *  range add/sum.
 * Time: $O(\log N)$
 * Status: stress-tested
 */
#pragma once

struct Node {
	using V = ll;
	using U = ll;
	static constexpr V id = 0;
	static constexpr V def = 0;
	static constexpr U idU = 0;
	V binop(V a, V b) { return a + b; }
	V applyUpdate(U u, V v, int len) { return v + u * len; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	Node *l = 0, *r = 0;
	V val = def, agg = def;
	int y, c = 1;
	U lazy = idU;
	Node(V val = def) : val(val), agg(val), y(rand()) {}
	void apply(U u) {
		val = applyUpdate(u, val, 1);
		agg = applyUpdate(u, agg, c);
		lazy = mergeUpdate(lazy, u);
	}
	void push() {
		if (lazy == idU) return;
		if (l) l->apply(lazy);
		if (r) r->apply(lazy);
		lazy = idU;
	}
	void pull();
};

int cnt(Node* n) { return n ? n->c : 0; }
Node::V query(Node* n) { return n ? n->agg : Node::id; }
void Node::pull() {
	c = cnt(l) + cnt(r) + 1;
	agg = binop(binop(query(l), val), query(r));
}

template<class F> void each(Node* n, F f) {
	if (!n) return;
	n->push(); each(n->l, f);
	f(n->val); each(n->r, f);
}
pair<Node*, Node*> split(Node* n, int k) { // left gets k nodes
	if (!n) return {};
	n->push();
	if (cnt(n->l) >= k) { // "n->val >= k" for lower_bound(k)
		auto [L,R] = split(n->l, k);
		n->l = R;
		n->pull();
		return {L, n};
	} else {
		auto [L,R] = split(n->r,k - cnt(n->l) - 1); // and just "k"
		n->r = L;
		n->pull();
		return {n, R};
	}
}

Node* merge(Node* l, Node* r) {
	if (!l) return r;
	if (!r) return l;
	if (l->y > r->y) {
		l->push();
		l->r = merge(l->r, r);
		return l->pull(), l;
	} else {
		r->push();
		r->l = merge(l, r->l);
		return r->pull(), r;
	}
}

Node* ins(Node* t, Node* n, int pos) {
	auto [l,r] = split(t, pos);
	return merge(merge(l, n), r);
}

// Example application: move the range [l, r) to index k
void move(Node*& t, int l, int r, int k) {
	Node *a, *b, *c;
	tie(a,b) = split(t, l); tie(b,c) = split(b, r - l);
	if (k <= l) t = merge(ins(a, b, k), c);
	else t = merge(a, ins(c, b, k - r));
}
