/**
 * Author: someone on Codeforces
 * Date: 2026-08-18
 * Source: folklore
 * Description: Implicit treap on a sequence. Split/merge by
 *  index in $O(\log N)$. Subtree sum and lazy range add.
 *  Half-open index ranges $[l,r)$.
 * Time: $O(\log N)$
 * Status: stress-tested
 */
#pragma once

struct Node {
	Node *l = 0, *r = 0;
	ll val;
	int y, c = 1;
	ll sum = 0;
	ll add = 0;
	Node(ll val) : val(val), y(rand()), sum(val) {}
	void applyAdd(ll x) {
		val += x; sum += x * c; add += x;
	}
	void push() {
		if (add) {
			if (l) l->applyAdd(add);
			if (r) r->applyAdd(add);
			add = 0;
		}
	}
	void recalc();
};

int cnt(Node* n) { return n ? n->c : 0; }
ll lsum(Node* n) { return n ? n->sum : 0; }
void Node::recalc() {
	c = cnt(l) + cnt(r) + 1;
	sum = val + lsum(l) + lsum(r);
}

template<class F> void each(Node* n, F f) {
	if (n) { n->push(); each(n->l, f); f(n->val); each(n->r, f); }
}
pair<Node*, Node*> split(Node* n, int k) {
	if (!n) return {};
	n->push();
	if (cnt(n->l) >= k) { // "n->val >= k" for lower_bound(k)
		auto [L,R] = split(n->l, k);
		n->l = R;
		n->recalc();
		return {L, n};
	} else {
		auto [L,R] = split(n->r,k - cnt(n->l) - 1); // and just "k"
		n->r = L;
		n->recalc();
		return {n, R};
	}
}

Node* merge(Node* l, Node* r) {
	if (!l) return r;
	if (!r) return l;
	if (l->y > r->y) {
		l->push();
		l->r = merge(l->r, r);
		return l->recalc(), l;
	} else {
		r->push();
		r->l = merge(l, r->l);
		return r->recalc(), r;
	}
}

// Example application: move the range [l, r) to index k
void move(Node*& t, int l, int r, int k) {
	Node *a, *b, *c;
	tie(a,b) = split(t, l); tie(b,c) = split(b, r - l);
	if (k <= l) {
		Node *a1, *a2;
		tie(a1, a2) = split(a, k);
		t = merge(merge(a1, b), merge(a2, c));
	} else {
		Node *c1, *c2;
		tie(c1, c2) = split(c, k - r);
		t = merge(a, merge(merge(c1, b), c2));
	}
}
