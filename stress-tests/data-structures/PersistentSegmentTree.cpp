#include "../utilities/template.h"

#include "../../content/data-structures/PersistentSegmentTree.h"

static_assert(PersistNode::id == 0);

void testSmall(int n, int iters) {
	vector<PersistNode*> roots = {new PersistNode()};
	vector<vi> hist = {vi(n)};
	rep(it,0,iters) {
		if (rand() % 2 || sz(roots) == 1) {
			int ver = rand() % sz(roots);
			int i = rand() % n;
			int x = rand() % 21 - 10;
			roots.push_back(
				roots[ver]->update(0, n - 1, i, x));
			vi nxt = hist[ver];
			nxt[i] = x;
			hist.push_back(nxt);
		} else {
			int ver = rand() % sz(roots);
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int got = roots[ver]->query(0, n - 1, l, r);
			int exp = 0;
			rep(i,l,r+1) exp += hist[ver][i];
			assert(got == exp);
		}
	}
}

void testLargeDomain() {
	const int L = 0, R = 1'000'000'000;
	PersistNode* t0 = new PersistNode();
	PersistNode* t1 = t0->update(L, R, 42, 7);
	assert(t0->query(L, R, 0, R) == 0);
	assert(t1->query(L, R, 42, 42) == 7);
	assert(t1->query(L, R, 0, 41) == 0);
	PersistNode* t2 = t1->update(L, R, 100, 3);
	assert(t1->query(L, R, 42, 100) == 7);
	assert(t2->query(L, R, 42, 100) == 10);
	PersistNode* t3 = t2->update(L, R, 42, 0);
	assert(t2->query(L, R, 0, R) == 10);
	assert(t3->query(L, R, 0, R) == 3);
}

int main() {
	rep(n,1,30) testSmall(n, 4000);
	testLargeDomain();
	cout << "Tests passed!" << endl;
}
