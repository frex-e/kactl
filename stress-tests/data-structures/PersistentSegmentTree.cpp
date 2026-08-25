#include "../utilities/template.h"

#include "../../content/data-structures/LazySegmentTree.h"

namespace sparse {
#include "../../content/data-structures/SparseLazySegmentTree.h"
}
namespace persist {
#include "../../content/data-structures/PersistentSegmentTree.h"
}

using persist::Node;
using Sparse = sparse::Node;

static_assert(Node::id == INT_MIN);

void testSmall(int n, int iters) {
	vector<Node*> roots = {new Node()};
	vector<vi> hist = {vi(n)};
	rep(it,0,iters) {
		int ver = rand() % sz(roots);
		int op = rand() % 3;
		if (op == 0 || sz(roots) == 1) {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int x = rand() % 11 - 5;
			roots.push_back(
				roots[ver]->update(0, n - 1, l, r, x));
			vi nxt = hist[ver];
			rep(i,l,r+1) nxt[i] += x;
			hist.push_back(nxt);
		} else if (op == 1) {
			int i = rand() % n;
			int x = rand() % 21 - 10;
			roots.push_back(
				roots[ver]->set(0, n - 1, i, x));
			vi nxt = hist[ver];
			nxt[i] = x;
			hist.push_back(nxt);
		} else {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int got = roots[ver]->query(0, n - 1, l, r);
			int exp = INT_MIN;
			rep(i,l,r+1) exp = max(exp, hist[ver][i]);
			assert(got == exp);
		}
	}
}

void testVsSparse(int n, int iters) {
	Node* p = new Node();
	Sparse sp;
	LazyUpdateTree tr(n);
	vi v(n);
	rep(it,0,iters) {
		int op = rand() % 3;
		if (op == 0) {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int x = rand() % 11 - 5;
			p = p->update(0, n - 1, l, r, x);
			sp.update(0, n - 1, l, r, x);
			tr.update(l, r, x);
			rep(i,l,r+1) v[i] += x;
		} else if (op == 1) {
			int i = rand() % n;
			int x = rand() % 21 - 10;
			p = p->set(0, n - 1, i, x);
			sp.set(0, n - 1, i, x);
			tr.set(i, x);
			v[i] = x;
		} else {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int exp = INT_MIN;
			rep(i,l,r+1) exp = max(exp, v[i]);
			assert(p->query(0, n - 1, l, r) == exp);
			assert(sp.query(0, n - 1, l, r) == exp);
			assert(tr.query(l, r) == exp);
		}
	}
}

void testLargeDomain() {
	const int L = 0, R = 1'000'000'000;
	Node* t0 = new Node();
	assert(t0->query(L, R, 0, 0) == 0);
	Node* t1 = t0->set(L, R, 42, 7);
	assert(t0->query(L, R, 0, R) == 0);
	assert(t1->query(L, R, 42, 42) == 7);
	assert(t1->query(L, R, 0, 41) == 0);
	Node* t2 = t1->update(L, R, 40, 50, 3);
	assert(t1->query(L, R, 42, 42) == 7);
	assert(t2->query(L, R, 42, 42) == 10);
	assert(t2->query(L, R, 40, 40) == 3);
	Node* t3 = t2->set(L, R, 42, 1);
	assert(t2->query(L, R, 42, 42) == 10);
	assert(t3->query(L, R, 42, 42) == 1);
	assert(t3->query(L, R, 40, 40) == 3);
	assert(t3->query(L, R, 40, 50) == 3);
}

int main() {
	rep(n,1,30) testSmall(n, 4000);
	rep(n,1,30) testVsSparse(n, 4000);
	testLargeDomain();
	cout << "Tests passed!" << endl;
}
