#include "../utilities/template.h"

#include "../../content/data-structures/LazySegmentTree.h"
#include "../../content/data-structures/SparseLazySegmentTree.h"

void testVsDense(int n, int iters) {
	SparseLazyNode sp;
	LazyUpdateTree tr(n);
	vi v(n);
	rep(it,0,iters) {
		int l = rand() % n, r = rand() % n;
		if (l > r) swap(l, r);
		int x = rand() % 11 - 5;
		if (rand() % 2) {
			sp.update(0, n - 1, l, r, x);
			tr.update(l, r, x);
			rep(i,l,r+1) v[i] += x;
		} else {
			int gotSp = sp.query(0, n - 1, l, r);
			int gotTr = tr.query(l, r);
			int exp = INT_MIN;
			rep(i,l,r+1) exp = max(exp, v[i]);
			assert(gotSp == exp);
			assert(gotTr == exp);
		}
	}
}

void testMixed(int n, int iters) {
	SparseLazyNode sp;
	vi v(n);
	rep(it,0,iters) {
		int op = rand() % 3;
		if (op == 0) {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int x = rand() % 11 - 5;
			sp.update(0, n - 1, l, r, x);
			rep(i,l,r+1) v[i] += x;
		} else if (op == 1) {
			int i = rand() % n;
			int x = rand() % 21 - 10;
			sp.set(0, n - 1, i, x);
			v[i] = x;
		} else {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int got = sp.query(0, n - 1, l, r);
			int exp = INT_MIN;
			rep(i,l,r+1) exp = max(exp, v[i]);
			assert(got == exp);
		}
	}
}

void testLargeDomain() {
	const int L = 0, R = 1'000'000'000;
	SparseLazyNode tr;
	assert(tr.query(L, R, 0, 0) == 0);
	tr.update(L, R, 10, 20, 5);
	assert(tr.query(L, R, 10, 20) == 5);
	assert(tr.query(L, R, 0, 9) == 0);
	assert(tr.query(L, R, 21, 30) == 0);
	assert(tr.query(L, R, 15, 15) == 5);
	tr.update(L, R, 15, 25, 3);
	assert(tr.query(L, R, 15, 15) == 8);
	assert(tr.query(L, R, 10, 10) == 5);
	assert(tr.query(L, R, 25, 25) == 3);
	assert(tr.query(L, R, 21, 21) == 3);
	assert(tr.query(L, R, 10, 25) == 8);
	tr.set(L, R, 42, 7);
	assert(tr.query(L, R, 42, 42) == 7);
	assert(tr.query(L, R, 0, 41) == 0);
	tr.update(L, R, 40, 50, 3);
	assert(tr.query(L, R, 42, 42) == 10);
	assert(tr.query(L, R, 40, 40) == 3);
	tr.set(L, R, 42, 1);
	assert(tr.query(L, R, 42, 42) == 1);
	assert(tr.query(L, R, 40, 40) == 3);
	assert(tr.query(L, R, 40, 50) == 3);
}

int main() {
	rep(n,1,30) testVsDense(n, 10000);
	rep(n,1,30) testMixed(n, 10000);
	testLargeDomain();
	cout << "Tests passed!" << endl;
}
