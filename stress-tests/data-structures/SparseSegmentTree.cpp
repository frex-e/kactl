#include "../utilities/template.h"

#include "../../content/data-structures/SparseSegmentTree.h"

void testSmall(int n, int iters) {
	SparseNode tr;
	vi v(n);
	rep(it,0,iters) {
		if (rand() % 2) {
			int i = rand() % n;
			int x = rand() % 21 - 10;
			tr.update(0, n - 1, i, x);
			v[i] = x;
		} else {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int got = tr.query(0, n - 1, l, r);
			int exp = 0;
			rep(i,l,r+1) exp += v[i];
			assert(got == exp);
		}
	}
}

void testLargeDomain() {
	const int L = 0, R = 1'000'000'000;
	SparseNode tr;
	assert(tr.query(L, R, 0, 0) == 0);
	tr.update(L, R, 42, 7);
	assert(tr.query(L, R, 42, 42) == 7);
	assert(tr.query(L, R, 0, 41) == 0);
	assert(tr.query(L, R, 43, 100) == 0);
	assert(tr.query(L, R, 40, 50) == 7);
	tr.update(L, R, 100, 3);
	assert(tr.query(L, R, 42, 100) == 10);
	tr.update(L, R, 42, 0);
	assert(tr.query(L, R, 0, R) == 3);
}

int main() {
	rep(n,1,30) testSmall(n, 10000);
	testLargeDomain();
	cout << "Tests passed!" << endl;
}
