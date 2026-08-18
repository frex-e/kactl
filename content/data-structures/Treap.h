/**
 * Author: someone on Codeforces
 * Date: 2026-08-18
 * Source: folklore
 * Description: Implicit treap on a sequence. Split/merge by
 *  index in $O(\log N)$. Subtree sum, lazy range add, lazy
 *  range reverse. Half-open index ranges $[l,r)$.
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
	bool rev = false;
	Node(ll val) : val(val), y(rand()), sum(val) {}
	void applyAdd(ll x) {
		val += x; sum += x * c; add += x;
	}
	void applyRev() { rev ^= 1; }
	void push() {
		if (rev) {
			swap(l, r);
			if (l) l->applyRev();
			if (r) r->applyRev();
			rev = false;
		}
		if (add) {
			if (l) l->applyAdd(add);
			if (r) r->applyAdd(add);
			add = 0;
		}
	}
	void recalc();
};

int cnt(Node* n) { return n ? n->c : 0; }
void Node::recalc() {
	c = cnt(l) + cnt(r) + 1;
	sum = val + (l ? l->sum : 0) + (r ? r->sum : 0);
}

template<class F> void each(Node* n, F f) {
	if (n) { n->push(); each(n->l, f); f(n->val); each(n->r, f); }
}
pair<Node*, Node*> split(Node* n, int k) {
	if (!n) return {};
	n->push();
	if (cnt(n->l) >= k) {
		auto [L,R] = split(n->l, k);
		n->l = R;
		n->recalc();
		return {L, n};
	} else {
		auto [L,R] = split(n->r,k - cnt(n->l) - 1);
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

Node* ins(Node* t, Node* n, int pos) {
	auto [l,r] = split(t, pos);
	return merge(merge(l, n), r);
}

void rangeAdd(Node*& t, int l, int r, ll x) {
	Node *a, *b, *c;
	tie(a, b) = split(t, l); tie(b, c) = split(b, r - l);
	if (b) b->applyAdd(x);
	t = merge(merge(a, b), c);
}

void rangeRev(Node*& t, int l, int r) {
	Node *a, *b, *c;
	tie(a, b) = split(t, l); tie(b, c) = split(b, r - l);
	if (b) b->applyRev();
	t = merge(merge(a, b), c);
}

ll rangeSum(Node*& t, int l, int r) {
	Node *a, *b, *c;
	tie(a, b) = split(t, l); tie(b, c) = split(b, r - l);
	ll res = b ? b->sum : 0;
	t = merge(merge(a, b), c);
	return res;
}

// Example application: move the range [l, r) to index k
void move(Node*& t, int l, int r, int k) {
	Node *a, *b, *c;
	tie(a,b) = split(t, l); tie(b,c) = split(b, r - l);
	if (k <= l) t = merge(ins(a, b, k), c);
	else t = merge(a, ins(c, b, k - r));
}
