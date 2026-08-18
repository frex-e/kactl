#include "../utilities/template.h"

#include "../../content/data-structures/Treap.h"

pair<Node*, Node*> split2(Node* n, int v) {
	if (!n) return {};
	n->push();
	if (n->val >= v) {
		auto pa = split2(n->l, v);
		n->l = pa.second;
		n->recalc();
		return {pa.first, n};
	} else {
		auto pa = split2(n->r, v);
		n->r = pa.first;
		n->recalc();
		return {n, pa.second};
	}
}

int ra() {
	static unsigned x;
	x *= 4176481;
	x += 193861934;
	return x >> 1;
}

ll naiveSum(const vi& v, int l, int r) {
	ll s = 0;
	rep(i,l,r) s += v[i];
	return s;
}

void testLazy(int n, int iters) {
	vector<Node> nodes;
	vi exp(n);
	rep(i,0,n) {
		nodes.emplace_back(i);
		exp[i] = i;
	}
	Node* t = 0;
	rep(i,0,n) t = merge(t, &nodes[i]);

	auto rangeAdd = [&](int l, int r, ll x) {
		Node *a, *b, *c;
		tie(a, b) = split(t, l);
		tie(b, c) = split(b, r - l);
		if (b) b->applyAdd(x);
		t = merge(merge(a, b), c);
	};

	auto rangeSum = [&](int l, int r) -> ll {
		Node *a, *b, *c;
		tie(a, b) = split(t, l);
		tie(b, c) = split(b, r - l);
		ll res = b ? b->sum : 0;
		t = merge(merge(a, b), c);
		return res;
	};

	rep(it,0,iters) {
		int op = rand() % 3;
		if (op == 0) { // range add
			int l = rand() % (n + 1), r = rand() % (n + 1);
			if (l > r) swap(l, r);
			int x = rand() % 11 - 5;
			rangeAdd(l, r, x);
			rep(i,l,r) exp[i] += x;
		} else if (op == 1) { // range sum
			int l = rand() % (n + 1), r = rand() % (n + 1);
			if (l > r) swap(l, r);
			assert(rangeSum(l, r) == naiveSum(exp, l, r));
		} else {
			// move [i,j) to index k
			int i = rand() % (n + 1), j = rand() % (n + 1);
			if (i > j) swap(i, j);
			int k = rand() % (n + 1);
			if (i < k && k < j) continue;
			move(t, i, j, k);
			int nk = (k >= j ? k - (j - i) : k);
			vi iv(exp.begin() + i, exp.begin() + j);
			exp.erase(exp.begin() + i, exp.begin() + j);
			exp.insert(exp.begin() + nk, all(iv));
		}
		int ind = 0;
		each(t, [&](ll x) { assert(x == exp[ind++]); });
	}
}

int main() {
	srand(3);
	rep(it,0,1000) {
		vector<Node> nodes;
		vi exp;
		rep(i,0,10) {
			nodes.emplace_back(i*2+2);
			exp.emplace_back(i*2+2);
		}
		Node* n = 0;
		rep(i,0,10)
			n = merge(n, &nodes[i]);

		int v = rand() % 25;
		int left = cnt(split2(n, v).first);
		int rleft = (int)(lower_bound(all(exp), v) - exp.begin());
		assert(left == rleft);
	}

	rep(it,0,10000) {
		vector<Node> nodes;
		vi exp;
		rep(i,0,10) nodes.emplace_back(i);
		rep(i,0,10) exp.emplace_back(i);
		Node* n = 0;
		rep(i,0,10)
			n = merge(n, &nodes[i]);

		int i = ra() % 11, j = ra() % 11;
		if (i > j) swap(i, j);
		int k = ra() % 11;
		if (i < k && k < j) continue;

		move(n, i, j, k);

		int nk = (k >= j ? k - (j - i) : k);
		vi iv(exp.begin() + i, exp.begin() + j);
		exp.erase(exp.begin() + i, exp.begin() + j);
		exp.insert(exp.begin() + nk, all(iv));

		int ind = 0;
		each(n, [&](ll x) {
			assert(x == exp[ind++]);
		});
	}

	rep(n,1,30) testLazy(n, 5000);
	cout<<"Tests passed!"<<endl;
	return 0;
}
